#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LOGGER_INCLUDE_LOGGER_H_
#define LOGGER_INCLUDE_LOGGER_H_

typedef unsigned long tid_t;

#ifndef __cplusplus
typedef unsigned char bool;
#define true  1
#define false 0
#define nullptr NULL
#endif

#define KB(x) ( (x) * 1000)
#define MB(x) (KB(x) * 1000)
#define GB(x) (MB(x) * 1000)

typedef enum {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_NONE
} kLogLevel;

typedef struct {
  void (*trace) (const char *fn, int ln, const char *fmt, ...);
  void (*debug) (const char *fn, int ln, const char *fmt, ...);
  void (*info)  (const char *fn, int ln, const char *fmt, ...);
  void (*warn)  (const char *fn, int ln, const char *fmt, ...);
  void (*error) (const char *fn, int ln, const char *fmt, ...);

  /* Logger Control */
  /// @brief update log_level to specified
  ///		 must be called once after create_logger() is called
  /// @param level  Requested Log Level
  bool (*update_log_level) (kLogLevel level);

  bool (*update_level) (const char *level);

  /// @brief close the current file and start a new file
  void (*cycle_file) (void);
} logger_t;

logger_t* create_logger(const char *service, const char *path);

bool set_max_file_size(size_t max);

logger_t* get_logger(void);

bool close_logger(void);

#define TRACE(fmt, ...) get_logger()->trace(__func__, __LINE__, fmt, __VA_ARGS__)
#define DEBUG(fmt, ...) get_logger()->debug(__func__, __LINE__, fmt, __VA_ARGS__)
#define INFO(fmt, ...) get_logger()->info(__func__, __LINE__, fmt, __VA_ARGS__)
#define WARN(fmt, ...) get_logger()->warn(__func__, __LINE__, fmt, __VA_ARGS__)
#define ERROR(fmt, ...) get_logger()->error(__func__, __LINE__, fmt, __VA_ARGS__)

#endif //LOGGER_INCLUDE_LOGGER_H_
