/*******************************************************************************
* @file    non_blocking_client.c
* @brief   
* @date    2026-03-31 06:59
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        close(client);
        return 1;
    }

    unsigned long ul1 = 1;
    ioctl(client, FIONBIO, &ul1);
    unsigned long ul2 = 1;
    ioctl(STDIN_FILENO, FIONBIO, &ul1);

    // Client is now connected to the server
    printf("Connected to server on port 8080...\n");

    // Client vua nhan du lieu tu server, vua nhan du lieu tu ban phim
    char buf[256];
    int len;
    while (1) {
        // Nhan du lieu tu ban phim
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            // Xay ra loi
            if (errno == EWOULDBLOCK) {
                // Dang cho du lieu
            } else {
                // Loi khac
                break;
            }
        } else {
            // Co du lieu, gui sang server
            send(client, buf, strlen(buf), 0);
            if (strcmp(buf, "exit\n") == 0)
                break;
        }

        // Nhan du lieu tu server
        len = recv(client, buf, sizeof(buf), 0);
        if (len == -1) {
            if (errno == EWOULDBLOCK) {
                // Dang cho du lieu tu server, bo qua xu ly
            } else {
                break;
            }
        } else {
            if (len == 0)
                break;
            buf[len] = 0;
            printf("Received: %s\n", buf);
        }
    }

    close(client);
    return 0;
}