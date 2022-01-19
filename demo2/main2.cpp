#include <iostream>
extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
};
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
int main(int argc, char *argv[])
{
    AVFormatContext *ifmt_ctx = NULL;
    AVPacket pkt;
    AVFrame *pframe = NULL;
    int ret, i;
    int videoindex = -1;

    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    const char *in_filename = "rtmp://58.200.131.2:1935/livetv/hunantv"; //芒果台rtmp地址
    const char *out_filename_v = "test.h264";                            // Output file URL
    
    // Register
    // av_register_all();
    
    // Network
    avformat_network_init();
    // Input
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0)
    {
        printf("Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0)
    {
        printf("Failed to retrieve input stream information");
        return -1;
    }

    videoindex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
        }
    }
    // Find H.264 Decoder
    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (pCodec == NULL)
    {
        printf("Couldn't find Codec.\n");
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Couldn't open codec.\n");
        return -1;
    }

    pframe = av_frame_alloc();
    if (!pframe)
    {
        printf("Could not allocate video frame\n");
        exit(1);
    }

    FILE *fp_video = fopen(out_filename_v, "wb+"); //用于保存H.264

    AVBitStreamFilterContext *h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");

    while (av_read_frame(ifmt_ctx, &pkt) >= 0)
    {
        if (pkt.stream_index == videoindex)
        {

            av_bitstream_filter_filter(h264bsfc, ifmt_ctx->streams[videoindex]->codec, NULL, &pkt.data, &pkt.size,
                                       pkt.data, pkt.size, 0);

            printf("Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);

            //保存为h.264 该函数用于测试
            // fwrite(pkt.data, 1, pkt.size, fp_video);

            // Decode AVPacket
            if (pkt.size)
            {
                ret = avcodec_send_packet(pCodecCtx, &pkt);
                if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    std::cout << "avcodec_send_packet: " << ret << std::endl;
                    continue;
                }
                // Get AVframe
                ret = avcodec_receive_frame(pCodecCtx, pframe);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    std::cout << "avcodec_receive_frame: " << ret << std::endl;
                    continue;
                }
            }
        }
        // Free AvPacket
        av_packet_unref(&pkt);
    }
    // Close filter
    av_bitstream_filter_close(h264bsfc);
    fclose(fp_video);
    avformat_close_input(&ifmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        printf("Error occurred.\n");
        return -1;
    }
    return 0;
}
