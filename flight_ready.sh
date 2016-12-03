#!/bin/bash

echo "Removing previous logs and video files..."

rm /var/log/stderr_gps.log -v
rm /home/pi/nslsbp/vitow_input/* -v

echo "Done."
