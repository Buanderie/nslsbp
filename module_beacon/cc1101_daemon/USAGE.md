USAGE:

Configuration for ALICE:
sudo nice -n -20 sudo ./picc1101 -v2 -D /home/phy -P 255 -d /dev/spidev0.0 --gdo0=4 --gdo2=5 -f 426000000 -R8 -M4 -F -W

Configuration for RASPi-JF:
sudo nice -n -20 sudo ./picc1101 -v2 -D /home/phy -P 255 -d /dev/spidev0.1 --gdo0=3 --gdo2=4 -f 426000000 -R8 -M4 -F -W

Explained:

nice -n -20 gives priority to the process
-v2 gives verbosity level 2 (enough)
-D indicates the UNIX SOCKET SERVER that will be created
-P the packet size
-d The spi device
--gdo0 and --gdo2 the GDO pins of CC1101
-f the central freq
-R the baudrate 
-M the modulation type
-F FEC ON
-W Whitehing ON