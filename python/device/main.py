from utils import *

if __name__ == '__main__':
    ctb_device = CtbDevice()
    logger.debug("main ")
    length = ctb_device.ctb_write(('1111').encode(), len(('1111').encode()))
    logger.debug("write length: %s" % (length))
    ctb_device.ctb_release()
