#include "Log.hh"

#include <time.h>

const char* psee_time() {
  time_t unix_time;
  time(&unix_time);
  char* str = ctime(&unix_time);
  str[24] = '\0';

  return str;
}

int prx_log(const char* lvl, const char* fmt, ...) {
  char buffer[PRX_FMT_BUFFER_SIZE];

  va_list args;
  va_start(args, fmt);
  int fmtr = vsnprintf(buffer, PRX_FMT_BUFFER_SIZE - 1, fmt, args);
  va_end(args);

  if (fmtr < 0) return fmtr;
  return fprintf(stderr, "%s " APP_SHORTNAME ":%s %s\n", psee_time(), lvl, buffer);
}
