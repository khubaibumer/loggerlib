#include "logger.h"
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <syscall.h>
#include <stdarg.h>
#include <sys/queue.h>

#define return_if(x, y) ({if (x) {return y;}})

/* Forward Declarations */
void print_trace(const char *fn, int ln, const char *fmt, ...);
void print_debug(const char *fn, int ln, const char *fmt, ...);
void print_info(const char *fn, int ln, const char *fmt, ...);
void print_warn(const char *fn, int ln, const char *fmt, ...);
void print_error(const char *fn, int ln, const char *fmt, ...);
bool change_log_level(kLogLevel level);
void change_file(void);
void *logging_thread(void *args);

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
  FILE *logfile;
  kLogLevel log_level;
  bool is_running;

  pthread_t loggerThread;
  TAILQ_HEAD(tailhead, log_info) head;
  pthread_spinlock_t queue_lock;
} loggerData_t;

static logger_t logger = {
	.trace = &print_trace,
	.debug = &print_debug,
	.info = &print_info,
	.warn = &print_warn,
	.error = &print_error,

	.update_log_level = &change_log_level,
	.cycle_file = &change_file
};

char *mkfile_name(bool startup) {
  time_t timer;
  char buffer[36] = {0};
  struct tm *tm_info;

  timer = time(NULL);
  tm_info = localtime(&timer);

  size_t bytes = strftime(buffer, 36, "%Y_%m_%d_%H_%M_%S", tm_info);
  char *time_stamp = calloc(32 + bytes, sizeof(char));
  if (startup) {
	sprintf(time_stamp, "LogFile_%s_startup.log", buffer);
  } else {
	sprintf(time_stamp, "LogFile_%s.log", buffer);
  }
  return time_stamp;
}

static loggerData_t *this = NULL;

logger_t *create_logger(const char *path) {
  if (this == NULL) {
	this = calloc(1, sizeof(loggerData_t));
	this->logger = &logger;
	pthread_spin_init(&this->queue_lock, 0);
	this->file_path = strdup(path);
  }

  assert(chdir(this->file_path) == 0);
  this->is_running = true;
  char *file = mkfile_name(true);
  this->logfile = fopen(file, "w+");
  TAILQ_INIT(&this->head);
  free(file);

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
  pthread_spin_lock(&this->queue_lock);
  TAILQ_INSERT_TAIL(&this->head, node, entries);
  pthread_spin_unlock(&this->queue_lock);
}

log_info_t *pop_from_queue() {
  pthread_spin_lock(&this->queue_lock);
  log_info_t *msg = this->head.tqh_first;
  TAILQ_REMOVE(&this->head, this->head.tqh_first, entries);
  pthread_spin_unlock(&this->queue_lock);
  return msg;
}

void print_trace(const char *fn, int ln, const char *fmt, ...) {
  return_if(this->log_level >= LOG_TRACE,);

  log_info_t *info = prepare_data(fn, ln, LOG_TRACE);
  va_list temp, args;
  va_start (temp, fmt);
  va_copy(args, temp);
  size_t len = vsnprintf(NULL, 0, fmt, temp);
  va_end(temp);
  info->log_line = calloc(len + 1, sizeof(char));
  vsnprintf(info->log_line, len + 1, fmt, args);
  va_end (args);

  push_to_queue(info);
}

void print_debug(const char *fn, int ln, const char *fmt, ...) {
  return_if(this->log_level >= LOG_DEBUG,);

  log_info_t *info = prepare_data(fn, ln, LOG_DEBUG);
  va_list temp, args;
  va_start (temp, fmt);
  va_copy(args, temp);
  size_t len = vsnprintf(NULL, 0, fmt, temp);
  va_end(temp);
  info->log_line = calloc(len + 1, sizeof(char));
  vsnprintf(info->log_line, len + 1, fmt, args);
  va_end (args);

  push_to_queue(info);
}

void print_info(const char *fn, int ln, const char *fmt, ...) {
  return_if(this->log_level >= LOG_INFO,);

  log_info_t *info = prepare_data(fn, ln, LOG_INFO);
  va_list temp, args;
  va_start (temp, fmt);
  va_copy(args, temp);
  size_t len = vsnprintf(NULL, 0, fmt, temp);
  va_end(temp);
  info->log_line = calloc(len + 1, sizeof(char));
  vsnprintf(info->log_line, len + 1, fmt, args);
  va_end (args);

  push_to_queue(info);
}

void print_warn(const char *fn, int ln, const char *fmt, ...) {
  return_if(this->log_level >= LOG_WARN,);

  log_info_t *info = prepare_data(fn, ln, LOG_WARN);
  va_list temp, args;
  va_start (temp, fmt);
  va_copy(args, temp);
  size_t len = vsnprintf(NULL, 0, fmt, temp);
  va_end(temp);
  info->log_line = calloc(len + 1, sizeof(char));
  vsnprintf(info->log_line, len + 1, fmt, args);
  va_end (args);

  push_to_queue(info);
}

void print_error(const char *fn, int ln, const char *fmt, ...) {
  return_if(this->log_level >= LOG_ERROR,);

  log_info_t *info = prepare_data(fn, ln, LOG_ERROR);
  va_list temp, args;
  va_start (temp, fmt);
  va_copy (args, temp);
  size_t len = vsnprintf(NULL, 0, fmt, temp);
  va_end(temp);
  info->log_line = calloc(len + 1, sizeof(char));
  vsnprintf(info->log_line, len + 1, fmt, args);
  va_end (args);

  push_to_queue(info);
}

const char *get_log_str(kLogLevel level) {
  switch (level) {
	case LOG_TRACE: return "TRACE";
	case LOG_DEBUG: return "DEBUG";
	case LOG_INFO: return "INFO";
	case LOG_WARN: return "WARN";
	case LOG_ERROR: return "ERROR";
	default: return "UNKNOWN";
  }
}

void change_file() {
  printf("Changing File, current File Size %lu\n", this->current_file_size);
  fclose(this->logfile);
  char *filename = mkfile_name(false);
  this->logfile = fopen(filename, "w+");
  assert(this->logfile != NULL);
  free(filename);
  this->current_file_size = 0;
}

char *get_current_time_stamp() {
  time_t timer;
  char buffer[36] = {0};
  struct tm *tm_info;

  timer = time(NULL);
  tm_info = localtime(&timer);

  strftime(buffer, 36, "%Y %m %d %H:%M:%S", tm_info);
  return strdup(buffer);
}

void *logging_thread(void *args) {
  (void)args;
  const struct timespec sleep_time = {
	  .tv_nsec = 10,
	  .tv_sec = 0
  };

  while (this->is_running) {
	while (TAILQ_EMPTY(&this->head)) {
	  nanosleep(&sleep_time, NULL);
	}

	char *ts = get_current_time_stamp();
	log_info_t *msg = pop_from_queue();
	this->current_file_size += fprintf(this->logfile, "%s [%lu] %s : %s:%d > %s", ts, msg->tid, get_log_str(msg->level),
									   msg->func,
									   msg->line, msg->log_line);
	fflush(this->logfile);
	free(msg);
	free(ts);

	if (this->current_file_size >= this->max_file_size) {
	  this->logger->cycle_file();
	}
  }
  return NULL;
}