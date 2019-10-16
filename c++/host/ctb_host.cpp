// felix

#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include "slog.h"
extern "C"
{
#include <libusb-1.0/libusb.h>
}

#define USB_MAX_PAYLOAD (1024 * 1024)


#define BUF_LEN         1024 * 1024

uint8_t mBulkReadBuffer[BUF_LEN];
uint8_t mBulkWriteBuffer[BUF_LEN];


struct CTB_PACKET_READ {
        uint32_t pkt_head;              // pkt head
        uint32_t pkt_length;            // pkt size (with self)
        uint32_t pkt_type;              // read
        uint8_t  pkt_session[8];        // is it the same interaction
        uint8_t  pkt_data[];            // data
};


struct CTB_PACKET_WRITE {
        uint32_t pkt_head;              // pkt head
        uint32_t pkt_length;            // pkt size (with self)
        uint32_t pkt_type;              // write
        uint8_t  pkt_session[8];        // is it the same interaction
        uint8_t  pkt_data[];            // data
};


int usbRead(libusb_device_handle *handle, uint8_t addr, void *data, int len)
{
    char *buf = static_cast<char *>(data);
    int orig_len = len;
    while (len > 0)
    {
        int read_len = std::min(USB_MAX_PAYLOAD, len);
        int n = 0;
        int ret = libusb_bulk_transfer(handle, addr, (unsigned char *)buf, read_len, &n, 500);
        if (ret == LIBUSB_ERROR_TIMEOUT)
        {
            printf("read time out \n");
            return 0;
            // continue;
        }
        else if (ret < 0)
        {
            printf("usb read failed: ret = %d: %s\n", ret, libusb_error_name((libusb_error)ret));
            return ret;
        }
        buf += n;
        len -= n;
    }
    return orig_len;
}

int usbWrite(libusb_device_handle *handle, uint8_t addr, const void *data, int len)
{
    const char *buf = static_cast<const char *>(data);
    int orig_len = len;
    while (len > 0)
    {
        int write_len = std::min(USB_MAX_PAYLOAD, len);
        int n = 0;
        int ret = libusb_bulk_transfer(handle, addr, (unsigned char *)buf, write_len, &n, 500);
        if (ret == LIBUSB_ERROR_TIMEOUT)
        {
            printf("write timeout \n");
            return 0;
            // continue;
        }
        else if (ret < 0)
        {
            if (ret == -99) {
                printf("usb write failed!, usb ret = %d: %s, len = %d, n = %d, write_len = %d\n", ret, libusb_error_name((libusb_error)ret), len, n, write_len);
                if (n == 0) {
                    buf += write_len;
                    len -= write_len;
                } else {
                    buf += n;
                    len -= n;
                }
            } else {
                printf("usb write failed: ret = %d: %s\n", ret, libusb_error_name((libusb_error)ret));
                return ret;
            }
            printf("usb write retry, remain len = %d\n", len);
            //abort();
            continue;
        }
        buf += n;
        len -= n;
    }
    return orig_len;
}

int main(int argc, char **argv)
{
    if (libusb_init(NULL) != 0)
        return -1;
    libusb_device **usb_devs = NULL;
    if (libusb_get_device_list(NULL, &usb_devs) < 0)
        return -1;
    libusb_device *device = NULL;
    int n = 0;
    uint8_t addr_in = 0;
    uint8_t addr_out = 0;
    bool found_ctb = false;
    int interface_num = 0;
    while ((device = usb_devs[n++]) != NULL)
    {
        struct libusb_device_descriptor device_desc;
        if (libusb_get_device_descriptor(device, &device_desc) < 0)
            continue;
        libusb_config_descriptor *config = NULL;
        if (libusb_get_active_config_descriptor(device, &config) != 0)
            continue;
        for (interface_num = 0; interface_num < config->bNumInterfaces; ++interface_num)
        {
            const libusb_interface_descriptor &interface_desc = config->interface[interface_num].altsetting[0];
            if (interface_desc.bInterfaceClass != 0x18 || interface_desc.bInterfaceSubClass != 0x08 || interface_desc.bInterfaceProtocol != 0x2)
                continue;
            printf("interface_desc.bInterfaceClass: %d interface_desc.bInterfaceSubClass: %d interface_desc.bInterfaceProtocol: %d interface_desc.bNumEndpoints: %d\n", interface_desc.bInterfaceClass, interface_desc.bInterfaceSubClass, interface_desc.bInterfaceProtocol, interface_desc.bNumEndpoints);
            for (int i = 0; i < interface_desc.bNumEndpoints; ++i)
            {
                const uint8_t addr = interface_desc.endpoint[i].bEndpointAddress;
                printf("addr: %d\n", addr);
                if ((addr & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_OUT)
                    addr_out = addr;
                else
                    addr_in = addr;
            }
            printf("addr_out: %d addr_in: %d\n", addr_out, addr_in);
            found_ctb = true;
            break;
        }
        if (found_ctb)
            break;
    }
    if (!found_ctb)
        return -1;
    libusb_device_handle *handle = NULL;
    if (libusb_open(device, &handle) != 0)
        return -1;
    libusb_ref_device(device);
    libusb_free_device_list(usb_devs, 1);
    if (libusb_claim_interface(handle, interface_num) < 0)
        return -1;
    printf("interface_num = %d, addr_in = %x, addr_out = %d\n", interface_num, addr_in, addr_out);
    // uint32_t packet[256];
    while (true)
    {
        mBulkWriteBuffer[0] = 0x44;
        mBulkWriteBuffer[4] = 0x18;
        mBulkWriteBuffer[8] = 0x1;
        mBulkWriteBuffer[12] = 0xaa;
        mBulkWriteBuffer[20] = 0x11;

        printf("write size = %ld\n", sizeof(mBulkWriteBuffer));
        if (usbWrite(handle, addr_out, mBulkWriteBuffer, sizeof(mBulkWriteBuffer)) < 0)
            return -1;

        printf("read size = %ld\n", sizeof(mBulkReadBuffer));

        if (usbRead(handle, addr_in, mBulkReadBuffer, sizeof(mBulkReadBuffer)) < 0)
            return -1;
        auto read_data = (CTB_PACKET_READ *) mBulkReadBuffer;
        DUMPDATA(mBulkReadBuffer, read_data->pkt_length);
    }
    return 0;
}
