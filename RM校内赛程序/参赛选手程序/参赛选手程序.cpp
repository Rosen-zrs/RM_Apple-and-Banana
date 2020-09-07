#include "TCP_Client.h"
#include <opencv2\opencv.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <time.h>

using namespace std;
using namespace cv;


int main()
{
	TCP_Client client = TCP_Client("127.0.0.1");
	while (!client.Open()) //如果TCP成功打开就跳过，否则将一直循环。
	{
		cout << "TCP连接失败，请按任意键重试。" << endl;
		system("pause"); //这句话将程序阻塞，直到按下任意键。
	}
	cout << "TCP已成功连接。" << endl;
	VideoCapture capture(1);
	//capture.open("C:/Users/86159/Desktop/rm.mp4");       //读取视频

	//创建窗口
	namedWindow("原图", WINDOW_NORMAL);
	resizeWindow("原图", 600, 330);

	//定义输出变量count
	string count = "1";

	Mat src, hsv_src, binary_src, dst;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, time_count = 0;
	double start, end, mean_time, run_time = 0;

	//定义结构元素
	Mat kernel_01 = getStructuringElement(MORPH_RECT, Size(9, 9));
	Mat kernel_02 = getStructuringElement(MORPH_RECT, Size(12, 12));

	//创建vector容器
	vector<vector<Point> > contours_02;       
	vector<Vec4i> hierarchy_02;
	vector<vector<Point> > contours_01;
	vector<Vec4i> hierarchy_01;

	while (true)
	{
		capture >> src;

		//计算程序运行时间
		double start = static_cast<double>(getTickCount());         //获取开始执行时间
		time_count++;

		cvtColor(src, hsv_src, COLOR_BGR2HSV);                    //转化为HSV色彩空间
		inRange(hsv_src, Scalar(11, 43, 46), Scalar(25, 255, 255), binary_src);       //根据hsv值提取图像
		morphologyEx(binary_src, binary_src, MORPH_OPEN, kernel_01);			//开操作

		findContours(binary_src, contours_01, hierarchy_01, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));   //轮廓提取

		//定义容器
		vector<Moments> mu(contours_01.size());
		vector<Point2f> mc(contours_01.size());
		for (int i = 0; i < contours_01.size(); i++)
		{
			mu[i] = moments(contours_01[i], false);        //计算轮廓矩 

			//计算中心点坐标
			x1 = mu[i].m10 / mu[i].m00 + 50;			
			y1 = mu[i].m01 / mu[i].m00;
			mc[i] = Point2f(mu[i].m10 / mu[i].m00 - 30, mu[i].m01 / mu[i].m00 - 30);    //计算香蕉轮廓比较点
			circle(src, mc[i], 5, Scalar(255, 0, 255), -1, 8, 0);						//标定中心点
		}

		//判断如果结果与上次相同则直接采用上次结果
		if (((x2 - 20 < x1) && (x1 < x2 + 20)) && ((y2 - 20 < y1) && (y1 < y2 + 20)))   
		{
		}
		else               //否则计算红色轮廓
		{
			count = "1";
			Mat extra_red_src;

			//提取图片中的红色对象(苹果)
			inRange(hsv_src, Scalar(160, 43, 46), Scalar(180, 255, 255), dst);
			inRange(hsv_src, Scalar(0, 43, 46), Scalar(10, 255, 255), extra_red_src);
			bitwise_or(dst, extra_red_src, dst);        //红色图像相加

			//形态学滤波
			morphologyEx(dst, dst, MORPH_OPEN, kernel_02);	 //开操作
			morphologyEx(dst, dst, MORPH_CLOSE, kernel_02);  //闭操作

			//提取轮廓
			findContours(dst, contours_02, hierarchy_02, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
			vector<Moments> m1(contours_02.size());
			vector<Point2f> m2(contours_02.size());

			if (contours_01.size() != 0)	//判断如果存在苹果对象
			{
				//记录上次黄色中心点值，用于就算前条件比较
				x2 = mu[0].m10 / mu[0].m00 - 30;    
				y2 = mu[0].m01 / mu[0].m00 - 30;
			}
			for (int i = 0; i < contours_02.size(); i++)
			{
				//计算轮廓矩 
				m1[i] = moments(contours_02[i], false);        
				m2[i] = Point2f(m1[i].m10 / m1[i].m00, m1[i].m01 / m1[i].m00);
				circle(src, m2[i], 5, Scalar(255, 0, 255), -1, 8, 0);	//标定中心点
			}

			int x_count = 0, y_count = 0;
			//根据分别计算红色和黄色元素中心点比较判断
			for (int i = 0; i < contours_02.size(); i++)
			{
				if (m2[i].x < x2)
				{
					x_count++;
				}
			}
			for (int i = 0; i < contours_02.size(); i++)
			{
				if (m2[i].y < y2)
				{
					y_count++;
				}
			}
			if (x_count == 0) //如果x_count为0则目标位于最左列
			{
				switch (y_count)	//根据y_count判断所在的行
				{
				case 0:count = "1"; break;
				case 3:count = "4"; break;
				case 6:count = "7"; break;

				default:
					break;
				}
			}
			if (x_count == 3)	//如果x_count为3则目标位于中间列
			{
				switch (y_count)	//根据y_count判断所在的行
				{
				case 0:count = "2"; break;
				case 3:count = "5"; break;
				case 6:count = "8"; break;

				default:
					break;
				}
			}
			if (x_count == 6)	//如果x_count63则目标位于最右列
			{
				switch (y_count)	//根据y_count判断所在的行
				{
				case 0:count = "3"; break;
				case 3:count = "6"; break;
				case 6:count = "9"; break;

				default:
					break;
				}
			}
			extra_red_src.release();
			//计算运行时间
			end = static_cast<double>(getTickCount());
			run_time += (end - start) / getTickFrequency();
		}

		imshow("原图", src);
		//cout << "Select " << count << endl;	       //打印结果
		client.Send("Select " + count + "\n");

		//计算运行时间
		if (waitKey(30) > 0)
		{
			mean_time = (double)(run_time / time_count);
			cout << "处理总次数：" << time_count << endl;
			cout << "平均每次计算时间：" << mean_time << "s" << endl;
			break;
		}
	}
	return 0;
}