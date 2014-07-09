#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <communicationISLH/robotInfo.h>
#include <communicationISLH/networkInfo.h>
#include <QString>
#include "network/Client.h"

class Robot : public QObject
{
    Q_OBJECT
public:

    explicit Robot(QObject *parent = 0);

    void setRobotInfo(communicationISLH::robotInfo info);

    void sendCoordinatorUpdatetoCoordinator(communicationISLH::neighborInfo info);

    void sendNetworkInfo(QStringList info);

    void sendOutgoingHotspotMessage(communicationISLH::helpMessage msg);

    communicationISLH::robotInfo getRobotInfo();

    void setName(QString nam);

    QString getName();

    void setIncomingClient(Client* client);

    void setOutgoingClient(Client* client);

    QString getIP();

    void setIP(QString ip);

    bool isOutgoingConnected();

    bool isIncomingConnected();

    void setOutGoingConnected(bool status);

    void setIncomingConnected(bool status);

    void sendRobotInfo(communicationISLH::robotInfo info);

    bool isCoordinator();

    void setCoordinator(bool status);
private:

    communicationISLH::robotInfo info;

    QString IP;

    Client* incomingclient;

    Client* outgoingclient;

    bool outgoingConnected;

    bool incomingConnected;

    QString name;

    bool coordinator;
    
signals:

    void networkInfo(QStringList list);
    
public slots:
    void getClientDisconnected(int type);

    // Receive robot info from a neighbor
    void receiveRobotInfo(communicationISLH::robotInfo info);

    // Receive a coordinator update info from a robot
    void receiveCoordinatorUpdate(communicationISLH::neighborInfo info);

    void receiveNetworkInfoFromCoordinator(QStringList list);

    void receiveHotspotMessage(QStringList list);
    
};

#endif // ROBOT_H
