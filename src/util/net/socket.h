#ifndef UTIL_NET_SOCKET_H
#define UTIL_NET_SOCKET_H

#include <string>

class Socket {
 public:
  Socket();
  bool Connect(const std::string host, const unsigned int port);
  void Close();
  bool IsValid() const;
  int get_sock() const;
  bool Send(const std::string msg) const;
  std::string *Recv() const;

 private:
  // Disallow copy and assign.
  Socket(const Socket&);
  void operator=(const Socket&);

  // Buffer size for recv operations. Must not be changed during a call of Recv,
  // so just make it const altogether.
  const int buff_size_ = 4096;
  // File descriptor of contained socket, -1 means invalid.
  int sock_ = -1;
};

#endif // UTIL_NET_SOCKET_H
