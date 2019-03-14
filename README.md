srs rtmp stats
===========

The library is built based on SRS(https://github.com/ossrs/srs)

This repository is exported by SRS3s.0+, please read the wiki(
[CN](https://github.com/ossrs/srs/wiki/v3_CN_SrsLibrtmp#export-srs-librtmp),
[EN](https://github.com/ossrs/srs/wiki/v3_EN_SrsLibrtmp#export-srs-librtmp)
).

## RTMP Stats

This lib is used to stat rtmp stream every 10 seconds for Bandwidth, Keyframe interval, FPS, Frame drop rate, Audio codec, Video codec, Resolution.

### Build and make

```
$ git clone https://github.com/robinyeeh/srs-stream-stats.git
$ cd srs-stream-stats
$ cmake .
$ make
```

### Usage

```
$ chmod +x srs-stream-stats
$./srs-stream-stats rtmp://127.0.0.1:1935/live/livestream,rtmp://127.0.0.1:1935/live/livestream2
```

###Example Results

```
[2019-03-14 05:56:25.56] [140687622948608]Stream : /app_andy/stream_andy, Bandwidth : 1504856 bps, Keyframe Interval : 2, FPS : 20, Frame drops : 0.00 %, Audio codec : AAC, Video codec : H.264, Resolution: 1280 * 720  

[2019-03-14 05:56:35.66] [140687622948608]Stream : /app_andy/stream_andy, Bandwidth : 1510412 bps, Keyframe Interval : 2, FPS : 20, Frame drops : 0.00 %, Audio codec : AAC, Video codec : H.264, Resolution: 1280 * 720
  
[2019-03-14 05:56:45.76] [140687622948608]Stream : /app_andy/stream_andy, Bandwidth : 1509655 bps, Keyframe Interval : 2, FPS : 20, Frame drops : 0.00 %, Audio codec : AAC, Video codec : H.264, Resolution: 1280 * 720
  
[2019-03-14 05:56:55.86] [140687622948608]Stream : /app_andy/stream_andy, Bandwidth : 1520843 bps, Keyframe Interval : 2, FPS : 20, Frame drops : 0.00 %, Audio codec : AAC, Video codec : H.264, Resolution: 1280 * 720  

```