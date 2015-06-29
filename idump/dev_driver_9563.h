/*
 * dev_driver_9563.h
 *
 *  Created on: 2015年5月13日
 *      Author: hzc
 */

#ifndef DRIVER_DEV_DRIVER_9563_H_
#define DRIVER_DEV_DRIVER_9563_H_

#include "stdint.h"

struct rx_info {
        uint64_t ri_mactime; // MAC时间
        int32_t ri_power; // 信号值
        int32_t ri_noise; // 噪音
        uint32_t ri_channel; // 频道
        uint32_t ri_freq; // 频率 2.412
        uint32_t ri_rate; // 速率 1M
        uint32_t ri_antenna; // 天线，标志
}  __attribute__ ((__packed__));

class DevDriver9563  {
public:
	DevDriver9563();
	virtual ~DevDriver9563();

public:
	virtual int RadioTapParse(const unsigned char* buff, int len, struct rx_info *rx_info);
};

#endif /* DRIVER_DEV_DRIVER_9563_H_ */
