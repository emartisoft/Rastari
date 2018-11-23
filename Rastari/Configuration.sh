#!/bin/bash
# Rastari Configuration
YELLOW='\033[0;33m'
NOCOLOR='\033[0m'
conf=$HOME/FirstBoot
rastariHome=/usr/local/rastari

if [ -f "$conf" ]
then
	clear
	cd $rastariHome
	# may be different Rpi
	rm -f /etc/udev/rules.d/70-persistent-net.rules
	# welcome
	./Configuration
	# switch audio output
	./AudioOutput
	# resize sd card
	./ResizeSD
	rm -f $conf
	clear
	figlet -k Rastari
	echo -e "${YELLOW}Rebooting...${NOCOLOR}"
	reboot	
fi









