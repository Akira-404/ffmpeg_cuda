#include <iostream>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include "libavutil/samplefmt.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/bsf.h"
};
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

void AVFrame2Img(AVFrame *pFrame, cv::Mat &img);
void Yuv420p2Rgb32(const uchar *yuvBuffer_in, const uchar *rgbBuffer_out, int width, int height);

int main(int argc, char *argv[])
{
    // AVFormatContext: 封装的上下文
    // AVStream : 存放的是音频流或视频流的参数信息
    // AVPacket: 针对于具体的解封装完后的一个一个的数据包

    for (int i = 0; i < argc; i++)
        std::cout << "argument[" << i << "]:" << argv[i] << std::endl;
    
    const char *in_filename = nullptr;
    const char *out_filename_v = nullptr;

    if (argv[1] == NULL)
        in_filename = "http://img.ksbbs.com/asset/Mon_1704/15868902d399b87.flv";
    else
        in_filename = argv[1];

    out_filename_v = "test.h264"; // Output file URL
    
    printf("in_filename:%s\n", in_filename);
    printf("out_filename_v:%s\n", out_filename_v);

    AVFormatContext *ifmt_ctx = nullptr;
    AVPacket pkt;
    AVFrame *pFrame = nullptr;
    int ret, i;
    int videoIndex = -1;

    AVCodecContext *pCodecCtx=nullptr;
    const AVCodec *pCodec=nullptr;
        
    // Init Network
    avformat_network_init();

    // Input
    // 用于打开多媒体数据并解析
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

    videoIndex = -1;
    AVCodecID codecId;
    AVStream *stream=nullptr;

    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            stream = ifmt_ctx->streams[i];
            codecId = stream->codecpar->codec_id;
        }
    }
    std::cout<<"codecId:"<<codecId<<std::endl;
    // Find H.264 Decoder
    // pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    //查找编码器信息
    pCodec = avcodec_find_decoder(codecId);
    std::cout<<"pCodec->long_name:"<<pCodec->long_name<<std::endl;
    if (pCodec == NULL)
    {
        printf("Couldn't find Codec.\n");
        return -1;
    }
    //根据编码器申请配置编码器上下文空间
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    //根据流信息配置编码器上下文参数
    ret = avcodec_parameters_to_context(pCodecCtx, stream->codecpar);
    if (ret < 0)
    {
        printf("Failed to copy context from input to output stream codec context\n");
        return -1;
    }

    // 打开视频解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Couldn't open codec.\n");
        return -1;
    }
    // 申请一个frame空间
    pFrame = av_frame_alloc();
    if (!pFrame)
    {
        printf("Could not allocate video frame\n");
        exit(1);
    }

    FILE *fp_video = fopen(out_filename_v, "wb+"); //用于保存H.264

    cv::Mat cvImg;

    AVBSFContext *bsf_ctx = nullptr;
    // AVBitStreamFilterContext *h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
    const AVBitStreamFilter *h264bsfc = av_bsf_get_by_name("h264_mp4toannexb");
    av_bsf_alloc(h264bsfc, &bsf_ctx);

    while (av_read_frame(ifmt_ctx, &pkt) >= 0)
    {
        if (pkt.stream_index == videoIndex)
        {
            av_bsf_send_packet(bsf_ctx, &pkt);
            av_bsf_receive_packet(bsf_ctx, &pkt);

            // av_bitstream_filter_filter(h264bsfc, ifmt_ctx->streams[videoIndex]->codec, NULL, &pkt.data, &pkt.size,pkt.data, pkt.size, 0);

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
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    std::cout << "avcodec_receive_frame: " << ret << std::endl;
                    continue;
                }
                // AVframe to rgb
                AVFrame2Img(pFrame, cvImg);
                cv::imshow("out", cvImg);
                cv::waitKey(25);
                cvImg.release();
            }
        }
        // Free AvPacket
        av_packet_unref(&pkt);
    }

    // Close filter
    av_bsf_free(&bsf_ctx);
    // av_bitstream_filter_close(h264bsfc);
    std::fclose(fp_video);
    avformat_close_input(&ifmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        printf("Error occurred.\n");
        return -1;
    }
    return 0;
}

void Yuv420p2Rgb32(const uchar *yuvBuffer_in, const uchar *rgbBuffer_out, int width, int height)
{
    uchar *yuvBuffer = (uchar *)yuvBuffer_in;
    uchar *rgb32Buffer = (uchar *)rgbBuffer_out;

    int channels = 3;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * width + x;

            int indexY = y * width + x;
            int indexU = width * height + y / 2 * width / 2 + x / 2;
            int indexV = width * height + width * height / 4 + y / 2 * width / 2 + x / 2;

            uchar Y = yuvBuffer[indexY];
            uchar U = yuvBuffer[indexU];
            uchar V = yuvBuffer[indexV];

            int R = Y + 1.402 * (V - 128);
            int G = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128);
            int B = Y + 1.772 * (U - 128);
            R = (R < 0) ? 0 : R;
            G = (G < 0) ? 0 : G;
            B = (B < 0) ? 0 : B;
            R = (R > 255) ? 255 : R;
            G = (G > 255) ? 255 : G;
            B = (B > 255) ? 255 : B;

            rgb32Buffer[(y * width + x) * channels + 2] = uchar(R);
            rgb32Buffer[(y * width + x) * channels + 1] = uchar(G);
            rgb32Buffer[(y * width + x) * channels + 0] = uchar(B);
        }
    }
}

void AVFrame2Img(AVFrame *pFrame, cv::Mat &img)
{
    int frameHeight = pFrame->height;
    int frameWidth = pFrame->width;
    int channels = 3;
    //输出图像分配内存
    img = cv::Mat::zeros(frameHeight, frameWidth, CV_8UC3);
    // cv::Mat output = cv::Mat::zeros(frameHeight, frameWidth, CV_8U);

    //创建保存yuv数据的buffer
    uchar *pDecodedBuffer = (uchar *)malloc(frameHeight * frameWidth * sizeof(uchar) * channels);

    //从AVFrame中获取yuv420p数据，并保存到buffer
    int i, j, k;
    //拷贝y分量
    for (i = 0; i < frameHeight; i++)
    {
        memcpy(pDecodedBuffer + frameWidth * i,
               pFrame->data[0] + pFrame->linesize[0] * i,
               frameWidth);
    }
    //拷贝u分量
    for (j = 0; j < frameHeight / 2; j++)
    {
        memcpy(pDecodedBuffer + frameWidth * i + frameWidth / 2 * j,
               pFrame->data[1] + pFrame->linesize[1] * j,
               frameWidth / 2);
    }
    //拷贝v分量
    for (k = 0; k < frameHeight / 2; k++)
    {
        memcpy(pDecodedBuffer + frameWidth * i + frameWidth / 2 * j + frameWidth / 2 * k,
               pFrame->data[2] + pFrame->linesize[2] * k,
               frameWidth / 2);
    }

    //将buffer中的yuv420p数据转换为RGB;
    Yuv420p2Rgb32(pDecodedBuffer, img.data, frameWidth, frameHeight);

    //释放buffer
    std::free(pDecodedBuffer);
    // output.release();
}