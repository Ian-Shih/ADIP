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
int avg[540][960];
int avg10[540][960];
int avg200[540][960];
int masksize = 3;
void gaussian(double sig,double Outmask[5][5]) {
	//生成高斯濾波器
	int i, j;
	double pi = 3.14,sum=0;
	for (i = -2; i <= 2; i++) {
		for (j = 2; j >= -2; j--) {
			Outmask[i + 2][2-j] =	(1 / (2 * pi * sig*sig)) 
									* exp(-(i*i + j*j)  
									/ ( 2 * sig * sig) );
			sum += Outmask[i + 2][2 - j];
		}
	}
	printf("sig=%.1f\n", sig);
	for (i = 0; i <= 4; i++) {
		for (j = 0; j <= 4; j++) {
			Outmask[i][j] /= sum;
			printf("%f ", Outmask[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
void padding_zero(Mat InMat, Mat OutMat, int mask[3][3]) {
	//0填充
	int i, j, p, q, border_x, border_y;
	double sum = 0;
	masksize = 3;
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			sum = 0;
			for (p = i - masksize / 2; p <= (i + masksize / 2); p++) {
				for (q = j - masksize / 2; q <= (j + masksize / 2); q++) {
					border_x = p;
					border_y = q;
					//超出邊界不計算
					if (p < 0 || p>(InMat.rows - 1) || q < 0 || q>(InMat.cols - 1)) {
						sum += 0;
					}
					else {
						sum +=	InMat.data[border_x * InMat.cols + border_y] * 
								mask[p - (i - masksize / 2)][q - (j - masksize / 2)];
					}
				}
			}
			if (sum < 0) {
				OutMat.data[i * OutMat.cols + j] = 0;
			}
			else if (sum > 255) {
				OutMat.data[i * OutMat.cols + j] = 255;
			}
			else {
				OutMat.data[i * OutMat.cols + j] = sum;
			}
		}
	}
}
void padding_rep(Mat InMat, Mat OutMat, int mask[3][3]) {
	//複製填充
	int i, j,p,q, border_x, border_y;
	double sum = 0;
	masksize = 3;
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			sum = 0;
			for (p = i - masksize / 2; p <= (i + masksize / 2); p++) {
				for (q = j - masksize / 2; q <= (j + masksize / 2); q++) {
					border_x = p;
					border_y = q;
					//超出邊界則當作邊界
					if (p < 0 || p>(InMat.rows-1) || q < 0 || q>(InMat.cols-1)) {
						if (p < 0) {
							border_x = 0;
						}
						else if (p > 511) {
							border_x = 511;
						}
						if (q < 0) {
							border_y = 0;
						}
						else if (q > 511) {
							border_y = 511;
						}
					}
					sum +=	InMat.data[border_x * InMat.cols + border_y] *
							mask[p - (i - masksize / 2)][q - (j - masksize / 2)];
				}
			}
			if (sum < 0) {
				OutMat.data[i * OutMat.cols + j] = 0;
			}
			else if (sum > 255) {
				OutMat.data[i * OutMat.cols + j] = 255;
			}
			else {
				OutMat.data[i * OutMat.cols + j] = sum;
			}
		}
	}
}
void padding_mir(Mat InMat, Mat OutMat, double mask[5][5]) {
	//鏡像填充
	int i, j, p, q, border_x, border_y;
	double sum = 0;
	masksize = 5;
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			sum = 0;
			for (p = i - masksize / 2; p <= (i + masksize / 2); p++) {
				for (q = j - masksize / 2; q <= (j + masksize / 2); q++) {
					border_x = p;
					border_y = q;
					//超出邊界則以邊界為軸鏡射
					if (p < 0 || p>(InMat.rows - 1) || q < 0 || q>(InMat.cols - 1)) {
						if (p < 0) {
							border_x = p *(-1);
						}
						else if (p > 511) {
							border_x = 511*2-p;
						}
						if (q < 0) {
							border_y = q *(-1);
						}
						else if (q > 511) {
							border_y = 511 * 2 - q;
						}
					}
					sum +=	InMat.data[border_x * InMat.cols + border_y] * 
							mask[p - (i - masksize / 2)][q - (j - masksize / 2)];
				}
			}
			if (sum < 0) {
				OutMat.data[i * OutMat.cols + j] = 0;
			}
			else if (sum > 255) {
				OutMat.data[i * OutMat.cols + j] = 255;
			}
			else {
				OutMat.data[i * OutMat.cols + j] = sum;
			}
		}
	}
}
void pdf(Mat InMat,double* cnt) {
	//計算pdf
	int i, j;
	for (i = 0; i < 256; i++) {
		cnt[i] = 0;
	}
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			cnt[InMat.data[i * InMat.cols + j]]++;
		}
	}
	for (i = 0; i < 256; i++) {
		cnt[i] /= InMat.rows * InMat.cols;
	}
}
void cdf(double* cnt) {
	//將pdf轉成cdf
	int i;
	for (i = 1; i < 256; i++) {
		cnt[i] += cnt[i - 1];
	}
}
void histogram(double *cnt,Mat OutMat) {
	//畫直方圖
	int i, j;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			OutMat.data[i * OutMat.cols + j] = 0;
		}
	}
	for (i = 0; i < 256; i++)
	{
		line(OutMat, Point(i, 255), Point(i, 255 - (cnt[i] * 255)), Scalar(255));
	}
}
void local_enhance(Mat InMat, Mat OutMat,int mask) {
	double cnt_mask[256];
	int i, j, s, p, q;
	//以區域直方圖來進行等化
	for (i = mask / 2; i < InMat.rows - mask / 2; i++) {
		for (j = mask / 2; j < InMat.cols - mask / 2; j++) {
			for (s = 0; s < 256; s++) {
				cnt_mask[s] = 0;
			}
			for (p = i - mask / 2; p <= (i + mask / 2); p++) {
				for (q = j - mask / 2; q <= (j + mask / 2); q++) {
					cnt_mask[InMat.data[p * InMat.cols + q]]++;
				}
			}
			cnt_mask[0] /= (mask * mask);
			for (s = 1; s < 256; s++) {
				cnt_mask[s] /= (mask * mask);
				cnt_mask[s] += (cnt_mask[s - 1]);
			}
			OutMat.data[i * OutMat.cols + j] = cnt_mask[InMat.data[i * InMat.cols + j]] * 255;
		}
	}
}
void hw1_1() {
	int width = 512;
	int height = 512;
	int size = width * height;
	int i, j;
	char OriFile[] = "Inpic/house512.raw";
	FILE* houseFile;
	houseFile = fopen(OriFile, "rb");
	if (houseFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* house = new unsigned char[size];
	fread(house, 1, size, houseFile);
	Mat houseMat(height, width, CV_8UC1, house);
	char OriFile2[] = "Inpic/lena512.raw";
	FILE* lenaFile;
	lenaFile = fopen(OriFile2, "rb");
	if (lenaFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* lenai = new unsigned char[size];
	fread(lenai, 1, size, lenaFile);
	Mat lenaMat(height, width, CV_8UC1, lenai);
	Mat househist(256, 256, CV_8UC1);
	Mat lenahist(256, 256, CV_8UC1);
	double cnt_house[256], cnt_lena[256];
	//取直方圖
	pdf(houseMat, cnt_house);
	histogram(cnt_house, househist);
	imshow("househist", househist);
	pdf(lenaMat, cnt_lena);
	histogram(cnt_lena, lenahist);
	imshow("lenahist", lenahist);
	cdf(cnt_house);
	histogram(cnt_house, househist);
	imshow("housecdf", househist);
	cdf(cnt_lena);
	histogram(cnt_lena, lenahist);
	imshow("lenacdf", lenahist);
	//直方圖匹配
	float diff_cdf[256][256];
    for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			diff_cdf[i][j] = fabs(cnt_house[i] - cnt_lena[j]);
		}
    }
	int lut[256] = { 0 };
	for (i = 0; i < 256; i++) {
		float min = diff_cdf[i][0];
		int index = 0;
		for (int j = 0; j < 256; j++) {
			if (min > diff_cdf[i][j]) {
				min = diff_cdf[i][j];
				index = j;
			}
		}
		lut[i] = index;
	}
	Mat fix(height, width, CV_8UC1);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fix.data[i * fix.cols + j] = lut[houseMat.data[i * houseMat.cols + j]];
		}
	}
	imshow("match", fix);
	imwrite("match.png", fix);
	double cnt_fix[256];
	pdf(fix, cnt_fix);
	Mat fixhist(256, 256, CV_8UC1);
	histogram(cnt_fix, fixhist);
	imshow("match_hist", fixhist);
	imwrite("match_hist.png", fixhist);
	cdf(cnt_fix);
	histogram(cnt_fix, fixhist);
	imshow("match_cdf", fixhist);	
	waitKey(0);
	destroyAllWindows();
}
void hw1_2() {
	int width = 512;
	int height = 512;
	int size = width * height;
	double cnt_house[256];
	char OriFile[] = "Inpic/house512.raw";
	FILE* houseFile;
	houseFile = fopen(OriFile, "rb");
	if (houseFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* house = new unsigned char[size];
	fread(house, 1, size, houseFile);
	Mat houseMat(height, width, CV_8UC1, house);
	Mat fix1(height, width, CV_8UC1);
	Mat mask_hist(256, 256, CV_8UC1);
	local_enhance(houseMat, fix1, 3);
	imshow("3*3", fix1);
	imwrite("mask3_3.png", fix1);
	pdf(fix1, cnt_house);
	histogram(cnt_house, mask_hist);
	imshow("hist3_3", mask_hist);
	imwrite("hist3_3.png", mask_hist);
	Mat fix2(height, width, CV_8UC1);
	local_enhance(houseMat, fix2, 5);
	imshow("5*5", fix2);
	imwrite("mask5_5.png", fix2);
	pdf(fix2, cnt_house);
	histogram(cnt_house, mask_hist);
	imshow("hist5_5", mask_hist);
	imwrite("hist5_5.png", mask_hist);
	Mat fix3(height, width, CV_8UC1);
	local_enhance(houseMat, fix3, 9);
	imshow("9*9", fix3);
	imwrite("mask9_9.png", fix3);
	pdf(fix3, cnt_house);
	histogram(cnt_house, mask_hist);
	imshow("hist9_9", mask_hist);
	imwrite("hist9_9.png", mask_hist);
	waitKey(0);
	destroyAllWindows();
}
void hw2() {
	//平均背景建模
	VideoCapture cap("Inpic/street.avi");
	if (!cap.isOpened()) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}		
	Mat sum(540,960,CV_8UC1);
	Mat sum10(540, 960, CV_8UC1);
	Mat sum200(540, 960, CV_8UC1);
	int cnt = 0;
	int i, j;
	for (i = 0; i < sum.rows; i++) {
		for (j = 0; j < sum.cols; j++) {
			sum.at<uchar>(i, j) = 0;
			avg[i][j]=0;
			sum10.at<uchar>(i, j) = 0;
			avg10[i][j] = 0;
			sum200.at<uchar>(i, j) = 0;
			avg200[i][j] = 0;
		}
	}
	while (1) {
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		cnt++;
		cvtColor(frame,frame,COLOR_BGR2GRAY);
		for (i = 0; i < frame.rows; i++) {
			for (j = 0; j < frame.cols; j++) {
				//取每1張
				avg[i][j] += frame.at<uchar>(i, j);
				//取前200張
				if (cnt <= 200) {
					avg200[i][j] += frame.at<uchar>(i, j);
				}
				//每40張取1張，共取10張
				if ((cnt%40==0)&&(cnt/40<=10)) {
					avg10[i][j] += frame.at<uchar>(i, j);
				}
			}
		}
	}
	for (i = 0; i < sum.rows; i++) {
		for (j = 0; j < sum.cols; j++) {
			//灰階平均
			avg[i][j] /= cnt;
			sum.at<uchar>(i, j)= avg[i][j];
			avg200[i][j] /= 200;
			sum200.at<uchar>(i, j) = avg200[i][j];
			avg10[i][j] /= 10;
			sum10.at<uchar>(i, j) = avg10[i][j];
		}
	}
	imshow("all", sum);
	imshow("200", sum200);
	imshow("10", sum10);
	imwrite("framesall.png", sum);
	imwrite("frames200.png", sum200);
	imwrite("frames10.png", sum10);
	waitKey(0);
	cap.release();
	destroyAllWindows();
}
void hw3() {
	int width = 512;
	int height = 512;
	int size = width * height;
	masksize = 3;
	char OriFile[] = "Inpic/walkbridge.raw";
	FILE* bridgeFile;
	bridgeFile = fopen(OriFile, "rb");
	if (bridgeFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* bridge = new unsigned char[size];
	fread(bridge, 1, size, bridgeFile);
	Mat bridgeMat(height, width, CV_8UC1, bridge);
	//邊緣濾波
	int mask1[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0}};
	int mask2[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
	Mat pad1(height, width, CV_8UC1);
	Mat pad2(height, width, CV_8UC1);
	//排除邊界問題
	padding_zero(bridgeMat, pad1, mask1);
	imshow("4_padding_zero", pad1);
	imwrite("4_padding_zero.png", pad1);
	padding_zero(bridgeMat, pad2, mask2);
	imshow("8_padding_zero", pad2);
	imwrite("8_padding_zero.png", pad2);
	padding_rep(bridgeMat, pad1, mask1);
	imshow("4_padding_rep", pad1);
	imwrite("4_padding_rep.png", pad1);
	padding_rep(bridgeMat, pad2, mask2);
	imshow("8_padding_rep", pad2);
	imwrite("8_padding_rep.png", pad2);
	Mat pad3(height, width, CV_8UC1);
	//high-boost
	int A = 1;
	int mask3[3][3] = { {0,-1,0},{-1,A+4,-1},{0,-1,0} };
	padding_rep(bridgeMat, pad3, mask3);
	imshow("hb1_padding_rep", pad3);
	imwrite("hb1_padding_rep.png", pad3);
	A = 2;
	mask3[1][1] = A+4;
	padding_rep(bridgeMat, pad3, mask3);
	imshow("hb2_padding_rep", pad3);
	imwrite("hb2_padding_rep.png", pad3);
	A = 4;
	mask3[1][1] = A+4;
	padding_rep(bridgeMat, pad3, mask3);
	imshow("hb4_padding_rep", pad3);
	imwrite("hb4_padding_rep.png", pad3);
	waitKey(0);
	destroyAllWindows();
}
void hw4() {
	masksize = 5;
	int width = 512;
	int height = 512;
	int size = width * height;
	char OriFile[] = "Inpic/turtle512.raw";
	FILE* turtleFile;
	turtleFile = fopen(OriFile, "rb");
	if (turtleFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	int i, j;
	unsigned char* turtle = new unsigned char[size];
	fread(turtle, 1, size, turtleFile);
	Mat turtleMat(height, width, CV_8UC1, turtle);
	Mat pad_g1(height, width, CV_8UC1);
	Mat pad_g2(height, width, CV_8UC1);
	Mat pad_g3(height, width, CV_8UC1);
	double test[5][5];
	//高斯濾波
	gaussian(0.8,test);
	padding_mir(turtleMat, pad_g1, test);
	imshow("gause0.8", pad_g1);
	imwrite("gause08.png", pad_g1);
	gaussian(1.3, test);
	padding_mir(turtleMat, pad_g1, test);
	imshow("gause1.3", pad_g1);
	imwrite("gause13.png", pad_g1);
	gaussian(2, test);
	padding_mir(turtleMat, pad_g1, test);
	imshow("gause2.0", pad_g1);
	imwrite("gause2.png", pad_g1);
	gaussian(0.5, test);
	padding_mir(turtleMat, pad_g1, test);
	gaussian(1.5, test);
	padding_mir(turtleMat, pad_g2, test);
	double diff = 0;
	//高斯差值
	for (i = 0; i < pad_g1.rows; i++) {
		for (j = 0; j < pad_g1.cols; j++) {
			diff= pad_g1.data[i * pad_g1.cols + j] - pad_g2.data[i * pad_g2.cols + j];
			if (diff < 0) {
				pad_g3.data[i * pad_g3.cols + j] = 0;
			}
			else if (diff > 255) {
				pad_g3.data[i * pad_g3.cols + j] = 255;
			}
			else {
				pad_g3.data[i * pad_g3.cols + j] = diff;
			}
		}
	}
	imshow("DoG", pad_g3);
	imwrite("DoG.png", pad_g3);
	for (i = 0; i < pad_g1.rows; i++) {
		for (j = 0; j < pad_g1.cols; j++) {
			if (pad_g3.data[i * pad_g3.cols + j]) {
				pad_g3.data[i * pad_g3.cols + j] = 255;
			}
		}
	}
	imshow("DoG_bright", pad_g3);
	imwrite("DoG_bright.png", pad_g3);
	waitKey(0);
	destroyAllWindows();
}
int main() {
	while (1) {
		printf("-----\nhw1_1\nhw1_2\nhw2\nhw3\nhw4\nend\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		if (strcmp(s, "hw1_1") == 0) {
			hw1_1();
		}
		else if (strcmp(s, "hw1_2") == 0) {
			hw1_2();
		}
		else if (strcmp(s, "hw2") == 0) {
			hw2();
		}
		else if (strcmp(s, "hw3") == 0) {
			hw3();
		}
		else if (strcmp(s, "hw4") == 0) {
			hw4();
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
