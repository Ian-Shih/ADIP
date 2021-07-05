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
	//傅立葉轉換(將2維轉換轉成2次1維轉換減少計算時間)
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
	//計算頻域強度圖
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
	//傅立葉逆轉換(將2維逆轉換轉成2次1維逆轉換減少計算時間)
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
	//計算時域強度圖(因為轉換時乘過pow(-1, i + j)，需要再乘一次)
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
	//用opencv計算頻域強度圖
	Mat tmp, OutMat;
	vector<Mat> split2chs;
	split(InMat, split2chs);
	magnitude(split2chs[0], split2chs[1], OutMat);//產生頻域強度圖
	Mat up_left, up_right, low_left, low_right;
	int u, v;
	int cx = 256 / 2;
	int cy = 256 / 2;
	//將圖切成左上、右上、左下、右下
	up_left = Mat(OutMat, Rect(0, 0, cx, cy));
	up_right = Mat(OutMat, Rect(cx, 0, cx, cy));
	low_left = Mat(OutMat, Rect(0, cy, cx, cy));
	low_right = Mat(OutMat, Rect(cx, cy, cx, cy));
	//由於直接進行傅立葉轉換會使中心跑至角落，因此將四塊小圖對角交換，將圖移至中心
	up_left.copyTo(tmp);
	low_right.copyTo(up_left);
	tmp.copyTo(low_right);
	up_right.copyTo(tmp);
	low_left.copyTo(up_right);
	tmp.copyTo(low_left);
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
	int width = 256;
	int height = 256;
	int size = width * height;
	char Ori_rect[] = "Inpic/rect256.raw";
	FILE* rectFile;
	rectFile = fopen(Ori_rect, "rb");
	if (rectFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* rect = new unsigned char[size];
	fread(rect, 1, size, rectFile);
	Mat rectMat(height, width, CV_8UC1, rect);
	Mat rect_ft(height, width, CV_8UC1);

	char Ori_circ[] = "Inpic/circle256.raw";
	FILE* circleFile;
	circleFile = fopen(Ori_circ, "rb");
	if (circleFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* circle = new unsigned char[size];
	fread(circle, 1, size, circleFile);
	Mat circleMat(height, width, CV_8UC1, circle);
	Mat circ_ft(height, width, CV_8UC1);

	char Ori_squa[] = "Inpic/Square256.raw";
	FILE* squareFile;
	squareFile = fopen(Ori_squa, "rb");
	if (squareFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* square = new unsigned char[size];
	fread(square, 1, size, squareFile);
	Mat squareMat(height, width, CV_8UC1, square);
	Mat squa_ft(height, width, CV_8UC1);

	char Ori_diam[] = "Inpic/square256_rota.raw";
	FILE* diamondFile;
	diamondFile = fopen(Ori_diam, "rb");
	if (diamondFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* diamond = new unsigned char[size];
	fread(diamond, 1, size, diamondFile);
	Mat diamondMat(height, width, CV_8UC1, diamond);
	Mat diam_ft(height, width, CV_8UC1);

	//進行傅立葉轉換
	imshow("rectMat", rectMat);
	FourierTF(rectMat, im_rect, re_rect);
	mag(im_rect, re_rect, rect_ft);
	imshow("rectFT", rect_ft);
	imwrite("Outpic/rectFT.png", rect_ft);

	//進行傅立葉逆轉換
	Mat rect_ift(height, width, CV_8UC1);
	IFT(im_rect, re_rect, rect_ift);
	imshow("rectIFT", rect_ift);
	imwrite("Outpic/rectIFT.png", rect_ift);
	printf("\nrect vs. org mse &psnr\n");
	mse_and_psnr(rectMat, rect_ift);

	//進行傅立葉轉換
	imshow("circleMat", circleMat);
	FourierTF(circleMat, im_circ, re_circ);
	mag(im_circ, re_circ, circ_ft);
	imshow("circleFT", circ_ft);
	imwrite("Outpic/circleFT.png", circ_ft);

	//進行傅立葉逆轉換
	Mat circ_ift(height, width, CV_8UC1);
	IFT(im_circ, re_circ, circ_ift);
	imshow("circIFT", circ_ift);
	imwrite("Outpic/circIFT.png", circ_ift);
	printf("\ncircle vs. org mse &psnr\n");
	mse_and_psnr(circleMat, circ_ift);

	//進行傅立葉轉換
	imshow("squareMat", squareMat);
	FourierTF(squareMat, im_squa, re_squa);
	mag(im_squa, re_squa, squa_ft);
	imshow("squareFT", squa_ft);
	imwrite("Outpic/squareFT.png", squa_ft);

	//進行傅立葉逆轉換
	Mat squa_ift(height, width, CV_8UC1);
	IFT(im_squa, re_squa, squa_ift);
	imshow("squaIFT", squa_ift);
	imwrite("Outpic/squaIFT.png", squa_ift);
	printf("\nsquare vs. org mse &psnr\n");
	mse_and_psnr(squareMat, squa_ift);

	//進行傅立葉轉換
	imshow("diamondMat", diamondMat);
	FourierTF(diamondMat, im_diam, re_diam);
	mag(im_diam, re_diam, diam_ft);
	imshow("diamondFT", diam_ft);
	imwrite("Outpic/diamondFT.png", diam_ft);
	Mat diam_ift(height, width, CV_8UC1);

	//進行傅立葉逆轉換
	IFT(im_diam, re_diam, diam_ift);
	imshow("diamIFT", diam_ift);
	imwrite("Outpic/diamIFT.png", diam_ift);
	printf("\ndiamond vs. org mse &psnr\n");
	mse_and_psnr(diamondMat, diam_ift);

	waitKey(0);
	destroyAllWindows();
}
void hw2_b() {
	//使用opencv進行傅立葉轉換及逆轉換
	int width = 256;
	int height = 256;

	int size = width * height;
	char Ori_rect[] = "Inpic/rect256.raw";
	FILE* rectFile;
	rectFile = fopen(Ori_rect, "rb");
	if (rectFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* rect = new unsigned char[size];
	fread(rect, 1, size, rectFile);
	Mat rectMat(height, width, CV_8UC1, rect);
	Mat rect_ft(height, width, CV_8UC1);
	char Ori_circ[] = "Inpic/circle256.raw";
	FILE* circleFile;
	circleFile = fopen(Ori_circ, "rb");
	if (circleFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* circle = new unsigned char[size];
	fread(circle, 1, size, circleFile);
	Mat circleMat(height, width, CV_8UC1, circle);
	Mat circ_ft(height, width, CV_8UC1);
	char Ori_squa[] = "Inpic/Square256.raw";
	FILE* squareFile;
	squareFile = fopen(Ori_squa, "rb");
	if (squareFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* square = new unsigned char[size];
	fread(square, 1, size, squareFile);
	Mat squareMat(height, width, CV_8UC1, square);
	Mat squa_ft(height, width, CV_8UC1);
	char Ori_diam[] = "Inpic/square256_rota.raw";
	FILE* diamondFile;
	diamondFile = fopen(Ori_diam, "rb");
	if (diamondFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* diamond = new unsigned char[size];
	fread(diamond, 1, size, diamondFile);
	Mat diamondMat(height, width, CV_8UC1, diamond);
	Mat diam_ft(height, width, CV_8UC1);
	Mat rect_cpx, circ_cpx, squa_cpx, diam_cpx;
	//傅立葉轉換by OpenCV
	rect_cpx  = opencv_dft(rectMat);
	circ_cpx = opencv_dft(circleMat);
	squa_cpx = opencv_dft(squareMat);
	diam_cpx = opencv_dft(diamondMat);
	//產生頻域強度圖
	rect_ft  = opencv_mag(rect_cpx);
	circ_ft = opencv_mag(circ_cpx);
	squa_ft = opencv_mag(squa_cpx);
	diam_ft = opencv_mag(diam_cpx);

	imshow("rect_dft_by_cv", rect_ft);
	imshow("circle_dft_by_cv", circ_ft);
	imshow("square_dft_by_cv", squa_ft);
	imshow("diamond_dft_by_cv", diam_ft);

	imwrite("Outpic/rect_dft_by_cv.png", rect_ft);
	imwrite("Outpic/circle_dft_by_cv.png", circ_ft);
	imwrite("Outpic/square_dft_by_cv.png", squa_ft);
	imwrite("Outpic/diamond_dft_by_cv.png", diam_ft);

	//傅立葉逆轉換by OpenCV
	Mat cv1, cv2, cv3, cv4;
	cv1 = opencv_idft(rect_cpx);
	cv2 = opencv_idft(circ_cpx);
	cv3 = opencv_idft(squa_cpx);
	cv4 = opencv_idft(diam_cpx);

	imshow("rect_idft_by_cv", cv1);
	imshow("circle_idft_by_cv", cv2);
	imshow("square_idft_by_cv", cv3);
	imshow("diamond_idft_by_cv", cv4);

	imwrite("Outpic/rect_idft_by_cv.png", cv1);
	imwrite("Outpic/circle_idft_by_cv.png", cv2);
	imwrite("Outpic/square_idft_by_cv.png", cv3);
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
