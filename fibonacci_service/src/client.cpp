#include "ros/ros.h"

#include <sstream>
#include <iostream>
#include "fibonacci_service/sequence.h" //once compiled the srv 
using  namespace std;
int main (int argc, char** argv){
ros::init(argc,argv,"fibonacci_client");
if (argc != 3)
     {
       ROS_INFO("usage: add index and length X Y");
       return 1;
     }
ros::NodeHandle nh;
ros::ServiceClient client = nh.serviceClient<fibonacci_service::sequence>("sequence");
fibonacci_service::sequence srv;
srv.request.index=atoi(argv[1]);
srv.request.length=atoi(argv[2]);
cout<<"your request is:  "<<srv.request.index<<"  "<<srv.request.length;
if(client.call (srv)){
ROS_INFO("server  response:");
for(int i=0;i<atoi(argv[2]);i++){
cout<<srv.response.result[i];
}
cout<<endl;
}else{
	ROS_ERROR("no service"); 
	return 1;
	}
ros::spinOnce();
return 0;
}

