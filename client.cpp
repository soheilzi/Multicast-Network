#include "client.hpp"

using namespace std;

int server_socket, router_socket;


void send_and_recv(int server_socket, char* buf){
    if(send(server_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");

    if(recv(server_socket, buf, TCP_BUFSIZE, 0) < 0){
        printf("Recv Err on : %d\n", server_socket);
        exit(1);
    }
}

void disconnect(int sock){
    close(sock);
    printf("%d got disconnected\n", sock);
}


void get_group_list_handler(int server_socket){
    char buf[TCP_BUFSIZE]; 
    string code(GET_GROUP_LIST_COMMAND_CODE);
    sprintf(buf, (code + ":" + "nothingmore").c_str());
    send_and_recv(server_socket, buf);
    printf("%s\n", buf);

}

void join_group_handler(){
    char buf[TCP_BUFSIZE];
    string message;
    cin >> message;
    sprintf(buf, "%d::%s", CONFIG_JOIN, message.c_str());
    if(send(router_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void* command_handler(void*){
    string command;
    while(cin >> command){
        if(command == GET_GROUP_LIST){
            get_group_list_handler(server_socket);
        }else if(command == JOIN_GROUP){
            join_group_handler();
        }
    }
}

void connect_server(int port_server, int &server_socket){
    // Creat sockets
    int bsocket, team_count, stat;
    struct sockaddr_in serv_addr, b_addr, bs_addr;

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Socke faild\n");
    }
    
 
    // Connect sockets

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_server);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Connection faild\n");
        close(server_socket);
        exit(1);
    }

    printf("connected to server/router with socket %d\n", server_socket);
}

void server_handler(){
    int stat, code;
    char buf[TCP_BUFSIZE];
    char buf_rest[TCP_BUFSIZE];
    char buf_temp[TCP_BUFSIZE];
    if((stat = recv(server_socket, buf, TCP_BUFSIZE, 0)) < 0){
        printf("Recv Err on : %d\n", server_socket);
        exit(1);
    }else if(stat == 0){
        disconnect(server_socket);
        return;
    }
    cout << "Got message " << string(buf) << endl;
    sscanf(buf, "%d::%s", &code, buf_rest);
    switch (code)
    {
        //
    }
}

void message_handler(char* buf){
    int code;
    char m_ip[TCP_BUFSIZE];
    char message[TCP_BUFSIZE];
    sscanf(buf, "%d::%s %s", &code, m_ip, message);
    cout << "Got message : " << string(message) << " from multicast ip : " << string(m_ip) << endl;
}

void send_cancel_parent(char* buf){
    int code, dist;
    char m_ip[TCP_BUFSIZE];
    char newbuf[TCP_BUFSIZE];
    sscanf(buf, "%d::%d::%s", &code, &dist, m_ip);
    sprintf(newbuf, "%d::%s", CONFIG_CANCEL_PARENT, m_ip);
    if(send(router_socket, newbuf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void router_handler(){
    int stat, code;
    char buf[TCP_BUFSIZE];
    char buf_rest[TCP_BUFSIZE];
    char buf_temp[TCP_BUFSIZE];
    if((stat = recv(router_socket, buf, TCP_BUFSIZE, 0)) < 0){
        printf("Recv Err on : %d\n", router_socket);
        exit(1);
    }else if(stat == 0){
        disconnect(router_socket);
        return;
    }
    cout << "Got message " << string(buf) << endl;
    sscanf(buf, "%d::%s", &code, buf_rest);
    switch (code)
    {
        case MULTICAST_MSG:
            message_handler(buf);
            break;
        case CONFIG_TREE:
            send_cancel_parent(buf);
            break;
    }
}

void setup_select(fd_set *readfds, int *max_fd){
    FD_ZERO(readfds);
    FD_SET(server_socket, readfds); 
    FD_SET(router_socket, readfds); 
    
    (*max_fd) = server_socket > router_socket ? server_socket : router_socket; 
}

void handle_incomming_from_tcp(){
    int max_fd;
    fd_set readfds;
    while(1){
        setup_select(&readfds, &max_fd);
        if(select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            printf("Select fail\n");
            exit(1);
        }
        if(FD_ISSET(server_socket, &readfds)){
            server_handler();
        }if(FD_ISSET(router_socket, &readfds)){
            router_handler();
        }
    }
}

int main(int argc, char** argv){
    int port_server;
    int port_router;
    sscanf(argv[1], "%d", &port_server);
    sscanf(argv[2], "%d", &port_router);
    
    connect_server(port_server, server_socket);
    connect_server(port_router, router_socket);

    pthread_t thread;

    if (pthread_create(&thread, NULL, command_handler, (void*)NULL)){
        printf("switch:ERROR; return code from pthread_create()\n");
        exit(-1);
    }
    
    handle_incomming_from_tcp();
    
}