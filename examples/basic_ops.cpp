// 示例 2：图像基础操作与像素（功能大类：图像基础操作）
// 功能点：
//   1) at() 逐像素访问：手写取反，并用 bitwise_not 校验一致性
//   2) 亮度调整：cv::add 饱和加法
//   3) ROI 区域拷贝
//   4) 颜色空间转换：cvtColor BGR<->GRAY / BGR<->HSV
//   5) split / merge 通道拆分与合并
//   6) flip 水平/垂直翻转
// 运行方式：直接运行，输出 out_basic_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 生成 0..255 沿 x 方向的渐变图，便于观察各种操作的效果
static Mat makeGradient(int rows, int cols) {
    Mat g(rows, cols, CV_8UC1);
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            g.at<uchar>(r, c) = saturate_cast<uchar>(255.0 * c / (cols - 1));
    return g;
}

int main() {
    const string prefix = "out_basic";
    Mat gray = makeGradient(96, 256);
    imwrite(prefix + "_gradient.png", gray);

    // 1) 手写取反 vs bitwise_not
    Mat invLoop = Mat::zeros(gray.size(), gray.type());
    for (int r = 0; r < gray.rows; ++r)
        for (int c = 0; c < gray.cols; ++c)
            invLoop.at<uchar>(r, c) = 255 - gray.at<uchar>(r, c);

    Mat invApi;
    bitwise_not(gray, invApi);
    Mat diff;
    absdiff(invLoop, invApi, diff);
    int mismatch = countNonZero(diff);
    cout << "[pixel invert] manual == bitwise_not, mismatched px = "
         << mismatch << endl;
    imwrite(prefix + "_invert.png", invLoop);

    // 2) 亮度增加 60（饱和）
    Mat brighter;
    add(gray, Scalar(60), brighter);
    imwrite(prefix + "_brighten60.png", brighter);

    // 3) ROI：把左上 64x64 拷贝到右下 64x64
    Mat work = gray.clone();
    Rect roi(0, 0, 64, 64);
    Mat roiDst = work(Rect(work.cols - 64, work.rows - 64, 64, 64));
    work(roi).copyTo(roiDst);
    imwrite(prefix + "_roi_copy.png", work);

    // 4) 颜色空间转换
    Mat color;
    cvtColor(gray, color, COLOR_GRAY2BGR);
    Mat hsv;
    cvtColor(color, hsv, COLOR_BGR2HSV);
    imwrite(prefix + "_hsv_h.png", hsv); // 伪彩色：hsv 3 通道直接写
    cout << "[cvtColor] GRAY->BGR->HSV 完成，HSV 尺寸 "
         << hsv.cols << "x" << hsv.rows << endl;

    // 5) split / merge
    vector<Mat> channels;
    split(color, channels);
    Mat merged;
    merge(channels, merged);
    absdiff(color, merged, diff);
    cout << "[split/merge] mismatch px = "
         << countNonZero(diff.reshape(1)) << endl; // 三通道转单通道再统计

    // 6) flip
    Mat hFlip, vFlip;
    flip(gray, hFlip, 1);   // 水平（绕 Y 轴）
    flip(gray, vFlip, 0);   // 垂直（绕 X 轴）
    imwrite(prefix + "_flip_h.png", hFlip);
    imwrite(prefix + "_flip_v.png", vFlip);

    // 汇总灰度统计
    double mn, mx;
    minMaxLoc(gray, &mn, &mx);
    cout << "[stats] gray min=" << mn << " max=" << mx << endl;
    return 0;
}
