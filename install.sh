#!/bin/bash
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NOCOLOR='\033[0m'
TARGET=/usr/local/rastari
ver=$(<VERSION)
iver=$(<$TARGET/VERSION)

if [[ "$ver" = "$iver" ]]; 
then
	echo -e  "${BLUE}--== Rastari is up to date  ==--${NOCOLOR}"
else
	echo -e  "${YELLOW}***************************************${NOCOLOR}"
	echo -e  "${YELLOW}****   COMPILE & INSTALL RASTARI   ****${NOCOLOR}"
	echo -e  "${YELLOW}***************************************${NOCOLOR}"
	echo -e  "${BLUE}--== Compiling bootScreen ==--${NOCOLOR}"
	cd bootScreen
	make clean_release release
	cd ..
	echo -e  "${BLUE}--== Compiling Rastari ==--${NOCOLOR}"
	cd Rastari
	make clean_debug debug
	cd ..
	echo -e  "${BLUE}--== Compiling SelectNetwork ==--${NOCOLOR}"
	cd SelectNetwork
	make clean_release release
	cd ..
	echo -e  "${BLUE}--== Compiling AudioOutput ==--${NOCOLOR}"
	cd AudioOutput
	make clean_release release
	cd ..
	echo -e  "${BLUE}--== Compiling Configuration ==--${NOCOLOR}"
	cd Configuration
	make clean_release release
	cd ..
	echo -e  "${BLUE}--== Compiling ResizeSD ==--${NOCOLOR}"
	cd ResizeSD
	make clean_release release
	cd ..
	mkdir -p $TARGET
	echo -e  "${BLUE}--== Copying files to Rastari path ==--${NOCOLOR}"
	cp ./bootScreen/bin/Release/bootScreen ./AudioOutput/bin/Release/AudioOutput ./Configuration/bin/Release/Configuration ./SelectNetwork/bin/Release/SelectNetwork ./Rastari/*.sh ./Rastari/bin/Debug/Rastari ./bootScreen/*.png ./Configuration/*.png ./AudioOutput/*.png ./bootScreen/*.wav ./SelectNetwork/*.wav ./SelectNetwork/*.png ./Rastari/*.png ./Rastari/*.wav ./Rastari/*.ttf ./ResizeSD/*.png ./ResizeSD/bin/Release/ResizeSD ./VERSION $TARGET
	cd $TARGET
	chmod a+x *.sh
	chmod a+x ./bootScreen
	chmod a+x ./Rastari
	chmod a+x ./SelectNetwork
	chmod a+x ./AudioOutput
	chmod a+x ./Configuration
	chmod a+x ./ResizeSD
	cd ~
	echo -e  "${BLUE}--== Rastari ${ver} is compiled & installed  ==--${NOCOLOR}"
fi


echo ""
echo -e  "${YELLOW}Press ENTER to continue...${NOCOLOR}"
read -p ""







