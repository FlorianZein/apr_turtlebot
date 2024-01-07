#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>


// #define KEY 81500
key_t KEY = 830;


// Prototype
void producerHandler (int sig);

// Global definitions and variables
struct Message           // Format of the messages
{
    long type;            // message type, required
    float data[2];             // item is an int, can by anything
};
enum MessageType {PROD_MSG=1, CONS_MSG };
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

    signal(SIGINT, producerHandler);
    std::cout << "I am the producer" << std::endl;

    // cons loop
    while(true)
    {
        std::cout << "Producer tries to read msg" << std::endl;
        Message prodMsg;
        prodMsg.type = PROD_MSG;
        prodMsg.data[0] = rand()%100*0.01; // random integer between 0 and 9999
        prodMsg.data[1] = rand()%100*0.01;

	// send message - should test for error
        msgsnd(msgqid, &prodMsg, sizeof(float) * 2, 0);
        std::cout << "Producer sent: " << prodMsg.data << std::endl;
        sleep(2);  // 0-3 seconds
    }


}

// Signal handler for the producer
void producerHandler (int sig)
{
       // remove message queue
   std::cout << "Producer exiting message queue\n";
//    if (msgctl(msgqid, IPC_RMID, 0) == -1) {
//       std::cerr << "msgctl(IPC_RMID) failed\n";
//       exit(EXIT_FAILURE);
//    }
   exit(EXIT_SUCCESS);
}  // end producerHandler