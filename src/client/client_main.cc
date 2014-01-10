#include <sys/socket.h>
#include <iostream>

#include "../util/net/socket.h"

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
  std::string request = "GET / HTTP/1.0\nHost: www.google.com\n\n";
  int status = send(sock.get_sock(), request.c_str(), request.size(), 0);

  // Receive reply.
  const int buff_size = 4096;
  char recv_buff[buff_size + 1]; // One larger for termination if buffer is full.
  std::string recv_msg;

  while (true) {
    status = recv(sock.get_sock(), recv_buff, buff_size, 0);

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
  sock.Close();

  exit(EXIT_SUCCESS);
}
