#!/bin/bash
# Run the Rastari
ver=$(</usr/local/rastari/VERSION)
cfg=$(</usr/local/rastari/autostart.cfg)
autostart="/usr/local/rastari/.autostart"

clear
if [ -f "$autostart" ]
then
	hatari -c "/root/.hatari/$cfg.cfg"
	clear
fi
./Rastari $ver
while : 
do
	./cmd.sh
	clear
	./Rastari $ver
done

