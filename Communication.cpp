#pragma execution_character_set("utf-8")
#include "Communication.h"
#include <QDebug>

Communication::Communication(QObject *parent)
	: QObject(parent)
{
	connect(this, &Communication::_signalOpenServer, this, &Communication::openServer);
	connect(this, &Communication::_signalOpenClient, this, &Communication::openClient);
	
	//多线程
	/*m_thread = new QThread(this);
	this->moveToThread(m_thread);
	m_thread->start();*/
	//QObject放在QThread中，由信号控制函数调用实现多线程

}


Communication::~Communication()
{
	if (m_tcpSocket != nullptr)
	{
		m_tcpSocket->close();
		delete m_tcpSocket;
		m_tcpSocket = nullptr;
	}
	if (m_tcpServer)
	{
		m_tcpServer->close();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}

}

void Communication::setIpAndPort(qint16 port, QString ip) {
	m_servantIP = ip;
	m_port = port;
}


void Communication::openServer()
{
	//服务器端建立通信
	qDebug() << "communication from thread:" << QThread::currentThreadId();
	if (m_tcpSocket != nullptr)
	{
		m_tcpSocket->close();
		delete m_tcpSocket;
		m_tcpSocket = nullptr;
	}
	if (m_tcpServer)
	{
		m_tcpServer->close();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
	m_tcpServer = new QTcpServer(this);
	//辅站监听信号
	m_tcpServer->listen(QHostAddress::Any, m_port);
		
	
	connect(m_tcpServer, &QTcpServer::newConnection, [=]()
		{
			//通信连接成功
			m_connectFlag = true;
			emit connected();
			m_tcpSocket = m_tcpServer->nextPendingConnection();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//通信断开连接
					m_connectFlag = false;
					emit disconnected();
				});
			connect(m_tcpSocket, &QTcpSocket::readyRead,
				[=]()
				{
					//获取来自主站的信息
					getMessage();
				});
		});

}


void Communication::openClient()
{
	//建立主站通信
	if (m_tcpSocket)
	{
		m_tcpSocket->close();
		delete m_tcpSocket;
		m_tcpSocket = nullptr;
	}
	if (m_tcpServer)
	{
		m_tcpServer->close();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
	m_tcpSocket = new QTcpSocket(this);
	connect(m_tcpSocket, &QTcpSocket::readyRead,
		[=]()
		{
			//获取来自辅站的信息
			getMessage();
		});
	connect(m_tcpSocket, &QTcpSocket::connected,
		[=]()
		{
			//通信连接成功
			m_connectFlag = true;
			emit connected();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//通信断开连接
					m_connectFlag = false;
					emit disconnected();
				}
			);	
			

		}
	);
	//连接辅站
	//m_tcpSocket->connectToHost(QHostAddress(m_servantIP), m_port);
	m_tcpSocket->connectToHost(QHostAddress(m_servantIP), m_port);	
	
	m_tcpSocket->waitForConnected();
}


void Communication::getMessage()
{
	//从通信中取出内容
	if (m_tcpSocket)
	{
		QByteArray mes = m_tcpSocket->readAll();
		if (mes == MES_OPEN)
		{
			//打开程序
			emit signalOpen();
		}
		else if (mes == MES_CLOSE)
		{
			//关闭程序
			emit signalClose();
		}
		else
		{
			m_readData.append(mes);
			QDateTime current_date_time = QDateTime::currentDateTime();
			QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
			qDebug() << "Communication data size:" << m_readData.size();
			qDebug() << "Communication time:" << current_date;
			//提取数据
			if (m_readData.size())
			{
				int dataBufferSize = m_readData.size();
				QByteArray ba;
				ba.resize(dataBufferSize);
				memcpy(ba.data(), m_readData, dataBufferSize);
				m_ByteArrayVec.push_back(ba);
				m_readData.remove(0, dataBufferSize);
				emit _signalReceData(m_readData);
			}
		}
	}

}


void Communication::breakConnect()
{
	//断开与服务器连接
	if (m_connectFlag)
	{
		//m_tcpSocket->close();
		m_tcpSocket->disconnectFromHost();
		m_tcpSocket->waitForDisconnected();
	}
	/*if (m_thread)
	{
		m_thread->quit();
		m_thread->wait();
	}*/
}


//发送数据
void Communication::sendData(const QByteArray sendByteArray)
{
	m_writeData.clear();
	m_writeData.append(sendByteArray);
	emit _signalSendData();
}