tail -f vitow_output | pv -L 56250 | \
    ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero \
            -i - \
            -ss 00:09:00 -i countdown.flv \
            -i watermark_no_barbie.png \
            -filter_complex "[1:v][2:v] overlay=[tmp]; [tmp][3:v] overlay=[out]; [out]split=2[out1][out2]" \
            -map "[out1]" -map 0:a -vcodec h264 -maxrate 1024k -bufsize 2048k -profile:v high -level 4.0 -acodec aac -ab 128k -g 50 -strict experimental \
            -f flv rtmp://a.rtmp.youtube.com/live2/rz6y-ezye-u0jy-93u6 \
            -map "[out2]" -map 0:a -vcodec h264 -preset ultrafast -maxrate 1024k -bufsize 2048k -profile:v high -level 4.0 -acodec aac -ab 128k -g 50 -strict experimental \
            -f mpegts "udp://localhost:5000"