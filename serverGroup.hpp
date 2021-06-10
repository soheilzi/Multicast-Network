#ifndef __GARD_GROUP
#define __GARD_GROUP
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string>


#define SET_IP_MULTICAST "set_ip_multicast"
#define RUN_DVMRP "start_dvmrp"
#define SEND_MULTICAST_MSG "send"

#define CONFIG_TREE 11
#define MULTICAST_MSG 14

#define IP_NAME_TO_SERVER "00"

#define ERROR_OTHER "500: Error"

#define SUCCESS_NUMBER 226

#define MAX_INPUT_SIZE 512
#define TCP_BUFSIZE 100
#define UDP_BUFSIZE 100
#define MAX_DATA_MSG 200

#endif