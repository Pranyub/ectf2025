#!/bin/bash
# this script will start a virtual serial device that will automatically send serial inputs to the decoder
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

sudo socat -d -d pty,raw,echo=0,link=$HOME/pts/VUART pty,raw,echo=0,link=$HOME/pts/VUART_out &
sleep .5
sudo chmod 777 $HOME/pts/VUART
sudo chmod 777 $HOME/pts/VUART_out
echo Use $HOME/pts/VUART for serial communication to the decoder

cat < $HOME/pts/VUART_out | ./decoder/decoder | tee $HOME/pts/VUART_out
