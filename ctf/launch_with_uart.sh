#!/bin/bash
# this script will start a virtual serial device that will automatically send serial inputs to the decoder
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

mkdir -p pts
tty_dir="./pts"
sudo socat -d -d pty,raw,echo=0,link=${tty_dir}/VUART pty,raw,echo=0,link=${tty_dir}/VUART_out &
sleep 1

sudo chmod 777 ${tty_dir}/VUART
sudo chmod 777 ${tty_dir}/VUART_out

GREEN='\033[0;32m'
NC='\033[0m'

tty_path="$(realpath --no-symlinks $tty_dir/VUART)"
echo -e "Use ${GREEN}${tty_path}${NC} for serial communication to the decoder"

cat < ${tty_dir}/VUART_out | ./decoder/decoder | tee ${tty_dir}/VUART_out
