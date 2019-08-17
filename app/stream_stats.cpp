//
// Created by robin-laptop on 2019/3/8.
//

#include <stream_stats.hpp>
#include <srs_librtmp.hpp>
#include <srs_kernel_error.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <inttypes.h>
#include <srs_core.hpp>
#include <chrono>
#include <cmath>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>
#include <srs_kernel_codec.hpp>
#include <srs_core_autofree.hpp>

using namespace std;

using namespace std::chrono;

const char human_flv_tag_type2string(char type) {
    static const char audio = 'A';
    static const char video = 'V';
    static const char data = 'D';
    static const char unknown = 'U';

    switch (type) {
        case SRS_RTMP_TYPE_AUDIO:
            return audio;
        case SRS_RTMP_TYPE_VIDEO:
            return video;
        case SRS_RTMP_TYPE_SCRIPT:
            return data;
        default:
            return unknown;
    }

    return unknown;
}

const char human_flv_video_frame_type2string(char frame_type) {
    static const char keyframe = 'I';
    static const char interframe = 'P';
    static const char *disposable_interframe = "DI";
    static const char *generated_keyframe = "GI";
    static const char *video_infoframe = "VI";
    static const char unknown = 'U';

    switch (frame_type) {
        case 1:
            return keyframe;
        case 2:
            return interframe;
        default:
            return unknown;
    }

    return unknown;
}

string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<string> spit_str(const string &str, char splitter) {
    if (str.empty()) {
        return {};
    }

    vector<string> vect;
    stringstream ss(str);

    while (ss.good()) {
        string substr;
        getline(ss, substr, splitter);

        substr = trim(substr);
        vect.push_back(substr);
    }

    return vect;
}


//int64_t get_timestamp_now() {
//    microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
//    return ms.count()/1000;
//}

string get_stream_name(const char *url) {
    string url_str = url;
    url_str = url_str.substr(7);
    url_str = url_str.substr(url_str.find('/'));
    url_str = url_str.substr(0, url_str.rfind('?'));

    return url_str;
}

int play_stream3(const char *url) {
    srs_human_trace("start play stream, rtmp url: %s", url);
    srs_rtmp_t rtmp = srs_rtmp_create(url);

    string stream_name;
    stream_name = get_stream_name(url);

    int count_internal_sec = 10;
    int key_frames = 0, v_frames = 0;
    int64_t total_length = 0;
    string a_codec, v_codec;
    int64_t pre_now = srs_utils_time_ms();
    int64_t nb_packets = 0;
    int64_t pre_timestamp = 0;
    int64_t pre_interval_ts = 0;

    int64_t pre_kframe_pts = 0;
    int64_t pre_kframe_ts = 0;
    int32_t total_v_frames = 0;
    int32_t pre_kframe_num = 0;

    if (srs_rtmp_handshake(rtmp) != 0) {
        srs_human_trace("simple handshake failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("simple handshake success");

    if (srs_rtmp_connect_app(rtmp) != 0) {
        srs_human_trace("connect vhost/app failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("connect vhost/app success");

    if (srs_rtmp_play_stream(rtmp) != 0) {
        srs_human_trace("play stream failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("play stream success");

    SrsAvcAacCodec *srsAvcAacCodec;
    srsAvcAacCodec = new SrsAvcAacCodec();

    SrsCodecSample *srsCodecSample;
    srsCodecSample = new SrsCodecSample();

    for (;;) {
        int size;
        char type;
        char *data;
        u_int32_t timestamp;

        if (srs_rtmp_read_packet(rtmp, &type, &timestamp, &data, &size) != 0) {
            srs_human_trace("RTMP connection disconnected.");
            goto rtmp_destroy;
        }


        int ret = ERROR_SUCCESS;

        int64_t now_ts = srs_utils_time_ms();

        u_int32_t pts;
        char frame_type;
        char read_type;

        if (srs_utils_parse_timestamp(timestamp, type, data, size, &pts) != 0) {
//            srs_human_trace("Rtmp packet id=%" PRId64 ", type=%s, dts=%d, ndiff=%d, diff=%d, size=%d, DecodeError",
//                            nb_packets, srs_human_flv_tag_type2string(type), timestamp, ndiff, diff, size
//            );

            read_type = human_flv_tag_type2string(type);
            frame_type = 'U';
        }

        if (type == SRS_RTMP_TYPE_VIDEO) {
            read_type = human_flv_tag_type2string(type);
            frame_type = human_flv_video_frame_type2string(srs_utils_flv_video_frame_type(data, size));

            if (v_codec.empty()) {
                v_codec = srs_human_flv_video_codec_id2string(srs_utils_flv_video_codec_id(data, size));
            }

            if ((now_ts - pre_interval_ts) >= (count_internal_sec * 1000)) {
                // packets interval in milliseconds.

                int gfps = 0;
                if (v_frames > 0) {
                    gfps = 1000 / ((now_ts - pre_interval_ts) / v_frames);
                }

                int fps = 1000 / (timestamp - pre_timestamp);
                int total_frames = (int) (count_internal_sec * fps);

                srsAvcAacCodec->video_avc_demux(data, size, srsCodecSample);
                int width = srsAvcAacCodec->width;
                int height = srsAvcAacCodec->height;

                int key_frame_interval = 0;
                if (key_frames > 0) {
                    key_frame_interval = ceil(count_internal_sec / key_frames);
                }

//                srs_human_trace("total_frames : %d, v_frames : %d", total_frames, v_frames);
                float drop_frame_rate = (float) (total_frames - v_frames) / total_frames * 100;
                if (drop_frame_rate <= 0) {
                    drop_frame_rate = 0.0;
                }

                int bandwidth = total_length * 8 / count_internal_sec;

                srs_human_trace(
                        "Stream : %s, Bandwidth : %d bps, Keyframe Interval : %d, FPS : %d, Frame drops : %.2f %%, "
                        "Audio codec : %s, Video codec : %s, Resolution: %d * %d",
                        stream_name.c_str(), bandwidth, key_frame_interval, gfps, drop_frame_rate, a_codec.c_str(),
                        v_codec.c_str(), width, height);

                pre_interval_ts = now_ts;
                key_frames = 0;
                total_length = 0;
                a_codec = "";
                v_codec = "";
                v_frames = 0;
            }

            if ('I' == frame_type) {
                int64_t now_kframe_ts = srs_utils_time_ms();

                int64_t kframe_ts_diff = now_kframe_ts - pre_kframe_ts;
                int64_t kframe_pts_diff = pts - pre_kframe_pts;
                int32_t kframe_num_diff = total_v_frames - pre_kframe_num;

                srs_human_trace(
                        "Stream : %s, key frame local ts : %lld, local ts diff : %lld, Keyframe pts : %d, "
                        "pts diff : %lld, Vframe number : %d, Keyframe num diff : %d, , Audio codec : %s, Video codec : %s",
                        stream_name.c_str(), now_kframe_ts, kframe_ts_diff, pts, kframe_pts_diff, total_v_frames,
                        kframe_num_diff, a_codec.c_str(), v_codec.c_str());

                key_frames++;
                pre_kframe_ts = now_kframe_ts;
                pre_kframe_pts = pts;
                pre_kframe_num = total_v_frames;
            }

            total_length += size;

            v_frames++;
            nb_packets++;
            total_v_frames++;

            pre_timestamp = timestamp;
            pre_now = now_ts;

        } else if (type == SRS_RTMP_TYPE_AUDIO) {
            read_type = human_flv_tag_type2string(type);
            frame_type = 'U';

            if (a_codec.empty()) {
                a_codec = srs_human_flv_audio_sound_format2string(srs_utils_flv_audio_sound_format(data, size));
            }
        } else if (type == SRS_RTMP_TYPE_SCRIPT) {

            read_type = human_flv_tag_type2string(type);
            frame_type = 'U';
        } else {
            read_type = 'U';
            frame_type = 'U';
        }

        free(data);
    }

    rtmp_destroy:
    srs_rtmp_destroy(rtmp);

    SrsAutoFree(SrsCodecSample, srsCodecSample);
    SrsAutoFree(SrsAvcAacCodec, srsAvcAacCodec);
}

int play_stream2(const char *url, short retry_times) {
    short times = 1;
    while (times <= retry_times) {
        srs_human_trace("Read packet error : %d.", ERROR_SOCKET_TIMEOUT);
        play_stream3(url);

        times++;

        this_thread::sleep_for(chrono::seconds(1));
    }

    srs_human_trace("Stop playing stream : %s.", url);
}

int play_stream(const char *url, short retry_times) {
    vector<string> vect = spit_str(url, ',');

    vector<thread> th(vect.size());

    for (int i = 0; i < vect.size(); ++i) {
        th[i] = thread(play_stream2, vect[i].c_str(), retry_times);
    }

    for (auto &j : th) {
        j.join();
    }
}


