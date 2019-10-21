import numpy as np
import threading
import cv2 as cv
from rknn.api import RKNN
import os, sys
from log import *

logger = InitLog("ctb_device", "ctb_device.log")



class rknn_server:
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

    def __del__(self):
        logger.debug("release resource")
        if self.rfd >= 0:
            logger.debug("release rfd")
            os.close(self.rfd)
        if self.wfd >= 0:
            logger.debug("release wfd")
            os.close(self.wfd)

    def service(self, model, post_func):
        t = threading.Thread(target=self.__deal, args=(model, post_func))
        t.start()


    def __deal(self, model, post_func):


        rknn = RKNN()
        ret = rknn.load_rknn(path=model)

        # init runtime environment
        logger.debug('--> Init runtime environment')
        ret = rknn.init_runtime()
        if ret != 0:
            logger.debug('Init runtime environment failed')
            exit(ret)
        logger.debug('done')
        # self.wfd.write(b'ready')
        os.write(self.wfd, b'ready')

        while True:
            decimg = self.__recieve_frame()
            logger.debug("__recieve_frame: %s" % (decimg))
            if decimg is None:
                break

            outputs = rknn.inference(inputs=[decimg])
            data = post_func(outputs)
            self.__send_result(data)


        # self.wfd.close()
        os.close(self.wfd)
        rknn.release()
        logger.debug("__deal finish")

    def __recieve_frame(self):
        try:
            length = self.__recvall(16)
            stringData = self.__recvall(int(length))
            data = np.frombuffer(stringData, np.uint8)
            decimg=cv.imdecode(data, cv.IMREAD_COLOR)
        except (RuntimeError, TypeError, NameError):
            return None

        return decimg

    def __recvall(self, count):
        buf = b''
        while count:
            # newbuf = self.rfd.read(count)
            newbuf = os.read(self.rfd, count)
            if not newbuf: return None
            buf += newbuf
            count -= len(newbuf)
        return buf

    def __pack_np(self, data):
        len_dt = np.dtype(np.int32)
        stringData = ""

        if data is None:
            type_name = "None".ljust(8)
            stringData = type_name.encode()
        else:
            type_name = str(data.dtype).ljust(8)
            shape_len = len(data.shape) * len_dt.itemsize
            shape = np.array(data.shape, dtype=len_dt)
            data_len = data.size*data.itemsize
            stringData = type_name.encode() + \
                         str.encode(str(shape_len).ljust(8)) + str.encode(str(data_len).ljust(8))
            stringData = stringData + shape.tostring() + data.tostring()

        return stringData
            
    def __send_result(self, result):
        count = len(result)

        len_info = np.empty(count, dtype=np.int32)

        ctb_data = b''

        for i in range(count):
            tmp = self.__pack_np(result[i])
            len_info[i] = len(tmp)
            ctb_data = ctb_data + tmp

        ctb_data = str.encode(str(count).ljust(8)) + len_info.tostring() + ctb_data

        # self.wfd.write(ctb_data)
        os.write(self.wfd, ctb_data)