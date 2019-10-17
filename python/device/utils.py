"""1808 ctb devide control"""

import select
import os, sys
from log import *


logger = InitLog("ctb_device", "ctb_device.log")

class CtbDevice():
    def __init__(self):

        logger.debug("init")

        self.rfd = os.open("/dev/usb-ffs/ctb/ep1", os.O_RDWR | os.O_NONBLOCK)
        if self.rfd < 0:
            logger.error("open read node failed")
            sys.exit(-1)
        self.wfd = os.open("/dev/usb-ffs/ctb/ep2", os.O_RDWR | os.O_NONBLOCK)
        if self.wfd < 0:
            logger.error("open read node failed")
            sys.exit(-1)
        '''
        try:
            epoll_rfd = select.epoll()
            epoll_rfd.register(self.rfd, select.EPOLLIN)
        except select.error as error_rmsg:
            logger.error("error_rmsg rfd: %s" % (error_rmsg))

        try:
            epoll_wfd = select.epoll()
            epoll_wfd.register(self.wfd, select.EPOLLOUT)
        except select.error as error_wmsg:
            logger.error("error_wmsg wfd: %s" % (error_wmsg))
        '''
        logger.debug("init succeed")

    def ctb_read(self, length):
        logger.debug("read")
        data = os.read(self.rfd, length)
        logger.debug("read data: %s" % (data))

        return data

    def ctb_write(self, data, length):
        logger.debug("write")
        length_count = 0
        while True:
            length_count += os.write(self.wfd, data)
            if length_count == length:
                break
        logger.debug("the numbler of bytes: %s" % (length_count))
        return length_count

    def ctb_release(self):
        logger.debug("release resource")
        if self.rfd < 0:
            logger.error("open read node failed")
            os.close(self.rfd)
        if self.wfd < 0:
            logger.error("open read node failed")
            os.close(self.wfd)
