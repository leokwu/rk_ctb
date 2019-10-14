//
// Created by leok on 2019/10/12.
//

#ifndef CTB_DEVICE_H
#define CTB_DEVICE_H


#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/eventfd.h>

#include "slog.h"

#define BUF_LEN         1024 * 1024
#define DEVICE_PATH "/dev/usb-ffs/ctb"


struct CTB_PACKET_READ {
	uint32_t pkt_head;      	// pkt head
	uint32_t pkt_length;    	// pkt size (with self)
	uint32_t pkt_type;      	// read
	uint8_t  pkt_session[8];	// is it the same interaction
	uint8_t  pkt_data[];    	// data
};


struct CTB_PACKET_WRITE {
	uint32_t pkt_head;              // pkt head
	uint32_t pkt_length;            // pkt size (with self)
	uint32_t pkt_type;              // write
	uint8_t  pkt_session[8];        // is it the same interaction
	uint8_t  pkt_data[];            // data
};



class CtbDeviceUnit {
public:
	CtbDeviceUnit();
	~CtbDeviceUnit();
	uint8_t mBulkReadBuffer[BUF_LEN];
	uint8_t mBulkWriteBuffer[BUF_LEN];

	ssize_t readData(void *buf, size_t len);
	ssize_t writeData(void *buf, size_t len);

protected:

private:
	int mEp[2];
	fd_set mRfds, mWfds;	
};

#endif // CTB_DEVICE_H
