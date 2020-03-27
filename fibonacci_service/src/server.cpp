#include "ros/ros.h"
#include <sstream>
#include <vector>
#include "fibonacci_service/sequence.h" //once compiled the srv generates a .h file
using namespace std;


//req is actually what the server take as input and response is the output
bool service_callback(fibonacci_service::sequence::Request &req , fibonacci_service::sequence::Response &res){
ROS_INFO("new request");

if(req.index < 0){return false;}
else{
	int n;
	int a=0;
	int b=1;
	int fib=0;
	int fine=req.index+req.length;
	int i=0;
	while(i<fine){
	if(i==0){fib=0;b=1;a=0;}
	else if(i==1){fib=1;b=1;a=0;}
	else{   fib=a+b;
		a=b;
		b=fib;		
		}
	if(i>=req.index){ res.result.push_back(fib); }
	i++;	
	}


return true;
}


//cout<<"From client,server says: index:  "<<req.index<<"  length: "<<req.length;
//cout <<"client req"<<req.in<<endl; //or false if something has gonewrong it's our choice of implementation
return true;
}
int main ( int argc, char** argv){
ros::init(argc,argv, "fibonacci_server");
ros::NodeHandle nh;
ros::ServiceServer service=nh.advertiseService("sequence", service_callback);
 ROS_INFO("waiting for fibonacci requests.");
while(ros::ok()){
ros::spinOnce();}
return 0;}
