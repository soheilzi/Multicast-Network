#include "server.hpp"
// #include "io.hpp"
using namespace std;

string multicast_groups_list = "";

void setup_listener(int port, int *listen_socket){
    struct sockaddr_in serv_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char str[10];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if((*listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Socket faild\n");
        exit(1);
    }
    
    if(bind(*listen_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("bind faild\n");
        close(*listen_socket);
        exit(1);
    }

    if(listen(*listen_socket, 5) < 0){
        printf("listen faild\n");
        close(*listen_socket);
        exit(1);
    }
    printf("Listening on port %d\n", port);
}

void close_all(struct Client* clients, int count_clients){
    int i;
    for(i = 0;i < count_clients - 1; i++)
        close(clients[i].sock);
}

void add_client(struct Client* clients, int* count_clients, int listen_socket){
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr), clientid, data_sock;
    char str[10];
    if((clientid = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0){
        printf("accepting error\n");
        close_all(clients, *count_clients);
        close(listen_socket);
        exit(1);
    }

    (*count_clients)++;

    clients[(*count_clients) - 1].sock = clientid;
    
    printf("Client with socket %d added\n", clientid);
}

void setup_select(fd_set *readfds, int *max_fd, int listen_socket, struct Client *clients, int count_clients){
    int i;
    FD_ZERO(readfds);
    FD_SET(listen_socket, readfds); 
    (*max_fd) = listen_socket; 
    for(i = 0; i < count_clients; i++){
        FD_SET(clients[i].sock, readfds);
        if ((*max_fd) < clients[i].sock)
            (*max_fd) = clients[i].sock;
    }
    return;
}

void disconnect(struct Client* clients, int i, int* count_clients){
    int j, temp = clients[i].sock;
    char str[10];
    close(clients[i].sock);
    close(clients[i].dataSock);
    clients[i] = clients[(*count_clients) - 1];
    (*count_clients)--;
    printf("%d got disconnected\n", temp);
}

void send_and_recv(int server_socket, char* buf){
    if(send(server_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");

    if(recv(server_socket, buf, TCP_BUFSIZE, 0) < 0){
        printf("Recv Err on : %d\n", server_socket);
        exit(1);
    }
}


void got_ip_name_handler(char* buf){
    string new_data(buf);
    multicast_groups_list = multicast_groups_list + new_data + "\n";
}

void send_group_list_handler(struct Client* clients, int i, char* buf){
    char buf_groups[TCP_BUFSIZE];
    sprintf(buf_groups, "%s", multicast_groups_list.c_str());
    if(send(clients[i].sock, buf_groups, TCP_BUFSIZE, 0) < 0){
        printf("Faild sending to:%d\n", clients[i].sock);
        exit(1);
    }
}

void client_handler(struct Client* clients, int i, int* count_clients){
    int stat;
    char buf[TCP_BUFSIZE];
    char str[10];
    if((stat = recv(clients[i].sock, buf, TCP_BUFSIZE, 0)) < 0){
        printf("Recv Err on : %d\n", clients[i].sock);
        exit(1);
    }else if(stat == 0){
        disconnect(clients, i, count_clients);
        return;
    }
    cout << "got :" << string(buf) << endl;
    switch ((buf[0] - '0') * 10 + (buf[1] - '0')){
        case IP_NAME_TO_SERVER:
            got_ip_name_handler(buf);
            break;
        case SEND_GROUP_LIST:
            cout << multicast_groups_list << "heh\n";
            send_group_list_handler(clients, i, buf);
    }
    
}   



int main(int argc, char** argv){
    int listen_socket, max_fd = 0, port, count_clients = 0;
    struct Client clients[15];
    int i;
    sscanf(argv[1], "%d", &port);

    setup_listener(port, &listen_socket);

    fd_set readfds;
    
    while(1){
        setup_select(&readfds, &max_fd, listen_socket, clients, count_clients);
        if(select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            printf("Select fail\n");
            close_all(clients, count_clients);
            close(listen_socket);
            exit(1);
        }
        if(FD_ISSET(listen_socket, &readfds)){
            add_client(clients, &count_clients, listen_socket);
        }else{
            for(i = 0; i < count_clients; i++){
                if(FD_ISSET(clients[i].sock, &readfds)){
                    client_handler(clients, i, &count_clients);
                    break;
                }
            }
        }
    }

    close(listen_socket);
}
