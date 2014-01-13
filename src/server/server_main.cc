#include <iostream>

#include "util/net/socket.h"

int main(void) {
  const unsigned short my_port = 2014;
  const unsigned short backlog_size = 10;

  // Create socket, bind and listen to port.
  Socket sock;
  sock.Bind(my_port, true);
  sock.Listen(backlog_size);

  while (true) {
    // Get socket for next incoming connection. This call is blocking.
    Socket *new_sock = sock.Accept();
    if (new_sock == nullptr) {
      std::cout << "Socket::Accept did not return a connection, exiting"
                << std::endl;
      break;
    }

    // Print info about new connection.
    std::cout << "Got a new connection!" << std::endl;

    // Send something to caller.

    // Cleanup new socket.
    delete new_sock;
  }

  exit(EXIT_SUCCESS);
}
