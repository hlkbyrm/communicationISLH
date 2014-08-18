#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QString>
#include "network/Client.h"

class Robot : public QObject
{
    Q_OBJECT
public:

    explicit Robot(QObject *parent = 0);

    //void setRobotInfo(communicationISLH::robotInfo info);

    void sendOutgoingMessage(communicationISLH::outMessage msg, int msgIndx);

  //  communicationISLH::robotInfo getRobotInfo();

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

private:

   // communicationISLH::robotInfo info;

    QString IP;

    Client* incomingclient;

    Client* outgoingclient;

    bool outgoingConnected;

    bool incomingConnected;

    QString name;

    bool coordinator;
    
signals:

    
public slots:
    void getClientDisconnected(int type);

    void receiveMessage(QString message);
    
};

#endif // ROBOT_H
