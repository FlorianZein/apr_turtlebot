CPPFLAG = g++ --std=c++17 -Wall -Wextra -pedantic


# program: msgq_creater.o  
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
	$(CPPFLAG) msgq_creater.cpp -o msgq_creater

tcp_odom : TCP_Listener_odom_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_odom_msgq.cpp DieWithError.cpp -o tcp_odom

tcp_laser : TCP_Listener_laserscan_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Listener_laserscan_msgq.cpp DieWithError.cpp -o tcp_laser

tcp_vel : TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp
	$(CPPFLAG) TCP_Commander_cmd_vel_msgq.cpp DieWithError.cpp -o tcp_vel