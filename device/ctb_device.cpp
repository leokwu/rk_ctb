//
// Created by leok on 2019/10/12.
//

#include "ctb_device.h"


using namespace std;


CtbDeviceUnit::CtbDeviceUnit() {
	

}

CtbDeviceUnit::~CtbDeviceUnit() {
	
	
}

ssize_t CtbDeviceUnit::writeData(const void *buf, size_t len) {
	

}
ssize_t CtbDeviceUnit::readData(void *buf, size_t len) {
	int i, ret, size;
	char ep_path[32];

	int ep[2];

	fd_set rfds;
	fd_set wfds;

	void *buf_in, *buf_out;
	
	for (i = 0; i < 2; ++i) {
		sprintf(ep_path, "%s/ep%d", DEVICE_PATH, i+1);
		ep[i] = open(ep_path, O_RDWR | O_NONBLOCK);
		if (ep[i] < 0) {
			printf("unable to open ep%d: %s\n", i+1,
			       strerror(errno));
			return 1;
		}
	}
	
	/* alloc buffers and requests */
	buf_in = malloc(BUF_LEN);
	buf_out = malloc(BUF_LEN);
	while (1) {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(ep[0], &rfds);
		FD_SET(ep[1], &wfds);
		
		printf("----1\n");
		ret = select(((ep[0] > ep[1]) ? ep[0] : ep[1]) + 1,
				&rfds, &wfds, NULL, NULL);
		printf("----2\n");
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			/* customer must deal with error */
			perror("select");
			break;
		}
		if (FD_ISSET(ep[0], &rfds)) {
			size = read(ep[0], buf_in, BUF_LEN);
			printf("read size: %d buf_in: %s\n", size, buf_in);
		}
		printf("----3\n");
#if 0
		size = read(ep[0], buf_in, BUF_LEN);
		printf("read size: %d buf_in: %s\n", size, buf_in);
		// size = write(ep[1], buf_out, BUF_LEN);
		size = write(ep[1], "1111", 4);
		printf("write size: %d buf_out: %s\n", size, buf_out);
#endif
	}

	/* free resources */
	if (buf_in) {
		free(buf_in);
		buf_in = NULL;
	}
	if (buf_out) {
		free(buf_out);
		buf_out = NULL;
	}

	for (i = 0; i < 2; ++i) {
		if (ep[i] >= 0)
			close(ep[i]);
	}
	return 0;
}
