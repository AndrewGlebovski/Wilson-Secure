# Wilson-Secure

## Description

TCP client/server application. Now with TLS encryption.

## Author

Абишев Тимофей, Б05-232.

## Usage

Сначала нужно запускать сервер, потом клиент. Клиент и сервер обмениваются произвольным набором байт, после чего общение завершается. В результате работы создаются файлы ClientKeylog и ServerKeylog.

### Server

```sh
./Server port certificate-file private-key-file message-size iterations
```

### Client

```sh
./Client server-ip server-port message-size iterations
```
