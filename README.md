[![CodeFactor](https://www.codefactor.io/repository/github/khubaibumer/loggerlib/badge)](https://www.codefactor.io/repository/github/khubaibumer/loggerlib)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/451de471a0344463bcfc91acc8d5acf4)](https://www.codacy.com/gh/khubaibumer/loggerlib/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=khubaibumer/loggerlib&amp;utm_campaign=Badge_Grade)
[![Softacheck](https://softacheck.com/app/repository/khubaibumer/loggerlib/badge)](https://softacheck.com/app/repository/khubaibumer/loggerlib/issues)

# LoggerLib

## Functions

### logger_t* create_logger(const char *path)
Creates and returns pointer to the newly created by the logger at specified path

### logger_t* get_logger(void)
Returns pointer to the already created logger

### bool close_logger(void)
Close the logging library

### bool set_max_file_size(size_t max)
Automatically changes the file when max size is reached

## Macros
*  TRACE(fmt, ...)
*  DEBUG(fmt, ...)
*  INFO(fmt, ...)
*  WARN(fmt, ...)
*  ERROR(fmt, ...)

## Sample Code
  ```
  create_logger("/va/logs/LoggerTest");
  set_max_file_size(KB(3));
  get_logger()->trace(__func__, __LINE__, "%s\n", "ftw");
  TRACE("%s\n", "Wow!!!");
  close_logger();
  ```
