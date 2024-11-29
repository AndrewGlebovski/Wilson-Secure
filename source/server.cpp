/**
 * \file
 * \brief Contains Server implementation.
*/

// ============================================================================

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include <memory.h>       // memcpy

#include <common.hpp>

// ============================================================================

void ServerKeylogCallback(const SSL* /* ignored */, const char* line) {
  FILE* fp = fopen("ServerKeylog", "a");
  ASSERT(fp != NULL, "Failed to open log file\n");
  
  fprintf(fp, "%s\n", line);
  
  fclose(fp);
}

// ============================================================================

class Server {
 public:
  Server(const char* port) {
    bio = BIO_new_accept(port);
    ASSERT(BIO_do_accept(bio), "Failed to create server BIO\n");

    printf("Setup server BIO\n");
  }

  void SetupSSL(const char* cert_file, const char* key_file) {
    ctx = SSL_CTX_new(TLS_method());
    ASSERT(ctx != nullptr, "Failed to create SSL context\n");

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_options(ctx, SSL_OP_ALL);
    SSL_CTX_set_default_verify_paths(ctx);

    SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM);
    ASSERT(SSL_CTX_check_private_key(ctx) == 1, "Failed to set private ket\n");

    FILE* fp = fopen("ServerKeylog", "w");
    ASSERT(fp != NULL, "Failed to create log file\n");
    fclose(fp);

    SSL_CTX_set_keylog_callback(ctx, ServerKeylogCallback);

    ssl = SSL_new(ctx);
    SSL_set_accept_state(ssl);
    SSL_set_bio(ssl, bio, bio);
  }

  void Accept() {
    ASSERT(SSL_accept(ssl) == 1, "Failed to accept connection\n");

    printf("Accepted client\n");
  }

  void Send(const char* msg, int msg_size) {
    printf("Sending message...\n");

    while (msg_size > 0) {
      int pkg_size = msg_size > 4096 ? 4096 : msg_size;

      ASSERT(SSL_write(ssl, msg, pkg_size) == pkg_size, "Failed to send msg\n");

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

      read_bytes = SSL_read(ssl, buffer, 4096);

      if (read_bytes <= 0) {
        break;
      }

      memcpy(msg, buffer, read_bytes);

      msg += read_bytes;
      msg_size -= read_bytes;
    };

    printf("Message received\n");
  }

  ~Server() {
    SSL_free(ssl);
    SSL_CTX_free(ctx);
  }

 private:
  BIO* bio;
  SSL_CTX *ctx;
  SSL *ssl;
};

// ============================================================================

int main(int argc, char* argv[]) {
  if (argc != 6) {
    printf("Usage: port certificate-file private-key-file msg-size iters\n");
    return 1;
  }

  OPENSSL_init_ssl(0, NULL);

  Server server(argv[1]);

  server.SetupSSL(argv[2], argv[3]);

  server.Accept();

  int msg_size = atoi(argv[4]);
  int iters = atoi(argv[5]);

  char* msg_buffer = new char[msg_size];

  for (; iters > 0; iters--) {
    server.Receive(msg_buffer, msg_size);
    server.Send(msg_buffer, msg_size);
  }

  delete[] msg_buffer;

  return 0;
}
