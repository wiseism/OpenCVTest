// 示例 5：几何变换（功能大类：几何变换）
// 功能点：
//   1) resize 缩放（半倍 / 2 倍，INTER_LINEAR）
//   2) rotate 90° 旋转（ROTATE_90_CLOCKWISE）
//   3) flip 镜像翻转（水平 / 垂直）
//   4) warpAffine 仿射：getRotationMatrix2D 任意角度旋转
//   5) warpPerspective 透视：getPerspectiveTransform 四点映射
// 运行方式：直接运行，输出 out_geometry_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 棋盘格 + 中心圆，可直观看出旋转/透视效果
static Mat makePattern(int rows = 256, int cols = 256) {
    Mat img(rows, cols, CV_8UC1, Scalar::all(255));
    int block = 32;
    for (int r = 0; r < rows / block; ++r)
        for (int c = 0; c < cols / block; ++c)
            if ((r + c) % 2 == 0)
                rectangle(img, Point(c * block, r * block),
                          Point((c + 1) * block - 1, (r + 1) * block - 1),
                          Scalar(0), FILLED);
    circle(img, Point(cols / 2, rows / 2), 40, Scalar(128), FILLED);
    return img;
}

int main() {
    const string prefix = "out_geometry";
    const Mat src = makePattern();
    cout << "[input] " << src.cols << "x" << src.rows << endl;

    // 1) resize
    Mat small, large;
    resize(src, small, Size(0, 0), 0.5, 0.5, INTER_LINEAR);
    resize(src, large, Size(0, 0), 2.0, 2.0, INTER_LINEAR);
    imwrite(prefix + "_half.png", small);
    imwrite(prefix + "_double.png", large);
    cout << "[resize] half=" << small.cols << "x" << small.rows
         << " , double=" << large.cols << "x" << large.rows << endl;

    // 2) rotate 90°
    Mat rot90;
    rotate(src, rot90, ROTATE_90_CLOCKWISE);
    imwrite(prefix + "_rot90.png", rot90);

    // 3) flip
    Mat flipH, flipV;
    flip(src, flipH, 1);
    flip(src, flipV, 0);
    imwrite(prefix + "_flip_h.png", flipH);
    imwrite(prefix + "_flip_v.png", flipV);

    // 4) warpAffine：绕中心旋转 45°（保持输出尺寸，边缘以 0 填充）
    Mat M = getRotationMatrix2D(Point2f(src.cols / 2.f, src.rows / 2.f), 45.0, 1.0);
    Mat affine;
    warpAffine(src, affine, M, src.size(), INTER_LINEAR, BORDER_CONSTANT);
    imwrite(prefix + "_affine45.png", affine);

    // 5) warpPerspective：把整幅图映射成“梯形”
    int w = src.cols, h = src.rows;
    vector<Point2f> srcPts = { Point2f(0, 0), Point2f(w, 0),
                               Point2f(0, h), Point2f(w, h) };
    vector<Point2f> dstPts = { Point2f(w * 0.12f, 0), Point2f(w * 0.88f, 0),
                               Point2f(0, h), Point2f(w, h) };
    Mat P = getPerspectiveTransform(srcPts, dstPts);
    Mat persp;
    warpPerspective(src, persp, P, src.size(), INTER_LINEAR, BORDER_CONSTANT);
    imwrite(prefix + "_perspective.png", persp);

    cout << "[warpAffine] 45° 仿射旋转输出" << endl;
    cout << "[warpPerspective] 透视梯形输出" << endl;
    return 0;
}
