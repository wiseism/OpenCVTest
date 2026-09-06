# OpenCV 示例程序详细设计

本文档对应仓库根 `examples/*.cpp` 的 8 个示例程序，覆盖 8 个 OpenCV 功能大类。
所有示例的共同设计约定：

- **零外部依赖**：程序内部合成测试图像（棋盘、渐变、几何图形等），无需真实图片；
- **无 GUI**：不使用 `imshow`，避免无头（headless）环境挂起；结果一律 `imwrite` 输出 `out_*.png`；
- **可回归**：程序内做自校验（比较像素、统计量、轮廓数等），全部通过返回 `0`；
- **API 兼容**：仅使用 OpenCV 3.2(main modules) 与 4.x 通用的 API；
- **构建**：由 CMakeLists 的 `file(GLOB examples/*.cpp)` 自动生成独立可执行文件并注册 CTest；
- **运行**：在构建目录直接运行可执行文件，产物输出到当前工作目录。

---

## 示例 1：图像读写与显示 — `examples/img_io.cpp`

### 目标
掌握图像文件的读写、编码格式与质量、图像基本属性查询、通道拆分。

### 涉及 API
`imread` / `imwrite`、`IMWRITE_JPEG_QUALITY`、`split`、`rectangle`、`absdiff`、`countNonZero`、`sum`

### 处理流程
1. 程序内生成 8×8 彩色棋盘（256×256，`CV_8UC3`）；
2. **PNG 无损回环**：`imwrite` 写盘 → `imread` 读回 → `absdiff` 与内存图比较 → `countNonZero(diff.reshape(1))` 统计差异像素，期望 0；
3. **JPEG 质量实验**：同一棋盘分别以质量 95 / 60 / 10 写 JPEG，读回后统计
   `平均绝对误差 = sum(|diff|) / (rows*cols*3)`，并打印文件字节数，观察"体积 vs 质量"的折衷；
4. 打印图像基本信息（宽、高、通道数、`depth()`）；
5. `split` 把 BGR 拆成三个单通道图并分别写盘。

### 输入 / 输出
- 输入：无（自合成）
- 输出：`out_img_io_checker.png`、`out_img_io_q95/60/10.jpg`、`out_img_io_ch_{B,G,R}.png`

### 自校验与预期
- PNG 回环差异像素 = 0（无损），非 0 则返回 1；
- JPEG 平均误差随质量下降而上升（输出供观察，不断言）。

---

## 示例 2：图像基础操作 — `examples/basic_ops.cpp`

### 目标
掌握逐像素操作、ROI、颜色空间、通道运算与翻转。

### 涉及 API
`Mat::at`、`bitwise_not`、`add`、`Rect`(ROI)、`cvtColor`、`split/merge`、`flip`、`minMaxLoc`

### 处理流程
1. 生成沿 X 方向 0→255 的灰度渐变图；
2. **逐像素取反**：双重循环 `at<uchar>` 手写 `255-v`，与 `bitwise_not` 结果做差，期望 0 差异（验证手写与 API 一致）；
3. **亮度**：`add(gray, Scalar(60))` 饱和加亮；
4. **ROI**：取左上 64×64 子块 `work(roi)`，`copyTo` 到右下角区域；
5. **颜色空间**：GRAY→BGR→HSV 两次转换并写盘；
6. **通道**：`split` 再 `merge`，与原始图像比较应无差异；
7. **翻转**：`flip` 参数 1（水平）、0（垂直）；
8. 输出灰度 min/max 统计。

### 输入 / 输出
- 输出：`out_basic_gradient.png`、`out_basic_invert.png`、`out_basic_brighten60.png`、
  `out_basic_roi_copy.png`、`out_basic_hsv_h.png`、`out_basic_flip_{h,v}.png`

### 自校验与预期
- 手写取反与 API 一致（0 差异）；
- `split`→`merge` 无损（0 差异）。

---

## 示例 3：图像滤波与形态学 — `examples/filter_morph.cpp`

### 目标
掌握噪声模拟、四大平滑滤波的差异、锐化以及形态学基本/组合运算。

### 涉及 API
`blur`、`GaussianBlur`、`medianBlur`、`bilateralFilter`、`filter2D`、`getStructuringElement`、
`erode`、`dilate`、`morphologyEx`、`threshold`、`RNG`、`Mat_<float>`(核初始化)

### 处理流程
1. 合成"深色几何图形 + 浅灰背景"图（`makeShapes`），另复制一份注入 4000 个椒盐噪声点；
2. 对含噪图依次施加 4 种滤波并各自写盘（直观对比：中值对椒盐效果最好、双边保边）；
3. 对干净图做拉普拉斯核锐化（核 `[[0,-1,0],[-1,5,-1],[0,-1,0]]`）；
4. 二值化后取 5×5 矩形结构元素，执行 `erode/dilate/open/close/gradient`；
5. 自校验：`morphologyEx(GRADIENT)` 应与 `dilate-erode`（`absdiff`）一致。

### 输入 / 输出
- 输出：`out_filter_clean/noisy/box5/gauss5/median5/bilateral9/sharpened.png`、
  `out_filter_erode5/dilate5/open5/close5/gradient5.png`

### 自校验与预期
- 形态学梯度两种算法差异≈0。

---

## 示例 4：阈值分割与边缘检测 — `examples/edge_threshold.cpp`

### 目标
掌握灰度图阈值化（固定/Otsu/自适应）与三类边缘检测算子。

### 涉及 API
`threshold`(+`THRESH_OTSU`)、`adaptiveThreshold`、`Sobel`、`Laplacian`、`Canny`、`convertScaleAbs`

### 处理流程
1. 合成"亮度左暗右亮渐变背景 + 深色矩形/圆"场景图（光照不均，便于比较三种阈值化）；
2. 固定阈值 127 二值化；
3. Otsu：不设阈值调用，打印算法自动求得的最优阈值；
4. 自适应阈值：`ADAPTIVE_THRESH_GAUSSIAN_C`，块 31、偏移 5；
5. Sobel：分别求 X、Y 方向一阶导数（`CV_16S`），`convertScaleAbs` 后再相加得幅值；
6. Laplacian 二阶导数同理转 8U；
7. Canny 双阈值 50/150；
8. 每个二值/边缘结果打印非零像素数，确保非空。

### 输入 / 输出
- 输出：`out_edge_scene/binary127/otsu/adaptive31/sobel_x/sobel_y/sobel_mag/laplacian/canny.png`

### 自校验与预期
- 各边缘图非零像素 > 0（合成图确有边缘）。

---

## 示例 5：几何变换 — `examples/geometry.cpp`

### 目标
掌握缩放、旋转、镜像与仿射/透视变换的矩阵思想与用法。

### 涉及 API
`resize`、`rotate`、`flip`、`getRotationMatrix2D`、`warpAffine`、
`getPerspectiveTransform`、`warpPerspective`

### 处理流程
1. 合成棋盘 + 中心圆测试图（网格能直观反映几何失真）；
2. `resize`：0.5 倍与 2 倍；
3. `rotate`：顺时针 90°；
4. `flip`：水平/垂直镜像；
5. 仿射：绕中心旋转 45°（缩放 1.0），输出尺寸保持原图、边缘 `BORDER_CONSTANT`；
6. 透视：取原图四角作为源点，映射到"梯形"目标点，`warpPerspective` 输出。

### 输入 / 输出
- 输出：`out_geometry_half/double/rot90/flip_h/flip_v/affine45/perspective.png`

### 自校验与预期
- resize 后尺寸打印复核（0.5x=128、2x=512）。

---

## 示例 6：轮廓检测与形状分析 — `examples/contours.cpp`

### 目标
掌握轮廓查找与基于几何特征、多边形逼近的形状识别流程。

### 涉及 API
`findContours`、`contourArea`、`arcLength`、`boundingRect`、`minAreaRect`、`moments`、
`approxPolyDP`、`drawContours`、`fillConvexPoly`、`circle`

### 处理流程
1. 黑底画 4 类白色图形：正方形、圆形、三角形、扁矩形（含缺口扁矩形演示 minAreaRect 意义）；
2. `findContours`（`RETR_EXTERNAL`、`CHAIN_APPROX_SIMPLE`）取外轮廓；
3. 对每个轮廓计算并打印：面积、周长、外接矩形、最小外接矩形、质心（`moments`）；
4. `approxPolyDP`（epsilon=3% 周长）逼近，按逻辑分类：
   - 顶点数 3 → 三角形；4 → 依宽高比判 square/rectangle；
   - ≥6 计算圆度 `4πA/P²`，>0.75 判圆，否则多边形；
5. `drawContours` 上色 + 质心标点。

### 输入 / 输出
- 输出：`out_contours_mask.png`（源）、`out_contours_result.png`（标注结果）

### 自校验与预期
- 找到轮廓数 = 5（正方形、圆、三角形、扁矩形、缺口矩形）；非空且程序打印分类结果，return 0。

---

## 示例 7：直方图与色彩分析 — `examples/histogram.cpp`

### 目标
掌握直方图统计、可视化与直方图均衡化。

### 涉及 API
`calcHist`、`normalize`、`minMaxLoc`、`equalizeHist`、`meanStdDev`、`rectangle`

### 处理流程
1. 合成低对比度图（灰度集中在 60~160 附近窄区间，便于展示均衡化"拉开对比"）；
2. `calcHist`：1 通道、256 bin、range [0,256)，找到像素数最多的灰度值（peak bin）；
3. 可视化：`normalize(hist, 0..图高)` 后逐 bin 画竖线，生成直方图图像；
4. `equalizeHist` 均衡化，重新统计并输出均衡前后的 min/max/均值/标准差。

### 输入 / 输出
- 输出：`out_hist_lowcontrast.png`、`out_hist_histogram.png`、`out_hist_equalized.png`、`out_hist_hist_equalized.png`

### 自校验与预期
- 均衡化后灰度范围显著展宽、标准差增大（打印对比观察）。

---

## 示例 8：特征检测与匹配 — `examples/features2d.cpp`

### 目标
掌握 ORB 特征点检测、描述子提取与暴力匹配的完整流程。

### 涉及 API
`ORB::create`、`detectAndCompute`、`BFMatcher`(`NORM_HAMMING`)、`drawKeypoints`、`drawMatches`

### 处理流程
1. 合成 8×8 棋盘（ORB 角点密集）；
2. 生成第二幅视图：把棋盘平移到 340×300 黑底画布的 (45,25) 处，模拟"同一场景的平移观测"；
3. 对两图分别 `detectAndCompute`，打印关键点数与描述子列数（32）；
4. 检查描述子类型为 `CV_8U`（ORB 用汉明距离的前提）；
5. `drawKeypoints` 画第一图关键点；
6. `BFMatcher(NORM_HAMMING).match` 全量匹配，按 `DMatch.distance` 升序取前 60；
7. `drawMatches` 输出匹配连线图，打印匹配总数与最优 60 条的均值距离。

### 输入 / 输出
- 输出：`out_features2d_keypoints_a.png`、`out_features2d_matches.png`

### 自校验与预期
- 两图关键点均非空；最优匹配数 ≥ 10，否则返回 1。

---

## 扩展方向（把示例改造成"真实输入"版本）

每个示例都可以改为读入命令行图片：

```cpp
// 以示例 4 为例，把 makeScene() 替换为：
Mat src = argc > 1 ? imread(argv[1], IMREAD_GRAYSCALE)
                   : makeScene();
if (src.empty()) { cerr << "cannot read: " << argv[1] << endl; return 1; }
```

示例 6 需先 `threshold`/`Canny` 得到二值图再 `findContours`；示例 8 可用两张内容相似的照片做匹配演示。
