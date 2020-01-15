#ifndef GPS_DATA_H
#define GPS_DATA_H

struct GPSDATA {

    unsigned char head1_ ;  //头1
    unsigned char head2_ ; //头2
    unsigned short  data_length_; //数据长度 不包括同步字、结束符、校验码
    char  fight_number_[8];  //航班号  8个字节ASCII码 （数据帧中从高到低顺序解析）
    char  aircratf_address_[8];   //飞机标识 （8个字节，只有前六个字节有用，数据帧中从高到低顺序解析）
    double longitude_;  //经度 （从高到低解析）8字节
    double latitude_; //纬度
    unsigned int gps_height_; //GPS高度
    unsigned int aircraft_height_; //飞机高度
    unsigned char day_; //日
    unsigned char month_; //月
    unsigned char year_; //年
    unsigned char hour_; //时
    unsigned char minute_; //分
    unsigned char second_; //秒
    unsigned short  millisecond_; //毫秒
    unsigned short check_code_; //校验码
    unsigned char end_; //结束符

   // 0a a0 32 00 43 45 53 35 31 35 38 20 37 38 30 43 41 31 00 00  b7 26 dd 96 c8 80 5d 40 e8 15 4f 3d d2 ac 3f 40 69 78 00 00 ac 71 00 00 0b 0a 0e 0d 1c 27 39 02 47 56 cc
};

#endif // GPS_DATA_H
