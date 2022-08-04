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
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_NONE
} kLogLevel;

typedef struct {
	/// @fn trace
	/// @param fn caller function name
	/// @param ln caller line number
	/// @param fmt string format
	/// @param ... variadic list of arguments
	/// @brief Prints Trace Level logs
	void (*trace)(const char *fn, int ln, const char *fmt, ...);

	/// @fn debug
	/// @param fn caller function name
	/// @param ln caller line number
	/// @param fmt string format
	/// @param ... variadic list of arguments
	/// @brief Prints Debug Level logs
	void (*debug)(const char *fn, int ln, const char *fmt, ...);

	/// @fn info
	/// @param fn caller function name
	/// @param ln caller line number
	/// @param fmt string format
	/// @param ... variadic list of arguments
	/// @brief Prints Info Level logs
	void (*info)(const char *fn, int ln, const char *fmt, ...);

	/// @fn warn
	/// @param fn caller function name
	/// @param ln caller line number
	/// @param fmt string format
	/// @param ... variadic list of arguments
	/// @brief Prints Warn Level logs
	void (*warn)(const char *fn, int ln, const char *fmt, ...);

	/// @fn error
	/// @param fn caller function name
	/// @param ln caller line number
	/// @param fmt string format
	/// @param ... variadic list of arguments
	/// @brief Prints Error Level logs
	void (*error)(const char *fn, int ln, const char *fmt, ...);

	/* Logger Control */
	/// @fn update_log_level
	/// @param level  Requested Log Level
	/// @brief update log_level to specified
	///		 must be called once after create_logger() is called
	bool (*update_log_level)(kLogLevel level);

	/// @fn cycle_file
	/// @brief close the current file and start a new file
	void (*cycle_file)(void);
} logger_t;

/// @fn create_logger
/// @param service Name of the Service creating a Logger
/// @param path Path where Logfile will be created
/// @brief create a new logger object
logger_t *create_logger(const char *service, const char *path);

/// @fn set_max_file_size
/// @param max set max file size
/// @brief Once file reaches the size mentioned here we create a new file and zips old file
bool set_max_file_size(size_t max);

/// @fn get_logger
/// @brief returns pointer to logger object already cleated
logger_t *get_logger(void);

/// @fn close_logger
/// @brief closes the current open file and zips it and closes it
bool close_logger(void);

#define TRACE(fmt, ...) get_logger()->trace(__func__, __LINE__, fmt, __VA_ARGS__)
#define DEBUG(fmt, ...) get_logger()->debug(__func__, __LINE__, fmt, __VA_ARGS__)
#define INFO(fmt, ...) get_logger()->info(__func__, __LINE__, fmt, __VA_ARGS__)
#define WARN(fmt, ...) get_logger()->warn(__func__, __LINE__, fmt, __VA_ARGS__)
#define ERROR(fmt, ...) get_logger()->error(__func__, __LINE__, fmt, __VA_ARGS__)

#endif //LOGGER_INCLUDE_LOGGER_H_
