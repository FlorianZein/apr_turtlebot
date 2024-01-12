#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>


key_t KEY_ODOM          = 810;
key_t KEY_LASER         = 820;
key_t KEY_VEL           = 830;

key_t KEY_ODOM_ACT       = 710;
key_t KEY_LASER_ACT      = 720;
key_t KEY_VEL_ACT        = 730;




int msgqid_odom, msgqid_laser, msgqid_vel, msgqid_odom_act, msgqid_laser_act, msgqid_vel_act;  

void msgqHandler(int sig);

int main() 
{
    msgqid_odom = msgget(KEY_ODOM, 0666 | IPC_CREAT);
    if (msgqid_odom == -1) {
      std::cerr << "msgget odom failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_laser = msgget(KEY_LASER, 0666 | IPC_CREAT);
    if (msgqid_laser == -1) {
      std::cerr << "msgget laser failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel = msgget(KEY_VEL, 0666 | IPC_CREAT);
    if (msgqid_vel == -1) {
      std::cerr << "msgget vel failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_odom_act = msgget(KEY_ODOM_ACT, 0666 | IPC_CREAT);
    if (msgqid_odom_act == -1) {
      std::cerr << "msgget odom cast failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_laser_act = msgget(KEY_LASER_ACT, 0666 | IPC_CREAT);
    if (msgqid_laser_act == -1) {
      std::cerr << "msgget last cast failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel_act = msgget(KEY_VEL_ACT, 0666 | IPC_CREAT);
    if (msgqid_vel_act == -1) {
      std::cerr << "msgget vel cast failed\n";
      exit(EXIT_FAILURE);
    }


    signal(SIGINT, msgqHandler);
    
    std::cout << "creator waits till cancelation" << std::endl;
    while(true)
    {
        // wait till cancelation 
        usleep(100);
    }
}

void msgqHandler(int sig)
{
    std::cout << "Creater of queues kills queues" << std::endl;
    if (msgctl(msgqid_odom, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgqid_laser, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgqid_vel, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgqid_odom_act, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgqid_laser_act, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgqid_vel_act, IPC_RMID, 0) == -1) {
        std::cerr << "msgctl(IPC_RMID) failed\n";
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
