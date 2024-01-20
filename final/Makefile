CPPFLAG = g++ --std=c++17 -Wall -Wextra -pedantic

program: bot_control.o msgq_creater.o tcp_odom.o tcp_laser.o tcp_vel.o tcp_vel_stop.o

bot_control.o : botControl.cpp
	$(CPPFLAG) botControl.cpp -o bot_control

msgq_creater.o : msgq_creater.cpp
	$(CPPFLAG) msgq_creater.cpp -o msgq_creater

tcp_odom.o : TCP_Listener_odom_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_odom_msgq.cpp DieWithError.cpp -o tcp_odom

tcp_laser.o : TCP_Listener_laserscan_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_laserscan_msgq.cpp DieWithError.cpp -o tcp_laser

tcp_vel.o : TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp -o tcp_vel

tcp_vel_stop.o : TCP_Commander_cmd_vel_stop.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Commander_cmd_vel_stop.cpp DieWithError.cpp -o tcp_vel_stop

clean : 
	rm *.out
