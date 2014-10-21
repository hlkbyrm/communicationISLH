#include "Client.h"
#include "tcpComm.h"
//#include <opencv2/opencv.hpp>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHostInfo>
#include <QtCore/QString>
#include <string>
#include <sstream>

using namespace std;

bool firstTime = true;
	
Client::Client(QTcpSocket* sock, int clientType, QObject* parent):QObject(parent)
{
    written = true;

    myRecData = "";

	socket = sock;

    socket->setParent(this);

    socket->setReadBufferSize(0);

	type = clientType;

    IP = socket->peerAddress().toString(); // get IP

    connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

    connect(this, SIGNAL(clientDisconnected(int)),this->parent(), SLOT(getClientDisconnected(int)));

    connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));

    connect(this,SIGNAL(incomingMessage(QString)),this->parent(),SLOT(receiveMessage(QString)));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));

    clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

	speedCounter = 0;

	speedTest = false;

    qDebug()<<"Client IP is: "<<IP;


}
// Outgoing client constructor
Client::Client(int clientType, QObject* parent):QObject(parent)
{
    socket = new QTcpSocket(this);
	    
	socket->setReadBufferSize(0);

	type = clientType;



    connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

    //connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));



	clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

}
Client::~Client()
{


}

void Client::receiveData(){
	
    // Read the buffer;
    myRecDataBA = socket->readAll();

    // Convert to QString
    myRecData += QString::fromAscii(myRecDataBA);

    qDebug()<<"Incoming message-> robot id:"<< getHostName() << "Message: "<< myRecData;

    // Received data might contain more than one data package
    // We must split data packages
    QStringList datas = myRecData.split("<EOF>",QString::KeepEmptyParts);
    for(int i=0;i<datas.count() - 1;i++){

        QString data = datas.at(i);

        // check package consistency
        // package -> AA * messageType * messageSubType * dataSize * data
        QStringList dataParts = data.split("*",QString::SkipEmptyParts);

        if ( (dataParts.size() == MESSAGE_DATA_PARTS) && (dataParts.at(3).toInt() == (dataParts.at(4).size())) )
        {
            myRecData = data;
            receiveMessage();
        }
        else
        {
            qDebug()<<"The received message from " << getHostName() <<" is not consistent.";
        }

    }
    myRecData = datas.last();
}

void Client::sendData(QByteArray data){
    if(!written){
        waitingMessages.push_back(data);
        return;
    }

    QByteArray m;
    foreach(QByteArray msg,waitingMessages)
        m.append(msg);
    waitingMessages.clear();

    m.append(data);

    written = false;
    socket->write(m);
}

void Client::bytesWritten(qint64 byteNum){
    written = true;
    qDebug() << "sending finished" << byteNum;
}

void Client::getSocketDisconnected()
{


    // if remote host has closed the connection

    emit(clientDisconnected(type));


}
// Displays socket error in a MessageBox
void Client::displaySocketError(QAbstractSocket::SocketError socketError){

    qDebug()<<"Socket Error!!!";

    if(type == 0)
    emit(clientDisconnected(type));

   // socket->deleteLater();

    return;

    QMessageBox information;

	switch (socketError) {
	 case QAbstractSocket::RemoteHostClosedError:
		 clientSocketError = socketError;
		 break;
	 case QAbstractSocket::HostNotFoundError:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The host was not found. Please check the "
			 "host name and port settings."));
		 information.exec();
		 break;
	 case QAbstractSocket::ConnectionRefusedError:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The connection was refused by the peer. "
			 "Make sure the fortune server is running, "
			 "and check that the host name and port "
			 "settings are correct."));
		 information.exec();
		 break;

	 default:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The following error occurred: %1.")
			 .arg(socket->errorString()));
		 information.exec();
	}

	socket->deleteLater();
    emit(clientDisconnected(type));
}
QString Client::getHostName(){

    return hostName;

}
void Client::setHostName(QString Name){

    hostName = Name;


}

void Client::receiveMessage()
{
    //QStringList list = myRecData.split(";",QString::SkipEmptyParts);

    //qDebug()<<list;

    //emit incomingMessage(list);

    emit incomingMessage(myRecData);
}
void Client::setIP(QString ip){

	IP = ip;

}
QString Client::getIP(){


    return IP;

}


void Client::receiveHostName(){


    this->hostName = myRecData;


}


void Client::sendOutgoingMessage(ISLH_msgs::outMessage msg, int msgIndx)
{
/*
    QByteArray data;

    QString temp = QString::number(msg.messageid);

    data.append(temp);

    int dataSize = data.size();

    QByteArray dat;

    if(msg.messageid == HMT_HELP_REQUEST )
        dat = makeDataPackage(RECV_HELP_REQUEST,dataSize,data);
    else
        dat = makeDataPackage(SEND_HELP_RESPONSE,dataSize,data);
*/

    QByteArray data;

    QString temp = QString::fromStdString(msg.message[msgIndx]) + "<EOF>";

    qDebug()<<temp;

    data.append(temp);

    //this->socket->waitForBytesWritten(2500);

    sendData(data);

}
