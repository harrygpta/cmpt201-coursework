// ======== server.c (Fixed newline handling and compilation errors) ========
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define MAX_MSG_SIZE 1024

typedef struct {
  int sockfd;
  struct sockaddr_in addr;
  int done;
  char buffer[MAX_MSG_SIZE];
  int buffer_len;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
int expected_clients = 0;

void broadcast(uint8_t *msg, ssize_t len) {
  for (int i = 0; i < client_count; i++) {
    if (send(clients[i].sockfd, msg, len, 0) < 0) {
      perror("send");
    }
  }
}

void remove_client(int index) {
  close(clients[index].sockfd);
  for (int i = index; i < client_count - 1; i++) {
    clients[i] = clients[i + 1];
  }
  client_count--;
}

int all_clients_done() {
  if (client_count != expected_clients)
    return 0;
  for (int i = 0; i < client_count; i++) {
    if (!clients[i].done)
      return 0;
  }
  return 1;
}

void handle_message(int i, uint8_t *msg, ssize_t len) {
  if (msg[0] == 0 && len > 1) {
    uint8_t out[MAX_MSG_SIZE];
    out[0] = 0;
    uint32_t ip = clients[i].addr.sin_addr.s_addr;
    uint16_t port = clients[i].addr.sin_port;
    memcpy(out + 1, &ip, 4);
    memcpy(out + 5, &port, 2);

    size_t chat_len = len - 1;

    // Ensure message ends in '\n'
    if (msg[1 + chat_len - 1] != '\n') {
      msg[1 + chat_len] = '\n';
      chat_len++;
    }

    memcpy(out + 7, msg + 1, chat_len);
    broadcast(out, 7 + chat_len);

  } else if (msg[0] == 1) {
    clients[i].done = 1;
    if (all_clients_done()) {
      uint8_t end_msg[2] = {1, '\n'};
      broadcast(end_msg, 2);
      for (int j = 0; j < client_count; j++) {
        close(clients[j].sockfd);
      }
      exit(EXIT_SUCCESS);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: ./server <port> <# of clients>\n");
    exit(EXIT_FAILURE);
  }

  int port = atoi(argv[1]);
  expected_clients = atoi(argv[2]);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_addr = {.sin_family = AF_INET,
                                    .sin_addr.s_addr = INADDR_ANY,
                                    .sin_port = htons(port)};

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  listen(server_fd, expected_clients);

  fd_set read_fds;
  int max_fd = server_fd;

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    for (int i = 0; i < client_count; i++) {
      FD_SET(clients[i].sockfd, &read_fds);
      if (clients[i].sockfd > max_fd)
        max_fd = clients[i].sockfd;
    }

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(server_fd, &read_fds)) {
      struct sockaddr_in client_addr;
      socklen_t addrlen = sizeof(client_addr);
      int new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
      if (new_fd >= 0 && client_count < MAX_CLIENTS) {
        clients[client_count].sockfd = new_fd;
        clients[client_count].addr = client_addr;
        clients[client_count].done = 0;
        clients[client_count].buffer_len = 0;
        client_count++;
      }
    }

    for (int i = 0; i < client_count; i++) {
      if (FD_ISSET(clients[i].sockfd, &read_fds)) {
        ssize_t len =
            recv(clients[i].sockfd, clients[i].buffer + clients[i].buffer_len,
                 MAX_MSG_SIZE - clients[i].buffer_len, 0);
        if (len <= 0) {
          remove_client(i);
          i--;
          continue;
        }
        clients[i].buffer_len += len;

        int start = 0;
        for (int j = 0; j < clients[i].buffer_len; j++) {
          if (clients[i].buffer[j] == '\n') {
            int msg_len = j - start + 1;
            if (msg_len >= 1) {
              handle_message(i, (uint8_t *)(clients[i].buffer + start),
                             msg_len);
            }
            start = j + 1;
          }
        }

        if (start > 0) {
          memmove(clients[i].buffer, clients[i].buffer + start,
                  clients[i].buffer_len - start);
          clients[i].buffer_len -= start;
        }
      }
    }
  }

  return 0;
}
