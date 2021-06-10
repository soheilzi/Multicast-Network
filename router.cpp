#include "router.hpp"

using namespace std;

struct Client clients[15]; // port number is this array index
int count_clients = 0;
int selfpipe_fd;

map<string, vector<int>> children;
map<string, int> parent;
int dist = 100000;


void send_cost_config(int router_socket, int cost){
    char buf[TCP_BUFSIZE];
    sprintf(buf, "%d::%d", COST_CONFIG, cost);
    if(send(router_socket, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void connect_router_handler(){
    int server_socket, bsocket, team_count, stat, port_server, clientid, cost;
    cin >> port_server;
    cin >> cost;
    char temp[TCP_BUFSIZE];
    sprintf(temp, "wakeup");

    struct sockaddr_in serv_addr, b_addr, bs_addr;

    // Creat sockets

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

    count_clients++;
    clientid = server_socket;
    clients[count_clients - 1].sock = clientid;
    clients[count_clients - 1].cost = cost;
    
    if(write(selfpipe_fd, temp, TCP_BUFSIZE) < 0){
        cout << "selfpipe error\n";
    }

    printf("connected to router on port %d with link cost %d\n", count_clients - 1, cost);
    send_cost_config(server_socket, cost);
}

void change_cost_handler(){
    int port, newcost;
    cin >> port >> newcost;
    clients[port].cost = newcost;
    send_cost_config(clients[port].sock, newcost);
    cout << "cost of port " << port << " changed to " << newcost << endl;
}

void show_stat(){
    cout << "parents :\n";
    for(map<string, int>::iterator it = parent.begin(); it != parent.end(); it++){
        cout << "\tfor group ip " << it->first << "parent is " << it->second << endl;
    }
    cout << "children :\n";
    for(map<string, vector<int>>::iterator it = children.begin(); it != children.end(); it++){
        cout << "\tfor group ip " << it->first << "children are :" << endl;
        for(int i = 0; i < it->second.size(); i++){
            cout << "\t\tchild " << i << " is " << it->second[i] << endl;
        }
    }
}

void* command_line_handler(void* arg){
    string command;
    while(cin >> command){
        if(command == CONNECT_ROUTER){
            connect_router_handler();
        }else if(command == CHANGE_COST){
            change_cost_handler();
        }else if(command == COMMAND_SHOW){
            show_stat();
        }
    }
    return NULL;
}


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

void add_client(int listen_socket){
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr), clientid, data_sock;
    char str[10];
    if((clientid = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0){
        printf("accepting error\n");
        close_all(clients, count_clients);
        close(listen_socket);
        exit(1);
    }

    (count_clients)++;

    clients[(count_clients) - 1].sock = clientid;
    
    printf("Client with socket %d added on port %d\n", clientid, count_clients - 1);
}

void setup_select(fd_set *readfds, int *max_fd, int listen_socket, int selfpipe_fd){
    int i;
    FD_ZERO(readfds);
    FD_SET(listen_socket, readfds); 
    FD_SET(selfpipe_fd, readfds); 
    
    (*max_fd) = listen_socket > selfpipe_fd ? listen_socket : selfpipe_fd; 
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

void send_to_all(char* buf, string m_ip){
    for(int i = 0; i < count_clients; i++){
        if(i == parent[m_ip])
            continue;
        if(send(clients[i].sock, buf, TCP_BUFSIZE, 0) < 0)
            printf("send faild\n");
    }
}

void notify_parent(string m_ip, int old_parent){
    char buf[TCP_BUFSIZE];
    sprintf(buf, "%d::%s", CONFIG_CONFIRM_PARENT, m_ip.c_str());
    if(send(clients[parent[m_ip]].sock, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
    if(old_parent == -1 or parent[m_ip] == old_parent)
        return;
    sprintf(buf, "%d::%s", CONFIG_CANCEL_PARENT, m_ip.c_str());
    if(send(clients[old_parent].sock, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void config_tree_handler(char* buf, int i){
    int code, dist_from_root;
    char m_ip[TCP_BUFSIZE];
    sscanf(buf, "%d::%d::%s", &code, &dist_from_root, m_ip);
    string mip_str = string(m_ip);
    int old_parent;
    if(parent.find(mip_str) == parent.end())
        old_parent = -1;
    else
        old_parent = parent[mip_str];
    if(parent.find(mip_str) == parent.end() or dist > dist_from_root + clients[i].cost or parent[mip_str] == i){
        parent[mip_str] = i;
        dist = dist_from_root + clients[i].cost;
        cout << "found parent for " << mip_str << " on port " << i <<" dist is " << dist << endl;
        sprintf(buf, "%d::%d::%s", code, dist, m_ip);
        send_to_all(buf, mip_str);
        notify_parent(mip_str, old_parent);
    }
}

void join_group(char* mip){
    char buf[TCP_BUFSIZE];
    sprintf(buf, "%d::%s", CONFIG_JOIN, mip);
    if(send(clients[parent[string(mip)]].sock, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void add_child(char* mip, int port){
    if(children.find(string(mip)) == children.end()){
        children[string(mip)] = {};
        children[string(mip)].push_back(port);
    }else if(find(children[string(mip)].begin(), children[string(mip)].end(), port) == children[string(mip)].end()){
        children[string(mip)].push_back(port);
    }
}
void join_group_handler(char* buf, int port){
    int code;
    char mip[TCP_BUFSIZE];
    sscanf(buf, "%d::%s", &code, mip);
    add_child(mip, port);
    if(children[string(mip)].size() == 1){
        join_group(mip);
    }
}

void parent_config_handler(char* buf, int port){
    int code;
    char mip[TCP_BUFSIZE];
    sscanf(buf, "%d::%s", &code, mip);
    add_child(mip, port);
}

void send_cancel_parent(char* mip){
    char buf[TCP_BUFSIZE];
    sprintf(buf, "%d::%s", CONFIG_CANCEL_PARENT, mip);
    if(send(clients[parent[string(mip)]].sock, buf, TCP_BUFSIZE, 0) < 0)
        printf("send faild\n");
}

void cancel_parent_handler(char* buf, int port){
    int code;
    char mip[TCP_BUFSIZE];
    sscanf(buf, "%d::%s", &code, mip);
    if(children[string(mip)].size() != 0)
        children[string(mip)].erase(find(children[string(mip)].begin(), children[string(mip)].end(), port));
    if(children[string(mip)].size() == 0)
        send_cancel_parent(mip); 
}

void forward_multicast_msg_handler(char* buf, int port){
    int code;
    char mip[TCP_BUFSIZE];
    char temp[TCP_BUFSIZE];
    sscanf(buf, "%d::%s %s", &code, mip, temp);
    string mip_str = string(mip);
    for(int i = 0; i < children[mip_str].size(); i++){
        if(children[mip_str][i] == port)
            continue;
        if(send(clients[children[mip_str][i]].sock, buf, TCP_BUFSIZE, 0) < 0)
            printf("send faild\n");
    }
    if(parent[mip_str] != port){
        if(send(clients[parent[mip_str]].sock, buf, TCP_BUFSIZE, 0) < 0)
            printf("send faild\n");
    }
}

void client_handler(struct Client* clients, int i, int* count_clients){
    int stat, code;
    char buf[TCP_BUFSIZE];
    char buf_rest[TCP_BUFSIZE];
    char buf_temp[TCP_BUFSIZE];
    if((stat = recv(clients[i].sock, buf, TCP_BUFSIZE, 0)) < 0){
        printf("Recv Err on : %d\n", clients[i].sock);
        exit(1);
    }else if(stat == 0){
        disconnect(clients, i, count_clients);
        return;
    }
    cout << "Got message " << string(buf) << " from port " << i << endl;
    sscanf(buf, "%d::%s", &code, buf_rest);
    switch (code)
    {
    case COST_CONFIG:
        int cost;
        sscanf(buf, "%d::%d", &code, &cost);
        clients[i].cost = cost;
        printf("cost on port %d is set to %d\n", i, cost); 
        break;

    case CONFIG_TREE:
        config_tree_handler(buf, i);
        break;
    
    case CONFIG_CONFIRM_PARENT:
        parent_config_handler(buf, i);
        break; 

    case CONFIG_CANCEL_PARENT:
        cancel_parent_handler(buf, i);
        break; 

    case MULTICAST_MSG:
        forward_multicast_msg_handler(buf, i);
        break;

    case CONFIG_JOIN:
        join_group_handler(buf, i);
        break;

    default:
        cout << "unknown command\n";
        break;
    }
    
}   


int main(int argc, char** argv){
    pthread_t thread;

    if (pthread_create(&thread, NULL, command_line_handler, (void*)NULL)){
        printf("switch:ERROR; return code from pthread_create()\n");
        exit(-1);
    }

    int listen_socket, max_fd = 0, port;
    int i;
    sscanf(argv[1], "%d", &port);

    setup_listener(port, &listen_socket);

    fd_set readfds;
    char pipename[PIPESIZE];
    
    sprintf(pipename, "selfpipe_%d", getpid());

    mkfifo(pipename, 0666);
    selfpipe_fd = open(pipename, O_RDWR);

    while(1){
        setup_select(&readfds, &max_fd, listen_socket, selfpipe_fd);
        if(select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            printf("Select fail\n");
            close_all(clients, count_clients);
            close(listen_socket);
            exit(1);
        }
        if(FD_ISSET(listen_socket, &readfds)){
            add_client(listen_socket);
        }if(FD_ISSET(selfpipe_fd, &readfds)){
            char temp[TCP_BUFSIZE];
            if(read(selfpipe_fd, temp, TCP_BUFSIZE) < 0){
                cout << "self pipe error\n";
            }
            continue;
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