# 修复 mac workflow：macos-12 runner 已退役导致排队 24h 超时

## 失败信息
- Run 43219287643（cmake build mac）：`queued` 24 小时后被取消
  - `The job has exceeded the maximum execution time while awaiting a runner for 24h0m0s`

## 原因分析
- workflow 中 `runs-on: macos-12`，该 runner 镜像已退役，作业永远等不到可用 runner。
- 排除容量假象：同账户 macos-latest 作业（as_call_c / cpptestdata）均秒级成功。

## 修复内容
- `.github/workflows/cmake_mac.yml`: `runs-on: macos-12` → `runs-on: macos-latest`。

## 验证记录
| Run | 结果 |
|---|---|
| 43219287643 (macos-12) | cancelled — 排队 24h 超时 |
| 待验证 (macos-latest) | 待填 |
