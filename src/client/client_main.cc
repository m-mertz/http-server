#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

int main(void) {
  // Set options for address resolution.
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  struct addrinfo *res_addrinfo;
  int status = getaddrinfo("www.google.com", "80", &hints, &res_addrinfo);
  if (status != 0) {
    std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
    exit(EXIT_FAILURE);
  }

  int sock;
  struct addrinfo *rp;
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

  if (rp == nullptr) {
    std::cerr << "Could not connect" << std::endl;
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(res_addrinfo);

  close(sock);

  exit(EXIT_SUCCESS);
}
