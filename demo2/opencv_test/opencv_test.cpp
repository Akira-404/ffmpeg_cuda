#include <opencv2/opencv.hpp>
using namespace cv;
int main()
{
    Mat img = imread("/home/cv/ffmpeg_code/demo2/opencv_test/img.jpg");
    imshow("img", img);
    waitKey(0);
}