// 示例 8：特征检测与匹配（功能大类：特征检测与匹配）
// 功能点：
//   1) ORB 特征点检测（棋盘格产生大量角点）
//   2) ORB 描述子计算（32 字节/个）
//   3) BFMatcher 暴力匹配（NORM_HAMMING）
//   4) 按距离排序取优 + drawMatches 可视化
//   5) drawKeypoints 显示单图关键点
// 说明：程序把同一棋盘格平移到更大画布生成第二幅图，两者应能匹配上。
// 运行方式：直接运行，输出 out_features2d_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

static Mat makeCheckerboard(int blocks = 8, int block = 32) {
    Mat img(blocks * block, blocks * block, CV_8UC1, Scalar::all(255));
    for (int r = 0; r < blocks; ++r)
        for (int c = 0; c < blocks; ++c)
            if ((r + c) % 2 == 0)
                rectangle(img, Point(c * block, r * block),
                          Point((c + 1) * block - 1, (r + 1) * block - 1),
                          Scalar(0), FILLED);
    return img;
}

int main() {
    const string prefix = "out_features2d";
    Mat imgA = makeCheckerboard();                       // 256x256
    Mat imgB = Mat::zeros(Size(340, 300), imgA.type());  // 更大画布
    imgA.copyTo(imgB(Rect(45, 25, imgA.cols, imgA.rows))); // 平移(45,25)

    Ptr<ORB> orb = ORB::create();
    vector<KeyPoint> kpA, kpB;
    Mat descA, descB;
    orb->detectAndCompute(imgA, noArray(), kpA, descA);
    orb->detectAndCompute(imgB, noArray(), kpB, descB);
    cout << "[ORB] keypoints A=" << kpA.size() << " , B=" << kpB.size() << endl;
    if (kpA.empty() || kpB.empty()) {
        cerr << "no keypoints detected" << endl;
        return 1;
    }
    cout << "[ORB] descriptor type=" << descA.type()
         << " (CV_8U=" << CV_8U << ") , cols=" << descA.cols << endl;

    // 单图关键点可视化
    Mat withKp;
    drawKeypoints(imgA, kpA, withKp, Scalar(0, 0, 255),
                  DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imwrite(prefix + "_keypoints_a.png", withKp);

    // 暴力匹配 + 按距离排序取优
    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher.match(descA, descB, matches);
    sort(matches.begin(), matches.end(),
         [](const DMatch& x, const DMatch& y) { return x.distance < y.distance; });

    size_t keep = min<size_t>(60, matches.size());
    vector<DMatch> good(matches.begin(), matches.begin() + keep);

    double distSum = 0;
    for (size_t i = 0; i < good.size(); ++i) distSum += good[i].distance;
    cout << "[match] total=" << matches.size() << " , keep=" << good.size()
         << " , meanDist(best " << good.size() << ")=" << distSum / good.size() << endl;
    if (good.size() < 10) {
        cerr << "too few good matches" << endl;
        return 1;
    }

    Mat canvas;
    drawMatches(imgA, kpA, imgB, kpB, good, canvas, Scalar(0, 0, 255),
                Scalar(0, 255, 0), vector<char>(),
                DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    imwrite(prefix + "_matches.png", canvas);
    cout << "[done] 匹配图已保存" << endl;
    return 0;
}
