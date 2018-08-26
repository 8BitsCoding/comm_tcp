 /*************************************************************************
 * Author: Abhinav Jain
 * Contact: abhinavjain241@gmail.com, abhinav.jain@heig-vd.ch
 * Date: 28/06/2016
 *
 * This file contains source code to the server node of the ROS package
 * comm_tcp developed at LaRA (Laboratory of Robotics and Automation)
 * as part of my project during an internship from May 2016 - July 2016.
 *
 * (C) All rights reserved. LaRA, HEIG-VD, 2016 (http://lara.populus.ch/)
 ***************************************************************************/
#include <ros/ros.h>

// tcp 통신을 위한 부분
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "std_msgs/String.h"

using namespace std;

// 에러처리
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main (int argc, char** argv)
{
 // ros topic publish
  ros::init(argc, argv, "server_node");
  ros::NodeHandle nh;
  ros::Publisher server_pub = nh.advertise<std_msgs::String>("/server_messages/", 1000);
 
  // sockfd : 서버 소켓 Socket file descriptors
  // newsockfd : 연결 소켓 Socket file descriptors
  // portno : port number
  int sockfd, newsockfd, portno; // Socket file descriptors and port number
 
  // 버퍼
  char buffer[256]; //buffer array of size 256
 
  // 여기는 사용법이니 받아들여야함.
  // 주소는 sockaddr_in 구조체를 통해서 선언
  // 사용법은 아래 참고
  socklen_t clilen; //object clilen of type socklen_t
  struct sockaddr_in serv_addr, cli_addr; ///two objects to store client and server address
 
 
  std_msgs::String message;
  std::stringstream ss;
  int n;
  ros::Duration d(0.01); // 100Hz
 
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
 
  // 여기가 사용법 여기를 참고할 것.
  portno = atoi(argv[1]);
  cout << "Hello there! This node is listening on port " << portno << " for incoming connections" << endl;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
  if (sockfd < 0)
      error("ERROR opening socket");
 
  int enable = 1;
 
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
      error("setsockopt(SO_REUSEADDR) failed");
 
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
 
  // socket bind
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
 
  // 대기
  listen(sockfd,5);
 
  // listen이 들어온다면 accept
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd,
              (struct sockaddr *) &cli_addr,
              &clilen);
 
  // 에러처리
  if (newsockfd < 0)
       error("ERROR on accept");
 
  while(ros::ok()) {
     // 버퍼를 비우고
     ss.str(std::string()); //Clear contents of string stream
     bzero(buffer,256);
   
     // read를 대기
     n = read(newsockfd,buffer,255);
   
     // 에러처리
     if (n < 0) error("ERROR reading from socket");
     // printf("Here is the message: %s\n",buffer);
     ss << buffer;
     message.data = ss.str();
     ROS_INFO("%s", message.data.c_str());
     server_pub.publish(message);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
     //close(newsockfd);
     //close(sockfd);
     //ros::spinOnce();
     //d.sleep();
  }
 
  return 0;
}
