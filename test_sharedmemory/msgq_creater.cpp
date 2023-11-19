#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>


key_t KEY = 815;

int msgqid;  

void msgqHandler(int sig);

int main() 
{
    msgqid = msgget(KEY, 0666 | IPC_CREAT);
    if (msgqid == -1) {
      std::cerr << "msgget failed\n";
      exit(EXIT_FAILURE);
    }

    signal(SIGINT, msgqHandler);
    
    std::cout << "creator waits till cancelation" << std::endl;
    while(true)
    {
        // wait till cancelation 
        usleep(1000);
    }
}

void msgqHandler(int sig)
{
    std::cout << "Creater of queues kills queues" << std::endl;
    if (msgctl(msgqid, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
