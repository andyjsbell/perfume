#!/usr/bin/python
# uskee.org

import time
import os

WAIT_TIME=20
CMD_STR="cp -f /var/www/streaming/mystream.m3u8 /var/www/streaming/index.m3u8"

if __name__ == '__main__':
	while True:
		time.sleep(WAIT_TIME)
		os.system(CMD_STR)
		WAIT_TIME=3
