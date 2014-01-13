#ifndef HTTPSERVER_UTIL_NET_SOCKET_H_
#define HTTPSERVER_UTIL_NET_SOCKET_H_

#include <string>

class Socket {
 public:
  Socket();
  ~Socket();
  bool Connect(const std::string &host, const unsigned int &port);
  bool Bind(const unsigned short &port, const bool &reuse_port);
  bool Listen(const unsigned short &backlog_size) const;
  Socket *Accept() const;
  void Close();
  bool IsValid() const;
  int get_sock() const;
  bool Send(const std::string msg) const;
  std::string *Recv() const;

 private:
  // Private constructor for creating a new socket object for an existing
  // socket file descriptor.
  Socket(const int &sock);
  // Disallow copy and assign.
  Socket(const Socket&);
  void operator=(const Socket&);

  // Buffer size for recv operations. Must not be changed during a call of Recv,
  // so just make it const altogether.
  const int buff_size_ = 4096;
  // File descriptor of contained socket, -1 means invalid.
  int sock_ = -1;
};

#endif // HTTPSERVER_UTIL_NET_SOCKET_H_
