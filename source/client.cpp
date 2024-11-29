/**
 * \file
 * \brief Contains Client implementation.
*/

// ============================================================================

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <memory.h>       // memcpy, memcmp

#include <common.hpp>

// ============================================================================

class Client {
 public:
  Client(const char* server) {
    server_bio = BIO_new_connect(server);
    ASSERT(server_bio != nullptr, "Failed to create server BIO\n");
  }

  void Connect() {
    ASSERT(BIO_do_connect(server_bio) == 1, "Error connecting to server\n");
    printf("Connected to server\n");
  }

  void Send(const char* msg, int msg_size) {
    printf("Sending message...\n");

    while (msg_size > 0) {
      int pkg_size = msg_size > 4096 ? 4096 : msg_size;

      ASSERT(BIO_write(server_bio, msg, pkg_size) == pkg_size, "Failed to send msg\n");

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

      read_bytes = BIO_read(server_bio, buffer, 4096);

      if (read_bytes <= 0) {
        break;
      }

      memcpy(msg, buffer, read_bytes);

      msg += read_bytes;
      msg_size -= read_bytes;
    };

    printf("Message received\n");
  }

  ~Client() {
    BIO_free(server_bio);
  }

 private:
  BIO* server_bio;
};

// ============================================================================

int main(int argc, char* argv[]) {
  if (argc != 5) {
    printf("Usage: server-ip server-port msg-size iters\n");
    return 1;
  }

  char buffer[sizeof("255.255.255.255:65000")];
  sprintf(buffer, "%s:%s", argv[1], argv[2]);

  Client client(buffer);

  client.Connect();

  int msg_size = atoi(argv[3]);
  int iters = atoi(argv[4]);

  char* original_msg = new char[msg_size];
  char* received_msg = new char[msg_size];

  for (; iters > 0; iters--) {
    client.Send(original_msg, msg_size);
    client.Receive(received_msg, msg_size);

    ASSERT(memcmp(original_msg, received_msg, msg_size) == 0, "Msg corrupted\n");
  }

  delete[] original_msg;
  delete[] received_msg;

  return 0;
}
