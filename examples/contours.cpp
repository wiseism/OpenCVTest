// 示例 6：轮廓检测与形状分析（功能大类：轮廓与形状分析）
// 功能点：
//   1) findContours 查找外轮廓（RETR_EXTERNAL + CHAIN_APPROX_SIMPLE）
//   2) 几何特征：contourArea / arcLength / boundingRect / minAreaRect
//   3) moments 求质心
//   4) approxPolyDP 多边形逼近，依据顶点数识别三角形/矩形
//   5) 圆度(circularity)识别近似圆形
//   6) drawContours + 标注质心
// 运行方式：直接运行，输出 out_contours_*.png
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

static string guessShape(const vector<Point>& poly, double area, double per) {
    int sides = static_cast<int>(poly.size());
    if (sides == 3) return "triangle";
    if (sides == 4) {
        Rect b = boundingRect(poly);
        float ratio = static_cast<float>(max(b.width, b.height)) /
                      static_cast<float>(max(1, min(b.width, b.height)));
        return ratio > 1.25f ? "rectangle" : "square";
    }
    if (sides >= 6) {
        double circ = 4 * CV_PI * area / (per * per); // 圆度：正圆 -> 1
        if (circ > 0.75) return "circle";
        return "polygon";
    }
    return "polygon";
}

int main() {
    const string prefix = "out_contours";

    // 黑底 + 白色图形（正方形、圆形、三角形、扁矩形、带缺口矩形）
    Mat mask(220, 360, CV_8UC1, Scalar(0));
    rectangle(mask, Point(20, 20), Point(120, 120), Scalar(255), FILLED);       // 正方形
    circle(mask, Point(230, 70), 45, Scalar(255), FILLED);                       // 圆形
    Point tri[] = { Point(40, 190), Point(100, 145), Point(140, 200) };          // 三角形
    fillConvexPoly(mask, tri, 3, Scalar(255));
    rectangle(mask, Point(200, 150), Point(340, 195), Scalar(255), FILLED);      // 扁矩形
    imwrite(prefix + "_mask.png", mask);

    // 查找轮廓
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "[findContours] 找到 " << contours.size() << " 个轮廓" << endl;
    if (contours.empty()) { cerr << "no contours found" << endl; return 1; }

    Mat result;
    cvtColor(mask, result, COLOR_GRAY2BGR);
    Scalar colors[] = { Scalar(0, 0, 255), Scalar(0, 200, 0), Scalar(255, 0, 0),
                        Scalar(0, 200, 200), Scalar(200, 0, 200) };

    for (size_t i = 0; i < contours.size(); ++i) {
        const vector<Point>& c = contours[i];
        double area = contourArea(c);
        double per = arcLength(c, true);
        Rect br = boundingRect(c);
        RotatedRect mr = minAreaRect(c);
        Moments m = moments(c);
        Point2f centroid(static_cast<float>(m.m10 / m.m00),
                         static_cast<float>(m.m01 / m.m00));

        vector<Point> poly;
        approxPolyDP(c, poly, 0.03 * per, true);
        string name = guessShape(poly, area, per);

        drawContours(result, contours, static_cast<int>(i), colors[i % 5], 2);
        circle(result, centroid, 3, colors[i % 5], FILLED);

        cout << "  #" << i << " [" << name << "] sides=" << poly.size()
             << " area=" << area << " per=" << per
             << " bbox=" << br.width << "x" << br.height
             << " minAreaRect=" << mr.size.width << "x" << mr.size.height
             << " centroid=(" << centroid.x << "," << centroid.y << ")" << endl;
    }

    imwrite(prefix + "_result.png", result);
    cout << "[done] result 已保存，见 " << prefix << "_result.png" << endl;
    return 0;
}
