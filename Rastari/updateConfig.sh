#!/bin/bash
sed -i 's/bConfirmQuit = TRUE/bConfirmQuit = FALSE/g' /root/.hatari/*.cfg
sed -i 's/\/usr\/local\/rastari/\/root\/rastari/g' /root/.hatari/*.cfg
sed -i 's/FastFloppy = FALSE/FastFloppy = TRUE/g' /root/.hatari/*.cfg
echo Hatari config files are updated
