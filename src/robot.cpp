#include "robot.h"
#include "communicationmanager.h"
#include "rosThread.h"

Robot::Robot(QObject *parent) :
    QObject(parent)
{
    incomingConnected = false;
    outgoingConnected = false;

    coordinator = false;
}
communicationISLH::robotInfo Robot::getRobotInfo(){

    return this->info;

}
void Robot::setRobotInfo(communicationISLH::robotInfo info){

    this->info = info;
}
void Robot::setName(QString nam){

    this->name = nam;
}
QString Robot::getName(){

    return name;
}
void Robot::setIncomingClient(Client *client)
{

    incomingclient = client;
}
void Robot::setOutgoingClient(Client *client)
{
    outgoingclient = client;


}
QString Robot::getIP(){
    return IP;
}
void Robot::setIP(QString ip){

    this->IP = ip;
}
bool Robot::isIncomingConnected(){

    return incomingConnected;
}
bool Robot::isOutgoingConnected(){

    return outgoingConnected;
}
void Robot::setOutGoingConnected(bool status){

    outgoingConnected = status;
}
void Robot::setIncomingConnected(bool status){

    incomingConnected = status;
}
void Robot::sendRobotInfo(communicationISLH::robotInfo info)
{

    if(outgoingConnected)
    {
        this->outgoingclient->sendRobotInfotoNeighbor(info);

        QString fileName = QDir::homePath();
        fileName.append("/fuerte_workspace/sandbox/toRobot.txt");

        QFile file(fileName);

        if(!file.exists())
        {
            file.open(QFile::WriteOnly);
        }
        else
        {
            file.open(QFile::Append);

        }
        QTextStream stream(&file);

        QString str = this->name;

        str.remove("IRobot");

        int val = Client::RECV_ROBOT_INFO;

        stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<" "<<info.posY<<" "<<info.radius<<" "<<str<<"\n";

        file.close();
    }
}
bool Robot::isCoordinator()
{
    return coordinator;
}
void Robot::setCoordinator(bool status)
{
    coordinator = status;
}
void Robot::sendNetworkInfo(QStringList info)
{
    if(this->outgoingConnected)
    {
        this->outgoingclient->sendNetworkInfo(info);
    }
}
void Robot::getClientDisconnected(int type)
{
    qDebug()<<"Robot "<<this->name<<" client "<<type<<" disconnected"<<"name "<<name;

    if(type == INCOMING_CLIENT)
    {
        //incomingclient->socket->abort();
        this->incomingConnected = false;
        this->incomingclient->deleteLater();
    }
    else
    {
        //outgoingclient->socket->abort();
        this->outgoingConnected = false;
        this->outgoingclient->deleteLater();
    }
}
void Robot::receiveRobotInfo(communicationISLH::robotInfo info)
{
    this->info = info;

    communicationISLH::neighborInfo ninfo;

    ninfo.name = this->name.toStdString();

    ninfo.posX = this->info.posX;

    ninfo.posY = this->info.posY;

    ninfo.radius = this->info.radius;

    ninfo.targetX = this->info.targetX;

    ninfo.targetY = this->info.targetY;

    CommunicationManager* manager = (CommunicationManager*)this->parent();

    manager->rosthread->neighborInfoPublisher.publish(ninfo);

    QString fileName = QDir::homePath();
    fileName.append("/fuerte_workspace/sandbox/fromRobot.txt");

    QFile file(fileName);

    if(!file.exists())
    {
        file.open(QFile::WriteOnly);
    }
    else
    {
        file.open(QFile::Append);

    }
    QTextStream stream(&file);

    QString str = this->name;

    str.remove("IRobot");

    int val = Client::RECV_ROBOT_INFO;

    stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<" "<<info.posY<<" "<<info.radius<<" "<<str<<"\n";

    file.close();

}
// Receive coordinator update from the client robot
void Robot::receiveCoordinatorUpdate(communicationISLH::neighborInfo info)
{


    info.name = name.toStdString();

    qDebug()<<QString::fromStdString(info.name)<<info.posX<<info.posY;

    CommunicationManager* manager = (CommunicationManager*)this->parent();

    manager->rosthread->coordinatorUpdatePublisher.publish(info);

    qDebug()<<"Received a coordinator update";

    QString fileName = QDir::homePath();
    fileName.append("/fuerte_workspace/sandbox/CoordinatorReceivedUpdate.txt");

    QFile file(fileName);

    if(!file.exists())
    {
        file.open(QFile::WriteOnly);
    }
    else
    {
        file.open(QFile::Append);

    }
    QTextStream stream(&file);

    QString str = this->name;

    str.remove("IRobot");

    int val = Client::RECV_COORDINATOR_UPDATE;

    stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<" "<<info.posY<<" "<<str<<"\n";

    file.close();

}
void Robot::sendCoordinatorUpdatetoCoordinator(communicationISLH::neighborInfo info)
{
    if(this->isOutgoingConnected())
        this->outgoingclient->sendCoordinatorUpdatetoCoordinator(info);




}
void Robot::receiveNetworkInfoFromCoordinator(QStringList list)
{

  //  CommunicationManager* manager = (CommunicationManager*)this->parent();

    qDebug()<<"Here i am";
    QStringList tempList = list;

    emit networkInfo(tempList);

}
void Robot::sendOutgoingHotspotMessage(communicationISLH::helpMessage msg)
{
    if(this->isOutgoingConnected())
        this->outgoingclient->sendHotspotOutgoingMessage(msg);

}
void Robot::receiveHotspotMessage(QStringList list)
{
        communicationISLH::helpMessage msg;

        QString name = this->getName();

        name.remove("IRobot");

        int id = name.toInt();

        msg.robotid = id;

        msg.messageid = list.at(0).toInt();

        CommunicationManager* manager = (CommunicationManager*)this->parent();

        manager->rosthread->hotspotHandlerMessageInPublisher.publish(msg);
}
