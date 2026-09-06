// 示例 4：阈值分割与边缘检测（功能大类：阈值与边缘检测）
// 功能点：
//   1) threshold：固定阈值二值化
//   2) Otsu 自动阈值（THRESH_OTSU），输出自动求得的阈值
//   3) adaptiveThreshold 自适应阈值（应对光照不均）
//   4) Sobel 一阶导数边缘（X/Y 方向与合成梯度幅值）
//   5) Laplacian 二阶导数边缘
//   6) Canny 双阈值边缘检测
// 运行方式：直接运行，输出 out_edge_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 造一张带亮度不均背景的测试图（左侧暗、右侧亮，中央有深色图形）
static Mat makeScene(int rows = 200, int cols = 300) {
    Mat img(rows, cols, CV_8UC1);
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            img.at<uchar>(r, c) = saturate_cast<uchar>(90.0 + 90.0 * c / cols);
    rectangle(img, Point(40, 40), Point(120, 150), Scalar(40), FILLED);
    circle(img, Point(230, 100), 45, Scalar(30), FILLED);
    return img;
}

static void reportNonZero(const string& tag, const Mat& bin) {
    cout << "  " << tag << " 非零像素 = " << countNonZero(bin) << endl;
}

int main() {
    const string prefix = "out_edge";
    const Mat src = makeScene();
    imwrite(prefix + "_scene.png", src);

    // 1) 固定阈值 127
    Mat bin;
    threshold(src, bin, 127, 255, THRESH_BINARY);
    imwrite(prefix + "_binary127.png", bin);
    reportNonZero("threshold(127)", bin);

    // 2) Otsu：不指定阈值，自动计算
    double otsuThr = threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imwrite(prefix + "_otsu.png", bin);
    cout << "[otsu] auto threshold = " << otsuThr << endl;

    // 3) 自适应阈值（对大块邻域灰度差判断，光照渐变下表现好）
    adaptiveThreshold(src, bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
                      THRESH_BINARY, 31, 5);
    imwrite(prefix + "_adaptive31.png", bin);
    reportNonZero("adaptiveThreshold(31,5)", bin);

    // 4) Sobel X / Y 及合成幅值（16 位中间量需 convertScaleAbs 后保存）
    Mat gx16, gy16, gxA, gyA;
    Sobel(src, gx16, CV_16S, 1, 0, 3);
    Sobel(src, gy16, CV_16S, 0, 1, 3);
    convertScaleAbs(gx16, gxA);
    convertScaleAbs(gy16, gyA);
    Mat sobelMag;
    add(gxA, gyA, sobelMag);
    imwrite(prefix + "_sobel_x.png", gxA);
    imwrite(prefix + "_sobel_y.png", gyA);
    imwrite(prefix + "_sobel_mag.png", sobelMag);
    reportNonZero("Sobel mag(阈值>30)", sobelMag > 30);

    // 5) Laplacian
    Mat lap16, lapA;
    Laplacian(src, lap16, CV_16S, 3);
    convertScaleAbs(lap16, lapA);
    imwrite(prefix + "_laplacian.png", lapA);
    reportNonZero("Laplacian(>30)", lapA > 30);

    // 6) Canny
    Mat edges;
    Canny(src, edges, 50, 150, 3);
    imwrite(prefix + "_canny.png", edges);
    reportNonZero("Canny(50,150)", edges);

    return 0;
}
