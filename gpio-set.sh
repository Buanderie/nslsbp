#! /bin/bash 
### BEGIN INIT INFO
# Provides: WiringPi safe pins
# Required-Start: 
# Required-Stop: 
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: start and stop GPIO utility for PAC pin
# Description:
### END INIT INFO


start() {
    echo -n "Setting up GPIOs for WiringPi"
    gpio mode 7 out
    gpio write 7 0
    echo " done."
}

stop() {
   echo "gpio halt"
}

### main logic ###
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart|reload)
        stop
        start
        ;;
    *)
        echo $"Usage: $0 {start|stop|restart|reload}"
        exit 1
esac
exit 0
