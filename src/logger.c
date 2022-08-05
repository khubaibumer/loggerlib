#include "logger.h"
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <syscall.h>
#include <stdarg.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <zlib.h>
#include <semaphore.h>
#include <dirent.h>
#include <stdatomic.h>

#define return_if(x, y) ({if (x) {return y;}})

#define GENERATE_PRINT_FUNC(LEVEL) \
    void print_##LEVEL(const char *fn, int ln, const char *fmt, ...) { \
        return_if(_this->log_level > LEVEL, ); \
        log_info_t *info = prepare_data(fn, ln, LEVEL); \
        va_list temp, args; \
        va_start(temp, fmt); \
        va_copy(args, temp); \
        size_t len = vsnprintf(nullptr, 0, fmt, temp); \
        va_end(temp); \
        info->log_line = calloc(len + 2, sizeof(char)); \
        vsnprintf(info->log_line, len, fmt, args); \
        va_end(args); \
        _this->push_back(info); \
    }

#define DECLARE_PRINT_FUNC(LEVEL) \
	void print_##LEVEL(const char *fn, int ln, const char *fmt, ...)

#define FUNC_PTR_PRINT(LEVEL) print_##LEVEL

#define GET_PRIVATE(x) (x)->p_data

/* Forward Declarations */
typedef struct log_info log_info_t;
DECLARE_PRINT_FUNC(LOG_TRACE);
DECLARE_PRINT_FUNC(LOG_DEBUG);
DECLARE_PRINT_FUNC(LOG_INFO);
DECLARE_PRINT_FUNC(LOG_WARN);
DECLARE_PRINT_FUNC(LOG_ERROR);
log_info_t *prepare_data(const char *fn, int ln, kLogLevel level);
bool change_log_level(kLogLevel level);
void change_file(void);
void *logging_thread(void *args);
bool queue_lock(void);
void queue_unlock(void);
bool push_to_queue(log_info_t *);
log_info_t *pop_from_queue(void);
void* zip_file_routine(void *);
void zip_all_uncompressed_files(void);
//bool ignite_compressor_routine(void);
//void *compress_log_file(void *);

typedef struct log_info {
	tid_t tid;
	kLogLevel level;
	int line;
	char *func;
	char *log_line;

	TAILQ_ENTRY(log_info) entries;
} log_info_t;

typedef struct {
	logger_t *logger;
	size_t current_file_size;
	char *file_path;
	char *service;
	size_t max_file_size;
	char *filename;
	size_t filename_len;
	FILE *logfile;
	kLogLevel log_level;
	bool is_running;
	pthread_t loggerThread;
	pthread_spinlock_t workerLock;

	struct {
		TAILQ_HEAD(tailhead, log_info) head;
		pthread_spinlock_t queue_lock;
	} p_data;

	struct {
		sem_t sem;
		pthread_t tid;
		void (*send_signal) (void);
		void (*wait_signal) (void);
	} file_zip;

	bool (*lock)(void);
	void (*unlock)(void);
	bool (*push_back)(log_info_t *);
	log_info_t *(*pop_front)(void);

} loggerData_t;

//typedef struct {
//	char *orig_filename;
//	size_t filename_len;
//	char *compressed_filename;
//	const char *file_path;
//} compressor_params_t;

static logger_t logger = {
	.trace = &FUNC_PTR_PRINT(LOG_TRACE),
	.debug = &FUNC_PTR_PRINT(LOG_DEBUG),
	.info = &FUNC_PTR_PRINT(LOG_INFO),
	.warn = &FUNC_PTR_PRINT(LOG_WARN),
	.error = &FUNC_PTR_PRINT(LOG_ERROR),

	.update_log_level = &change_log_level,
	.cycle_file = &change_file
};

static loggerData_t *_this = nullptr;

GENERATE_PRINT_FUNC(LOG_TRACE)

GENERATE_PRINT_FUNC(LOG_DEBUG)

GENERATE_PRINT_FUNC(LOG_INFO)

GENERATE_PRINT_FUNC(LOG_WARN)

GENERATE_PRINT_FUNC(LOG_ERROR)

void send_job_signal(void) { sem_post(&_this->file_zip.sem); }

void wait_for_job_signal(void) { sem_wait(&_this->file_zip.sem); }

void logger_free(void *ptr) {
	if (ptr) {
		free(ptr), ptr = nullptr;
	}
}


char *mkfile_name(bool startup, size_t *len) {
	time_t timer;
	char buffer[48] = {0};
	struct tm *tm_info;
	struct timespec cur_time = {0};

	timer = time(nullptr);
	tm_info = localtime(&timer);
	clock_gettime(CLOCK_MONOTONIC, &cur_time);

	size_t millis = (size_t)(cur_time.tv_nsec / 1.0e6);

	size_t bytes = strftime(buffer, 48, "%Y_%m_%d_%H_%M_%S", tm_info);
	*len = 40 + bytes;
	char *time_stamp = calloc(*len, sizeof(char));
	if (startup) {
		snprintf(time_stamp, *len,"LogFile_%s_%03zu_startup.log", buffer, millis);
	} else {
		snprintf(time_stamp, *len, "LogFile_%s_%03zu.log", buffer, millis);
	}
	return time_stamp;
}

logger_t *create_logger(const char *service, const char *path) {
	if (_this == nullptr) {
		_this = calloc(1, sizeof(loggerData_t));
		_this->service = strdup(service);
		_this->logger = &logger;
		pthread_spin_init(&GET_PRIVATE(_this).queue_lock, 0);
		pthread_spin_init(&_this->workerLock, 0);
		_this->file_path = strdup(path);
		_this->lock = &queue_lock;
		_this->unlock = &queue_unlock;
		_this->push_back = &push_to_queue;
		_this->pop_front = &pop_from_queue;
	}

	assert(sem_init(&_this->file_zip.sem, 0, 0) == 0);
	_this->file_zip.send_signal = &send_job_signal;
	_this->file_zip.wait_signal = &wait_for_job_signal;
	pthread_create(&_this->file_zip.tid, nullptr, zip_file_routine, nullptr);

	assert(chdir(_this->file_path) == 0);
	_this->is_running = true;
	_this->filename = mkfile_name(true, &_this->filename_len);
	_this->logfile = fopen(_this->filename, "w+");
	TAILQ_INIT(&GET_PRIVATE(_this).head);

	pthread_create(&_this->loggerThread, nullptr, logging_thread, nullptr);
}

bool set_max_file_size(size_t max) {
	if (_this == nullptr)
		return false;

	_this->max_file_size = max;
	return true;
}

bool change_log_level(kLogLevel level) {
	_this->log_level = level;
}

logger_t *get_logger(void) {
	return _this->logger;
}

bool queue_lock(void) {
	pthread_spin_lock(&GET_PRIVATE(_this).queue_lock);
}

void queue_unlock(void) {
	pthread_spin_unlock(&GET_PRIVATE(_this).queue_lock);
}

bool close_logger(void) {
	_this->is_running = false;
	pthread_join(_this->file_zip.tid, nullptr);
	pthread_join(_this->loggerThread, nullptr);
	fclose(_this->logfile);
	logger_free(_this->file_path);
	logger_free(_this->service);
	logger_free(_this);
}

log_info_t *prepare_data(const char *fn, int ln, kLogLevel level) {
	log_info_t *info = calloc(1, sizeof(log_info_t));
	info->level = level;
	info->tid = syscall(SYS_gettid);
	info->line = ln;
	info->func = strdup(fn);
	return info;
}

bool push_to_queue(log_info_t *node) {
	_this->lock();
	TAILQ_INSERT_TAIL(&GET_PRIVATE(_this).head, node, entries);
	_this->unlock();
}

log_info_t *pop_from_queue() {
	_this->lock();
	log_info_t *msg = GET_PRIVATE(_this).head.tqh_first;
	TAILQ_REMOVE(&GET_PRIVATE(_this).head, GET_PRIVATE(_this).head.tqh_first, entries);
	_this->unlock();
	return msg;
}

const char *get_log_str(kLogLevel level) {
	switch (level) {
		case LOG_TRACE: return "TRACE";
		case LOG_DEBUG: return "DEBUG";
		case LOG_INFO: return "INFO";
		case LOG_WARN: return "WARN";
		case LOG_ERROR: return "ERROR";
		case LOG_NONE: return "NO LOGGING";
		default: return "UNKNOWN";
	}
}

void change_file() {
	printf("Changing File, current File Size %zu\n", _this->current_file_size);
	fclose(_this->logfile);
	_this->file_zip.send_signal(); // Post compressor thread a new Job
	logger_free(_this->filename);
	_this->filename = mkfile_name(false, &_this->filename_len);
	_this->logfile = fopen(_this->filename, "w+");
	assert(_this->logfile != nullptr);
	_this->current_file_size = 0;
}

char *get_current_time_stamp() {
	time_t timer;
	char buffer[48] = {0};
	struct tm *tm_info;
	struct timespec curtime = {0};

	timer = time(nullptr);
	tm_info = localtime(&timer);
	clock_gettime(CLOCK_MONOTONIC, &curtime);
	size_t millis = (size_t)(curtime.tv_nsec / 1.0e6);

	size_t time_len = strftime(buffer, sizeof(buffer), "%Y %m %d %H:%M:%S", tm_info);
	size_t len = strnlen(buffer, time_len) + 10;
	char *ts = calloc(len, sizeof(char));
	snprintf(ts, len, "%s,%03zu", buffer, millis);
	return ts;
}

void *logging_thread(void *args) {
	(void)args;
	const struct timespec sleep_time = {
		.tv_nsec = 10,
		.tv_sec = 0
	};

	while (_this->is_running) {
		while (TAILQ_EMPTY(&GET_PRIVATE(_this).head)) {
			nanosleep(&sleep_time, nullptr);
		}

		char *ts = get_current_time_stamp();
		log_info_t *msg = _this->pop_front();
		_this->current_file_size +=
			fprintf(_this->logfile, "%s [%lu] %s : %s:%d > [ %s ]\n", ts, msg->tid, get_log_str(msg->level),
			        msg->func,
			        msg->line, msg->log_line);
		fflush(_this->logfile);
		logger_free(msg->func);
		logger_free(msg->log_line);
		logger_free(msg);
		logger_free(ts);

		if (_this->current_file_size >= _this->max_file_size) {
			_this->logger->cycle_file();
		}
	}
	return nullptr;
}

unsigned long file_size(char *filename) {
	FILE *pFile = fopen(filename, "rb");
	fseek(pFile, 0, SEEK_END);
	unsigned long size = ftell(pFile);
	fclose(pFile);
	return size;
}

bool do_compress(char *in, char *out) {
	FILE *infile = fopen(in, "r");
	gzFile outfile = gzopen(out, "wb");

	return_if(!infile || !outfile, false);

	char inbuffer[KB(4)] = {0};
	size_t num_read;
	size_t total_read = 0;
	while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
		total_read += num_read;
		gzwrite(outfile, inbuffer, num_read);
	}
	fclose(infile);
	remove(in);
	gzclose(outfile);
	printf("Read %zu bytes, Wrote %lu bytes, Compression factor %4.2f%%n\n",
	       total_read,
	       file_size(out),
	       (1.0 - file_size(out) * 1.0 / total_read) * 100.0);

	return true;
}

void* zip_file_routine(void *args) {
	(void)args;
	zip_all_uncompressed_files();
	while (_this->is_running) {
		_this->file_zip.wait_signal();
		zip_all_uncompressed_files();
	}
	return nullptr;
}

void zip_all_uncompressed_files(void) {
	DIR *cwd = nullptr;
	struct dirent *dir = nullptr;

	cwd = opendir(_this->file_path);
	assert(cwd != nullptr);
	while ((dir = readdir(cwd)) != nullptr)
	{
		char *file = strdup(dir->d_name);
		if ((file[0] == '.') || (strstr(file, ".z") != nullptr) || (strncmp(file, _this->filename, _this->filename_len) == 0)) {
			logger_free(file);
			continue;
		} else {
			size_t len = strlen(file) + 3;
			char *compressed_filename = calloc(len, sizeof(char));
			snprintf(compressed_filename, len, "%s.z", file);
			do_compress(file, compressed_filename);
			printf("Original file %s, New File Name: %s\n", file, compressed_filename);
			logger_free(compressed_filename);
		}
		logger_free(file);
	}
	closedir(cwd);
}

//void *compress_log_file(void *args) {
//	compressor_params_t *params = args;
//	size_t len = strnlen(params->orig_filename, params->filename_len) + 3;
//	params->compressed_filename = calloc(len, sizeof(char));
//	snprintf(params->compressed_filename, len, "%s.z", params->orig_filename);
//	do_compress(params->orig_filename, params->compressed_filename);
//
//	logger_free(params->compressed_filename);
//	logger_free(params->orig_filename);
//	logger_free(params);
//	pthread_exit(nullptr);
//}

//bool ignite_compressor_routine(void) {
//	pthread_spin_lock(&_this->workerLock);
//	compressor_params_t *params = calloc(1, sizeof(compressor_params_t));
//	params->file_path = _this->file_path;
//	params->orig_filename = _this->filename;
//	params->filename_len = _this->filename_len;
//	_this->filename = nullptr;
//
//	pthread_t worker;
//	pthread_create(&worker, nullptr, &compress_log_file, params);
//	pthread_detach(worker);
//	pthread_spin_unlock(&_this->workerLock);
//}
