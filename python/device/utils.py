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

    def ctb_read(self):
        print("read")

    def ctb_write(self):
        print("write")

    def ctb_release(self):
        print("release resource")
        if self.rfd < 0:
            print("open read node failed")
            os.close(self.rfd)
        if self.wfd < 0:
            print("open read node failed")
            os.close(self.wfd)
