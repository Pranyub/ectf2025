#!/bin/bash

nohup socat PTY,link=/dev/ttyPICO0,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4000&
nohup socat PTY,link=/dev/ttyPICO1,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4001&
nohup socat PTY,link=/dev/ttyPICO2,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4002&
nohup socat PTY,link=/dev/ttyPICO3,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4003&
nohup socat PTY,link=/dev/ttyPICO4,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4004&
nohup socat PTY,link=/dev/ttyPICO5,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4005&
nohup socat PTY,link=/dev/ttyPICO6,raw,echo=0,unlink-close=0 TCP:172.24.57.165:4006&