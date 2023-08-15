#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Log.hh"
#include "Prx.hh"
#include "SockS5.hh"

typedef int (*prx_connect_t)(int, const struct sockaddr*, socklen_t);
prx_connect_t sys_connect = NULL;

static struct sockaddr_in socks_address;

void libprx_init() {
  PRX_INF("LD INIT");

  const char* prx_addr = getenv(PRX_ENV_HOST);
  const char* str_port = getenv(PRX_ENV_PORT);
  int prx_port = atoi(str_port);

  bzero(&socks_address, sizeof(socks_address));
  inet_pton(AF_INET, prx_addr, &socks_address.sin_addr);
  socks_address.sin_family = AF_INET;
  socks_address.sin_port = htons(prx_port);

  *(void**)(&sys_connect) = dlsym(RTLD_NEXT, "connect");
}

int connect(int sockfd, __CONST_SOCKADDR_ARG addr, socklen_t len) {
  if (!sys_connect) libprx_init();

  const struct sockaddr_in* peerAddr = (const struct sockaddr_in*)addr;
  char* ip = inet_ntoa(peerAddr->sin_addr);
  uint16_t port = htons(peerAddr->sin_port);

  int octect = (::ntohl(peerAddr->sin_addr.s_addr) >> 24) & 0xFF;
  if (octect == 127) {
    PRX_INFx("Bypass loopback connection %s:%d", ip, port);
    return sys_connect(sockfd, addr, len);
  }

  int flags = ::fcntl(sockfd, F_GETFL, 0);
  ::fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);

  PRX_INFx("Establish connection to %s:%d", ip, (int)port);
  int retval = sys_connect(sockfd, (struct sockaddr*)(&socks_address), len);
  if (retval < 0) {
    if (errno != EINPROGRESS) return retval;

    struct pollfd pfd;
    pfd.fd = sockfd;
    pfd.events = POLLOUT;
    pfd.revents = 0;
    retval = ::poll(&pfd, 1, 0);

    if (retval < 0) {
      PRX_DBGx("%d errno=%d %s", retval, errno, strerror(errno));
      return retval;
    }
  }

  struct SockS5IdentRequest prxIdentRequest;
  prxIdentRequest.ver = SOCKS5_VERSION;
  prxIdentRequest.nmethods = 1;
  prxIdentRequest.methods[0] = SOCKS5_METHOD_NOAUTH;
  size_t prxIdentReqSize = 2 + prxIdentRequest.nmethods;

  retval = ::send(sockfd, &prxIdentRequest, prxIdentReqSize, 0);
  if (retval < 0) {
    PRX_ERRx("%d errno=%d %s", retval, errno, strerror(errno));
    return retval;
  }

  struct SockS5IdentReply prxIdentReply;
  retval = recv(sockfd, &prxIdentReply, sizeof(prxIdentReply), 0);
  if (retval < 0) return retval;
  PRX_DBGx("method=%#02x", prxIdentReply.method);

  struct SockS5Request prxRequest;
  prxRequest.ver = SOCKS5_VERSION;
  prxRequest.cmd = SOCKS5_CMD_CONNECT;
  prxRequest.rsv = 0x00;
  prxRequest.atyp = SOCKS5_ATYP_IPV4;

  char buffer[10];
  memcpy(buffer, &prxRequest, 4);
  memcpy(buffer + 4, &peerAddr->sin_addr, 4);
  memcpy(buffer + 8, &peerAddr->sin_port, 2);

  retval = send(sockfd, buffer, 10, 0);
  if (retval < 0) return retval;

  struct socks5_reply prx_reply;
  retval = recv(sockfd, &prx_reply, sizeof(prx_reply), 0);
  if (retval < 0) return retval;

  PRX_DBGx("ver=%#02x, rep=%#02x", prx_reply.ver, prx_reply.rep);
  return 0;
}
