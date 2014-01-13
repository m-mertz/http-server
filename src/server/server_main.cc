#include <iostream>

#include "util/net/socket.h"

int main(void) {
  const unsigned short my_port = 2014;
  const unsigned short backlog_size = 10;

  // Create socket, bind and listen to port.
  Socket sock;
  if (!sock.Bind(my_port, true))
    exit(EXIT_FAILURE);

  if (!sock.Listen(backlog_size))
    exit(EXIT_FAILURE);

  std::cout << "Listening on port " << my_port << std::endl;

  while (true) {
    // Get socket for next incoming connection. This call is blocking.
    Socket *new_sock = sock.Accept();
    if (new_sock == nullptr) {
      std::cout << "Socket::Accept did not return a connection, exiting"
                << std::endl;
      break;
    }

    // Print info about new connection.
    std::string *hostname = new_sock->GetPeerName();
    if (hostname == nullptr) {
      std::cout << "New connection, but cannot determine the host.."
                << std::endl;
    } else {
      std::cout << "New connection from " << *hostname << std::endl;
      delete hostname;
    }

    // Send something to caller.
    if (!new_sock->Send("Hello, this is a server!\n"))
      std::cerr << "Could not send message to client" << std::endl;

    // Cleanup new socket.
    delete new_sock;
  }

  exit(EXIT_SUCCESS);
}
