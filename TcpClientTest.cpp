#include "TcpClientTest.h"
#include <QMessageBox>

TcpClientTest::TcpClientTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_tcpClient = new Communication(this);

	//通信
	connect(ui.ptn_connect, &QPushButton::clicked, this, &TcpClientTest::on_ptn_connect_clicked);
	connect(ui.ptn_disconnect, &QPushButton::clicked, this, &TcpClientTest::on_ptn_disconnect_clicked);
	connect(ui.ptn_send, &QPushButton::clicked, this, &TcpClientTest::on_ptn_send_clicked);
	connect(m_tcpClient, &Communication::connected, this, &TcpClientTest::connectWarning);
	connect(m_tcpClient, &Communication::disconnected, this, &TcpClientTest::disconnectWarning);
	connect(m_tcpClient, &Communication::_signalReceData, this, &TcpClientTest::on_showMessage);
}

void TcpClientTest::on_showMessage(QByteArray buffer)
{
	buffer = m_tcpClient->getReceiveData();

	if (buffer.size() == 0)
		return;

	QDateTime current_date_time = QDateTime::currentDateTime();
	QString showStr = "Server(" + current_date_time.toString("hh:mm:ss.zzz") + "):";
	ui.textEdit_receive->append(showStr + QString::fromLocal8Bit(buffer));
}

void TcpClientTest::connectWarning()
{	
	ui.textEdit_receive->append("Connection is successful!");
}

void TcpClientTest::disconnectWarning()
{
	ui.textEdit_receive->append("Disconnection is successful!");
}


TcpClientTest::~TcpClientTest()
{
	if (m_tcpClient != nullptr)
	{
		if (m_tcpClient->isConnected())
		{
			m_tcpClient->breakConnect();
			delete m_tcpClient;
			m_tcpClient = nullptr;
		}

	}
	

}

void TcpClientTest::on_ptn_connect_clicked()
{

	QString ip = ui.lineEdit_ip->text();
	int port = ui.lineEdit_port->text().toInt();
	bool m_isLocalHost = true;
	m_tcpClient->setLocalHostStatus(m_isLocalHost);

	m_tcpClient->setIpAndPort(port, ip);
	m_tcpClient->openClient();
}

void TcpClientTest::on_ptn_disconnect_clicked()
{
	//断开连接
	m_tcpClient->breakConnect();
	ui.ptn_connect->setEnabled(true);
	//ui.ptn_disconnect->setEnabled(false);
}

void TcpClientTest::on_ptn_opcRead_clicked()
{
	QMessageBox::information(NULL,"Warning", m_tcpClient->getReceiveData());

}

void TcpClientTest::on_ptn_send_clicked() {


	QString text = ui.lineEdit_send->text();
	tcpSendData(text);
}

void TcpClientTest::tcpSendData(QString mes)
{
	if (!m_tcpClient->isConnected())
		return;
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString showStr = "Client(" + current_date_time.toString("hh:mm:ss.zzz") + "):";
	m_tcpClient->sendMessage(mes);
	ui.textEdit_receive->append(showStr + mes);
}
