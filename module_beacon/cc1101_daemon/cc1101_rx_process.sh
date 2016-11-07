#!/bin/bash
sudo nice -n -20 sudo picc1101 -v2 -P 120 -d /dev/spidev0.0 --gdo0=4 --gdo2=5 -f 426000000 -R8 -M4 -F -W -x 0