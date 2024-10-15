#!/bin/bash

nohup socat PTY,link=/dev/ttyPICO0,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4000&
nohup socat PTY,link=/dev/ttyPICO1,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4001&
nohup socat PTY,link=/dev/ttyPICO2,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4002&
nohup socat PTY,link=/dev/ttyPICO3,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4003&
nohup socat PTY,link=/dev/ttyPICO4,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4004&
nohup socat PTY,link=/dev/ttyPICO5,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4005&
nohup socat PTY,link=/dev/ttyPICO6,raw,group-late=dialout,mode=660,echo=0,unlink-close=0 TCP:172.24.57.165:4006&