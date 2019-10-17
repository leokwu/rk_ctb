from utils import *

if __name__ == '__main__':
    print("main")
    ctb_device = CtbDevice()
    len = ctb_device.ctb_write("1111")
    print("len: \n", len)