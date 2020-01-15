#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "include/udpclient.h"
#include <QNetworkInterface>
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //UDP网络通信部分
    void connectInit(); //UDP连接初始化
    void udp_connect();
    void udp_break();
    QHostAddress getLocalHostIP();

private:
    Ui::MainWindow *ui;

    bool _is_connect;  //
    QString _local_ip;
    QString _remote_ip;  //远程IP
    quint16 _remote_port;  //远程端口
    quint16 _local_port;  //本地端口
    UDPClient _client;



public slots:
        void on_connect_pushButton(); //UDP开始连接
};

#endif // MAINWINDOW_H
