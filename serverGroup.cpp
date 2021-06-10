#include "serverGroup.hpp"

using namespace std;

string group_name;

int server_socket;
int router_socket;
string multicast_ip;

void send_and_recv(int server_socket, char* buf){
    if(send(server_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");

    if(recv(server_socket, buf, TCP_BUFSIZE, 0) < 0){
        printf("Recv Err on : %d\n", server_socket);
        exit(1);
    }
}


void set_ip_multicast_handler(){
    char buf[TCP_BUFSIZE];
    string group_ip, code = IP_NAME_TO_SERVER;
    cin >> group_ip;
    multicast_ip = group_ip;
    sprintf(buf, (code + ":" + group_ip + "::" + group_name).c_str());
    if(send(server_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
    cout << "Data sent to server in format " << string(buf) << endl;

}

void run_dvmrp(){
    char buf[TCP_BUFSIZE];
    int dist_from_root = 0;
    sprintf(buf, "%d::%d::%s", CONFIG_TREE, dist_from_root, multicast_ip.c_str());
    if(send(router_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
    cout << "Message " << string(buf) << " sent\n";
}

void send_multicast_msg_handler(){
    char buf[TCP_BUFSIZE];
    string message;
    cin >> message;
    sprintf(buf, "%d::%s %s", MULTICAST_MSG, multicast_ip.c_str(), message.c_str());
    if(send(router_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
    cout << "Message " << string(buf) << " sent\n";
}

void* command_handler(void*){
    string command;
    while(cin >> command){
        if(command == SET_IP_MULTICAST){
            set_ip_multicast_handler();
        }else if(command == RUN_DVMRP){
            run_dvmrp();
        }else if(command == SEND_MULTICAST_MSG){
            send_multicast_msg_handler();
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

void setup_select(fd_set *readfds, int *max_fd){
    FD_ZERO(readfds);
    FD_SET(server_socket, readfds); 
    FD_SET(router_socket, readfds); 
    
    (*max_fd) = server_socket > router_socket ? server_socket : router_socket; 
}

void disconnect(int sock){
    close(sock);
    printf("%d got disconnected\n", sock);
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
        //
    }
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
    int port_server, port_router;
    sscanf(argv[1], "%d", &port_server);
    sscanf(argv[2], "%d", &port_router);
    group_name = string(argv[3]);
    connect_server(port_server, server_socket);
    connect_server(port_router, router_socket);
    
    pthread_t thread;

    if (pthread_create(&thread, NULL, command_handler, (void*)NULL)){
        printf("switch:ERROR; return code from pthread_create()\n");
        exit(-1);
    }
    
    handle_incomming_from_tcp();
    

    
}