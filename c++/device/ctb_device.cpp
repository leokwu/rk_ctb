//
// Created by leok on 2019/10/12.
//

#include "ctb_device.h"

using namespace std;


CtbDeviceUnit::CtbDeviceUnit() {

	mEp[0] = -1;
	mEp[1] = -1;	
	int i;
	char ep_path[32];
	
	for (i = 0; i < 2; ++i) {
		sprintf(ep_path, "%s/ep%d", DEVICE_PATH, i+1);
		mEp[i] = open(ep_path, O_RDWR | O_NONBLOCK);
		if (mEp[i] < 0) {
			LOGE("unable to open ep%d: %s\n", i+1,
					strerror(errno));
			exit(-1);
		}
	}	
}

CtbDeviceUnit::~CtbDeviceUnit() {
	int i;

	for (i = 0; i < 2; ++i) {
		if (mEp[i] >= 0)
			close(mEp[i]);
	}	
}

ssize_t CtbDeviceUnit::writeData(void *buf,  size_t len) {
	size_t i, ret, size;
	ssize_t write_bytes = 0;
	size_t write_len = 0;


	FD_ZERO(&mWfds);
	FD_SET(mEp[1], &mWfds);

	LOGD("----1-1\n");
	ret = select(mEp[1] + 1,
			NULL, &mWfds, NULL, NULL);
	LOGD("----2-2 select ret: %d mEp[1]: %d\n", ret, mEp[1]);
	if (ret < 0) {
		if (errno == EINTR)
			return 0;
		/* customer must deal with error */
		LOGE("write select");
		exit(-1);
	}
	if (FD_ISSET(mEp[1], &mWfds)) {



		while (write_len < len) {
			if ((write_bytes = write(mEp[1], buf + write_len,
								   len - write_len)) == -1) {
				LOGE("write error: %s\n", strerror(errno));
				goto exit;
			}
			write_len += write_bytes;
		}
		LOGD("write size: %d buf: %s error: %s\n", size, buf, strerror(errno));
	}
	LOGD("----3-3\n");

exit:
	return write_len;
}

ssize_t CtbDeviceUnit::readData(void *buf, size_t len) {
	size_t i, ret, size;
	ssize_t read_bytes = 0;
	size_t read_len = 0;

	/* alloc buffers and requests */
	FD_ZERO(&mRfds);
	FD_SET(mEp[0], &mRfds);

	LOGD("----1\n");
	ret = select(mEp[0] + 1,
			&mRfds, NULL, NULL, NULL);
	LOGD("----2 select ret: %d mEp[0]: %d\n", ret, mEp[0]);
	if (ret < 0) {
		if (errno == EINTR)
			return 0;
		/* customer must deal with error */
		LOGE("read select");
		exit(-1);
	}
	if (FD_ISSET(mEp[0], &mRfds)) {


		while (read_len < len) {
			if ((read_bytes = read(mEp[0], buf + read_len,
								   len - read_len)) == -1) {
				LOGE("read error: %s\n", strerror(errno));
				goto exit;
			}
			read_len += read_bytes;
		}
		LOGD("read size: %d buf: %s error: %s\n", size, buf, strerror(errno));
	}
	LOGD("----3\n");

exit:
	return read_len;
}
