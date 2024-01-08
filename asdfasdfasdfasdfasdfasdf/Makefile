CPPFLAG = g++ --std=c++17 -Wall -Wextra -pedantic


<<<<<<< HEAD
# program: msgq_creater.o  
=======
# program: msgq_creater.o  tcp_odom.o tcp_laser.o tcp_vel.o
>>>>>>> final dir, makefile, tcp clients to do
# 	$(CPPFLAG) $(CPPDEBUG)

# msgq_creater.o : msgq_creater.cpp
# 	$(CPPFLAG) msgq_creater.cpp -o msgq_creater

# tcp_odom.o : TCP_Listener_odom_msgq.cpp DieWithError.cpp
# 	$(CPPFLAG) TCP_Listener_odom_msgq.cpp DieWithError.cpp -o tcp_odom

# tcp_laser.o : TCP_Listener_laserscan_msgq.cpp DieWithError.cpp
# 	$(CPPFLAG) TCP_Listener_laserscan_msgq.cpp DieWithError.cpp -o tcp_laser

# tcp_vel.o : TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp
# 	$(CPPFLAG) TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp -o tcp_vel

msgq_creater : msgq_creater.cpp
<<<<<<< HEAD
	$(CPPFLAG) msgq_creater.cpp -o msgq_creater

tcp_odom : TCP_Listener_odom_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_odom_msgq.cpp DieWithError.cpp -o tcp_odom

tcp_laser : TCP_Listener_laserscan_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_laserscan_msgq.cpp DieWithError.cpp -o tcp_laser

tcp_vel : TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp -o tcp_vel
=======
	$(CPPFLAG) msgq_creater.cpp -o msgq_creater.out

tcp_odom : TCP_Listener_odom_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_odom_msgq.cpp DieWithError.cpp -o tcp_odom.out

tcp_laser : TCP_Listener_laserscan_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_laserscan_msgq.cpp DieWithError.cpp -o tcp_laser.out

tcp_vel : TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp -o tcp_vel.out


clean : 
	rm *.out
>>>>>>> final dir, makefile, tcp clients to do
