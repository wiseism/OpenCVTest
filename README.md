# OpenCVTest

基于 C++ 的 OpenCV 学习与示例工程。同一套源码可跨平台构建：

- **Windows（MSVC）**
  - `OpenCVTest.sln`（Debug/Release x64，OpenCV 官方 vc14 预编译包，`OpenCVRoot` 属性默认 `D:\opencv`）
  - CMake：`cmake -S . -B build -A x64 && cmake --build build --config Release`
- **Linux（CMake）**：`cmake -S . -B build && cmake --build build`（apt 安装 `libopencv-dev`）

GitHub Actions 三条流水线覆盖：Ubuntu CMake、Windows .sln(MSBuild)、Windows CMake。

## OpenCV 功能大类总览

| # | 功能大类 | 示例程序 | 详细设计 |
|---|----------|----------|----------|
| 1 | 图像读写与显示 | `examples/img_io.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 2 | 图像基础操作 | `examples/basic_ops.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 3 | 图像滤波与形态学 | `examples/filter_morph.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 4 | 阈值分割与边缘检测 | `examples/edge_threshold.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 5 | 几何变换 | `examples/geometry.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 6 | 轮廓检测与形状分析 | `examples/contours.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 7 | 直方图与色彩分析 | `examples/histogram.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |
| 8 | 特征检测与匹配 | `examples/features2d.cpp` | [docs/opencv-examples.md](#opencv-示例程序详细设计) |

### 大类 1：图像读写与显示
程序：`examples/img_io.cpp`
- `imread` / `imwrite` 读写图像，PNG 无损往返自校验（逐像素比对，要求差异为 0）
- JPEG 有损压缩：`IMWRITE_JPEG_QUALITY` 质量档(95/60/10)对文件体积与平均绝对误差的影响
- 图像基本信息查询：宽 / 高 / 通道数 / 位深
- `split` 拆分 BGR 通道，各通道单独存为灰度图

### 大类 2：图像基础操作
程序：`examples/basic_ops.cpp`
- 像素级访问 `at()`：手写取反并与 `bitwise_not` 逐像素比对
- 亮度调整：`add` 饱和加法
- ROI 区域拷贝：左上角图块复制到右下角
- 颜色空间转换：`cvtColor` 的 BGR→GRAY / GRAY→BGR / BGR→HSV
- 通道拆分与合并：`split` / `merge`
- 翻转：`flip` 水平 / 垂直

### 大类 3：图像滤波与形态学
程序：`examples/filter_morph.cpp`
- 椒盐噪声模拟（`RNG` 随机撒点）
- 平滑滤波对比：`blur`(均值)、`GaussianBlur`(高斯)、`medianBlur`(中值，抗椒盐最佳)、`bilateralFilter`(双边保边)
- 锐化：`filter2D` + 拉普拉斯核
- 形态学：`erode` / `dilate` / `morphologyEx`（开运算、闭运算、形态学梯度，`getStructuringElement` 取核）
- 自校验：`|(dilate-erode) - morphologyEx(gradient)|` 应为 0

### 大类 4：阈值分割与边缘检测
程序：`examples/edge_threshold.cpp`
- 固定阈值 `threshold(THRESH_BINARY)` 与 Otsu 自动阈值（输出自动阈值）
- `adaptiveThreshold` 自适应阈值（光照渐变场景）
- `Sobel` X/Y 一阶导数及梯度幅值（16 位中间量 + `convertScaleAbs`）
- `Laplacian` 二阶导数
- `Canny` 双阈值边缘检测

### 大类 5：几何变换
程序：`examples/geometry.cpp`
- `resize` 缩放（0.5x / 2x）
- `rotate` 90° 整数倍旋转
- `flip` 镜像翻转
- `warpAffine` + `getRotationMatrix2D`：任意角度仿射旋转
- `warpPerspective` + `getPerspectiveTransform`：四点透视变换

### 大类 6：轮廓检测与形状分析
程序：`examples/contours.cpp`
- `findContours`（`RETR_EXTERNAL` + `CHAIN_APPROX_SIMPLE`）提取外轮廓
- 几何特征：`contourArea` / `arcLength` / `boundingRect` / `minAreaRect`
- `moments` 计算质心
- `approxPolyDP` 多边形逼近，按顶点数 + 圆度识别 三角形/矩形/圆/多边形
- `drawContours` 标注轮廓与质心

### 大类 7：直方图与色彩分析
程序：`examples/histogram.cpp`
- `calcHist` 灰度直方图计算与峰值 bin 定位
- 直方图可视化：`normalize` 后逐 bin 绘制
- `equalizeHist` 均衡化，对比均衡前后 min/max/均值/标准差

### 大类 8：特征检测与匹配
程序：`examples/features2d.cpp`
- `ORB::create` 特征点检测（棋盘格产生密集角点）
- 描述子计算与类型检查
- `BFMatcher`（`NORM_HAMMING`）暴力匹配，按距离排序取优
- `drawKeypoints` / `drawMatches` 可视化
- 校验：两幅平移视图应能匹配出足够多特征点

## 示例运行与回归

所有示例程序均**自生成测试图像、不依赖真实图片与 GUI**，直接运行即输出 `out_*.png` 到当前目录并打印校验信息，退出码 0 表示通过；它们同时注册为 CTest 用例（Ubuntu CI 自动执行）。

```bash
# 构建后运行全部示例并做回归
ctest --test-dir build -C Release --output-on-failure
# 或单独运行某个示例
./build/Release/img_io
```

## 文档
- 各功能大类的示例程序详细设计（API、处理流程、输出与自校验设计）：[docs/opencv-examples.md](docs/opencv-examples.md)
