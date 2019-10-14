#include "ctb_device.h"

static class CtbDeviceUnit ctb_device;

int main(int argc, char *argv[])
{
	int i, ret, size;

	ctb_device.mBulkReadBuffer[0] = 0x55;
	ctb_device.mBulkReadBuffer[4] = 0x18;
	ctb_device.mBulkReadBuffer[8] = 0x2;
	ctb_device.mBulkReadBuffer[12] = 0xbb;
	ctb_device.mBulkReadBuffer[20] = 0x22;

	while (1) {
		ret = ctb_device.readData(ctb_device.mBulkReadBuffer, sizeof(ctb_device.mBulkReadBuffer));		
		LOGD("read len: %d ctb_device.mBulkReadBuffer: %s", ret, ctb_device.mBulkReadBuffer);
		if (ret > 0) {
			ret = ctb_device.writeData(ctb_device.mBulkWriteBuffer, sizeof(ctb_device.mBulkWriteBuffer));
			LOGD("write len: %d", ret);
		}
	
	}
	return 0;
}
