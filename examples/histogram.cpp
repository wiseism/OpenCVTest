// 示例 7：直方图与色彩分析（功能大类：直方图与色彩分析）
// 功能点：
//   1) calcHist 计算灰度直方图，寻找峰值 bin
//   2) 直方图可视化：normalize 后逐 bin 绘制成图
//   3) equalizeHist 直方图均衡化，对比均衡前后 min/max/方差
// 运行方式：直接运行，输出 out_hist_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 生成对比度偏低的图像（灰度集中在窄区间，便于观察均衡化效果）
static Mat makeLowContrast(int rows = 120, int cols = 200) {
    Mat img(rows, cols, CV_8UC1, Scalar(110));
    rectangle(img, Point(30, 30), Point(90, 90), Scalar(150), FILLED);
    circle(img, Point(150, 60), 35, Scalar(160), FILLED);
    rectangle(img, Point(100, 20), Point(130, 45), Scalar(60), FILLED);
    return img;
}

// 把 1x256 直方图画成 256x220 的可视化图
static Mat drawHist(const Mat& hist, int imgH = 220) {
    Mat histImg(imgH, 256, CV_8UC1, Scalar(255));
    Mat norm;
    normalize(hist, norm, 0, imgH, NORM_MINMAX);
    for (int i = 0; i < 256; ++i) {
        int h = cvRound(norm.at<float>(i));
        rectangle(histImg, Point(i, imgH - 1), Point(i, imgH - 1 - h), Scalar(0), FILLED);
    }
    return histImg;
}

int main() {
    const string prefix = "out_hist";
    const Mat src = makeLowContrast();
    imwrite(prefix + "_lowcontrast.png", src);

    // 1) 直方图计算
    Mat hist;
    int bins = 256;
    float range[] = { 0, 256 };
    const float* ranges[] = { range };
    calcHist(&src, 1, 0, Mat(), hist, 1, &bins, ranges, true, false);

    double maxVal = 0;
    minMaxLoc(hist, nullptr, &maxVal);
    int peakBin = 0;
    for (int i = 0; i < bins; ++i)
        if (hist.at<float>(i) >= maxVal) { peakBin = i; maxVal = hist.at<float>(i); }
    cout << "[histogram] peak bin = " << peakBin
         << " (灰度为 " << peakBin << " 的像素最多, count=" << maxVal << ")" << endl;

    // 2) 直方图可视化
    imwrite(prefix + "_histogram.png", drawHist(hist));

    // 3) 均衡化对比
    Mat eq;
    equalizeHist(src, eq);
    imwrite(prefix + "_equalized.png", eq);

    Mat eqHist;
    calcHist(&eq, 1, 0, Mat(), eqHist, 1, &bins, ranges, true, false);
    imwrite(prefix + "_hist_equalized.png", drawHist(eqHist));

    Scalar m1, s1, m2, s2;
    meanStdDev(src, m1, s1);
    meanStdDev(eq, m2, s2);
    double a1, b1, a2, b2;
    minMaxLoc(src, &a1, &b1);
    minMaxLoc(eq, &a2, &b2);
    cout << "[before] min=" << a1 << " max=" << b1
         << " mean=" << m1[0] << " std=" << s1[0] << endl;
    cout << "[after ] min=" << a2 << " max=" << b2
         << " mean=" << m2[0] << " std=" << s2[0] << " (方差应变大)" << endl;
    return 0;
}
