/*Node1 in an infinite loop generates two random float numbers and publish them on topic Twofield
just to see the result this node will be a subscriber of Node2 topic
*/

#include "ros/ros.h"
#include "add_random/custom.h"
//#include "add_random/response.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
class asker{
	public:
	asker();
//	void callback(add_random::response res);
	void publishing(add_random::custom req);

	private:
	ros::NodeHandle _nh;
	ros::Subscriber _topic_sub;
	ros::Publisher _topic_pub;

};
asker::asker(){
//_topic_sub = _nh.subscribe("/total", 3, &asker::callback,this);
_topic_pub = _nh.advertise<add_random::custom>("/question",3);
}

//void asker::callback(add_random::response res){	
//ROS_INFO("GOT AN ANSWER");
//cout<<endl<<res.first<<"+"<<res.second<<"="<<res.result<<endl;
//}

void asker::publishing(add_random::custom req){
_topic_pub.publish(req);
}

int main (int argc,char** argv){
int i=0;
ros::init(argc,argv,"ros_asker");
asker as;
add_random::custom msg;
ros::Rate rate(5);
while(ros::ok()){
	i++;
	srand(i);
	msg.second=float(rand())/float(RAND_MAX);
	i++;
	srand(i);
	msg.first=float(rand())/float(RAND_MAX);
	as.publishing(msg);
	ROS_INFO_STREAM("Sending:"<<msg.first<<"+"<<msg.second);
	rate.sleep();
	}
return 0;
}
