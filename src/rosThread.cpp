#include "rosThread.h"
#include "std_msgs/String.h"

RosThread::RosThread()
{
    shutdown = false;

}
RosThread::RosThread(CommunicationManager *currentmanager)
{

    robotConnectionInfoPub = n.advertise<std_msgs::String>("communicationISLH/robotConnectionInfo", queueSize);

    messageInPublisher = n.advertise<ISLH_msgs::inMessage>("communicationISLH/messageIn", queueSize);

    manager = currentmanager;

    messageOutSubscriber = n.subscribe("messageDecoderISLH/messageOut", queueSize, &CommunicationManager::handleMessageOut,this->manager);

}


void RosThread::work(){

    if(!ros::ok()){

        emit rosStartFailed();

        return;
    }

     emit rosStarted();




     QString path = QDir::homePath();
     path.append("/ISL_workspace/src/configISL.json");


     if(!readConfigFile(path)){

         qDebug()<< "Read Config File Failed!!!";

         ros::shutdown();

         emit rosFinished();
     }



    ros::Rate loop(5);

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



// Reads the config file
bool RosThread::readConfigFile(QString filename)
{
    QFile file(filename);

    if(!file.exists()) return false;

    if(!file.open(QFile::ReadOnly)) return false;

    QJson::Parser parser;

    bool ok;

    QVariantMap result = parser.parse(&file,&ok).toMap();

    if(!ok){

        file.close();
        qDebug()<<"Fatal reading error";

        return false;
    }
    else
    {

        queueSize = result["queueSize"].toInt();
        qDebug()<<result["queueSize"].toString();

    }
    file.close();
    return true;

}
