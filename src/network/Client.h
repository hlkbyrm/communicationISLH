// Client class implements communication class btw a server and its clients
// For each new client an instance of a client is created

#pragma once
#include <QtNetwork>
#include <QtCore/QByteArray>
#include <ISLH_msgs/outMessage.h>

#define INCOMING_CLIENT 0
#define OUTGOING_CLIENT 1
#define COMMA_OFFSET 3

//#define MESSAGE_STAR_OFFSET 4 // number of star characters in the received message
#define MESSAGE_DATA_PARTS 5  // number of data parts of the received message

class tcpComm;
enum HotspotMessageType
{
    HMT_NONE = -1,
    HMT_HELP_REQUEST = 0,
    HMT_HELPING = 1,
    HMT_NOT_HELPING = 2
};
class Client : public QObject
{
		Q_OBJECT
public:
	
    // Constructor for incoming Client
    Client(QTcpSocket* sock, int clientType, QObject* parent = 0);
	
    // Constructor for outgoing Client
    Client(int clientType, QObject* parent = 0);

    ~Client(void);
	

    // Socket for communication
    QTcpSocket* socket;

    // Pointer to the parent tcpComm class (redundant)
    tcpComm* TcpComm;

    // Return client IP
    QString getIP();

    void setIP(QString ip);

    // Return client hostname
    QString getHostName();

    void setHostName(QString Name);

    // Prepare package for sending data
    //QByteArray makeDataPackage(int task, int dataSize, QByteArray data); // Prepare the Data Package

    // Handle the incoming task
	void handleTask(int task, int dataSize);

    // Send the hostname to the outgoing client
	void sendHostName();

    // Receive the incoming client's hostname
    void receiveHostName();

    // Send the current robot information to the outgoing client
    //void sendRobotInfotoNeighbor(ISLH_msgs::robotInfo info);

    // Send the coordinator update to the outgoing client
    //void sendCoordinatorUpdatetoCoordinator(ISLH_msgs::neighborInfo info);

    // Sends the network information from coordinator to clients
    //void sendNetworkInfo(QStringList info);

    // Sends the network information from coordinator to clients
    //void receiveNetworkInfo();

    // Receive the coordinator update from the incoming client
    //void receiveCoordinatorUpdateFromClient();

    // Receive the robot info from the incoming client
    //void receiveRobotInfoFromNeighbor();

    // Send incoming hotspot related message to outgoing client
    void sendOutgoingMessage(ISLH_msgs::outMessage msg, int msgIndx);

    void receiveMessage();

	void receiveAcknowledge();

	void sendAcknowledge(bool status);

	long speedCounter;

	bool speedTest;
	
private:

    QVector<QByteArray> waitingMessages;
    bool written;


    // 0: incoming client, 1: outgoing client
    int type;

    QString IP;

	QString hostName;
	
	QAbstractSocket::SocketError clientSocketError;

    // Received and Sent data
    QString myRecData, mySentData;

	QByteArray myRecDataBA;

    //long imageDataSize;
	
	public slots:

    void sendData(QByteArray data);
    void bytesWritten(qint64 byteNum);
		void receiveData();
		void displaySocketError(QAbstractSocket::SocketError socketError);
		void getSocketDisconnected();

		signals:

        void clientDisconnected(int type);
        void sendClientInfo(const QList<QString>& list, int type);
		void acknowledgeReceived(int id);
        //void neighborInfo(ISLH_msgs::robotInfo info);
        //void coordinatorUpdate(ISLH_msgs::neighborInfo info);
        //void networkInfo(QStringList list);
        void incomingMessage(QString message);
};
