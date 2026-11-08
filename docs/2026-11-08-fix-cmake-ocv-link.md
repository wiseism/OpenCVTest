# 修复记录：OpenCV CI 构建失败（2026-11-08）

## 失败信息
- 仓库：wiseism/OpenCVTest
- Workflow：cmake build（ubuntu）/ cmake build win
- 失败 run：[42425946342](https://github.com/wiseism/OpenCVTest/actions/runs/42425946342)（ubuntu）、[42425946326](https://github.com/wiseism/OpenCVTest/actions/runs/42425946326)（win）

## 错误日志摘录
```
# ubuntu: vcpkg 安装 opencv 时 gperf port 配置失败
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:112 (message):
    Command failed: /usr/local/bin/ninja -v
error: building gperf:x64-linux failed with: BUILD_FAILED
# win: 链接失败
error LNK2019: unresolved external symbol cv::imread / cv::imshow / cv::namedWindow / cv::waitKey
error: pathspec '2024.03.25' did not match any file(s)
```

## 原因分析
1. ubuntu：'ubuntu-latest' 已是 26.04，自带较新 CMake（≥3.31 触发 CMP0174），
   与钉住的 vcpkg `2024.03.25` 不兼容，gperf port 在 Configure 阶段失败。
2. win：`git checkout 2024.03.25` 在**克隆目录外执行**（cd vcpkg 之前），导致
   实际按仓库 HEAD 的 vcpkg 安装；
3. win：`OpenCVTest2/CMakeLists.txt` 只链接了
   `opencv_ml/dnn/core/flann`，而代码使用了 `imread/imshow/highgui`
   （缺 `opencv_imgcodecs` + `opencv_highgui`）；
4. Linux else 分支未 find_package/链接 OpenCV 头文件同样不完整。

## 修复内容
1. `.github/workflows/cmake.yml`：runner 固定 `ubuntu-22.04`（CMake≈3.28，
   与 vcpkg 2024.03.25 兼容）。
2. `.github/workflows/cmake_window.yml`：把 `git checkout 2024.03.25`
   移到 `cd vcpkg` 之后。
3. `OpenCVTest2/CMakeLists.txt`：统一各平台 `find_package(OpenCV CONFIG REQUIRED)`，
   并补上 `opencv_imgcodecs`、`opencv_highgui` 链接。

## 验证
| run id | 结果 |
| --- | --- |
| 本次推送触发 | 待填 |

### 复核
- 42443446245 (vcpkg apt 22.04+pin): expat 下载再失败 → at-spi2-core/meson 失败。vcpkg 2024.03.25 与新环境 arc 不稳。
- 改 apt libopencv-dev 后 run 42479107385 ✅ success。
