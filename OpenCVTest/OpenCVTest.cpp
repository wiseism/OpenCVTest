// OpenCVTest.cpp : 定义控制台应用程序的入口点。
//

// 预编译头（MSVC /Yu 要求其必须是首条指令；stdafx.h 内部已做 Windows/Linux 判断）
#include "stdafx.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;

int main() {
	string s1;//初始化字符串，空字符串
	string s2 = s1; //拷贝初始化，深拷贝字符串
	string s3 = "I am Yasuo"; //直接初始化，s3存了字符串
	string s4(10, 'a'); //s4存的字符串是aaaaaaaaaa
	string s5(s4); //拷贝初始化，深拷贝字符串
	string s6("I am Ali"); //直接初始化
	string s7 = string(6, 'c'); //拷贝初始化，cccccc
	cout << s7;
	// 读入一张图片（游戏原画，位于本仓库根目录的 1.jpg）
	// 程序的工作目录随运行方式而变（IDE 调试=工程目录、命令行=仓库根、双击 exe=产物目录），
	// 因此依次尝试几个常见位置，避免硬编码绝对路径：
	//   "./1.jpg"      —— 在仓库根目录运行（如命令行 OpenCVTest\1.jpg 所在目录）
	//   "../1.jpg"     —— VS 调试时工作目录为工程目录 OpenCVTest\
	//   "../../1.jpg"  —— 双击运行，工作目录为 x64\Debug 等产物目录
	Mat img;
	const char* imageCandidates[] = { "1.jpg", "../1.jpg", "../../1.jpg" };
	for (const char* path : imageCandidates) {
		img = imread(path);
		if (!img.empty()) {
			cout << "Loaded image: " << path << endl;
			break;
		}
	}
	if (img.empty()) {
		cerr << "Failed to load 1.jpg (tried: 1.jpg, ../1.jpg, ../../1.jpg)" << endl;
		return 1;
	}
	// 创建一个名为 "游戏原画"窗口
	namedWindow("游戏原画");
	// 在窗口中显示游戏原画
	imshow("游戏原画", img);
	// 等待6000 ms后窗口自动关闭
	waitKey(6000);
	return 0;
}
