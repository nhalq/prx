#include "Prx.hh"

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Log.hh"
#include "SockS5.hh"

#define PWDSIZE 128
#define BUFSIZE 512
#define ENVSIZE 64

#define PRX_ENV_DEF_HOST "10.30.80.254"
#define PRX_ENV_DEF_PORT "8880"

char* prxenv_host = NULL;
char* prxenv_port = NULL;

void envii();
void cleanup();

int main(const int argc, const char* argv[]) {
  envii();

  char cwd[PWDSIZE];
  ::readlink("/proc/self/exe", cwd, PWDSIZE);
  char* workdir = ::dirname(cwd);
  PRX_INFx("WORKDIR=%s", workdir);

  char buffer[BUFSIZE];
  ::snprintf(buffer, sizeof(buffer), "LD_PRELOAD=%s/%s", workdir, SOFILE_NAME);
  ::putenv(buffer);
  PRX_INF("Injected");
  PRX_INFx("LD_PRELOAD=%s", getenv("LD_PRELOAD"));

  int idx = 1;
  PRX_DBGx("ARGS=%d", argc);
  if (idx >= argc) return EXIT_SUCCESS;

  PRX_INF("Start program");
  int retval = execvp(argv[idx], (char* const*)argv + idx);
  if (retval < 0) PRX_ERRx("execvp() errno=%d %s", errno, strerror(errno));

  cleanup();
  return retval;
}

void envii() {
  if (!getenv(PRX_ENV_HOST)) {
    prxenv_host = (char*)malloc(ENVSIZE);
    snprintf(prxenv_host, BUFSIZE, "%s=%s", PRX_ENV_HOST, PRX_ENV_DEF_HOST);
    putenv(prxenv_host);
  }

  if (!getenv(PRX_ENV_PORT)) {
    prxenv_port = (char*)malloc(ENVSIZE);
    snprintf(prxenv_port, BUFSIZE, "%s=%s", PRX_ENV_PORT, PRX_ENV_DEF_PORT);
    putenv(prxenv_port);
  }

  PRX_INFx("%s=%s", PRX_ENV_HOST, getenv(PRX_ENV_HOST));
  PRX_INFx("%s=%s", PRX_ENV_PORT, getenv(PRX_ENV_PORT));
}

void cleanup() {
  if (prxenv_host) free(prxenv_host);
  if (prxenv_port) free(prxenv_port);
}
