#!/bin/bash
if [ "$1" -gt "-1" ]; then
    old_proc=$(pgrep -f PICO$1)
    kill $old_proc &> /dev/null
    nohup socat PTY,link=/dev/ttyPICO$1,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:400$1 >/dev/null 2>&1 &
fi