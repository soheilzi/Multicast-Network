#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <algorithm>

#define UDP_BUFSIZE 100
#define MSGSIZE 100
#define PIPESIZE 20
#define FRAME_SIZE 100

#define CONNECT 0
#define SEND 1
#define RECV 2

#define TCP_BUFSIZE 100
#define UDP_BUFSIZE 100

#define DUMMY_STRING "DUMMY"
#define DUMMY_INT 0

#define CONNECT_ROUTER "connect"
#define CHANGE_COST "change_cost"
#define COMMAND_SHOW "show"

#define NOT_CONNECTED_TO_SWITCH 0
#define CONNECTED_TO_SWITCH 1

#define CONNECT_COMMAND 1
#define SEND_COMMAND 2

#define COST_CONFIG 10
#define CONFIG_TREE 11
#define CONFIG_CONFIRM_PARENT 12
#define CONFIG_CANCEL_PARENT 13
#define MULTICAST_MSG 14
#define CONFIG_JOIN 15


#define CONFIGMSG -1
#define CONFIGPARENTNOTICE -2 
#define SEND_CONFIG_PERIOD 2
#define ROOT_TIMEOUT 10

void create_switch_handler(std::vector<int>& ids, int port_command);
void create_system_handler(std::vector<int>& ids, int port_command);
void connect_switch_handler(std::vector<int>& ids, int port_command);
void connect_system_handler(std::vector<int>& ids, int port_command);
void send_message_handler(std::vector<int>& ids, int port_command);

int main_system(int port_command, int id);
int main_switch(int port_command, int id, int count_port);

void setup_reading(int* bsocket, int bport, struct sockaddr_in* b_addr);

struct Client{
    int sock;
    int dataSock;
    int loggedin;
    int cost = 1;
    struct User* user;
};