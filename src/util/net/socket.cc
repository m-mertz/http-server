#include "util/net/socket.h"

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Socket::Socket() {}

Socket::~Socket() {
  // Safety measure: when socket object is destroyed, the contained socket is
  // closed.
  if (IsValid())
    Close();
}

bool Socket::Connect(const std::string &host, const unsigned int &port) {
  if (IsValid())
    Close(); // Close currently open socket before opening a new one.

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

bool Socket::Bind(const unsigned short &port) {
  if (IsValid())
    Close(); // Close currently open socket before opening a new one.

  // Create socket, check for failure.
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1) {
    perror("Socket::Bind: creating the socket failed");
    return false;
  }

  // Create configuration struct for bind call.
  sockaddr_in my_addr;
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Allow any IP address.
  my_addr.sin_port = htons(port);

  // Bind to selected port, check for failure.
  int status = bind(sock, reinterpret_cast<sockaddr *>(&my_addr), sizeof(my_addr));
  if (status == -1) {
    perror("Socket::Bind: bind call failed");
    close(sock); // Close the new socket before returning.
    return false;
  }

  // Everything ok, store socket file descriptor and signal success.
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

bool Socket::Send(const std::string msg) const {
  if (!IsValid())
    return false;

  // FIXME: This assumes that either the whole message is sent in one step or
  // the function fails. send returns the number of bytes sent and should be
  // evaluated, possibly we need multiple sends to transfer the whole message.
  int status = send(sock_, msg.c_str(), msg.size(), 0);
  if (status == -1) {
    perror("Socket::Send failed");
    return false;
  }

  return true;
}

std::string *Socket::Recv() const {
  if (!IsValid())
    return nullptr;

  // Buffer size + 1 for termination of full buffer.
  char *buff = new char[buff_size_ + 1];
  std::string *msg = new std::string();

  int status;
  while (true) {
    status = recv(sock_, buff, buff_size_, 0);

    // status contains the number of bytes received if everything went well.
    if (status > 0) {
      // Null-terminate the received string in the buffer and append to message.
      buff[status] = '\0';
      *msg = *msg + buff;
    }
    else {
      // If there was an error, report it, else we received 0 bytes and thus are
      // done with this transmission.
      if (status == -1)
        perror("Recv failed");
      break;
    }
  }
  delete[] buff;

  // If there was an error during recv, we signal that by returning a nullptr.
  // The part of the message that was possibly already received is then lost.
  // Since it is incomplete, that should be ok. But it is a design decision,
  // change this behaviour if you want, only if you return a message on error
  // then how will the error be signaled to the caller?
  if (status == -1) {
    delete msg;
    return nullptr;
  }

  // Everything went well, return message.
  return msg;
}
