"""1808 ctb devide control"""

import select
import os, sys


class CtbDevice():
    def __init__(self):
        print("init")
        rfd = os.open("./read.log", os.O_RDWR | os.O_NONBLOCK)
        if rfd < 0:
            print("open read node failed")
            sys.exit(-1)
        wfd = os.open("./write.log", os.O_RDWR | os.O_NONBLOCK)
        if wfd < 0:
            print("open read node failed")
            sys.exit(-1)

        print("init succeed")

    def ctb_read(self, len):
        print("read")
        read_data = os.read(self.rfd, len)
        print("read_data: %s\n", read_data)

        return read_data

    def ctb_write(self, write_data):
        print("write")
        write_len = os.write(self.wfd, write_data)
        print("the numbler of bytes: %d\n", write_len)
        return write_len

    def ctb_release(self):
        print("release resource")
        if self.rfd < 0:
            print("open read node failed")
            os.close(self.rfd)
        if self.wfd < 0:
            print("open read node failed")
            os.close(self.wfd)
