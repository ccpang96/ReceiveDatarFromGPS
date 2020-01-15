#include "include/mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <iostream>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    unsigned char head1 = 0x0a;
    unsigned char head2 = 0xa0;

    QByteArray data_buf;
    data_buf.append((char*)&head1, sizeof(head1));
    data_buf.append((char*)&head2, sizeof(head2));

    unsigned char *head1_ = (unsigned char*)data_buf.data();
    unsigned char *head2_ = (unsigned char*)data_buf.data() + 1;

   // data_pointer += 1;
    //char * head1 = (char*) data_pointer;

  //  std::cout <<"head1: " << *data_pointer << std::endl;
   qDebug()<<  "head1: = " << "binary: " << QString::number(*head1_,2) << "hex: " <<QString::number(*head1_,16) <<"\n" ;
   qDebug()<<  "head1: = " << "binary: " << QString::number(*head2_,2) << "hex: " <<QString::number(*head2_,16) <<"\n" ;

    */


    return a.exec();
}
