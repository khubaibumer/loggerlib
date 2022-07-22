#include "logger.h"
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <syscall.h>
#include <stdarg.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <zlib.h>
#include <thread_db.h>

#define return_if(x, y) ({if (x) {return y;}})

#define GENERATE_PRINT_FUNC(LEVEL) \
    void print_##LEVEL(const char *fn, int ln, const char *fmt, ...) { \
        return_if(this->log_level > LEVEL, );                       \
                                      \
        log_info_t *info = prepare_data(fn, ln, LEVEL);                   \
        va_list temp, args;                 \
        va_start(temp, fmt);          \
        va_copy(args, temp);          \
        size_t len = vsnprintf(NULL, 0, fmt, temp);                       \
        va_end(temp);                       \
        info->log_line = calloc(len + 2, sizeof(char));                   \
        vsnprintf(info->log_line, len, fmt, args);                        \
        va_end(args);                 \
                                      \
        this->push_back(info);\
    }

#define DECLARE_PRINT_FUNC(LEVEL) \
	void print_##LEVEL(const char *fn, int ln, const char *fmt, ...)

#define FUNC_PTR_PRINT(LEVEL) print_##LEVEL

//#define GENERATE_PRINT(info, fmt) \
//({                       \
//    va_list temp, args;     \
//    va_start(temp, (fmt)); \
//    va_copy(args, temp); \
//    size_t len = vsnprintf(NULL, 0, (fmt), temp); \
//    va_end(temp);           \
//    (info)->log_line = calloc(len + 1, sizeof(char)); \
//    vsnprintf((info)->log_line, len, (fmt), args);  \
//    va_end(args);\
//})

#define GET_PRIVATE(x) (x)->private

/* Forward Declarations */
typedef struct log_info log_info_t;
DECLARE_PRINT_FUNC(LOG_TRACE);
DECLARE_PRINT_FUNC(LOG_DEBUG);
DECLARE_PRINT_FUNC(LOG_INFO);
DECLARE_PRINT_FUNC(LOG_WARN);
DECLARE_PRINT_FUNC(LOG_ERROR);
//void print_trace(const char *fn, int ln, const char *fmt, ...);
//void print_debug(const char *fn, int ln, const char *fmt, ...);
//void print_info(const char *fn, int ln, const char *fmt, ...);
//void print_warn(const char *fn, int ln, const char *fmt, ...);
//void print_error(const char *fn, int ln, const char *fmt, ...);
log_info_t *prepare_data(const char *fn, const int ln, kLogLevel level);
bool change_log_level(kLogLevel level);
void change_file(void);
void *logging_thread(void *args);
bool queue_lock(void);
void queue_unlock(void);
bool push_to_queue(log_info_t *);
log_info_t *pop_from_queue(void);
bool ignite_compressor_routine(void);
void *compress_log_file(void *);

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
	size_t max_file_size;
	char *filename;
	FILE *logfile;
	kLogLevel log_level;
	bool is_running;
	pthread_t loggerThread;
	thread_t compressorThread;

	struct {
		TAILQ_HEAD(tailhead, log_info) head;
		pthread_spinlock_t queue_lock;
	} private;

	bool (*lock)(void);
	void (*unlock)(void);
	bool (*push_back)(log_info_t *);
	log_info_t *(*pop_front)(void);
	bool (*ignite_compressor)(void);

} loggerData_t;

typedef struct {
	char *orig_filename;
	char *compressed_filename;
	const char *file_path;
} compressor_params_t;

static logger_t logger = {
	.trace = &FUNC_PTR_PRINT(LOG_TRACE),
	.debug = &FUNC_PTR_PRINT(LOG_DEBUG),
	.info = &FUNC_PTR_PRINT(LOG_INFO),
	.warn = &FUNC_PTR_PRINT(LOG_WARN),
	.error = &FUNC_PTR_PRINT(LOG_ERROR),

	.update_log_level = &change_log_level,
	.cycle_file = &change_file
};

static loggerData_t *this = NULL;

GENERATE_PRINT_FUNC(LOG_TRACE);

GENERATE_PRINT_FUNC(LOG_DEBUG);

GENERATE_PRINT_FUNC(LOG_INFO);

GENERATE_PRINT_FUNC(LOG_WARN);

GENERATE_PRINT_FUNC(LOG_ERROR);

char *mkfile_name(bool startup) {
	time_t timer;
	char buffer[48] = {0};
	struct tm *tm_info;
	struct timespec curtime = {0};

	timer = time(NULL);
	tm_info = localtime(&timer);
	clock_gettime(CLOCK_MONOTONIC, &curtime);

	size_t millis = (size_t)(curtime.tv_nsec / 1.0e6);

	size_t bytes = strftime(buffer, 48, "%Y_%m_%d_%H_%M_%S", tm_info);
	char *time_stamp = calloc(40 + bytes, sizeof(char));
	if (startup) {
		snprintf(time_stamp, 40 + bytes,"LogFile_%s_%03ld_startup.log", buffer, millis);
	} else {
		snprintf(time_stamp, 40 + bytes, "LogFile_%s_%03ld.log", buffer, millis);
	}
	return time_stamp;
}

logger_t *create_logger(const char *path) {
	if (this == NULL) {
		this = calloc(1, sizeof(loggerData_t));
		this->logger = &logger;
		pthread_spin_init(&GET_PRIVATE(this).queue_lock, 0);
		this->file_path = strdup(path);
		this->lock = &queue_lock;
		this->unlock = &queue_unlock;
		this->push_back = &push_to_queue;
		this->pop_front = &pop_from_queue;
		this->ignite_compressor = &ignite_compressor_routine;
	}

	assert(chdir(this->file_path) == 0);
	this->is_running = true;
	this->filename = mkfile_name(true);
	this->logfile = fopen(this->filename, "w+");
	TAILQ_INIT(&GET_PRIVATE(this).head);

	pthread_create(&this->loggerThread, NULL, logging_thread, NULL);
	pthread_detach(this->loggerThread);
}

bool set_max_file_size(size_t max) {
	if (this == NULL)
		return false;

	this->max_file_size = max;
	return true;
}

bool change_log_level(kLogLevel level) {
	this->log_level = level;
}

logger_t *get_logger(void) {
	return this->logger;
}

bool queue_lock(void) {
	pthread_spin_lock(&GET_PRIVATE(this).queue_lock);
}

void queue_unlock(void) {
	pthread_spin_unlock(&GET_PRIVATE(this).queue_lock);
}

bool close_logger(void) {
	this->is_running = false;
	fclose(this->logfile);
	free(this->file_path);
	free(this);
}

log_info_t *prepare_data(const char *fn, const int ln, kLogLevel level) {
	log_info_t *info = calloc(1, sizeof(log_info_t));
	info->level = level;
	info->tid = syscall(SYS_gettid);
	info->line = ln;
	info->func = strdup(fn);
	return info;
}

bool push_to_queue(log_info_t *node) {
	this->lock();
	TAILQ_INSERT_TAIL(&GET_PRIVATE(this).head, node, entries);
	this->unlock();
}

log_info_t *pop_from_queue() {
	this->lock();
	log_info_t *msg = GET_PRIVATE(this).head.tqh_first;
	TAILQ_REMOVE(&GET_PRIVATE(this).head, GET_PRIVATE(this).head.tqh_first, entries);
	this->unlock();
	return msg;
}

//void print_trace(const char *fn, int ln, const char *fmt, ...) {
//	return_if(this->log_level > LOG_TRACE,);
//
//	log_info_t *info = prepare_data(fn, ln, LOG_TRACE);
//	GENERATE_PRINT(info, fmt);
//	this->push_back(info);
//}
//
//void print_debug(const char *fn, int ln, const char *fmt, ...) {
//	return_if(this->log_level > LOG_DEBUG,);
//
//	log_info_t *info = prepare_data(fn, ln, LOG_DEBUG);
//	GENERATE_PRINT(info, fmt);
//	this->push_back(info);
//}
//
//void print_info(const char *fn, int ln, const char *fmt, ...) {
//	return_if(this->log_level > LOG_INFO,);
//
//	log_info_t *info = prepare_data(fn, ln, LOG_INFO);
//	GENERATE_PRINT(info, fmt);
//	this->push_back(info);
//}
//
//void print_warn(const char *fn, int ln, const char *fmt, ...) {
//	return_if(this->log_level > LOG_WARN,);
//
//	log_info_t *info = prepare_data(fn, ln, LOG_WARN);
//	GENERATE_PRINT(info, fmt);
//	this->push_back(info);
//}
//
//void print_error(const char *fn, int ln, const char *fmt, ...) {
//	return_if(this->log_level > LOG_ERROR,);
//
//	log_info_t *info = prepare_data(fn, ln, LOG_ERROR);
//	GENERATE_PRINT(info, fmt);
//	this->push_back(info);
//}

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
	printf("Changing File, current File Size %zu\n", this->current_file_size);
	fclose(this->logfile);
	this->ignite_compressor();
	this->filename = mkfile_name(false);
	this->logfile = fopen(this->filename, "w+");
	assert(this->logfile != NULL);
	this->current_file_size = 0;
}

char *get_current_time_stamp() {
	time_t timer;
	char buffer[48] = {0};
	struct tm *tm_info;
	struct timespec curtime = {0};

	timer = time(NULL);
	tm_info = localtime(&timer);
	clock_gettime(CLOCK_MONOTONIC, &curtime);
	size_t millis = (size_t)(curtime.tv_nsec / 1.0e6);

	strftime(buffer, sizeof(buffer), "%Y %m %d %H:%M:%S", tm_info);
	size_t len = strlen(buffer) + 5;
	char *ts = calloc(len, sizeof(char));
	snprintf(ts, len + 6, "%s,%03ld", buffer, millis);
	return ts;
}

void *logging_thread(void *args) {
	(void)args;
	const struct timespec sleep_time = {
		.tv_nsec = 10,
		.tv_sec = 0
	};

	while (this->is_running) {
		while (TAILQ_EMPTY(&GET_PRIVATE(this).head)) {
			nanosleep(&sleep_time, NULL);
		}

		char *ts = get_current_time_stamp();
		log_info_t *msg = this->pop_front();
		this->current_file_size +=
			fprintf(this->logfile, "%s [%lu] %s : %s:%d > [ %s ]\n", ts, msg->tid, get_log_str(msg->level),
			        msg->func,
			        msg->line, msg->log_line);
		fflush(this->logfile);
		free(msg->func);
		free(msg->log_line);
		free(msg);
		free(ts);

		if (this->current_file_size >= this->max_file_size) {
			this->logger->cycle_file();
		}
	}
	return NULL;
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

	char inbuffer[KB(4)];
	int num_read = 0;
	unsigned long total_read = 0, total_wrote = 0;
	while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
		total_read += num_read;
		gzwrite(outfile, inbuffer, num_read);
	}
	fclose(infile);
	gzclose(outfile);
	printf("Read %lud bytes, Wrote %ld bytes, Compression factor %4.2f%%n",
	       total_read,
	       file_size(out),
	       (1.0 - file_size(out) * 1.0 / total_read) * 100.0);
}

void *compress_log_file(void *args) {
	compressor_params_t *params = args;
	size_t len = strlen(params->orig_filename) + 3;
	params->compressed_filename = calloc(len, sizeof(char));
	snprintf(params->compressed_filename, len, "%s.z", params->orig_filename);
	do_compress(params->orig_filename, params->compressed_filename);

	free(params->compressed_filename);
	free(params->orig_filename);
	free(params);
	pthread_exit(NULL);
}

bool ignite_compressor_routine(void) {
	compressor_params_t *params = calloc(1, sizeof(compressor_params_t));
	params->file_path = this->file_path;
	params->orig_filename = this->filename;
	this->filename = NULL;

	pthread_create(&this->compressorThread, NULL, &compress_log_file, params);
	pthread_detach(this->compressorThread);
}
