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

 private:
  // Disallow copy and assign.
  Socket(const Socket&);
  void operator=(const Socket&);

  int sock_ = -1; // File descriptor of contained socket, -1 means invalid.
};

#endif // UTIL_NET_SOCKET_H
