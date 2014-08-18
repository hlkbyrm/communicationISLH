#include "robot.h"
#include "communicationmanager.h"
#include "rosThread.h"

Robot::Robot(QObject *parent) :
    QObject(parent)
{
    incomingConnected = false;
    outgoingConnected = false;

    //coordinator = false;
}
/*
communicationISLH::robotInfo Robot::getRobotInfo(){

    return this->info;

}
void Robot::setRobotInfo(communicationISLH::robotInfo info){

    this->info = info;
}
*/
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

void Robot::sendOutgoingMessage(communicationISLH::outMessage msg, int msgIndx)
{
    if(this->isOutgoingConnected())
        this->outgoingclient->sendOutgoingMessage(msg, msgIndx);

}
void Robot::receiveMessage(QString message)
{
        communicationISLH::inMessage msg;

        QString name = this->getName();

        //name.remove("IRobot");

        int id = name.toInt();

        msg.robotid = id;

        msg.message = message.toStdString();

        CommunicationManager* manager = (CommunicationManager*)this->parent();

        manager->rosthread->messageInPublisher.publish(msg);
}
