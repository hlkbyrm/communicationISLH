#include "communicationmanager.h"
#include "rosThread.h"

#include <QThread>
#include <ros/ros.h>
#include <QApplication>

int main(int argc,char** argv)
{

    ros::init(argc,argv,"communicationISL");

    QApplication app(argc,argv);

    CommunicationManager manager;

    RosThread* rosthread = new RosThread(&manager);

    manager.rosthread = rosthread;

    QThread thr;

    rosthread->moveToThread(&thr);

    QObject::connect(rosthread,SIGNAL(rosFinished()),&thr,SLOT(quit()));

    QObject::connect(&thr,SIGNAL(finished()),&app,SLOT(quit()));

    QObject::connect(&thr,SIGNAL(finished()),rosthread,SLOT(deleteLater()));

    QObject::connect(&thr,SIGNAL(started()),rosthread,SLOT(work()));

    thr.start();


    return app.exec();

  //  ros::NodeHandle n;

//    ros::Subscriber amclSub = n.subscribe("amcl_pose",2,amclPoseCallback);

 //   ros::spin();

   // tcpComm comm;

   // comm.myServer->setupServer();

}


