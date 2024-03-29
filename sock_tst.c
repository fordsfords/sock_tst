/* sock_tst.c */
/*
# This code and its documentation is Copyright 2002-2021 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/sock_tst
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cprt.h"

#define PORT 12000
#define BUF_SIZE (8*1024)

int main() {
  int sock_fd;
  struct sockaddr_in servaddr, cliaddr;
  char buffer[BUF_SIZE];
  fd_set read_fdset;
  int max_sd, num_socks;

  CPRT_EM1(sock_fd = socket(AF_INET, SOCK_DGRAM, 0));

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  CPRT_EM1(bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)));

  while (1) {
    FD_ZERO(&read_fdset);

    FD_SET(sock_fd, &read_fdset);
    max_sd = sock_fd;

close(sock_fd);
    num_socks = select(max_sd + 1, &read_fdset, NULL, NULL, NULL);

    if ((num_socks < 0) && (errno != EINTR)) {
      CPRT_PERRNO("select error");
      break;
    }

    if (FD_ISSET(sock_fd, &read_fdset)) {
      ssize_t rtn_len;
      socklen_t addr_len = sizeof(cliaddr);

      memset(buffer, 0, BUF_SIZE);
      rtn_len = recvfrom(sock_fd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &addr_len);
      printf("rtn_len=%ld\n", (long)rtn_len);
    }
  }  /* while */

  close(sock_fd);
  return 0;
}
