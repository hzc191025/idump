/*
 * dev_driver_9563.cpp
 *
 *  Created on: 2015年5月13日
 *      Author: hzc
 */

#include "dev_driver_9563.h"
#include <arpa/inet.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cstdio>

#define u_int32_t unsigned int
#define u_int16_t unsigned short

typedef struct{
    u_int32_t msgcode;
    u_int32_t msglen;
}LinkLayer80211PrismHeader;

typedef struct{
    u_int32_t did;
    u_int16_t status;
    u_int16_t len;
    u_int32_t data;
}LinkLayer80211PrismItem;

DevDriver9563::DevDriver9563() {

}

DevDriver9563::~DevDriver9563() {

}

template<typename T, typename U>
class ParseItem : public std::unary_function<T, void> {
private:
	U &mRxInfo;
public:
	ParseItem(U& rxInfo) : mRxInfo(rxInfo) {}

	void operator() (const LinkLayer80211PrismItem& item) {
		switch(item.did){
		case 0x44000100:// hosttime
			break;
		case 0x44000200:// mactime
			break;
		case 0x44000300:// channel
			mRxInfo.ri_channel = ntohl(item.data);
			break;
		case 0x44000400:// rssi
			mRxInfo.ri_power = ntohl(item.data);
			break;
		case 0x44000600:// signal
			break;
		case 0x44000800:// rate
			mRxInfo.ri_rate = ntohl(item.data);
			break;
		case 0x44000900:// istx
			break;
		case 0x44000a00:// frame len
			break;
		default:
			break;
		}
	}
};

int DevDriver9563::RadioTapParse(const unsigned char* buff, int len, struct rx_info *ri)
{
	// default remove crc tail
	LinkLayer80211PrismHeader* hdr = (LinkLayer80211PrismHeader*)buff;
	int hdrlen = ntohl(hdr->msglen);
	int datalen = ntohl(*(int*)(buff+hdrlen-4));

	LinkLayer80211PrismItem* item = (LinkLayer80211PrismItem*)(buff+sizeof(LinkLayer80211PrismHeader)+16);
	int end_pos = (hdrlen - sizeof(LinkLayer80211PrismHeader)-16) / sizeof(LinkLayer80211PrismItem);
	std::for_each(item, item+end_pos, ParseItem<LinkLayer80211PrismItem, struct rx_info>(*ri));

	return 0;
}
