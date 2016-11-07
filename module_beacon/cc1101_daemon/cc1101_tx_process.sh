#!/bin/bash
sudo nice -n -20 sudo picc1101 -v2 -P 120 -d /dev/spidev0.1 --gdo0=3 --gdo2=4 --pac=3 -f 426000000 -R8 -M4 -F -W -x 1