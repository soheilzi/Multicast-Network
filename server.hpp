#ifndef __GARD_SERVER
#define __GARD_SERVER
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
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <sstream>
// #include <json/value.h>
// #include <jsoncpp>


#define MAX_TEAMS 20
#define STARTING_BROADCAST_PORT 8080
#define MAX_ADDRESS 100
#define MAX_USERNAME 100
#define MAX_FILE_NAME 100
#define MAX_PATH_SIZE 70
#define MAX_DATA_MSG 200
#define MAX_LINE_SIZE 50

#define MAX_INPUT_SIZE 512
#define TCP_BUFSIZE 100
#define UDP_BUFSIZE 100
#define MAX_DATA_MSG 200

#define IP_NAME_TO_SERVER 0
#define SEND_GROUP_LIST 1

#define USERNAME 0
#define PASSWORD 1 
#define PWD 2 // :S
#define DELFILE 3 // :D :)
#define DELDIR 4 // :D :)
#define MKDIR 5 // :D :)
#define LS 6 // :S -> DSz
#define CWD 7 // :D :)
#define RENAME 8 // :S
#define RETR 9 // DS
#define HELP 10 // DS
#define QUIT 11 //D


#define USER_COUNT 15
#define FILE_COUNT 15

#define BASEDIR "./baseDir/"

#define USERNAME_OK "331: User name okay need password."
#define USERNAME_NOT_OK "430: Invalid username of password."

#define PASSWORD_NOT_OK "430: Invalid username of password."
#define PASSWORD_OK "230: User logged in, proceed. Logged out if appropriate."
#define BAD_SEQUENCE_OF_COMMANDS "503: Bad sequence of commands."

#define NEED_ACCOUNT "332: Need account for login."
#define SYNTAX_ERROR "501: Syntax error in parameters of arguments."
#define ERROR_OTHER "500: Error"

#define SUCCESSFUL_DOWNLOAD "226: Successful Download."
#define SUCCESSFUL_CHANGE "250: Successful change."
#define SUCCESSFUL_QUIT "221: Successful Quit."


#define JSONFILE "config.json"
#define USERS "users"
#define FILES "files"
#define USER "user"
#define PASS "password"
#define ADMIN "admin"
#define SIZE "size"
#define FILES "files"
#define TRUE "true"

struct Client{
    int sock;
    int dataSock;
    int loggedin;
    struct User* user;
};
struct User{
    int usage;
    int isAdmin;
    char address[MAX_ADDRESS];
    char password[MAX_USERNAME];
    char username[MAX_USERNAME];
};

#endif