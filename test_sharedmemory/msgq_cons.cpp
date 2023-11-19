#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>


key_t KEY = 815;

// Prototypes
void consumerHandler (int sig);

// Global definitions and variables
struct Message           // Format of the messages
{
   long type;            // message type, required
   int data;             // item is an int, can by anything
};
enum MessageType { PROD_MSG=1, CONS_MSG };
// CONS_MSG is not used in this program, since the consumer doesn't
// send messages to the producer.  Can also have more than 2 types
// of messages if needed.

int msgqid;                // Message queue id

int main()
{

    msgqid = msgget(KEY, 0666 | IPC_CREAT);
    if (msgqid == -1) {
      std::cerr << "msgget failed\n";
      exit(EXIT_FAILURE);
    }

    signal(SIGINT, consumerHandler);
    std::cout << "I am the comsumer" << std::endl;

    // cons loop
    while(true)
    {
        std::cout << "Cons tries to read msg" << std::endl;
        Message consMsg;

        //receive msg
        msgrcv(msgqid, &consMsg, sizeof(int), PROD_MSG, 0);
        std::cout << "Cons read: " << consMsg.data << std::endl;
        usleep(1000); //sleeping 1ms
    }

}

void consumerHandler(int sig)
{
    std::cout << "Cons exiting" << std::endl;
    // std::cout << "Consumer removing message queue\n";
    // if (msgctl(msgqid, IPC_RMID, 0) == -1) {
    //     std::cerr << "msgctl(IPC_RMID) failed\n";
    //     exit(EXIT_FAILURE);
    // }
    exit(EXIT_SUCCESS);
}