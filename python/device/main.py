from utils import *

if __name__ == '__main__':
    print("main")
    ctb_device = CtbDevice()
    length = ctb_device.ctb_write(('1111').encode(), len(('1111').encode()))
    print("write length: \n", length)
    ctb_device.ctb_release()
