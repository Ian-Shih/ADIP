#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <time.h>
#include <string.h>
using namespace cv;
using namespace std;
void shuffle(int* ran) {
	unsigned seed;
	int i;
	seed = (unsigned)time(NULL);
	srand(seed);
	for (i = 0; i < 16; i++) {
		ran[i] = rand() % 16;
		//如果位置衝突就再洗牌，直到不衝突為止
		for (int j = 0; j < i; j++) {
			while (ran[j] == ran[i]) {
				j = 0;
				ran[i] = (rand() % 16);
			}
		}
	}
}
int hw1_2_B(Mat InMat, unsigned char* lenai);
int hw1_2_C(Mat InMat, unsigned char* lenai);
int hw1_2_D(Mat InMat, unsigned char* lenai);
int hw1_2_E(Mat InMat, unsigned char* lenai);
int hw1_2_F();
int hw1_3_A(Mat InMat, unsigned char* lenai);
int hw1_3_B(Mat InMat, unsigned char* lenai);
int hw2();

int main() {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OriFile[] = "Inpic/lena_256.raw";
	FILE* lenaFile;
	lenaFile = fopen(OriFile, "rb");
	if (lenaFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* lenai = new unsigned char[size];
	fread(lenai, 1, size, lenaFile);
	Mat lenaMat(height, width, CV_8UC1, lenai);
	//-----------------------algorithm-----------------------//
	while (1) {
		printf("-----\nhw1_2_B\nhw1_2_C\nhw1_2_D\nhw1_2_E\nhw1_2_F\nhw1_3_A\nhw1_3_B\nhw2\nend\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		if (strcmp(s, "hw1_2_B") == 0) {
			hw1_2_B(lenaMat, lenai);
		}
		else if (strcmp(s, "hw1_2_C") == 0) {
			hw1_2_C(lenaMat, lenai);
		}
		else if (strcmp(s, "hw1_2_D") == 0) {
			hw1_2_D(lenaMat, lenai);
		}
		else if (strcmp(s, "hw1_2_E") == 0) {
			hw1_2_E(lenaMat, lenai);
		}
		else if (strcmp(s, "hw1_2_F") == 0) {
			hw1_2_F();
		}
		else if (strcmp(s, "hw1_3_A") == 0) {
			hw1_3_A(lenaMat, lenai);
		}
		else if (strcmp(s, "hw1_3_B") == 0) {
			hw1_3_B(lenaMat, lenai);
		}
		else if (strcmp(s, "hw2") == 0) {
			hw2();
		}
		else if (strcmp(s, "end") == 0) {
			break;
		}
		else {
			printf("Please enter again\n\n");
		}
	}

	//-----------------------Release memory-----------------------//
	fclose(lenaFile);
	delete lenai;
	return 0;
}
int hw1_2_B(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	printf("1.2_b(1) The intensity at (123,234) is %3d \n", InMat.at<uchar>(123, 234));
	printf("1.2_b(2) The 5487th pixel's intensity  is %3d \n", InMat.at<uchar>(5487));
	printf("1.2_b(2) The 5487th pixel is at (%3d,%3d) \n", (5487 / height), (5487 % height));
	printf("1.2_b(2) Check the intensity at(21,111)：%3d \n", InMat.at<uchar>(5487 / height, 5487 % height));
	return 0;
}
int hw1_2_C(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OutFile[] = "Outpic/hw_1_2_c.raw";
	FILE* resultFile;
	resultFile = fopen(OutFile, "wb");
	if (resultFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	namedWindow("hw_1.2_c", 0);
	resizeWindow("hw_1.2_c", 256, 256);
	moveWindow("hw_1.2_c", 100, 100);
	imshow("hw_1.2_c", InMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(lenai, 1, size, resultFile);
	fclose(resultFile);
	destroyWindow("hw_1.2_c");
	return 0;
}
int hw1_2_D(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OutFile_r[] = "Outpic/hw_1_2_d.raw";
	FILE* resultFile_r;
	resultFile_r = fopen(OutFile_r, "wb");
	if (resultFile_r == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	int i, j, tmp;
	Mat OutMat(height, width, CV_8UC1);
	//exchange i and j
	for (i = 0; i <= 255; i++) {
		for (j = 0; j <= i; j++) {
			tmp = InMat.at<uchar>(i, j);
			OutMat.at<uchar>(i, j) = InMat.at<uchar>(j, i);
			OutMat.at<uchar>(j, i) = tmp;
		}
	}
	namedWindow("hw_1.2_d", 0);
	resizeWindow("hw_1.2_d", 256, 256);
	moveWindow("hw_1.2_d", 100, 100);
	imshow("hw_1.2_d", OutMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat.ptr(0, 0), 1, size, resultFile_r);
	//-----------------------Release memory-----------------------//
	fclose(resultFile_r);
	destroyWindow("hw_1.2_d");
	return 0;
}
int hw1_2_E(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	Mat test[16];
	//-----------------------Read File-----------------------//
	char OutFile[] = "Outpic/hw_1_2_e.raw";
	char OutFile2[] = "Outpic/hw_1_2_e_2.raw";
	FILE* resultFile, * resultFile2;
	resultFile = fopen(OutFile, "wb");
	resultFile2 = fopen(OutFile2, "wb");
	if (resultFile == NULL || resultFile2 == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	Mat OutMat(height, width, CV_8UC1);
	Mat OutMat2(height, width, CV_8UC1);
	//-----------------------cut picture-----------------------//
	int i, j, m, n;
	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= 3; j++) {
			Mat	test2(64, 64, CV_8UC1);
			for (m = 0; m <= 63; m++) {
				for (n = 0; n <= 63; n++) {
					test2.at<uchar>(m, n) = InMat.at<uchar>(m + 64 * i, n + 64 * j);
				}
			}
			test[4 * i + j] = test2;
		}
	}
	//-----------------------shuffle_1-----------------------//
	int ran[16];
	shuffle(ran);
	//-----------------------add to new mat-----------------------//

	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= 3; j++) {
			for (m = 0; m <= 63; m++) {
				for (n = 0; n <= 63; n++) {
					OutMat.at<uchar>(m + 64 * i, n + 64 * j) = test[ran[4 * i + j]].at<uchar>(m, n);
				}
			}
		}
	}
	namedWindow("hw_1.2_e", 0);
	resizeWindow("hw_1.2_e", 256, 256);
	moveWindow("hw_1.2_e", 100, 100);
	imshow("hw_1.2_e", OutMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat.ptr(0, 0), 1, size, resultFile);
	//-----------------------Release memory-----------------------//
	fclose(resultFile);
	//-----------------------shuffle_2-----------------------//

	int ran2[16];
	shuffle(ran2);
	//-----------------------add to new mat-----------------------//
	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= 3; j++) {
			for (m = 0; m <= 63; m++) {
				for (n = 0; n <= 63; n++) {
					OutMat2.at<uchar>(m + 64 * i, n + 64 * j) = test[ran2[4 * i + j]].at<uchar>(m, n);
				}
			}
		}
	}
	namedWindow("hw_1.2_e_2", 0);
	resizeWindow("hw_1.2_e_2", 256, 256);
	moveWindow("hw_1.2_e_2", 100, 100);
	imshow("hw_1.2_e_2", OutMat2);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat2.ptr(0, 0), 1, size, resultFile2);
	//-----------------------Release memory-----------------------//
	fclose(resultFile2);
	destroyWindow("hw_1.2_e");
	destroyWindow("hw_1.2_e_2");
	return 0;
}
int hw1_2_F() {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char picture_01[] = "Inpic/p01.raw";
	char picture_02[] = "Inpic/p02.raw";
	char picture_03[] = "Inpic/p03.raw";
	char picture_04[] = "Inpic/p04.raw";
	FILE* File_01, * File_02, * File_03, * File_04;
	File_01 = fopen(picture_01, "rb");
	File_02 = fopen(picture_02, "rb");
	File_03 = fopen(picture_03, "rb");
	File_04 = fopen(picture_04, "rb");

	char OutFile[] = "Outpic/hw_1_2_f.raw";
	FILE* resultFile;
	resultFile = fopen(OutFile, "wb");
	Mat OutMat(height, width, CV_8UC1);

	if (File_01 == NULL || File_02 == NULL || File_03 == NULL || File_04 == NULL || resultFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* p01_i = new unsigned char[size];
	unsigned char* p02_i = new unsigned char[size];
	unsigned char* p03_i = new unsigned char[size];
	unsigned char* p04_i = new unsigned char[size];
	fread(p01_i, 1, size, File_01);
	fread(p02_i, 1, size, File_02);
	fread(p03_i, 1, size, File_03);
	fread(p04_i, 1, size, File_04);
	Mat p01Mat(height, width, CV_8UC1, p01_i);
	Mat p02Mat(height, width, CV_8UC1, p02_i);
	Mat p03Mat(height, width, CV_8UC1, p03_i);
	Mat p04Mat(height, width, CV_8UC1, p04_i);
	//-----------------------compress 256*256 to 128*128 -----------------------//
	int m, n;
	Mat	p01_tmp(128, 128, CV_8UC1);
	for (m = 0; m <= 255; m += 2) {
		for (n = 0; n <= 255; n += 2) {
			p01_tmp.at<uchar>(m / 2, n / 2) = (p01Mat.at<uchar>(m, n) / 4 + p01Mat.at<uchar>(m + 1, n) / 4 + p01Mat.at<uchar>(m, n + 1) / 4 + p01Mat.at<uchar>(m + 1, n + 1) / 4);
		}
	}
	Mat	p02_tmp(128, 128, CV_8UC1);
	for (m = 0; m <= 255; m += 2) {
		for (n = 0; n <= 255; n += 2) {
			p02_tmp.at<uchar>(m / 2, n / 2) = (p02Mat.at<uchar>(m, n) / 4 + p02Mat.at<uchar>(m + 1, n) / 4 + p02Mat.at<uchar>(m, n + 1) / 4 + p02Mat.at<uchar>(m + 1, n + 1) / 4);
		}
	}
	Mat	p03_tmp(128, 128, CV_8UC1);
	for (m = 0; m <= 255; m += 2) {
		for (n = 0; n <= 255; n += 2) {
			p03_tmp.at<uchar>(m / 2, n / 2) = (p03Mat.at<uchar>(m, n) / 4 + p03Mat.at<uchar>(m + 1, n) / 4 + p03Mat.at<uchar>(m, n + 1) / 4 + p03Mat.at<uchar>(m + 1, n + 1) / 4);
		}
	}
	Mat	p04_tmp(128, 128, CV_8UC1);
	for (m = 0; m <= 255; m += 2) {
		for (n = 0; n <= 255; n += 2) {
			p04_tmp.at<uchar>(m / 2, n / 2) = (p04Mat.at<uchar>(m, n) / 4 + p04Mat.at<uchar>(m + 1, n) / 4 + p04Mat.at<uchar>(m, n + 1) / 4 + p04Mat.at<uchar>(m + 1, n + 1) / 4);
		}
	}
	//-----------------------do a puzzle -----------------------//
	Mat assembled_tmp[4];
	assembled_tmp[0] = p03_tmp;
	assembled_tmp[1] = p04_tmp;
	assembled_tmp[2] = p02_tmp;
	assembled_tmp[3] = p01_tmp;

	int i, j;
	for (i = 0; i <= 1; i++) {
		for (j = 0; j <= 1; j++) {
			for (m = 0; m <= 127; m++) {
				for (n = 0; n <= 127; n++) {
					OutMat.at<uchar>(m + 127 * i, n + 127 * j) = assembled_tmp[2 * i + j].at<uchar>(m, n);
				}
			}
		}
	}

	namedWindow("hw_1.2_f", 0);
	resizeWindow("hw_1.2_f", 256, 256);
	moveWindow("hw_1.2_f", 100, 100);
	imshow("hw_1.2_f", OutMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat.ptr(0, 0), 1, size, resultFile);
	//-----------------------Release memory-----------------------//
	fclose(File_01);
	fclose(File_02);
	fclose(File_03);
	fclose(File_04);
	fclose(resultFile);
	delete p01_i, p02_i, p03_i, p04_i;
	destroyWindow("hw_1.2_f");
	return 0;

}
int hw1_3_A(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OutFile[] = "Outpic/hw_1_3_a.raw";
	FILE* resultFile;
	resultFile = fopen(OutFile, "wb");
	if (resultFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	fread(lenai, 1, size, resultFile);
	namedWindow("origin", 0);
	resizeWindow("origin", 256, 256);
	moveWindow("origin", 100, 100);
	imshow("origin", InMat);
	waitKey(0);

	int i, j;
	Mat OutMat(height, width, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			if (InMat.at<uchar>(i, j) >= 205) {
				OutMat.at<uchar>(i, j) = 255;
			}
			else {
				OutMat.at<uchar>(i, j) = InMat.at<uchar>(i, j) + 50;
			}
		}
	}

	namedWindow("hw_1.3_a", 0);
	resizeWindow("hw_1.3_a", 256, 256);
	moveWindow("hw_1.3_a", 100, 100);
	imshow("hw_1.3_a", OutMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat.ptr(0, 0), 1, size, resultFile);
	//-----------------------Release memory-----------------------//
	fclose(resultFile);
	destroyWindow("origin");
	destroyWindow("hw_1.3_a");
	return 0;
}
int hw1_3_B(Mat InMat, unsigned char* lenai) {
	int width = 256;
	int height = 256;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OutFile[] = "Outpic/hw_1_3_b.raw";
	FILE* resultFile;
	resultFile = fopen(OutFile, "wb");
	if (resultFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	//-----------------------add random value of brightness-----------------------//
	int ran;
	unsigned seed;
	seed = (unsigned)time(NULL);
	srand(seed);
	ran = ((rand() % 100) - 50);
	printf("rand val =%d", ran);

	namedWindow("origin", 0);
	resizeWindow("origin", 256, 256);
	moveWindow("origin", 100, 100);
	imshow("origin", InMat);
	waitKey(0);

	int i, j;
	Mat OutMat(height, width, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			if (ran > 0) {
				if (InMat.at<uchar>(i, j) >= 255 - ran) {
					OutMat.at<uchar>(i, j) = 255;
				}
				else {
					OutMat.at<uchar>(i, j) = InMat.at<uchar>(i, j) + ran;
				}
			}
			else if (ran < 0) {
				if (InMat.at<uchar>(i, j) <= -ran) {
					OutMat.at<uchar>(i, j) = 0;
				}
				else {
					OutMat.at<uchar>(i, j) = InMat.at<uchar>(i, j) + ran;
				}
			}
		}
	}

	namedWindow("hw_1.3_b", 0);
	resizeWindow("hw_1.3_b", 256, 256);
	moveWindow("hw_1.3_b", 100, 100);
	imshow("hw_1.3_b", OutMat);
	waitKey(0);
	//-----------------------Save Image as raw format-----------------------//
	fwrite(OutMat.ptr(0, 0), 1, size, resultFile);
	//-----------------------Release memory-----------------------//
	fclose(resultFile);
	destroyWindow("origin");
	destroyWindow("hw_1.3_b");
	return 0;
}
int hw2() {
	int width = 512;
	int height = 512;
	int size = width * height;
	char OriFile[] = "Inpic/haha.raw";
	char OutFile[] = "Outpic/hw_2.raw";
	//-----------------------Read File-----------------------//
	FILE* hahaFile;
	hahaFile = fopen(OriFile, "rb");
	FILE* resultFile;
	resultFile = fopen(OutFile, "wb");
	if (hahaFile == NULL || resultFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* hahai = new unsigned char[size];
	fread(hahai, 1, size, hahaFile);
	//-----------------------compress 512*512 to 256*256 -----------------------//
	int m, n;
	Mat hahaMat(height, width, CV_8UC1, hahai);
	Mat hahatmp(256, 256, CV_8UC1);
	for (m = 0; m <= 511; m += 2) {
		for (n = 0; n <= 511; n += 2) {
			hahatmp.at<uchar>(m / 2, n / 2) = (hahaMat.at<uchar>(m, n) / 4 + hahaMat.at<uchar>(m + 1, n) / 4 + hahaMat.at<uchar>(m, n + 1) / 4 + hahaMat.at<uchar>(m + 1, n + 1) / 4);
		}
	}
	//-----------------------write text on picture -----------------------//
	putText(hahatmp, "109318044", Point(0, 30), FONT_HERSHEY_TRIPLEX, 1, (0, 0, 0));
	putText(hahatmp, "Too Difficult", Point(10, 240), FONT_HERSHEY_TRIPLEX, 1, (0, 0, 0));
	namedWindow("hw_2", 0);
	resizeWindow("hw_2", 256, 256);
	moveWindow("hw_2", 100, 100);
	imshow("hw_2", hahatmp);
	waitKey(0);
	//-----------------------Save Image as raw and png format-----------------------//	
	fwrite(hahatmp.ptr(0, 0), 1, 256 * 256, resultFile);
	imwrite("Outpic/hw_2.png", hahatmp);
	//-----------------------Release memory-----------------------//
	fclose(hahaFile);
	fclose(resultFile);
	delete hahai;
	destroyWindow("hw_2");
	return 0;
}
