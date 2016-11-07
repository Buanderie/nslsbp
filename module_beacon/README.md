# cc_beacon
PICC1101 based beacon, 1 Transmitting, 1 Receiving

Folder cc1101_daemon is a modification of PICC101 from f4exb
Folder cc1101_handler is the interface between the daemon and another program. Using CMAKE it crates a library from the files at SRC, and link it to files at APPS

##  cc1101 daemon
Explanation at USAGE.md from its folder but-
*For make:*
```bash
make clean
make
```
Executed as sudo ./picc1101 with arguments:

`sudo nice -n -20 sudo ./picc1101 -v2 -P 255 -d /dev/spidev0.0 --gdo0=4 --gdo2=5 --pac=3 -f 426000000 -R8 -M4 -F -W -x 1`

Where -x  indicates that is a TX beacon.  --gdo are the interruption pins (gpios) and --pac is the PAC pin for the amplifier (only tx)

Pins are in WiringPi format:
[Wiring Pi] https://es.pinout.xyz/pinout/wiringpi

`sudo nice -n -20 sudo ./picc1101 -v2 -P 255 -d /dev/spidev0.0 --gdo0=4 --gdo2=5 -f 426000000 -R8 -M4 -F -W -x 0`

## cc1101 handler
Compile with:
```bash
mkdir build
cd build
cmake ..
make
```
And execute in several terminals the contents of bin (example tx and example rx).

Execute 1 rx and several tx.
