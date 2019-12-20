// OpenCVTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

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
	cout<< s7;
	// 读入一张图片（游戏原画）    
	Mat img = imread("D:\\visual studio 2015\\Projects\\OpenCVTest\\1.jpg");
	// 创建一个名为 "游戏原画"窗口    
	namedWindow("游戏原画");
	// 在窗口中显示游戏原画    
	imshow("游戏原画", img);
	// 等待6000 ms后窗口自动关闭    
	waitKey(6000);
	return 0;
}

