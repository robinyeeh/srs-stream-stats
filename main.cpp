/*
The MIT License (MIT)

Copyright (c) 2013-2015 SRS(ossrs)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**
gcc srs_play.c ../../objs/lib/srs_librtmp.a -g -O0 -lstdc++ -o srs_play
*/

#include <stdio.h>
#include <stream_stats.hpp>
#include <string>
#include <srs_librtmp.hpp>

#define IS_LOCAL 1

using namespace std;


int main(int argc, char **argv) {
    printf("suck rtmp stream like rtmpdump\n");
    printf("srs(ossrs) client librtmp library.\n");
    printf("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());

    if (!IS_LOCAL) {
        if (argc <= 1) {
            printf("Usage: %s <rtmp_url>\n"
                   "   rtmp_url     RTMP stream url to play\n"
                   "For example:\n"
                   "   %s rtmp://127.0.0.1:1935/live/livestream,rtmp://127.0.0.1:1935/live/livestream2\n",
                   argv[0], argv[0]);
            exit(-1);
        }

        play_stream(argv[1], (short) 30);
    } else {
        string url = "rtmp://192.168.3.43:1935/linear/test001?token=1551176683-ea6e1e45cecec8d802906ea44d412902,rtmp://192.168.3.43:1935/app_andy/stream_andy?token=1544522842-7b6f57ab5b622b56dda68a7a8fafa7b6";
        play_stream(url.c_str(), (short) 30);
    }


    return 0;
}



