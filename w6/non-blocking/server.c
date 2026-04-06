#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 9000
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

typedef struct
{
    int socket;
    int step;
    char name[100];
    char mssv[50];
} Client;

void set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    Client clients[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].socket = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    set_nonblocking(server_fd);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd,
         (struct sockaddr *)&address,
         sizeof(address));

    listen(server_fd, 10);

    printf("Server started on port %d\n", PORT);

    fd_set readfds;

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(server_fd, &readfds);
        int max_sd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = clients[i].socket;

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        select(max_sd + 1,
               &readfds,
               NULL,
               NULL,
               NULL);

        if (FD_ISSET(server_fd, &readfds))
        {
            new_socket = accept(
                server_fd,
                (struct sockaddr *)&address,
                (socklen_t *)&addrlen);

            set_nonblocking(new_socket);

            printf("New client connected\n");

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].socket == 0)
                {
                    clients[i].socket = new_socket;
                    clients[i].step = 0;

                    send(new_socket,
                         "Nhap ho ten:\n",
                         14,
                         0);

                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = clients[i].socket;

            if (FD_ISSET(sd, &readfds))
            {
                char buffer[BUFFER_SIZE];
                int valread =
                    recv(sd,
                         buffer,
                         BUFFER_SIZE,
                         0);

                if (valread <= 0)
                {
                    close(sd);
                    clients[i].socket = 0;
                    printf("Client disconnected\n");
                }
                else
                {
                    buffer[valread] = '\0';

                    if (clients[i].step == 0)
                    {
                        strcpy(
                            clients[i].name,
                            buffer);

                        send(sd,
                             "Nhap MSSV:\n",
                             12,
                             0);

                        clients[i].step = 1;
                    }
                    else if (clients[i].step == 1)
                    {
                        strcpy(
                            clients[i].mssv,
                            buffer);

                        char email[200];

                        sprintf(
                            email,
                            "Email cua ban: %s@sis.hust.edu.vn\n",
                            clients[i].mssv);

                        send(sd,
                             email,
                             strlen(email),
                             0);

                        clients[i].step = 2;
                    }
                }
            }
        }
    }

    return 0;
}