#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s port_s ip_d port_d\n", argv[0]);
        return 1;
    }

    int port_s = atoi(argv[1]);
    char *ip_d = argv[2];
    int port_d = atoi(argv[3]);

    int sockfd;
    struct sockaddr_in local_addr, dest_addr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // Set non-blocking
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // Local address
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port_s);

    // Bind
    if (bind(sockfd, (struct sockaddr *)&local_addr,
             sizeof(local_addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // Destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port_d);

    if (inet_pton(AF_INET, ip_d,
                  &dest_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 1;
    }

    printf("UDP Chat started\n");
    printf("Listening on port: %d\n", port_s);
    printf("Sending to: %s:%d\n", ip_d, port_d);
    printf("Type message (exit to quit)\n");

    fd_set readfds;

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = sockfd > STDIN_FILENO ?
                    sockfd : STDIN_FILENO;

        int activity = select(
            maxfd + 1,
            &readfds,
            NULL,
            NULL,
            NULL);

        if (activity < 0)
        {
            perror("select");
            break;
        }

        // Receive data
        if (FD_ISSET(sockfd, &readfds))
        {
            struct sockaddr_in sender;
            socklen_t sender_len =
                sizeof(sender);

            int len = recvfrom(
                sockfd,
                buffer,
                BUFFER_SIZE - 1,
                0,
                (struct sockaddr *)&sender,
                &sender_len);

            if (len > 0)
            {
                buffer[len] = '\0';

                printf("\nReceived: %s\n",
                       buffer);
                printf("> ");
                fflush(stdout);
            }
        }

        // Send data
        if (FD_ISSET(STDIN_FILENO,
                     &readfds))
        {
            if (fgets(buffer,
                      BUFFER_SIZE,
                      stdin) != NULL)
            {
                buffer[strcspn(
                    buffer,
                    "\n")] = '\0';

                if (strcmp(buffer,
                           "exit") == 0)
                {
                    printf("Exiting...\n");
                    break;
                }

                sendto(
                    sockfd,
                    buffer,
                    strlen(buffer),
                    0,
                    (struct sockaddr *)
                        &dest_addr,
                    sizeof(dest_addr));
            }
        }
    }

    close(sockfd);
    return 0;
}