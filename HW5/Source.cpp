#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <time.h>
#include <string.h>
#include <math.h>
using namespace cv;
using namespace std;
#define pi 3.14
double im_rect[256][256] = { 0 }, re_rect[256][256] = { 0 };
double im_circ[256][256] = { 0 }, re_circ[256][256] = { 0 };
double im_squa[256][256] = { 0 }, re_squa[256][256] = { 0 };
double im_diam[256][256] = { 0 }, re_diam[256][256] = { 0 };
int cv_rect[256][256] = { 0 };
void FourierTF(Mat InMat, double im_uv[][256], double re_uv[][256]) {
	//傅立葉轉換(將2維轉換轉成2個1維轉換減少計算時間)
	int i, j, u, v;
	double maxv = 0, minv = 999;
	vector<vector<double>>	input(InMat.rows, vector<double>(InMat.cols)),
							im_xv(InMat.rows, vector<double>(InMat.cols)),
							re_xv(InMat.rows, vector<double>(InMat.cols));
	//將頻域圖移至中心
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			input[i][j] = InMat.at<uchar>(i, j) * pow(-1, i + j);
		}
	}
	//第一次一維轉換(將f(x,y)轉為F(x,v))
	for (i = 0; i < InMat.rows; i++) {
		for (v = 0; v < InMat.cols; v++) {
			re_xv[i][v] = 0;
			im_xv[i][v] = 0;
			for (j = 0; j < InMat.cols; j++) {
				re_xv[i][v] += input[i][j] * cos(-2 * pi * v * j / (double)InMat.cols) / (double)(InMat.cols);
				im_xv[i][v] += input[i][j] * sin(-2 * pi * v * j / (double)InMat.cols) / (double)(InMat.cols);
			}
		}
	}
	//第二次一維轉換(將F(x,v)轉為F(u,v))
	for (u = 0; u < InMat.rows; u++) {
		for (v = 0; v < InMat.cols; v++) {
			re_uv[u][v] = 0;
			im_uv[u][v] = 0;
			for (i = 0; i < InMat.rows; i++) {
				re_uv[u][v] += re_xv[i][v] * cos(-2 * pi * u * i / (double)InMat.rows) / (double)(InMat.rows);
				re_uv[u][v] -= im_xv[i][v] * sin(-2 * pi * u * i / (double)InMat.rows) / (double)(InMat.rows);
				im_uv[u][v] += im_xv[i][v] * cos(-2 * pi * u * i / (double)InMat.rows) / (double)(InMat.rows);
				im_uv[u][v] += re_xv[i][v] * sin(-2 * pi * u * i / (double)InMat.rows) / (double)(InMat.rows);
			}
		}
	}
}
void mag(double im_uv[][256], double re_uv[][256], Mat OutMat) {
	int u, v;
	double maxv = 0, minv = 999;
	Mat tmp(OutMat.rows, OutMat.cols, CV_32F);
	//歸一化
	for (u = 0; u < OutMat.rows; u++) {
		for (v = 0; v < OutMat.cols; v++) {
			tmp.at<float>(u, v) = sqrt(pow(re_uv[u][v], 2) + pow(im_uv[u][v], 2));
			if (tmp.at<float>(u, v) > maxv) {
				maxv = tmp.at<float>(u, v);
			}
		}
	}
	tmp /= maxv;
	//Gamma校正
	for (u = 0; u < OutMat.rows; u++) {
		for (v = 0; v < OutMat.cols; v++) {
			tmp.at<float>(u, v) = pow(tmp.at<float>(u, v), 0.4);
		}
	}
	tmp *= 255;
	for (u = 0; u < OutMat.rows; u++) {
		for (v = 0; v < OutMat.cols; v++) {
			OutMat.at<uchar>(u, v) = uchar(tmp.at<float>(u, v));
		}
	}
}
void IFT(double im_uv[][256], double re_uv[][256], Mat OutMat) {
	//傅立葉逆轉換(將2維逆轉換轉成2個1維逆轉換減少計算時間)
	int i = 0, j = 0, u = 0, v = 0;
	vector<vector<double>>	input(OutMat.rows, vector<double>(OutMat.cols)),
		im_uy(OutMat.rows, vector<double>(OutMat.cols)),
		re_uy(OutMat.rows, vector<double>(OutMat.cols)),
		im_xy(OutMat.rows, vector<double>(OutMat.cols)),
		re_xy(OutMat.rows, vector<double>(OutMat.cols));
	//第一次一維逆轉換(將F(u,v)轉為F(u,y))
	for (u = 0; u < OutMat.rows; u++) {
		for (j = 0; j < OutMat.cols; j++) {
			re_uy[u][j] = 0;
			im_uy[u][j] = 0;
			for (v = 0; v < OutMat.cols; v++) {
				re_uy[u][j] += re_uv[u][v] * cos(2 * pi * v * j / (double)OutMat.cols);
				re_uy[u][j] -= im_uv[u][v] * sin(2 * pi * v * j / (double)OutMat.cols);
				im_uy[u][j] += im_uv[u][v] * cos(2 * pi * v * j / (double)OutMat.cols);
				im_uy[u][j] += re_uv[u][v] * sin(2 * pi * v * j / (double)OutMat.cols);
			}
		}
	}
	//第二次一維逆轉換(將F(u,y)轉為f(x,y))
	for (i = 0; i < OutMat.rows; i++) {
		for (j = 0; j < OutMat.cols; j++) {
			re_xy[i][j] = 0;
			im_xy[i][j] = 0;
			for (u = 0; u < OutMat.rows; u++) {
				re_xy[i][j] += re_uy[u][j] * cos(2 * pi * u * i / (double)OutMat.rows);
				re_xy[i][j] -= im_uy[u][j] * sin(2 * pi * u * i / (double)OutMat.rows);
				im_xy[i][j] += im_uy[u][j] * cos(2 * pi * u * i / (double)OutMat.rows);
				im_xy[i][j] += re_uy[u][j] * sin(2 * pi * u * i / (double)OutMat.rows);
			}
		}
	}
	//magnitude(因為轉換時乘過pow(-1, i + j)，需要再乘一次)
	for (i = 0; i < OutMat.rows; i++) {
		for (j = 0; j < OutMat.cols; j++) {
			OutMat.at<uchar>(i, j) = sqrt(pow(re_xy[i][j] * pow(-1, i + j), 2) + pow(im_xy[i][j] * pow(-1, i + j), 2));
		}
	}
}
void mse_and_psnr(Mat oriMat, Mat InMat) {
	//評估誤差，用mse與psnr表示
	double mse = 0;
	double psnr = 0;
	int i, j;
	for (i = 0; i < oriMat.rows; i++) {
		for (j = 0; j < oriMat.cols; j++) {
			mse += pow((oriMat.data[i * oriMat.cols + j] - InMat.data[i * InMat.cols + j]), 2);
		}
	}
	mse /= (double)(oriMat.rows * oriMat.cols);
	psnr = 10 * log10(pow(255, 2) / mse);
	printf("vs. org mse: %f\n", mse);
	printf("vs. org psnr: %f\n", psnr);
}
Mat opencv_dft(Mat InMat) {
	//opencv的傅立葉轉換
	Mat to64F, OutMat;
	InMat.convertTo(to64F, CV_64F);
	dft(to64F, OutMat, DFT_COMPLEX_OUTPUT);
	return OutMat;
}
Mat opencv_mag(Mat InMat) {
	Mat tmp, OutMat;
	vector<Mat> split2chs;
	split(InMat, split2chs);
	magnitude(split2chs[0], split2chs[1], OutMat);//產生頻域圖
	Mat q0, q1, q2, q3;
	int u, v;
	int cx = 256 / 2;
	int cy = 256 / 2;
	q0 = Mat(OutMat, Rect(0, 0, cx, cy));
	q1 = Mat(OutMat, Rect(cx, 0, cx, cy));
	q2 = Mat(OutMat, Rect(0, cy, cx, cy));
	q3 = Mat(OutMat, Rect(cx, cy, cx, cy));
	//將頻域圖移至中心
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(OutMat, OutMat, 0,10, NORM_MINMAX);
	OutMat.convertTo(OutMat, CV_8U, 255, 0);
	return OutMat;
}
Mat opencv_idft(Mat InMat) {
	//opencv的傅立葉逆轉換
	Mat to64F, OutMat;
	idft(InMat, to64F, DFT_REAL_OUTPUT + DFT_INVERSE + DFT_SCALE);
	to64F.convertTo(OutMat, CV_8UC1);
	return OutMat;
}
void hw2_a_and_c() {
	//傅立葉轉換 by C++

	int width = 256;
	int height = 256;
	int size = width * height;
	int i, j;
	char OriFile[] = "Inpic/rect256.raw";
	FILE* rectFile;
	rectFile = fopen(OriFile, "rb");
	if (rectFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* rect = new unsigned char[size];
	fread(rect, 1, size, rectFile);
	Mat rectMat(height, width, CV_8UC1, rect);
	Mat test(height, width, CV_8UC1);

	char OriFile1[] = "Inpic/circle256.raw";
	FILE* circleFile;
	circleFile = fopen(OriFile1, "rb");
	if (circleFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* circle = new unsigned char[size];
	fread(circle, 1, size, circleFile);
	Mat circleMat(height, width, CV_8UC1, circle);
	Mat test1(height, width, CV_8UC1);

	char OriFile2[] = "Inpic/Square256.raw";
	FILE* squareFile;
	squareFile = fopen(OriFile2, "rb");
	if (squareFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* square = new unsigned char[size];
	fread(square, 1, size, squareFile);
	Mat squareMat(height, width, CV_8UC1, square);
	Mat test2(height, width, CV_8UC1);

	char OriFile3[] = "Inpic/square256_rota.raw";
	FILE* diamondFile;
	diamondFile = fopen(OriFile3, "rb");
	if (diamondFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* diamond = new unsigned char[size];
	fread(diamond, 1, size, diamondFile);
	Mat diamondMat(height, width, CV_8UC1, diamond);
	Mat test3(height, width, CV_8UC1);

	//顯示結果圖
	imshow("rectMat", rectMat);
	FourierTF(rectMat, im_rect, re_rect);
	mag(im_rect, re_rect, test);
	imshow("rectFT", test);
	imwrite("Outpic/rectFT.png", test);
	Mat out(height, width, CV_8UC1);
	IFT(im_rect, re_rect, out);
	imshow("rectIFT", out);
	imwrite("Outpic/rectIFT.png", out);
	printf("\nrect vs. org mse &psnr\n");
	mse_and_psnr(rectMat, out);
	waitKey(0);
	destroyAllWindows();

	imshow("circleMat", circleMat);
	FourierTF(circleMat, im_circ, re_circ);
	mag(im_circ, re_circ, test1);
	imshow("circleFT", test1);
	imwrite("Outpic/circleFT.png", test1);
	Mat out1(height, width, CV_8UC1);
	IFT(im_circ, re_circ, out1);
	imshow("circIFT", out1);
	imwrite("Outpic/circIFT.png", out1);
	printf("\ncircle vs. org mse &psnr\n");
	mse_and_psnr(circleMat, out1);
	waitKey(0);
	destroyAllWindows();

	imshow("squareMat", squareMat);
	FourierTF(squareMat, im_squa, re_squa);
	mag(im_squa, re_squa, test2);
	imshow("squareFT", test2);
	imwrite("Outpic/squareFT.png", test2);
	Mat out2(height, width, CV_8UC1);
	IFT(im_squa, re_squa, out2);
	imshow("squaIFT", out2);
	imwrite("Outpic/squaIFT.png", out2);
	printf("\nsquare vs. org mse &psnr\n");
	mse_and_psnr(squareMat, out2);
	waitKey(0);
	destroyAllWindows();

	imshow("diamondMat", diamondMat);
	FourierTF(diamondMat, im_diam, re_diam);
	mag(im_diam, re_diam, test3);
	imshow("diamondFT", test3);
	imwrite("Outpic/diamondFT.png", test3);
	Mat out3(height, width, CV_8UC1);
	IFT(im_diam, re_diam, out3);
	imshow("diamIFT", out3);
	imwrite("Outpic/diamIFT.png", out3);
	printf("\ndiamond vs. org mse &psnr\n");
	mse_and_psnr(diamondMat, out3);
	waitKey(0);
	destroyAllWindows();
}
void hw2_b() {
	//傅立葉轉換 by Opencv
	int width = 256;
	int height = 256;

	int size = width * height;
	char OriFile[] = "Inpic/rect256.raw";
	FILE* rectFile;
	rectFile = fopen(OriFile, "rb");
	if (rectFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* rect = new unsigned char[size];
	fread(rect, 1, size, rectFile);
	Mat rectMat(height, width, CV_8UC1, rect);
	Mat test(height, width, CV_8UC1);
	char OriFile1[] = "Inpic/circle256.raw";
	FILE* circleFile;
	circleFile = fopen(OriFile1, "rb");
	if (circleFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* circle = new unsigned char[size];
	fread(circle, 1, size, circleFile);
	Mat circleMat(height, width, CV_8UC1, circle);
	Mat test1(height, width, CV_8UC1);
	char OriFile2[] = "Inpic/Square256.raw";
	FILE* squareFile;
	squareFile = fopen(OriFile2, "rb");
	if (squareFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* square = new unsigned char[size];
	fread(square, 1, size, squareFile);
	Mat squareMat(height, width, CV_8UC1, square);
	Mat test2(height, width, CV_8UC1);
	char OriFile3[] = "Inpic/square256_rota.raw";
	FILE* diamondFile;
	diamondFile = fopen(OriFile3, "rb");
	if (diamondFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* diamond = new unsigned char[size];
	fread(diamond, 1, size, diamondFile);
	Mat diamondMat(height, width, CV_8UC1, diamond);
	Mat test3(height, width, CV_8UC1);
	Mat comp, comp1, comp2, comp3;

	//傅立葉轉換by OpenCV
	comp  = opencv_dft(rectMat);
	comp1 = opencv_dft(circleMat);
	comp2 = opencv_dft(squareMat);
	comp3 = opencv_dft(diamondMat);

	//產生頻域圖
	test  = opencv_mag(comp);
	test1 = opencv_mag(comp1);
	test2 = opencv_mag(comp2);
	test3 = opencv_mag(comp3);

	//傅立葉逆轉換by OpenCV
	Mat cv1, cv2, cv3, cv4;
	cv1 = opencv_idft(comp);
	cv2 = opencv_idft(comp1);
	cv3 = opencv_idft(comp2);
	cv4 = opencv_idft(comp3);

	//顯示結果圖
	imshow("rect_dft_by_cv", test);
	imwrite("Outpic/rect_dft_by_cv.png", test);
	imshow("rect_idft_by_cv", cv1);
	imwrite("Outpic/rect_idft_by_cv.png", cv1);
	waitKey(0);
	destroyAllWindows();

	imshow("circle_dft_by_cv", test1);
	imwrite("Outpic/circle_dft_by_cv.png", test1);
	imshow("circle_idft_by_cv", cv2);
	imwrite("Outpic/circle_idft_by_cv.png", cv2);
	waitKey(0);
	destroyAllWindows();

	imshow("square_dft_by_cv", test2);
	imwrite("Outpic/square_dft_by_cv.png", test2);
	imshow("square_idft_by_cv", cv3);
	imwrite("Outpic/square_idft_by_cv.png", cv3);
	waitKey(0);
	destroyAllWindows();

	imshow("diamond_dft_by_cv", test3);
	imwrite("Outpic/diamond_dft_by_cv.png", test3);
	imshow("diamond_idft_by_cv", cv4);
	imwrite("Outpic/diamond_idft_by_cv.png", cv4);
	waitKey(0);
	destroyAllWindows();
}
int main() {
	while (1) {
		printf("-----\nhw2_a_and_c\nhw2_b\nend\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		if (strcmp(s, "hw2_a_and_c") == 0) {
			hw2_a_and_c();
		}
		else if (strcmp(s, "hw2_b") == 0) {
			hw2_b();
		}
		else if (strcmp(s, "end") == 0) {
			break;
		}
		else {
			printf("Please enter again\n\n");
		}
	}
	return 0;
}