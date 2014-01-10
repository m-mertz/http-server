#include <sys/socket.h>
#include <iostream>

#include "util/net/socket.h"

int main(void) {
  // Create socket and connect to host.
  Socket sock;
  if (!sock.Connect("www.google.com", 80)) {
    std::cerr << "Could not connect to host" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Connection has been established, go do stuff!

  // Send HTTP request to server.
  // HTTP <1.1 has the server close the connection as soon as all data has been
  // sent, so it is easier to detect the end of message. If you choose HTTP/1.1
  // here, you must also adapt the reception process or this program will not
  // terminate!
  sock.Send("GET / HTTP/1.0\nHost: www.google.com\n\n");

  // Receive and print reply from server.
  std::string *reply;
  if ((reply = sock.Recv()) != nullptr)
    std::cout << *reply << std::endl;
  delete reply;

  // BEGIN cleanup and exit.
  sock.Close();

  exit(EXIT_SUCCESS);
}
