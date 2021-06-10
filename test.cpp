
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
    char buf[100] = "hello::world";
    char b1[100];
    char b2[100];
    sscanf(buf, "%s::%s", b1, b2);
    cout << string(b1) << endl;
    cout << string(b2) << endl;
    
}