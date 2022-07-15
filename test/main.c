#include <unistd.h>
#include "logger.h"

int main(int argc, char **argv) {

  create_logger("/home/khubaibumer/");
  set_max_file_size(KB(2));

  get_logger()->trace(__func__, __LINE__, "%s\n", "powwow");

  TRACE("%s\n", "Wow!!!");

  TRACE("%d %s\n", 123, "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%d %s\n", 345, "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO ("%d %s\n", 567, "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN ("%d %s\n", 987, "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%d %s\n", 110, "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  TRACE("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  DEBUG("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  INFO("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  WARN("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");
  ERROR("%s\n", "ancfjrqwjkyhfasurhwqerbdfisfgliwefweyhgrdfbiwfgbywehvrdfqwargdfdwhgfbwljhard");

  sleep(120);

  close_logger();

  return 0;
}
