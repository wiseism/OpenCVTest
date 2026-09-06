// 示例 3：图像滤波与形态学（功能大类：图像滤波与形态学）
// 功能点：
//   1) 噪声模拟：椒盐噪声
//   2) 平滑滤波：boxFilter/blur、GaussianBlur、medianBlur、bilateralFilter
//   3) 锐化：filter2D + 拉普拉斯核
//   4) 形态学：erode / dilate / morphologyEx(开、闭、梯度)
// 运行方式：直接运行，输出 out_filter_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 画简单几何图形作为处理对象
static Mat makeShapes(int rows = 220, int cols = 320) {
    Mat img(rows, cols, CV_8UC1, Scalar(235));
    rectangle(img, Point(20, 20), Point(120, 110), Scalar(40), FILLED);
    circle(img, Point(240, 60), 40, Scalar(40), FILLED);
    circle(img, Point(180, 170), 50, Scalar(80), FILLED);
    rectangle(img, Point(260, 150), Point(310, 200), Scalar(120), FILLED);
    return img;
}

static void addSaltPepper(Mat& img, RNG& rng, int nPoints) {
    for (int i = 0; i < nPoints; ++i) {
        int x = rng.uniform(0, img.cols);
        int y = rng.uniform(0, img.rows);
        img.at<uchar>(y, x) = (i % 2 == 0) ? 0 : 255;
    }
}

int main() {
    const string prefix = "out_filter";
    const Mat clean = makeShapes();
    imwrite(prefix + "_clean.png", clean);

    // 1) 椒盐噪声
    Mat noisy = clean.clone();
    addSaltPepper(noisy, RNG(1234), 4000);
    imwrite(prefix + "_noisy.png", noisy);

    // 2) 平滑滤波：同一含噪图上比较 4 种滤波器
    Mat dst;
    blur(noisy, dst, Size(5, 5));
    imwrite(prefix + "_box5.png", dst);                 // 均值 5x5

    GaussianBlur(noisy, dst, Size(5, 5), 0, 0);
    imwrite(prefix + "_gauss5.png", dst);               // 高斯 5x5

    medianBlur(noisy, dst, 5);
    imwrite(prefix + "_median5.png", dst);              // 中值 5（对椒盐最有效）

    bilateralFilter(noisy, dst, 9, 75, 75);
    imwrite(prefix + "_bilateral9.png", dst);           // 双边（保边）
    cout << "[smooth] box/gauss/median/bilateral 结果已写出" << endl;

    // 3) 锐化：拉普拉斯核 0 -1 0 / -1 5 -1 / 0 -1 0
    Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    Mat sharp;
    filter2D(clean, sharp, CV_8U, kernel);
    imwrite(prefix + "_sharpened.png", sharp);

    // 4) 形态学：先二值化（深色物体 -> 前景 0）
    Mat bin;
    threshold(clean, bin, 127, 255, THRESH_BINARY); // 背景 255，物体 0
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));

    erode(bin, dst, element);
    imwrite(prefix + "_erode5.png", dst);
    dilate(bin, dst, element);
    imwrite(prefix + "_dilate5.png", dst);

    Mat opened, closed, grad;
    morphologyEx(bin, opened, MORPH_OPEN, element);
    morphologyEx(bin, closed, MORPH_CLOSE, element);
    morphologyEx(bin, grad, MORPH_GRADIENT, element);
    imwrite(prefix + "_open5.png", opened);
    imwrite(prefix + "_close5.png", closed);
    imwrite(prefix + "_gradient5.png", grad);
    cout << "[morph] erode/dilate/open/close/gradient(5x5) 结果已写出" << endl;

    // 形态学梯度应等于 dilate-erode 之差，做个自检
    Mat d2, e2, expect;
    dilate(bin, d2, element);
    erode(bin, e2, element);
    absdiff(d2, e2, expect);
    absdiff(expect, grad, dst);
    cout << "[check] |(dilate-erode)-morphGradient| 非零像素 = "
         << countNonZero(dst) << " (应接近 0)" << endl;
    return 0;
}
