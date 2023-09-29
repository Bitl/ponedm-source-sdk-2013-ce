#!/bin/sh

cd ./mp/src
sudo chmod -R +x devtools/*
sudo chmod -R +x devtools/bin
sudo chmod -R +x devtools/bin/linux
sudo chmod +x creategameprojects
sudo bash creategameprojects
sudo make -f Game_ponedm.mak
