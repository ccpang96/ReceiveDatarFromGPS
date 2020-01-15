#include "include/udpclient.h"
#include <QMessageBox>
#include <QNetworkInterface>

/**
 * @brief UDPClient::UDPClient
 * @param parent
 * 说明：构造函数
 */
UDPClient::UDPClient(QObject *parent) :
    QObject(parent)
{
    qDebug("%s", __func__);
}


/**
 * @brief UDPClient::udpStart
 * @param localIp
 * @param listnerPort
 * @param remoteIp
 * @param remotePort
 * 说明：  启动UDP服务
 */
void UDPClient::udp_start(const QHostAddress &localIp, const quint16 &listnerPort, const QHostAddress &remoteIp, const quint16 &remotePort)
{
    Q_UNUSED(remoteIp);
    Q_UNUSED(remotePort);
    qDebug("%s", __func__);

    //send
    if(udpSendSocket == nullptr) {
        udpSendSocket = new QUdpSocket(this);
        qDebug() << "create udpSendSocket" <<endl;
        connect(udpSendSocket, SIGNAL(readyRead()), this, SLOT(on_ready_send_back())); //发送端口得到回传的数据
        connect(udpSendSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_connection_error(QAbstractSocket::SocketError))); //连接错误
    }

    //receive
    udpListnerSocket = new QUdpSocket(this);
    if(!udpListnerSocket->bind(localIp, listnerPort)) {
        qWarning("NULLcc");
    }
    connect(udpListnerSocket, SIGNAL(readyRead()), this, SLOT(on_ready_listen_come()));
    connect(udpListnerSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_connection_error(QAbstractSocket::SocketError)));
}




/**
 * @brief UDPClient::on_receiveResult
 * @说明： 用于接收工作线程处理好的数据
 */
void UDPClient::on_receiveResult(const QString &str) {


    qDebug() << str <<endl;
    qDebug() << "track complete! " <<endl;
    emit draw_track_data();

}


/**
 * @brief MyUDP::sendData
 * @param string   发送内容
 * @param remoteIp 目标IP地址
 * @param port     目标端口号
 * @说明：发送字符串
 */
void UDPClient::send_data(const QString &string, const QString &remoteIp, const quint16 &port)
{
    Q_UNUSED(remoteIp);
    qDebug("%s %d", __func__, __LINE__);
    if(udpSendSocket == nullptr) {
        qDebug() << "udpSendSocket is null" <<endl;
        return;
    }

    qDebug("%s %d", __func__, __LINE__);
    QByteArray send_data;
    send_data.append(string);
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, interfaceList) {
        QList<QNetworkAddressEntry> entryList = interface.addressEntries();
        foreach(QNetworkAddressEntry entry , entryList) {
            QString str = entry.broadcast().toString();
            if (str != "") {
                qDebug() << "send data's size = " << send_data.size() <<endl;
                udpSendSocket->writeDatagram(send_data, QHostAddress(str), port);
                qDebug() << "send succes" <<endl;
             }

        }
    }
}

/**
 * @brief UDPClient::sendData
 * @param send_data   发送内容
 * @param remoteIp 目标IP地址
 * @param port     目标端口号
 * @说明： 发送的是字节
 */
void UDPClient::send_data(const QByteArray &send_data, const QString &remoteIp, const quint16 &port) {
    if (udpSendSocket == nullptr) {
       qDebug() << "udpSendSocket is null" <<endl;
       return;
    }
    udpSendSocket->writeDatagram(send_data, QHostAddress(remoteIp), port);
}

/**
 * @brief UDPClient::readySendRead
 * 说明：处理接收到的数据，是发送端口返回的
 */
void UDPClient::on_ready_send_back()
{
    qDebug("%s", __func__);
    read_receive_data(udpSendSocket);
}

/**
 * @brief UDPClient::readyListnerRead
 * 监听读到数据
 */
void UDPClient::on_ready_listen_come()
{
    qDebug("%s", __func__);
    read_receive_data(udpListnerSocket);
}

/**
 * @brief UDPClient::readyRead
 * @param socket
 * 读取数据
 */
void UDPClient::read_receive_data(QUdpSocket* socket)
{
    qDebug("%s", __func__);
    while (socket->hasPendingDatagrams()) { //hasPendingDatagrams返回true时表示至少有一个数据报在等待被读取
        QByteArray data_buffer;  //temp data buffer to save receive data
        data_buffer.resize(socket->pendingDatagramSize());   //pendingDatagramSize为返回第一个在等待读取报文的size，resize函数是把datagram的size归一化到参数size的大小一样

        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(data_buffer.data(), data_buffer.size(), &sender, &senderPort);

        qDebug() << "Message from:" << sender.toString();
        qDebug() << "Message port:" << senderPort;
        //qDebug() << "Message: " << data_buffer;

        //取出一个完整的包
        process_receive_data(data_buffer); //处理一下数据，提取一个完整的包
    }
}

/**
 * @brief UDPClient::process_receive_data
 * @param data_buf
 * @说明： 处理测量数据的函数
 */
void UDPClient::process_receive_data(QByteArray &data_buf) {


    unsigned char *head1 = (unsigned char*)data_buf.data(); //head1
    unsigned char *head2 = (unsigned char*)data_buf.data() + 1; //head2
    if(*head1 == 0x0a && *head2 == 0xa0) {
        gps_data_.head1_ = *head1;
        gps_data_.head2_ = *head2;

        unsigned char *data_length1 = (unsigned char*)data_buf.data() + 3;
        unsigned char *data_length2 = (unsigned char*)data_buf.data() + 2;
        gps_data_.data_length_ = *data_length1 << 8 | *data_length2;   //此处存疑 数据长度

        //航班号
        char *fight_number1 = (char*)data_buf.data() + 4;
        gps_data_.fight_number_[0] = *fight_number1;
         char *fight_number2 = ( char*)data_buf.data() + 5;
        gps_data_.fight_number_[1] = *fight_number2;
         char *fight_number3 = ( char*)data_buf.data() + 6;
        gps_data_.fight_number_[2] = *fight_number3;
         char *fight_number4 = ( char*)data_buf.data() + 7;
        gps_data_.fight_number_[3] = *fight_number4;
         char *fight_number5 = ( char*)data_buf.data() + 8;
        gps_data_.fight_number_[4] = *fight_number5;
         char *fight_number6 = ( char*)data_buf.data() + 9;
        gps_data_.fight_number_[5] = *fight_number6;
         char *fight_number7 = ( char*)data_buf.data() + 10;
        gps_data_.fight_number_[6] = *fight_number7;
         char *fight_number8 = ( char*)data_buf.data() + 11;
        gps_data_.fight_number_[7] = *fight_number8;

        QString fight_number = QString::fromLocal8Bit(gps_data_.fight_number_, 8);


        //飞机唯一标识
         char *aircratf_address1 = ( char*)data_buf.data() + 12;
        gps_data_.aircratf_address_[0] = *aircratf_address1;
         char *aircratf_address2 = ( char*)data_buf.data() + 13;
        gps_data_.aircratf_address_[1] = *aircratf_address2;
         char *aircratf_address3 = ( char*)data_buf.data() + 14;
        gps_data_.aircratf_address_[2] = *aircratf_address3;
         char *aircratf_address4 = ( char*)data_buf.data() + 15;
        gps_data_.aircratf_address_[3] = *aircratf_address4;
         char *aircratf_address5 = ( char*)data_buf.data() + 16;
        gps_data_.aircratf_address_[4] = *aircratf_address5;
         char *aircratf_address6 = ( char*)data_buf.data() + 17;
        gps_data_.aircratf_address_[5] = *aircratf_address6;
         char *aircratf_address7 = ( char*)data_buf.data() + 18;
        gps_data_.aircratf_address_[6] = *aircratf_address7;
         char *aircratf_address8 = ( char*)data_buf.data() + 19;
        gps_data_.aircratf_address_[7] = *aircratf_address8;

        QString aircratf_address = QString::fromLocal8Bit(gps_data_.aircratf_address_, 6);

        //经度
        double longitude;
        memcpy(&longitude, data_buf.data() + 20, 8);
        gps_data_.longitude_ = longitude;

        //纬度
        double latitude;
        memcpy(&latitude, data_buf.data() + 28, 8);
        gps_data_.latitude_ = latitude;

        //GPS高度
         quint32 gps_height;
         memcpy(&gps_height, data_buf.data() + 36, 4);
         gps_data_.gps_height_ = gps_height;

         //飞机高度
         quint32 aircraft_height;
         memcpy(&aircraft_height, data_buf.data() + 40, 4);
         gps_data_.aircraft_height_ = aircraft_height;

        //日 月 年 时 分 秒
         unsigned char *day = (unsigned char*)data_buf.data() + 44;
         gps_data_.day_ = *day;
         unsigned char *month = (unsigned char*)data_buf.data() + 45;
         gps_data_.month_ = *month;
         unsigned char *year = (unsigned char*)data_buf.data() + 46;
         gps_data_.year_ = *year;
         unsigned char *hour = (unsigned char*)data_buf.data() + 47;
         gps_data_.hour_ = *hour;
         unsigned char *minute = (unsigned char*)data_buf.data() + 48;
         gps_data_.minute_ = *minute;
         unsigned char *second = (unsigned char*)data_buf.data() + 49;
         gps_data_.second_ = *second;

         //毫秒
         quint16 millisecond,millisecond_1,millisecond_2;
         unsigned char* millisecond1 = (unsigned char*)data_buf.data() + 50;
         unsigned char* millisecond2 = (unsigned char*)data_buf.data() + 51;
         millisecond_1 = *millisecond1;
         millisecond_2 = *millisecond2;
         millisecond = millisecond_2 << 8 | millisecond_1;
         gps_data_.millisecond_ = millisecond;

         //校验码
         quint16 check_code,check_code_1,check_code_2;
         unsigned char * check_code1 = (unsigned char*)data_buf.data() + 52;
         unsigned char * check_code2 = (unsigned char*)data_buf.data() + 53;
         check_code_1 = *check_code1;
         check_code_2 = *check_code2;
         check_code = check_code_2 << 8 | check_code_1;
         gps_data_.check_code_ = check_code;
         //结尾
         unsigned char* end = (unsigned char*)data_buf.data() + 54;
         gps_data_.end_ = *end;

         qDebug() << "同步头：" << QString::number(gps_data_.head1_,16) << " "<< QString::number(gps_data_.head2_,16)  \
                  <<"报文长度：" << gps_data_.data_length_ << "航班号：" <<fight_number << "飞机标识：" \
                  << aircratf_address << "经度 ：" << gps_data_.longitude_ << "纬度： " << gps_data_.latitude_ \
                  << "GPS高度：" << gps_data_.gps_height_ << "飞机高度：" << gps_data_.aircraft_height_ <<endl;

        qDebug() << gps_data_.year_ << "年" << gps_data_.month_ << "月"<< gps_data_.day_ << "日"<<gps_data_.hour_<<"时" \
                 <<gps_data_.minute_<<"分"<< gps_data_.second_<<"秒" << gps_data_.millisecond_<<"毫秒" \
                 << "校验码："<<gps_data_.check_code_ << "结尾：" <<QString::number(gps_data_.end_,16) << endl;


    }
}

/**
 * @brief MyUDP::connection_error
 * @param err
 * 说明：处理网络连接错误
 */
void UDPClient::on_connection_error(QAbstractSocket::SocketError err)
{
    qDebug("%s", __func__);
    switch(err){
    case 0:QMessageBox::critical(nullptr,"connection error","The connection was refused by the peer (or timed out).",QMessageBox::Ok);
           break;
    case 2:QMessageBox::critical(nullptr,"connection error","The host address was not found.",QMessageBox::Ok);
           break;
    case QAbstractSocket::NetworkError:QMessageBox::critical(nullptr,"connection error","An error occurred with the network .",QMessageBox::Ok);
           break;
    case QAbstractSocket::RemoteHostClosedError:
           QMessageBox::critical(nullptr,"connection error","disconnect .",QMessageBox::Ok);
           break;
    default:QMessageBox::critical(nullptr,"connection error","undefine error.",QMessageBox::Ok);
        qDebug()<<"error is ......."<<err;
        break;
    }
}


/**
 * @brief UDPClient::udpStop
 * @param string
 * @param remoteIp
 * @param port
 * 停止UDP服务
 */
void UDPClient::udp_stop()
{
    qDebug("%s", __func__);
    // 关闭发送Socket
    if(udpSendSocket != nullptr) {
        udpSendSocket->close();
        udpSendSocket = nullptr;
    }
    // 关闭Listner
    if(udpListnerSocket != nullptr) {
        udpListnerSocket->close();
        udpListnerSocket = nullptr;
    }

}






