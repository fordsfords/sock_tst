/* win_close.c - program to accept a TCP connection and immediately
 * force-close it.
 * Thanks to Anthropic's claude.ai for assistance writing this code. */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "12000"

int main(int argc, char **argv) {
  WSADATA wsa_data;
  SOCKET listen_sock = INVALID_SOCKET;
  SOCKET accepted_sock;
  int status;
  struct addrinfo *addr_info = NULL, hints;
  LINGER linger = {1, 0};

  /* Initialize Winsock */
  status = WSAStartup(MAKEWORD(2,2), &wsa_data);
  if (status != 0) {
    printf("WSAStartup failed: %d\n", status);
    exit(1);
  }

  while (1) {
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    /* Resolve the local address and port to be used by the server */
    status = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addr_info);
    if (status != 0) {
      printf("getaddrinfo failed: %d\n", status);
      exit(1);
    }
    
    /* Create a SOCKET for the server to listen for client connections */
    listen_sock = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (listen_sock == INVALID_SOCKET) {
      printf("Error at socket(): %ld\n", WSAGetLastError());
      exit(1);
    }

    /* Setup the TCP listening socket */
    status = bind(listen_sock, addr_info->ai_addr, (int)addr_info->ai_addrlen);
    if (status == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      exit(1);
    }

    freeaddrinfo(addr_info);

    /* Start listening for connections */
    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
      printf("Listen failed with error: %ld\n", WSAGetLastError());
      exit(1);
    }

    printf("Waiting for a connection...\n");

    accepted_sock = accept(listen_sock, NULL, NULL);
    if (accepted_sock == INVALID_SOCKET) {
      printf("accept failed: %d\n", WSAGetLastError());
      exit(1);
    }

    /* Forcefully close the socket */
    setsockopt(accepted_sock, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
    closesocket(accepted_sock);

    /* Cleanup */
    closesocket(listen_sock);

    printf("Connection closed\n");
  }  /* while 1 */

  WSACleanup();
  return 0;
}  /* main */
