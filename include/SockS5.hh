#ifndef SOCKS5_INCLUDED
#define SOCKS5_INCLUDED

#include <netinet/in.h>

enum SOCKS_VERSION { SOCKS5_VERSION = 0x05 };

enum SOCKS5_METHOD {
  SOCKS5_METHOD_NOAUTH = 0x00,
  SOCKS5_METHOD_GSSAPI = 0x01,
  SOCKS5_METHOD_USERPASS = 0x02,
  SOCKS5_METHOD_IANAASSIGNED = 0x03,
  SOCKS5_METHOD_RESERVED = 0x80,
  SOCKS5_METHOD_NOMETHOD = 0xFF
};

enum SOCKS5_ATYP {
  SOCKS5_ATYP_IPV4 = 0x01,
  SOCKS5_ATYP_DOMAIN = 0x03,
  SOCKS5_ATYP_IPV6 = 0x04
};

enum SOCKS5_CMD {
  SOCKS5_CMD_CONNECT = 0x01,
  SOCKS5_CMD_BIND = 0x02,
  SOCKS5_CMD_UDP_ASSOCIATE = 0x03
};

struct SockS5IdentRequest {
  unsigned char ver;
  unsigned char nmethods;
  unsigned char methods[255];
};

struct SockS5IdentReply {
  unsigned char ver;
  unsigned char method;
};

struct socks5_domain {
  unsigned char len;
  char name[256];
};

struct SockS5Request {
  unsigned char ver;
  unsigned char cmd;
  unsigned char rsv;
  unsigned char atyp;
  struct {
    union {
      struct in_addr ipv4;
      struct in6_addr ipv6;
      struct socks5_domain domain;
    } addr;
    unsigned short port;
  } dst;
};

struct socks5_reply {
  unsigned char ver;
  unsigned char rep;
  unsigned char rsv;
  unsigned char atyp;
  struct {
    union {
      struct in_addr IPv4;
      struct in6_addr IPv6;
      struct socks5_domain domain;
    } addr;
    unsigned short port;
  } bnd;
};

#endif  // SOCKS5_INCLUDED
