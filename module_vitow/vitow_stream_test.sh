tail -f vitow_output | pv -L 56250 | \
    ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 \
            -i /dev/zero -f h264 \
            -i - \
            -i countdown.flv \
            -i watermark_no_barbie.png \
            -filter_complex "[1:v][2:v] overlay=[tmp]; [tmp][3:v] overlay=0:0" \
            -vcodec h264 -profile:v high -level 4.0 -acodec aac -ab 128k -g 50 -strict experimental \
            -f flv rtmp://a.rtmp.youtube.com/live2/rz6y-ezye-u0jy-93u6
