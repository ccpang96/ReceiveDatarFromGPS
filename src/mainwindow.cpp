#include "include/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _client()
{
    ui->setupUi(this);
    connectInit();  //UDP初始化
    connect(ui->connect_pushButton_, SIGNAL(clicked()), this, SLOT(on_connect_pushButton()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*********************第二部分：UDP**********************/

void MainWindow::connectInit() {

    /*
    _remote_ip = QString("192.168.62.10");
    _local_ip = getLocalHostIP().toString();
    _remote_port = (QString("3333")).toUShort();  //远程端口
    _local_port =  (QString("2000")).toUShort();  //本地端口
    */

    _remote_ip = QString("192.168.1.3");
    _local_ip = getLocalHostIP().toString();
    _remote_port = (QString("3333")).toUShort();  //远程端口
    _local_port =  (QString("2000")).toUShort();  //本地端口


    ui->remoteIP_->setText(_remote_ip);
    ui->localIP_->setText(_local_ip);
    ui->remoteport_->setRange(1024,9999);
    ui->remoteport_->setValue(_remote_port);
    ui->localport_->setRange(1024,9999);
    ui->localport_->setValue(_local_port);
    _is_connect = false;

    ui->state_label_->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label_->setPalette(pa);
    ui->connect_pushButton_->setText("连接网络");

    ui->localIP_->setEnabled(true);
    ui->remoteIP_->setEnabled(true);
    ui->remoteport_->setEnabled(true);
    ui->localport_->setEnabled(true);
    ui->localIP_->setEnabled(false);
}

/**
 * @brief MainWindow::on_connect_pushButton
 * @state:
 */

void MainWindow::on_connect_pushButton() {
    if (_is_connect) {
        udp_break();
    } else {
        udp_connect();
    }
}

void MainWindow::udp_break() {
    _is_connect = false;
    ui->state_label_->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label_->setPalette(pa);

    ui->connect_pushButton_->setText("连接网络");

    // 使能远程端口，本地端口，远程IP
    ui->remoteIP_->setEnabled(true);
    ui->remoteport_->setEnabled(true);
    ui->localport_->setEnabled(true);
    _client.udp_stop(); //UDP通信停止
}
/**
 * @brief MainWindow::on_udp_connect
 * 说明：连接网络
 */
void MainWindow::udp_connect() {
    qDebug("%s", __func__);
    //获得人修改的IP参数
    _remote_ip = ui->remoteIP_->text();
    _remote_port = ui->remoteport_->text().toUShort();
    _local_port = ui->localport_->text().toUShort();

    _is_connect = true;
    ui->state_label_->setText("通");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::blue);
    ui->state_label_->setPalette(pa);

    ui->connect_pushButton_->setText("断开网络");
    // 禁用远程端口，本地端口，远程IP
    ui->remoteIP_->setEnabled(false);
    ui->remoteport_->setEnabled(false);
    ui->localport_->setEnabled(false);

   // connect(ui->hand_send_, SIGNAL(clicked()), this, SLOT(on_hand_send()));
    _client.udp_start(QHostAddress(_local_ip), _local_port, QHostAddress(_remote_ip), _local_port);
}

/**
 * @brief MainWindow::getLocalHostIP
 * @return
 * @说明：获得本机的IP地址
 */
QHostAddress MainWindow::getLocalHostIP()
{
  QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
  QHostAddress result;
  foreach(QHostAddress address, AddressList){
      if(address.protocol() == QAbstractSocket::IPv4Protocol &&
         address != QHostAddress::Null &&
         address != QHostAddress::LocalHost){
          if (address.toString().contains("127.0.")){
            continue;
          }
          result = address;
          break;
      }
  }
  return result;
}
