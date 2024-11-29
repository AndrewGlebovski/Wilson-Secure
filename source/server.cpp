/**
 * \file
 * \brief Contains Server implementation.
*/

// ============================================================================

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include <cstdio>         // printf
#include <cstdlib>        // exit
#include <memory.h>       // memcpy

#include <common.hpp>

// ============================================================================

class Server {
 public:
  Server(const char* port) {
    bio = BIO_new_accept(port);
    ASSERT(BIO_do_accept(bio), "Failed to create server BIO\n");

    printf("Setup server BIO\n");
  }

  void Accept() {
    ASSERT(BIO_do_accept(bio) == 1, "Failed to accept connection\n");
    client_bio = BIO_pop(bio);

    printf("Accepted client\n");
  }

  void Send(const char* msg, int msg_size) {
    printf("Sending message...\n");

    while (msg_size > 0) {
      int pkg_size = msg_size > 4096 ? 4096 : msg_size;

      ASSERT(BIO_write(client_bio, msg, pkg_size) == pkg_size, "Failed to send msg\n");

      msg += pkg_size;
      msg_size -= pkg_size;
    }

    printf("Message sent\n");
  }

  void Receive(char* msg, int msg_size) {
    printf("Receiving message...\n");

    int read_bytes = 0;

    while (msg_size > 0) {
      char buffer[4096];

      read_bytes = BIO_read(client_bio, buffer, 4096);

      if (read_bytes <= 0) {
        break;
      }

      memcpy(msg, buffer, read_bytes);

      msg += read_bytes;
      msg_size -= read_bytes;
    };

    printf("Message received\n");
  }

  void Disconnect() {
    BIO_free(client_bio);
    client_bio = nullptr;
    
    printf("Disconnected from client\n");
  }

  ~Server() {
    BIO_free(bio);
  }

 private:
  BIO* bio;
  BIO* client_bio;
};

// ============================================================================

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Usage: port msg-size iters\n");
    return 1;
  }

  Server server(argv[1]);

  server.Accept();

  int msg_size = atoi(argv[2]);
  int iters = atoi(argv[3]);

  char* msg_buffer = new char[msg_size];

  for (; iters > 0; iters--) {
    server.Receive(msg_buffer, msg_size);
    server.Send(msg_buffer, msg_size);
  }

  server.Disconnect();

  delete[] msg_buffer;

  return 0;
}
