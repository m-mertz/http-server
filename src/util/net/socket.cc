#include "socket.h"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Socket::Socket() {}

bool Socket::Connect(const std::string host, const unsigned int port) {
  // Set options for address resolution.
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve host, clean up and return if that fails.
  addrinfo *res_addrinfo = nullptr;
  int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints,
                           &res_addrinfo);
  if (status != 0) {
    std::cerr << "Socket::Connect: getaddrinfo failed: " << gai_strerror(status)
              << std::endl;
    // Release possibly allocated resources before returning.
    if (res_addrinfo != nullptr)
      freeaddrinfo(res_addrinfo);

    return false;
  }

  // Try to create socket, with each addrinfo in turn, until it succeeds
  // or we tried all.
  int sock;
  addrinfo *rp;
  for (rp = res_addrinfo; rp != nullptr; rp = rp->ai_next) {
    // Try to create socket, continue to next addrinfo if that fails.
    sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sock == -1)
      continue;

    // Try to connect to current address, break on success, else close socket
    // and try next addrinfo in next iteration.
    if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
      break;

    close(sock);
  }

  freeaddrinfo(res_addrinfo); // Clean up, no longer needed.

  // If rp reaches nullptr in the loop above, no connection could be established.
  if (rp == nullptr) {
    std::cerr << "Could not connect" << std::endl;
    return false;
  }

  // Connection has been made, store socket file descriptor and return true.
  sock_ = sock;
  return true;
}

void Socket::Close() {
  close(sock_);
  sock_ = -1; // Set file descriptor back to invalid value to mark closed state.
}

bool Socket::IsValid() const {
  // File descriptors must be non-negative, so negative values mean invalid.
  // Is 0 a valid file descriptor? Don't know, don't care, as long as it is
  // not used as an error code.
  return (sock_ >= 0);
}

int Socket::get_sock() const {
  return sock_;
}
