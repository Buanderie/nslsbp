#!/bin/bash

# Options:
#   ./vitow_stream.sh t           # Only shows video with Mplayer.
#   ./vitow_stream.sh s           # Streams video with watermark and countdown starting at T-10. [+ Mplayer]
#   ./vitow_stream.sh s + HH MM   # Streams video with watermark and countdown starting at T[+/-]HH:MM:00. [+ Mplayer]
#   ./vitow_stream.sh S           # Idem to option 's' but opens FFmpeg output instead of using VITOW input.
#   ./vitow_stream.sh S + HH MM   # Idem to option 's' but opens FFmpeg output instead of using VITOW input.
#   ./vitow_stream.sh y test      # Streams video with watermark, without countdown to the test live stream. [+ Mplayer]
#   ./vitow_stream.sh y           # Streams video with watermark, without countdown to the event live stream. [+ Mplayer]
#   ./vitow_stream.sh i           # Streams the no-video still image.

COUNTDOWN_DELAY=0:0:0
COUNTDOWN_PATH=countdown_t-10.flv
WATERMARK_PATH=watermark_480p.png
WATERMARK_CAM1_PATH=watermark_480p_cam1.png
NOVIDEO_PATH=no-video_480p.png
VBITRATE=450000
PVRATE=$(($VBITRATE / 8))

ACSETTINGS="aac -ab 128k -g 50 -strict experimental"
VCSETTINGS="h264 -profile:v high -level 4.0"
VCSETTINGS_LOCAL="h264 -preset ultrafast -profile:v high -level 4.0"

function printHelp {
    echo "./vitow_stream.sh t           # Only shows video with Mplayer."
    echo "./vitow_stream.sh s           # Streams video with watermark and countdown starting at T-10. [+ Mplayer]"
    echo "./vitow_stream.sh s + HH MM   # Streams video with watermark and countdown starting at T[+/-]HH:MM:00. [+ Mplayer]"
    echo "./vitow_stream.sh S           # Idem to option 's' but opens FFmpeg output instead of using VITOW input."
    echo "./vitow_stream.sh S + HH MM   # Idem to option 's' but opens FFmpeg output instead of using VITOW input."
    echo "./vitow_stream.sh y test      # Streams video with watermark, without countdown to the test live stream. [+ Mplayer]"
    echo "./vitow_stream.sh y           # Streams video with watermark, without countdown to the event live stream. [+ Mplayer]"
    echo "./vitow_stream.sh i           # Streams the no-video still image."
}

if [ $# -lt 1 ]; then
    echo "Wrong number of parameters"
    printHelp
    exit
# else
#     echo $#
fi
# Configuration:
case $1 in
    s|S)
        STREAM_ID_FILE=youtube_stream_event_camera_VITOW
        STREAM_ID=$(cat $STREAM_ID_FILE)
        STREAM=rtmp://a.rtmp.youtube.com/live2/$STREAM_ID
        if [ $# -eq 4 ]; then
            if [ $2 == "+" ]; then
                COUNTDOWN_DELAY_SEC=$(($3 * 3600 + $4 * 60 + 613))
            elif [ $2 == "-" ]; then
                if [ $4 -lt 10 ]; then
                    COUNTDOWN_DELAY_SEC=$(((10 - $4) * 60))
                elif [ $4 -ge 10 ]; then
                    COUNTDOWN_DELAY_SEC=0
                fi
            else
                echo "Unknown option '$2' in stream mode (s)"
                exit
            fi
            # echo $COUNTDOWN_DELAY_SEC
            COUNTDOWN_DELAY=$(($COUNTDOWN_DELAY_SEC / 3600)):$((($COUNTDOWN_DELAY_SEC / 60) % 60)):$(($COUNTDOWN_DELAY_SEC % 60))
        fi

        if [ $1 == "s" ]; then
            FILTER_COMPLEX="[1:v][2:v] overlay=[tmp]; [tmp][3:v] overlay=0:0"
            LOCAL_PLAYER="mplayer"
        else
            FILTER_COMPLEX="[1:v][2:v] overlay=[tmp]; [tmp][3:v]overlay=[tmp2]; [tmp2]split=2[out1][out2]"
            LOCAL_PLAYER="UDP stream"
        fi
        ;;
    y)
        if [ $# -ge 2 ]; then
            if [ $2 == "test" ]; then
                STREAM_ID_FILE=youtube_stream_live
            else
                STREAM_ID_FILE=youtube_stream_event_camera_VITOW
            fi
        else
            STREAM_ID_FILE=youtube_stream_event_camera_VITOW
        fi
        STREAM_ID=$(cat $STREAM_ID_FILE)
        STREAM=rtmp://a.rtmp.youtube.com/live2/$STREAM_ID
        FILTER_COMPLEX="overlay=0:0"
        LOCAL_PLAYER="mplayer"
        ;;
    t)
        # Do nothing.
        STREAM="(no stream)"
        LOCAL_PLAYER="mplayer"
        ;;
    i)
        STREAM_ID_FILE=youtube_stream_event_camera_VITOW
        STREAM_ID=$(cat $STREAM_ID_FILE)
        STREAM=rtmp://a.rtmp.youtube.com/live2/$STREAM_ID
        LOCAL_PLAYER="(no player)"
        ;;
    *)
        echo "Wrong mode '$1' selected. Options are t, s, S or y."
        printHelp
        exit
        ;;
esac

if [ ! -f $WATERMARK_PATH ] || [ ! -f $COUNTDOWN_PATH ]; then
    echo "Some files could not be found. Aboting."
    # exit
fi

#####################################################################################################
# Box drawing assets: ┏ ┓ ┗ ┛ ┃ ━ ┠ ┨ │ ─ ┯ ┼ ┷
printf "┏━━━━━━━━━━━━━━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n"
printf "┃  %20s │ %-80s ┃\n" "Countdown timecode" "$COUNTDOWN_DELAY"
printf "┃  %20s │ %-80s ┃\n" "Stream" "$STREAM"
printf "┃  %20s │ %-80s ┃\n" "Local player" "$LOCAL_PLAYER"
printf "┃  %20s │ %-80s ┃\n" "Filter complex" "$FILTER_COMPLEX"
printf "┃  %20s │ %-80s ┃\n" "Watermark file" "$WATERMARK_PATH"
printf "┃  %20s │ %-80s ┃\n" "Countdown file" "$COUNTDOWN_PATH"
printf "┃  %20s │ %-80s ┃\n" "Still image file" "$NOVIDEO_PATH"
printf "┃  %20s │ %6d bps (%5d B/s) %57s ┃\n" "Video stream bitrate" "$VBITRATE" "$PVRATE" " "
if [ $1 == "t" ]; then
    printf "┃  %20s │ %-80s ┃\n" "Mode" "Test mode. Play video with mplayer."
elif [ $1 == "S" ]; then
    printf "┃  %20s │ %-80s ┃\n" "Mode" "Stream video to event and output to udp://192.168.1.100:1234."
elif [ $1 == "s" ]; then
    printf "┃  %20s │ %-80s ┃\n" "Mode" "Stream video to event and play with mplayer."
elif [ $1 == "y" ]; then
    if [ $# -ge 2 ]; then
        if [ $2 == "test" ]; then
            printf "┃  %20s │ %-80s ┃\n" "Mode" "Youtube test: live stream. Play with mplayer."
        else
            printf "┃  %20s │ %-80s ┃\n" "Mode" "Youtube test. UNEXPECTED value $2."
        fi
    else
        printf "┃  %20s │ %-80s ┃\n" "Mode" "Youtube test: event stream. Play with mplayer."
    fi
elif [ $1 == "i" ]; then
    printf "┃  %20s │ %-80s ┃\n" "Mode" "No-video RX. Streaming still image."
fi
printf "┗━━━━━━━━━━━━━━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n"

# exit
case $1 in
    t)
        tail -f vitow_output | mplayer -fps 25 -framedrop -demuxer h264es -
        ;;
    s|S)
        if [ $1 == "s" ]; then
            tail -f vitow_output | tee >(mplayer -fps 25 -framedrop -demuxer h264es -) |            \
                pv -L $PVRATE -q |                                                                  \
                ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero           \
                        -i -                                                                        \
                        -ss $COUNTDOWN_DELAY -i $COUNTDOWN_PATH                                     \
                        -i $WATERMARK_PATH                                                          \
                            -filter_complex "$FILTER_COMPLEX"                                       \
                            -vcodec $VCSETTINGS                                                     \
                            -acodec $ACSETTINGS                                                     \
                        -f flv $STREAM
        else
            tail -f vitow_output | pv -L $PVRATE -q |                                               \
                ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero           \
                        -i -                                                                        \
                        -ss $COUNTDOWN_DELAY -i $COUNTDOWN_PATH                                     \
                        -i $WATERMARK_PATH                                                          \
                            -filter_complex "$FILTER_COMPLEX"                                       \
                            -map '[out1]' -map 0:a -vcodec $VCSETTINGS -acodec $ACSETTINGS          \
                                -f flv $STREAM                                                      \
                            -map '[out2]' -map 0:a -vcodec $VCSETTINGS_LOCAL -acodec $ACSETTINGS    \
                                -f mpegts udp://localhost:52001
        fi
        ;;
    y)
        tail -f vitow_output | pv -L $PVRATE -q |                                                   \
            ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero               \
                    -i -                                                                            \
                    -i $WATERMARK_CAM1_PATH                                                         \
                        -filter_complex "$FILTER_COMPLEX"                                           \
                        -vcodec $VCSETTINGS                                                         \
                        -acodec $ACSETTINGS                                                         \
                    -f flv $STREAM
        ;;
    i)
        VCSETTINGS="h264"
        ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero               \
                -loop 1 -i $NOVIDEO_PATH -r 5                                                  \
                    -vcodec $VCSETTINGS                                                         \
                    -acodec $ACSETTINGS                                                         \
                -f flv $STREAM
        ;;
esac

# Previous call:
# tail -f vitow_output | pv -L 93750 | \
#     ffmpeg  -re -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 \
#             -i /dev/zero -f h264 -i - -i watermark.png -filter_complex 'overlay=0:0' \
#             -vcodec h264 -profile:v high -level 4.0 -acodec aac -ab 128k -g 50 -strict experimental \
#             -f flv rtmp://a.rtmp.youtube.com/live2/xxxx-xxxx-xxxx-xxxx
