#include "rosThread.h"
#include <communicationISLH/neighborInfo.h>
#include "std_msgs/String.h"

RosThread::RosThread()
{
    shutdown = false;

}
RosThread::RosThread(CommunicationManager *currentmanager)
{
    manager = currentmanager;

}


void RosThread::work(){

    if(!ros::ok()){

        emit rosStartFailed();

        return;
    }

     emit rosStarted();


     messageInPublisher = n.advertise<communicationISLH::inMessage>("communicationISLH/messageIn",5);

     messageOutSubscriber = n.subscribe("messageDecoderISLH/messageOut",5,&CommunicationManager::handleMessageOut,this->manager);

    ros::Rate loop(30);

    while(ros::ok()){

        ros::spinOnce();
        loop.sleep();

    }

    emit rosFinished();


}
void RosThread::shutdownROS()
{
    ros::shutdown();
   // shutdown = true;


}


