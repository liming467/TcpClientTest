#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <QThread>
#include <QDataStream>


//通信宏
#define SIZE_MAXBYTEWRITE    50000
#define	MES_OPEN     	     "OPEN"
#define	MES_CLOSE	         "CLOSE"
#define MES_COMPLETED        "COMPLETED"

class Communication :
	public QObject
{
	Q_OBJECT
public:
	//通信构造函数
	Communication(QObject *parent = Q_NULLPTR);
	~Communication();

	void setIpAndPort(qint16 port, QString ip = "192.168.1.45");

	void openServer();
	void openClient();

	void breakConnect();//断开连接
	inline bool isConnected() noexcept { return m_connectFlag; }

	inline QByteArray getReceiveData() {
		QByteArray temp;
		int size = m_ByteArrayVec.size();
		if ( size == 0)
			return temp;
		temp = m_ByteArrayVec[size - 1];
		m_ByteArrayVec.pop_back();
		return temp;
	}

	void sendMessage(QString me) {
		//获取文本框内容并以ASCII码形式发送	
		QString add = "\r\n";
		QString aa = me + add;
		m_tcpSocket->write(aa.toLatin1());
		m_tcpSocket->flush(); //当即发送缓冲区的数据，不需等待
	}

	//发送大数据
	void sendData(const QByteArray sendByteArray);

	/*void sendServerMessage(QString str) {
		m_tcpSocket->write(str.toLocal8Bit(), str.toLocal8Bit().length());
		m_tcpSocket->flush();
	}*/	


signals:
	///外部信号
	//通信连接成功信号
	void connected();
	//通信断开连接信号
	void disconnected();

	void signalOpen();
	void signalClose();
	void _signalSendData();

	void _signalOpenServer();
	void _signalOpenClient();
	void _signalReceData(QByteArray aa);

private:
	

	

	void getMessage();

	


	QString                                m_servantIP{};
	qint16                                 m_port{};//端口
	QTcpServer*		                       m_tcpServer = nullptr;
	QTcpSocket*                            m_tcpSocket = nullptr;
	QByteArray                             m_writeMessage{};//需要传出的命令
	QByteArray                             m_writeData{}; //需要传出的数据
	QByteArray                             m_readData{};//读取的数据
	QVector<QByteArray>                    m_ByteArrayVec;//数据缓冲区
	bool                                   m_connectFlag = false;//
	//QThread*                               m_thread = nullptr;
	bool                                   m_isLocalHost = false;
};

