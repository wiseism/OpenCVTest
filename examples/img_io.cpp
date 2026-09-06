// 示例 1：图像读写与显示（功能大类：图像读写与显示）
// 功能点：
//   1) imread / imwrite：PNG 无损写读回环校验
//   2) JPEG 压缩质量(IMWRITE_JPEG_QUALITY)对文件大小与误差的影响
//   3) 图像基本信息：宽/高/通道数/位深
//   4) split 拆分 BGR 通道并分别保存为灰度图
// 运行方式：直接运行即可，无需输入图片，在当前目录生成 out_img_io_*.png/jpg
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

// 生成 8x8 彩色棋盘（用于测试编码是否无损 / 误差可见）
static Mat makeCheckerboard(int blocks = 8, int block = 32) {
    Mat img(blocks * block, blocks * block, CV_8UC3, Scalar::all(255));
    for (int r = 0; r < blocks; ++r)
        for (int c = 0; c < blocks; ++c)
            if ((r + c) % 2 == 0)
                rectangle(img, Point(c * block, r * block),
                          Point((c + 1) * block - 1, (r + 1) * block - 1),
                          Scalar(20, 120, 220), FILLED);
    return img;
}

static long long fileSizeBytes(const string& path) {
    ifstream f(path, ios::binary | ios::ate);
    return f ? static_cast<long long>(f.tellg()) : -1;
}

int main() {
    const string prefix = "out_img_io";
    const Mat src = makeCheckerboard();

    // 1) PNG 无损往返：内存图 -> 磁盘 -> 内存，逐像素比对
    const string pngPath = prefix + "_checker.png";
    imwrite(pngPath, src);
    Mat back = imread(pngPath, IMREAD_COLOR);
    if (back.empty()) { cerr << "read back PNG failed" << endl; return 1; }

    Mat diff;
    absdiff(src, back, diff);
    int changed = countNonZero(diff.reshape(1)); // 三通道任一不同即算
    cout << "[PNG round-trip] changed pixels = " << changed
         << " (lossless -> expect 0)" << endl;

    // 2) JPEG：同一图像在 95 / 60 / 10 三档质量下的体积与平均绝对误差
    for (int q : {95, 60, 10}) {
        vector<int> params;
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(q);
        const string path = prefix + "_q" + to_string(q) + ".jpg";
        imwrite(path, src, params);

        Mat jpg = imread(path, IMREAD_COLOR);
        if (jpg.empty()) { cerr << "read back JPEG failed: " << path << endl; return 1; }
        absdiff(src, jpg, diff);
        Scalar s = sum(diff);
        double meanErr = (s[0] + s[1] + s[2]) / double(src.total() * 3.0);
        cout << "[JPEG q=" << q << "] size=" << fileSizeBytes(path)
             << " B, mean abs err=" << meanErr << endl;
    }

    // 3) 图像基本信息
    cout << "[info] " << src.cols << " x " << src.rows
         << " , channels=" << src.channels()
         << " , depth=" << src.depth() << " (CV_8U=0)" << endl;

    // 4) split 拆分通道
    vector<Mat> bgr;
    split(src, bgr);
    imwrite(prefix + "_ch_B.png", bgr[0]);
    imwrite(prefix + "_ch_G.png", bgr[1]);
    imwrite(prefix + "_ch_R.png", bgr[2]);
    cout << "[channels] B/G/R 单通道图已写出" << endl;

    return changed == 0 ? 0 : 1;
}
