#include "communicationmanager.h"
#include <QTimer>
#include <QString>
#include <QTreeWidget>
#include <qjson/parser.h>
#include "rosThread.h"
#include "network/Client.h"

//!!!!
//!!! initilizeNetwork fonksiyonundaki this->connectToRobots(); satırının commentini kaldırmayı unutma !!!
//!!!!


CommunicationManager::CommunicationManager(QObject *parent) :
    QObject(parent)
{


    myrobot = new Robot(this);

    if(!this->initializeNetwork())
    {
        qDebug()<<"Initialization failed";
    }

        firstNetworkReceived = false;


}
bool CommunicationManager::readConfigFile(QString filename)
{
    QFile file(filename);

    if(!file.exists()) return false;

    if(!file.open(QFile::ReadOnly))return false;

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
        // qDebug()<<result["numrobots"].toString();

        int numrobots = result["numrobots"].toInt()-1;
      //  int iscoord =   result["iscoordinator"].toInt();
        int robotID =   result["robotID"].toInt();

        QString temp = QString::number(robotID);

        //QString temp2 = "IRobot"+temp;

        // qDebug()<<temp2;

        //myrobot->setName(temp2);
        myrobot->setName(temp);

       // if(iscoord == 1) myrobot->setCoordinator(true);



        this->robots.resize(numrobots);

        QVariantMap nestedMap = result["Robots"].toMap();

        //QVariantList nestedList = nestedMap["Robot"].toList();

        //qDebug()<<nestedList.at(0).toStringList();
        int count = 0;
        foreach (QVariant plugin, nestedMap["Robot"].toList()) {

            Robot* robot = new Robot(this);

            robot->setIP(plugin.toMap()["ip"].toString());

            robot->setName(plugin.toMap()["name"].toString());

            //int coord = plugin.toMap()["coordinator"].toInt();

            //if(coord == 1) robot->setCoordinator(true);

            /*
            if(robot->isCoordinator())
            {
                connect(robot,SIGNAL(networkInfo(QStringList)),this,SLOT(handleNetworkInfo(QStringList)));
            }
*/

            this->robots[count] = robot;

            count++;
            // qDebug() << "\t-" << plugin.toMap()["ip"].toString();
        }



    }

    file.close();
    return true;



}
void CommunicationManager::connecttt(){

    //  qDebug()<<"Error";
    //  this->connectToHost("193.140.195.52",1200);
    this->connectToHost("localhost",1200);
    //  QTimer::singleShot(2000,this,SLOT(connecttt()));

}
void CommunicationManager::connectToHostWithWait(QString hostAddress, quint16 port)
{
    Client* cl = new Client(OUTGOING_CLIENT,this);


    cl->setIP(hostAddress);

    while(1)
    {
        cl->socket->connectToHost(hostAddress,port);

        if(cl->socket->waitForConnected(5000)){

            for(int i = 0; i < robots.size() ; i++){

                //  qDebug()<<robots[i]->getIP();

                //  qDebug()<<tempClient->getClientIP();

                if(robots[i]->getIP() == cl->getIP())
                {
                    QObject::disconnect(cl,SIGNAL(clientDisconnected(int)),this,SLOT(getClientDisconnected(int)));

                    cl->setParent(robots[i]);

                    connect(cl, SIGNAL(clientDisconnected(int)),robots[i], SLOT(getClientDisconnected(int)));

                    robots[i]->setOutgoingClient(cl);

                    robots[i]->setOutGoingConnected(true);

                    qDebug()<<"Outgoing connected : "<<robots[i]->getIP();

                    //tempClient=0;
                    //tempClient->deleteLater();

                    return;
                }
            }

        }
        this->delay(1);
    }
/*    else
    {

        qDebug()<<"Error timeout";

        for(int i = 0; i < robots.size() ; i++){

            if(robots[i]->getIP() == cl->getIP())
            {

                robots[i]->setOutGoingConnected(false);
                break;

            }
        }

    }
*/
}
void CommunicationManager::connectToHost(QString hostAddress, quint16 port)
{
    Client* cl = new Client(OUTGOING_CLIENT,this);


    cl->setIP(hostAddress);

    cl->socket->connectToHost(hostAddress,port);

    if(cl->socket->waitForConnected(5000)){

        for(int i = 0; i < robots.size() ; i++){

            //  qDebug()<<robots[i]->getIP();

            //  qDebug()<<tempClient->getClientIP();

            if(robots[i]->getIP() == cl->getIP())
            {
                QObject::disconnect(cl,SIGNAL(clientDisconnected(int)),this,SLOT(getClientDisconnected(int)));

                cl->setParent(robots[i]);

                connect(cl, SIGNAL(clientDisconnected(int)),robots[i], SLOT(getClientDisconnected(int)));

                robots[i]->setOutgoingClient(cl);

                robots[i]->setOutGoingConnected(true);

                qDebug()<<"Outgoing connected";

                //tempClient=0;
                //tempClient->deleteLater();

                return;
            }
        }

    }
    else
    {

        qDebug()<<"Error timeout";

        for(int i = 0; i < robots.size() ; i++){

            if(robots[i]->getIP() == cl->getIP())
            {

                robots[i]->setOutGoingConnected(false);
                break;

            }
        }


    }
}
void CommunicationManager::handleSocketConnect()
{


    qDebug()<<"Connected";

}
void CommunicationManager::handleSocketError(QAbstractSocket::SocketError error){

    qDebug()<<"Error";

    for(int i = 0; i < robots.size() ; i++){

        if(robots[i]->getIP() == tempClient->getIP())
        {

            robots[i]->setOutGoingConnected(false);
            break;

        }
    }

}
void CommunicationManager::getClientDisconnected(int type)
{
    qDebug()<<"Error client disconnected";
    for(int i = 0; i < robots.size() ; i++){

        if(robots[i]->getIP() == tempClient->getIP())
        {

            robots[i]->setOutGoingConnected(false);
            break;

        }
    }

}
void CommunicationManager::delay(int duration)
{
    QTime dieTime= QTime::currentTime().addSecs(duration);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
bool CommunicationManager::initializeNetwork()
{

    // Read config file
    QString path = QDir::homePath();
    path.append("/ISL_workspace/src/configISL.json");
    if(!this->readConfigFile(path)) return false;

    // Initialize TCP server
    this->TcpComm = new tcpComm(this);

    // Start listening incoming connections
    this->TcpComm->myServer->setupServer();

    // Direct incoming connections to the slot
    QObject::connect(this->TcpComm->myServer,SIGNAL(newCommRequest(QTcpSocket*)),this,SLOT(handleNewCommRequest(QTcpSocket*)));

    // If I am the coordinator I should connect to other robots
    //if(myrobot->isCoordinator())
    //{

        this->connectToRobots();

        neighbors.clear();

        this->delay(5);

     //  this->connectToRobots();

    //}
    // After 5 seconds start to connect with other robots
    // QTimer::singleShot(5000,this,SLOT(connectToRobots()));

    return true;

}
void CommunicationManager::connectToRobots()
{

    // Connect to each robot in the list
    for(int i = 0; i < this->robots.size(); i++)
    {

        //this->connectToHost(robots.at(i)->getIP(),1200);

        this->connectToHostWithWait(robots.at(i)->getIP(),1200);
    }

}

void CommunicationManager::handleMessageOut(ISLH_msgs::outMessage msg)
{

   //multiple message sending
    for(int j=0; j<msg.robotid.size(); j++)
    {
        QString str = "IRobot";

        str.append(QString::number(msg.robotid[j]));

        qDebug()<<"The outgoing hotspot message robot name "<<str;

        for(int i = 0; i < robots.size(); i++ )
        {

            if(str == robots.at(i)->getName())
            {

                //robots.at(i)->sendOutgoingMessage(msg, j);

                robots.at(i)->sendOutgoingMessage(msg, msg.messageIndx[j]);
            }
        }
    }


}

void CommunicationManager::handleNewCommRequest(QTcpSocket *socket)
{
    int i;
    for( i = 0; i < robots.size(); i++){

        if(robots.at(i)->getIP() == socket->peerAddress().toString()){

            Client* aClient = new Client(socket,INCOMING_CLIENT,robots.at(i));

            robots.at(i)->setIncomingClient(aClient);

            robots.at(i)->setIncomingConnected(true);

            /*
            if(myrobot->isCoordinator())
            {
                //  connect(robots.at(i)->incomingclient,SIGNAL(coordinatorUpdate(navigationISL::robotInfo)), this,SLOT(handleCoordinatorInfo));

            }
            */

            qDebug()<<"A new connection";


            /*
            if (robots.at(i)->isOutgoingConnected()==false)
            {
                this->connectToHost(robots.at(i)->getIP(),1200);
            }
            */
            return;
        }
    }

    socket->abort();


}
