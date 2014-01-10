#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "../util/net/socket.h"

int main(void) {
  // Set options for address resolution.
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  addrinfo *res_addrinfo;
  int status = getaddrinfo("www.google.com", "80", &hints, &res_addrinfo);
  if (status != 0) {
    std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
    exit(EXIT_FAILURE);
  }

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

  if (rp == nullptr) {
    std::cerr << "Could not connect" << std::endl;
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(res_addrinfo); // No longer needed, connection has been made.

  // Connection has been established, go do stuff!

  // Send HTTP request to server.
  // HTTP <1.1 has the server close the connection as soon as all data has been
  // sent, so it is easier to detect the end of message. If you choose HTTP/1.1
  // here, you must also adapt the reception process or this program will not
  // terminate!
  std::string request = "GET / HTTP/1.0\nHost: www.google.com\n\n";
  status = send(sock, request.c_str(), request.size(), 0);

  // Receive reply.
  int const buff_size = 4096;
  char recv_buff[buff_size + 1]; // One larger for termination if buffer is full.
  std::string recv_msg;

  while (true) {
    status = recv(sock, recv_buff, buff_size, 0);

    // status contains the number of bytes received if everything went well.
    if (status > 0) {
      // Null-terminate the received string in the buffer and append to message.
      recv_buff[status] = '\0';
      recv_msg = recv_msg + recv_buff;
    }
    else {
      // If there was an error, report it, else we received 0 bytes and thus are
      // done with this transmission.
      if (status == -1)
        perror("Recv failed");
      break;
    }
  }

  std::cout << recv_msg << std::endl;

  // BEGIN cleanup and exit.
  close(sock);

  exit(EXIT_SUCCESS);
}
