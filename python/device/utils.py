"""1808 ctb devide control"""

import select
import os, sys
from log import *


logger = InitLog("ctb_device", "ctb_device.log")

class CtbDevice():
    def __init__(self):

        logger.debug("init")

        rfd = os.open("/dev/usb-ffs/ctb/ep1", os.O_RDWR | os.O_NONBLOCK)
        if rfd < 0:
            logger.error("open read node failed")
            sys.exit(-1)
        wfd = os.open("/dev/usb-ffs/ctb/ep2", os.O_RDWR | os.O_NONBLOCK)
        if wfd < 0:
            logger.error("open read node failed")
            sys.exit(-1)

        try:
            epoll_rfd = select.epoll()
            epoll_rfd.register(rfd, select.EPOLLIN)
        except select.error as error_rmsg:
            logger.error("error_rmsg rfd: \n", error_rmsg)

        try:
            epoll_wfd = select.epoll()
            epoll_wfd.register(wfd, select.EPOLLOUT)
        except select.error as error_wmsg:
            logger.error("error_wmsg rfd: \n", error_wmsg)

        logger.debug("init succeed")

    def ctb_read(self, len):
        logger.debug("read")
        read_data = os.read(self.rfd, len)
        logger.debug("read_data: \n", read_data)

        return read_data

    def ctb_write(self, write_data):
        logger.debug("write")
        write_len = os.write(self.wfd, write_data)
        logger.debug("the numbler of bytes: \n", write_len)
        return write_len

    def ctb_release(self):
        logger.debug("release resource")
        if self.rfd < 0:
            logger.error("open read node failed")
            os.close(self.rfd)
        if self.wfd < 0:
            logger.error("open read node failed")
            os.close(self.wfd)
