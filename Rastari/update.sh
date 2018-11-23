#!/bin/bash
# Update Rastari & Rpi
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NOCOLOR='\033[0m'
clear
# sync time
timedatectl set-ntp 1
echo -e ${BLUE}
figlet -k 'Software Updater'
echo -e ${NOCOLOR}
echo -e  "${YELLOW}Checking for Rpi updates...${NOCOLOR}"
apt-get update -y
apt-get upgrade -y
rpi-update
apt-get autoclean -y
apt-get autoremove -y
SetAutoLogin
apt-get install build-essential libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev  -y
echo -e  "${YELLOW}Checking for Rastari updates...${NOCOLOR}"
cd /tmp
git clone https://github.com/emartisoft/Rastari
cd Rastari/
./install.sh
