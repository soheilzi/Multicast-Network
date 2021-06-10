#ifndef __GARD_CLIENT
#define __GARD_CLIENT
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


#define GET_GROUP_LIST "get_group_list"
#define JOIN_GROUP "join"

#define GET_GROUP_LIST_COMMAND_CODE "01"


#define ERROR_OTHER "500: Error"

#define CONFIG_TREE 11
#define CONFIG_CONFIRM_PARENT 12
#define CONFIG_CANCEL_PARENT 13
#define MULTICAST_MSG 14
#define CONFIG_JOIN 15

#define SUCCESS_NUMBER 226

#define MAX_INPUT_SIZE 512
#define TCP_BUFSIZE 100
#define UDP_BUFSIZE 100
#define MAX_DATA_MSG 200

#endif