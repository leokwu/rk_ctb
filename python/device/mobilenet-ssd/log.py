import logging 
import sys
from logging.handlers import RotatingFileHandler

def InitLog(module_name, log_file):
    logger = logging.getLogger(module_name)
    logger.setLevel(logging.DEBUG)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    rt = RotatingFileHandler(log_file, maxBytes=10*1024*1024, backupCount=5)
    rt.setLevel(logging.DEBUG)

    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    ch.setFormatter(formatter)
    rt.setFormatter(formatter)

    logger.addHandler(rt)
    logger.addHandler(ch)

    return logger
