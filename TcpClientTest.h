#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TcpClientTest.h"
#include "Communication.h"


class TcpClientTest : public QMainWindow
{
    Q_OBJECT

public:
    TcpClientTest(QWidget *parent = nullptr);
    ~TcpClientTest();

private:
    Ui::TcpClientTestClass ui;

    Communication* m_tcpClient = nullptr;


    void on_ptn_send_clicked();
    void tcpSendData(QString mes);


private slots:
    void on_ptn_connect_clicked();
    void on_ptn_disconnect_clicked();
    void on_ptn_opcRead_clicked();

    void connectWarning();
    void disconnectWarning();

    void on_showMessage(QByteArray buffer);
};
