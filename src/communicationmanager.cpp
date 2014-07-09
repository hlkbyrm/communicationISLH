#include "communicationmanager.h"
#include <QTimer>
#include <QString>
#include <QTreeWidget>
#include <qjson/parser.h>
#include "rosThread.h"
#include "network/Client.h"
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

        int numrobots = result["numrobots"].toInt();
        int iscoord =   result["iscoordinator"].toInt();
        int robotID =   result["robotID"].toInt();

        QString temp = QString::number(robotID);

        QString temp2 = "IRobot"+temp;

        // qDebug()<<temp2;

        myrobot->setName(temp2);

        if(iscoord == 1) myrobot->setCoordinator(true);



        this->robots.resize(numrobots);

        QVariantMap nestedMap = result["Robots"].toMap();

        //QVariantList nestedList = nestedMap["Robot"].toList();

        //qDebug()<<nestedList.at(0).toStringList();
        int count = 0;
        foreach (QVariant plugin, nestedMap["Robot"].toList()) {

            Robot* robot = new Robot(this);

            robot->setIP(plugin.toMap()["ip"].toString());

            robot->setName(plugin.toMap()["name"].toString());

            int coord = plugin.toMap()["coordinator"].toInt();

            if(coord == 1) robot->setCoordinator(true);

            if(robot->isCoordinator())
            {
                connect(robot,SIGNAL(networkInfo(QStringList)),this,SLOT(handleNetworkInfo(QStringList)));
            }


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

        /* tempClient->socket->deleteLater();
        tempClient->socket = 0;*/
        //  tempClient->deleteLater();
        //  tempClient = 0;


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
// Send the information to the coordinator
void CommunicationManager::handleCoordinatorUpdate(communicationISLH::robotInfo info)
{
    communicationISLH::neighborInfo inf;

    inf.name = myrobot->getName().toStdString();
    inf.posX = info.posX;
    inf.posY = info.posY;

    QString fileName = QDir::homePath();
    fileName.append("/fuerte_workspace/sandbox/toCoordinator.txt");

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

    qDebug()<<"Coordinator update received from nav";

    if(myrobot->isCoordinator())
    {
        this->rosthread->coordinatorUpdatePublisher.publish(inf);
        file.close();
        return;
    }
    for(int i = 0; i < robots.size(); i++)
    {

        if(robots[i]->isCoordinator())
        {
            QString str = robots[i]->getName();

            str.remove("IRobot");

            int val = Client::RECV_COORDINATOR_UPDATE;

            stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<inf.posX<<" "<<inf.posY<<" "<<str<<"\n";

            robots[i]->sendCoordinatorUpdatetoCoordinator(inf);

            break;
        }
    }

    file.close();


}
void CommunicationManager::handleNavigationISLInfo(communicationISLH::robotInfo msg)
{
    communicationISLH::robotInfo info = msg;
    info.neighbors.erase(info.neighbors.begin(),info.neighbors.end());
    info.neighbors.resize(this->neighbors.size());

    //qDebug()<<"Neighbors size"<<neighbors.size();

   // qDebug()<<"Info size "<<info.neighbors.size();

    for(int i = 0; i < neighbors.size(); i++){

        info.neighbors[i] = neighbors.at(i).toStdString();
    }

  //   qDebug()<<"Info size "<<info.neighbors.size();

    for(int i = 0; i < neighbors.size(); i++){

        for(int j = 0; j < robots.size(); j++)
        {

            if(neighbors[i] ==  robots[j]->getName())
            {

                robots[j]->sendRobotInfo(info);

                break;


            }
        }


    }

}
void CommunicationManager::handleNewCommRequest(QTcpSocket *socket)
{
    for(int i = 0; i < robots.size(); i++){

        if(robots.at(i)->getIP() == socket->peerAddress().toString()){

            Client* aClient = new Client(socket,INCOMING_CLIENT,robots.at(i));

            robots.at(i)->setIncomingClient(aClient);

            robots.at(i)->setIncomingConnected(true);

            if(myrobot->isCoordinator())
            {
                //  connect(robots.at(i)->incomingclient,SIGNAL(coordinatorUpdate(navigationISL::robotInfo)), this,SLOT(handleCoordinatorInfo));

            }

            qDebug()<<"A new connection";



            return;
        }
    }

    socket->abort();
    // socket->deleteLater();

}
// Decode the network matrix received from coordinator
void CommunicationManager::handleNetworkUpdateFromCoordinator(communicationISLH::networkInfo info)
{
    QString str =  QString::fromStdString(info.network);

    QStringList nbrs = str.split(",");

    if(!firstNetworkReceived)
    {
        firstNetworkReceived = true;

        communicationISLH::neighborInfo inf;

        inf.name = "start";

        this->rosthread->neighborInfoPublisher.publish(inf);
    }

    //qDebug()<<nbrs;

    if(myrobot->isCoordinator())
    {
        QString temp = myrobot->getName();

        temp.remove("IRobot");

        int id = temp.toInt();

        if(nbrs.at(id-1).size()> 1)
        {

            this->neighbors.clear();

            neighbors = nbrs.at(id-1).split(";");

        }
        else
        {
            neighbors.clear();

            if(nbrs.at(id-1) != "0")
            {

                neighbors.push_back(nbrs.at(id-1));

            }


        }
        communicationISLH::neighborInfo inf;

        inf.name.append("Neighbors");

        for(int m = 0; m < neighbors.size(); m++)
        {
            inf.name.append(";");
            inf.name.append(neighbors.at(m).toStdString());


        }

         this->rosthread->neighborInfoPublisher.publish(inf);
        //qDebug()<<neighbors;
        //nbrs.removeAt(id-1);

        QString fileName = QDir::homePath();
        fileName.append("/fuerte_workspace/sandbox/toRobotsNetworkInfo.txt");

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


        for(int j = 0; j < nbrs.size(); j++)
        {
            if(j != (id-1))
            {
                if(nbrs.at(j).size()>= 1)
                {

                    QStringList list = nbrs.at(j).split(";");

                    QString name = "IRobot";
                    name.append(QString::number(j+1));

                    for(int k = 0; k < this->robots.size(); k++)
                    {
                        if(this->robots.at(k)->getName() == name)
                        {
                            //qDebug()<<list;


                            this->robots.at(k)->sendNetworkInfo(list);
                            QString strrr;
                            for(int l = 0; l < list.size(); l++ )
                            {
                                QString tmp = list.at(l);

                                tmp.remove("IRobot");

                                strrr.append(tmp);
                                strrr.append(";");

                            }
                            strrr.truncate(strrr.size()-1);
                            stream<<QDateTime::currentDateTime().toTime_t()<<" "<<Client::RECV_NETWORK_INFO<<" "<<strrr<<" "<<j+1<<"\n";

                        }
                    }



                }



            }

        }

        file.close();

    }




}
bool CommunicationManager::initializeNetwork()
{

    // Read config file
    QString path = QDir::homePath();
    path.append("/fuerte_workspace/sandbox/configISL.json");
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

        this->connectToHost(robots.at(i)->getIP(),1200);

    }

}
// Handle the new network info from coordinator
void CommunicationManager::handleNetworkInfo(QStringList list)
{
    this->neighbors = list;

    communicationISLH::neighborInfo inf;

    inf.name.append("Neighbors");

    for(int m = 0; m < neighbors.size(); m++)
    {
        inf.name.append(";");
        inf.name.append(neighbors.at(m).toStdString());


    }

    this->rosthread->neighborInfoPublisher.publish(inf);

    QString fileName = QDir::homePath();
    fileName.append("/fuerte_workspace/sandbox/fromCoordinator.txt");

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
    QString strr;
    for(int i = 0; i < neighbors.size(); i++){
        QString str = neighbors[i];

        str.remove("IRobot");

        strr.append(str);
        strr.append(";");

    }
    strr.truncate(strr.size()-1);

    stream<<QDateTime::currentDateTime().toTime_t()<<" "<<Client::RECV_NETWORK_INFO<<" "<<strr<<"\n";

    file.close();

    // If this is the first time we received the network info
    if(!firstNetworkReceived)
    {

        firstNetworkReceived = true;
        this->connectToRobots();

        communicationISLH::neighborInfo inf;

        inf.name = "start";

        this->rosthread->neighborInfoPublisher.publish(inf);
    }

    qDebug()<<myrobot->getName()<<" neighbors "<<this->neighbors;
}
void CommunicationManager::handleHotspotHandlerMessageOut(communicationISLH::helpMessage msg)
{

    communicationISLH::helpMessage ms = msg;

    QString str = "IRobot";

    str.append(QString::number(msg.robotid));

    qDebug()<<"The outgoing hotspot message robot name "<<str;

    for(int i = 0; i < robots.size(); i++ )
    {

        if(str == robots.at(i)->getName())
        {

            robots.at(i)->sendOutgoingHotspotMessage(ms);

        }
    }


}

