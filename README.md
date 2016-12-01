# NanoSat Lab Stratospheric Balloon Payload / VITOWx

This is the official repository for the NSL-SBP project (formerly _BSX/VITOW_), an open-source demonstration of a low-cost, reliable real-time video streaming from a stratospheric balloon carried out at _[NanoSat Lab](http://nanosatlab.upc.edu), Technical University of Catalonia - UPC BarcelonaTech._ This software framework is designed control a high-datarate (**2 Mbps**) communications payload (~1.6 kg.) that downloads real-time video. The NSL-SBP is solely based on commercial-off-the-shelf (COTS) components and widely-accepted standards (such as IEEE802.11):

#### System components
* The payload's on-board computer is implemented with a **Raspberry Pi 3** SBC, running Raspbian.
* Connected to the OBC, a **RaspiCam** (v2) is used to record video in streaming-quality (currently not HD).
* A **commercial USB IEEE802.11b/g/n dongle** is used to implement the downlink. This dongle is based on an Qualcomm Atheros AR9271 chipset, the drivers of which have been liberated for Linux and can be found here: [QCA's Open ath9k_htc firmware](https://github.com/qca/open-ath9k-htc-firmware).
* __Housekeeping sensors:__
    * Barometric and temperature sensors: **BMP180** (breakout board from [SparkFun](https://www.sparkfun.com/products/11824)) and **MCP9808** (breakout board from [Adafruit](https://learn.adafruit.com/adafruit-mcp9808-precision-i2c-temperature-sensor-guide/overview).)
    * IMU: 9-DoF **MPU-9150** (breakout board from [SparkFun](https://www.sparkfun.com/products/13762).)
    * GNSS positioning: [u-blox **MAX-M8**](https://www.u-blox.com/en/product/max-m8-series).
* A low-power 433 MHz transceiver based on TI's [**CC1101**](http://www.ti.com/product/CC1101) chipset is used to implement a secondary communications link (periodic beacon) to transmit housekeeping data and payload positioning.
* LDPC-Staircase Forward Error Correction library: [OpenFEC](http://openfec.org/) v1.4.2.
* __Ground Station:__
    * Motor controller and an **Arduino Uno**.
    * LNA **PGA-103+** (from [Minicircuits](https://www.minicircuits.com/pdfs/PGA-103+.pdf))
    * **USRP** B200 mini (from Ettus Research) implementing the CC1101 beacon signal receiver (based on [Joan Francesc Muñoz's GNURadio scheme](https://github.com/juan0fran/gr-cc_sdr)).
    * A 24 dBi parabolic antenna: Alpha Networks **AGA-2424**.
    * Linux PC (running Ubuntu 16.04).

### NSLSBP Modules
#### xLauncher and Ground Station controller
The programs `xlauncher` and `ground_station` shall be executed on both ends of the link (paylod's computer and PC, respectively.) The first will launch the video recording and transfer programs, while the latter is devoted to control the ground station infrastructure (antenna rotors, automatic positioning, etc.)
#### VITOW
The `module_vitow` includes all the necessary files to implement the communications link. Detailed information can be found in a dedicated section below.
#### Housekeeping and positioning
The `module_gps_temp` directory contains the housekeeping module (`gps_process` binary). This module reads data from the on-board sensors and retrieves GNSS data. Upon the reception of housekeeping and positioning data, the module sends this data through the beacon signal. This data is then used by the mobile ground station
#### Maps
`module_maps` encompasses a simplistic web-app which displays payload location and housekeeping status implemented in PHP and JavaScript.
#### Libraries
Housekeeping data in both ends of the communication channel is stored in MySQL databases. The Database Manager (`dbman`) has been included to facilitate interaction against MySQL to the rest of modules in this project.

## Video Transfer Over WiFi eXperiment (VITOWx)
This project is mainly based upon the **Video Transfer Over WiFi** ([VITOW](https://github.com/Seitam/VITOW)) project, created by Maties Pons (@Seitam). VITOW is essentially composed of two processes: `vitow_tx` and `vitow_tx` (transmission and reception process, respectively.) These processes implement a pipe-like transfer in which input data is read from the standard input (`stdin`) at RX, and copied to TX's standard output (`stdout`). VITOW is designed to transfer a continuous stream of data (i.e. a video stream or any other type of data) in unreliable communication channels. VITOW dumps and receives packets from a IEEE802.11 physical layer, implemented in this case with the Atheros NIC configured in monitor mode. Lost packets are automatically recovered thanks to its LDPC FEC layer. This module has been forked from its original repo and is likely to stay detached. Minor bug-fixes have been applied (mostly in the thread management and buffering section) and the code has been changed to operate at fixed/static K and N parameters (which were included in packet headers in the original version.) The VITOWx module utilizes these freed 8-bytes to transmit debug data (currently redundant housekeeping data.)

### How to build
All modules and programs can either be built using `make` or `cmake`:
* For `make`: a single `Makefile` is provided. Build with `make all` (install libraries with `make install`.)
* For `cmake`: the CMake scripts will be found in the directories. Buid the modules inside the `build` sub-directory with `cmake .. && make`.
