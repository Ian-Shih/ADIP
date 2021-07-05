#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <time.h>
#include <string.h>
#include <math.h>
using namespace cv;
using namespace std;
void pow_law(Mat InMat, Mat OutMat, double c, double r) {
	//power-law灰度校正
	int i, j;
	double x;
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			x = pow(InMat.data[i * InMat.cols + j], r);
			OutMat.data[i * OutMat.cols + j] = c * x;
		}
	}
}
void log_tf(Mat InMat, Mat OutMat, double c) {
	//log灰度校正
	int i, j;
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			OutMat.data[i * OutMat.cols + j] = c * log10(1 + InMat.data[i * InMat.cols + j]);
		}
	}
}
void bitplane(Mat InMat, Mat OutMat[8]) {
	//將圖分成bitplane
	int i, j, k;
	int pow_bit;
	for (i = 0; i < 8; i++) {	//bit plane
		OutMat[i] = Mat(InMat.rows, InMat.cols, CV_8UC1);
	}
	for (k = 0; k < 8; k++) {
		for (i = 0; i < InMat.rows; i++) {
			for (j = 0; j < InMat.cols; j++) {
				pow_bit = pow(2, k);
				OutMat[k].data[i * OutMat[k].cols + j] =
					(InMat.data[i * InMat.cols + j] & pow_bit) / pow_bit * 255;
			}
		}
	}
}
void mix(Mat InMat[8], Mat OutMat) {
	//將bitplane組成圖片
	int i, j, k;
	int pow_bit;
	for (i = 0; i < OutMat.rows; i++) {
		for (j = 0; j < OutMat.cols; j++) {
			OutMat.data[i * OutMat.cols + j] = 0;
		}
	}
	for (k = 0; k < 8; k++) {
		for (i = 0; i < InMat[k].rows; i++) {
			for (j = 0; j < InMat[k].cols; j++) {
				pow_bit = pow(2, k);
				OutMat.data[i * OutMat.cols + j] +=
					(InMat[k].data[i * InMat[k].cols + j] / 255 * pow_bit);
			}
		}
	}
}
void hw1() {
	int width = 256;
	int height = 256;
	int size = width * height;
	int i, j, k;
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

	char OutFile[] = "lena_with_baboon.raw";
	FILE* mixFile;
	mixFile = fopen(OutFile, "wb");
	if (mixFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}


	Mat lenaMat(height, width, CV_8UC1, lenai);
	imshow("lena", lenaMat);
	char OriFile2[] = "Inpic/baboon_256.raw";
	FILE* baboonFile;
	baboonFile = fopen(OriFile2, "rb");
	if (baboonFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* baboi = new unsigned char[size];
	fread(baboi, 1, size, baboonFile);
	Mat baboon(height, width, CV_8UC1, baboi);
	imshow("baboon", baboon);
	Mat	test1[8], test2[8], test3[8];

	bitplane(lenaMat, test1);
	bitplane(baboon, test2);
	int change = 4;
	//將B圖的部分bitplane藏到A圖
	for (k = 0; k < change; k++) {
		for (i = 0; i < lenaMat.rows; i++) {
			for (j = 0; j < lenaMat.cols; j++) {
				test1[k].data[i * test1[k].cols + j] =
					test2[k + 8 - change].data[i * test2[k + 8 - change].cols + j];
			}
		}
	}
	Mat M(height, width, CV_8UC1);
	mix(test1, M);
	imshow("lena_with_baboon", M);
	double mse, psnr;
	mse = 0;
	psnr = 0;
	for (i = 0; i < M.rows; i++) {
		for (j = 0; j < M.cols; j++) {
			mse += pow((M.data[i * M.cols + j] - lenaMat.data[i * lenaMat.cols + j]), 2);
		}
	}
	mse /= (double)size;
	psnr = 10 * log10(pow(255, 2) / mse);
	printf("\nmix vs. lena mse: %f\n", mse);
	printf("mix vs. lena psnr: %f\n", psnr);

	bitplane(M, test3);
	Mat final(height, width, CV_8UC1);
	int pow_bit;
	for (i = 0; i < final.rows; i++) {
		for (j = 0; j < final.cols; j++) {
			final.data[i * final.cols + j] = 0;
		}
	}
	for (k = 0; k < change; k++) {
		for (i = 0; i < test3[k].rows; i++) {
			for (j = 0; j < test3[k].cols; j++) {
				pow_bit = pow(2, k + 8 - change);
				final.data[i * final.cols + j] += (test3[k].data[i * test3[k].cols + j] / 255 * pow_bit);
			}
		}
	}
	for (i = 0; i < final.rows; i++) {
		for (j = 0; j < final.cols; j++) {
			mse += pow((final.data[i * final.cols + j] - baboon.data[i * baboon.cols + j]), 2);
		}
	}
	mse /= (double)size;
	psnr = 10 * log10(pow(255, 2) / mse);
	printf("\nextract vs. baboon mse: %f\n", mse);
	printf("extract vs. baboon psnr: %f\n", psnr);

	imshow("extract", final);
	waitKey(0);
	destroyAllWindows();
	fwrite(&M.data[0], 1, size, mixFile);
	imwrite("Outpic/hw1_lena_with_baboon.png", M);
	imwrite("Outpic/hw1_extract.png", final);
}
void hw2() {
	int width = 512;
	int height = 512;
	int size = width * height;
	int i, j, k, pow_bit, pow_bit2;
	char OriFile_a[] = "Inpic/a512x512.raw";
	char OriFile_b[] = "Inpic/b512x512.raw";
	char OriFile_c[] = "Inpic/c512x512.raw";
	char OriFile_d[] = "Inpic/d512x512.raw";
	char OriFile_e[] = "Inpic/e512x512.raw";
	char OriFile_f[] = "Inpic/f512x512.raw";
	char OriFile_g[] = "Inpic/g512x512.raw";
	char OriFile_h[] = "Inpic/h512x512.raw";
	FILE* lenaFile_a, * lenaFile_b, * lenaFile_c, * lenaFile_d, * lenaFile_e, * lenaFile_f, * lenaFile_g, * lenaFile_h;
	lenaFile_a = fopen(OriFile_a, "rb");
	lenaFile_b = fopen(OriFile_b, "rb");
	lenaFile_c = fopen(OriFile_c, "rb");
	lenaFile_d = fopen(OriFile_d, "rb");
	lenaFile_e = fopen(OriFile_e, "rb");
	lenaFile_f = fopen(OriFile_f, "rb");
	lenaFile_g = fopen(OriFile_g, "rb");
	lenaFile_h = fopen(OriFile_h, "rb");

	unsigned char* pic_a = new unsigned char[size],
		* pic_b = new unsigned char[size],
		* pic_c = new unsigned char[size],
		* pic_d = new unsigned char[size],
		* pic_e = new unsigned char[size],
		* pic_f = new unsigned char[size],
		* pic_g = new unsigned char[size],
		* pic_h = new unsigned char[size];
	fread(pic_a, 1, size, lenaFile_a);
	fread(pic_b, 1, size, lenaFile_b);
	fread(pic_c, 1, size, lenaFile_c);
	fread(pic_d, 1, size, lenaFile_d);
	fread(pic_e, 1, size, lenaFile_e);
	fread(pic_f, 1, size, lenaFile_f);
	fread(pic_g, 1, size, lenaFile_g);
	fread(pic_h, 1, size, lenaFile_h);
	Mat Apic(height, width, CV_8UC1, pic_a);
	Mat Bpic(height, width, CV_8UC1, pic_b);
	Mat Cpic(height, width, CV_8UC1, pic_c);
	Mat Dpic(height, width, CV_8UC1, pic_d);
	Mat Epic(height, width, CV_8UC1, pic_e);
	Mat Fpic(height, width, CV_8UC1, pic_f);
	Mat Gpic(height, width, CV_8UC1, pic_g);
	Mat Hpic(height, width, CV_8UC1, pic_h);
	Mat test(height, width, CV_8UC1);
	for (i = 0; i < test.rows; i++) {
		for (j = 0; j < test.cols; j++) {
			test.data[i * test.cols + j] = 0;
		}
	}
	//將8張bitplane重新組合成原圖
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			pow_bit = pow(2, 7);
			test.data[i * test.cols + j] += (Bpic.data[i * Bpic.cols + j] / 255) * pow_bit;
			pow_bit = pow(2, 6);
			test.data[i * test.cols + j] += (Gpic.data[i * Gpic.cols + j] / 255) * pow_bit;
			pow_bit = pow(2, 5);
			test.data[i * test.cols + j] += (1 - (Hpic.data[i * Hpic.cols + j] / 255)) * pow_bit;
			pow_bit = pow(2, 4);
			test.data[i * test.cols + j] += (Cpic.data[i * Cpic.cols + j] / 255) * pow_bit;
			pow_bit = pow(2, 3);
			test.data[i * test.cols + j] += (Apic.data[i * Apic.cols + j] / 255) * pow_bit;
			pow_bit = pow(2, 2);
			test.data[i * test.cols + j] += (1 - (Epic.data[i * Epic.cols + j] / 255)) * pow_bit;
			pow_bit = pow(2, 1);
			test.data[i * test.cols + j] += (1 - (Dpic.data[i * Dpic.cols + j] / 255)) * pow_bit;
			pow_bit = pow(2, 0);
			test.data[i * test.cols + j] += (1 - (Fpic.data[i * Fpic.cols + j] / 255)) * pow_bit;
		}
	}
	imshow("house", test);
	waitKey(0);
	destroyAllWindows();
	imwrite("Outpic/hw2.png", test);
}
void hw3() {
	int width = 512;
	int height = 512;
	int size = width * height;
	int i, j, k;
	char OriFile[] = "Inpic/livingroom_d512.raw";
	FILE* roomFile;
	roomFile = fopen(OriFile, "rb");
	if (roomFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* room = new unsigned char[size];
	fread(room, 1, size, roomFile);
	Mat roomMat(height, width, CV_8UC1, room);

	char OriFile2[] = "Inpic/cameraman_b512.raw";
	FILE* camera;
	camera = fopen(OriFile2, "rb");
	if (camera == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* man = new unsigned char[size];
	fread(man, 1, size, camera);
	Mat CameraMat(height, width, CV_8UC1, man);
	Mat  room100(height, width, CV_8UC1);
	Mat  room20(height, width, CV_8UC1);
	Mat  camera100(height, width, CV_8UC1);
	Mat  camera20(height, width, CV_8UC1);
	log_tf(roomMat, room100, 100);
	log_tf(CameraMat, camera100, 100);
	log_tf(roomMat, room20, 20);
	log_tf(CameraMat, camera20, 20);
	imshow("room100", room100);
	imshow("camera100", camera100);
	imshow("room20", room20);
	imshow("camera20", camera20);
	waitKey(0);
	destroyAllWindows();

	Mat  room0_2(height, width, CV_8UC1);
	Mat  room10(height, width, CV_8UC1);
	Mat  camera0_2(height, width, CV_8UC1);
	Mat  camera10(height, width, CV_8UC1);
	Mat  roomtest(height, width, CV_8UC1);
	Mat  roomtest2(height, width, CV_8UC1);
	Mat  cameratest(height, width, CV_8UC1);
	Mat  cameratest2(height, width, CV_8UC1);
	pow_law(roomMat, room0_2, 380, 0.2);
	pow_law(CameraMat, camera0_2, 165, 0.2);

	pow_law(roomMat, room10, 0.000000000004, 10);
	pow_law(CameraMat, camera10, 0.0000000000000000000002, 10);

	pow_law(roomMat, roomtest, 105, 0.2);
	pow_law(CameraMat, cameratest, 66, 0.2);

	pow_law(roomMat, roomtest2, 1, 10);
	pow_law(CameraMat, cameratest2, 1, 10);

	imshow("room0_2", room0_2);
	imshow("roomtest", roomtest);
	imshow("camera0_2", camera0_2);
	imshow("cameratest", cameratest);
	imshow("room10", room10);
	imshow("roomtest2", roomtest2);
	imshow("camera10", camera10);
	imshow("cameratest2", cameratest2);
	waitKey(0);
	destroyAllWindows();
	imwrite("Outpic/hw3_room_log100.png", room100);
	imwrite("Outpic/hw3_camera_log100.png", camera100);
	imwrite("Outpic/hw3_room_log20.png", room20);
	imwrite("Outpic/hw3_camera_log20.png", camera20);
	imwrite("Outpic/hw3_room_pow0_2.png", room0_2);
	imwrite("Outpic/hw3_camera_pow0_2.png", camera0_2);
	imwrite("Outpic/hw3_room_pow10.png", room10);
	imwrite("Outpic/hw3_camera_pow10.png", camera10);
	imwrite("Outpic/hw3_room_test.png", roomtest);
	imwrite("Outpic/hw3_camera_test.png", cameratest);
	imwrite("Outpic/hw3_room_test2.png", roomtest2);
	imwrite("Outpic/hw3_camera_test2.png", cameratest2);
}
void hw4() {
	int width = 512;
	int height = 512;
	int size = width * height;
	int i, j, k;
	char OriFile[] = "Inpic/livingroom_d512.raw";
	FILE* roomFile;
	roomFile = fopen(OriFile, "rb");
	if (roomFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* room = new unsigned char[size];
	fread(room, 1, size, roomFile);
	Mat roomMat(height, width, CV_8UC1, room);

	char OriFile2[] = "Inpic/cameraman_b512.raw";
	FILE* camera;
	camera = fopen(OriFile2, "rb");
	if (camera == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* man = new unsigned char[size];
	fread(man, 1, size, camera);
	double cnt_room[256] = { 0 }, cnt_camera[256] = { 0 };
	Mat CameraMat(height, width, CV_8UC1, man);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			cnt_room[roomMat.data[i * roomMat.cols + j]]++;
			cnt_camera[CameraMat.data[i * CameraMat.cols + j]]++;

		}
	}
	Mat hist1(256, 256, CV_8UC1);
	Mat hist2(256, 256, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			hist1.data[i * hist1.cols + j] = 0;
			hist2.data[i * hist2.cols + j] = 0;
		}
		cnt_room[i] /= height * width;
		cnt_camera[i] /= height * width;
	}
	for (int i = 0; i < 256; i++)
	{
		line(hist1, Point(i, 255), Point(i, 255 - (cnt_room[i] * 255)), Scalar(255));
		line(hist2, Point(i, 255), Point(i, 255 - (cnt_camera[i] * 255)), Scalar(255));
	}
	imshow("hist1", hist1);
	imshow("hist2", hist2);
	for (i = 1; i < 256; i++) {
		cnt_room[i] += cnt_room[i - 1];
		cnt_camera[i] += cnt_camera[i - 1];
	}
	Mat cdf1(256, 256, CV_8UC1);
	Mat cdf2(256, 256, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			cdf1.data[i * cdf1.cols + j] = 0;
			cdf2.data[i * cdf2.cols + j] = 0;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		line(cdf1, Point(i, 255), Point(i, 255 - cnt_room[i] * 255), Scalar(255));
		line(cdf2, Point(i, 255), Point(i, 255 - cnt_camera[i] * 255), Scalar(255));
	}
	imshow("cdf1", cdf1);
	imshow("cdf2", cdf2);
	double tmp[256] = { 0 };
	double tmp2[256] = { 0 };
	for (i = 0; i < 256; i++) {
		tmp[i] = cnt_room[i];
		tmp2[i] = cnt_camera[i];
	}
	Mat fix1(height, width, CV_8UC1);
	Mat fix2(height, width, CV_8UC1);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			for (k = 0; k < 256; k++) {
				if (k == roomMat.data[i * roomMat.cols + j]) {
					fix1.data[i * fix1.cols + j] = tmp[k] * 255;
				}
				if (k == CameraMat.data[i * CameraMat.cols + j]) {
					fix2.data[i * fix2.cols + j] = tmp2[k] * 255;
				}
			}
		}
	}
	imshow("fix1", fix1);
	imshow("fix2", fix2);
	for (i = 0; i < 256; i++) {
		cnt_room[i] = 0;
		cnt_camera[i] = 0;
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			cnt_room[fix1.data[i * fix1.cols + j]]++;
			cnt_camera[fix2.data[i * fix2.cols + j]]++;
		}
	}
	Mat hist_fix1(256, 256, CV_8UC1);
	Mat hist_fix2(256, 256, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			hist_fix1.data[i * hist_fix1.cols + j] = 0;
			hist_fix2.data[i * hist_fix2.cols + j] = 0;

		}
		cnt_room[i] /= height * width;
		cnt_camera[i] /= height * width;
	}
	for (int i = 0; i < 256; i++)
	{
		line(hist_fix1, Point(i, 255), Point(i, 255 - cnt_room[i] * 255), Scalar(255));
		line(hist_fix2, Point(i, 255), Point(i, 255 - cnt_camera[i] * 255), Scalar(255));
	}
	imshow("hist_fix1", hist_fix1);
	imshow("hist_fix2", hist_fix2);

	for (i = 1; i < 256; i++) {
		cnt_room[i] += cnt_room[i - 1];
		cnt_camera[i] += cnt_camera[i - 1];
	}
	Mat cdf_fix1(256, 256, CV_8UC1);
	Mat cdf_fix2(256, 256, CV_8UC1);
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			cdf_fix1.data[i * cdf_fix1.cols + j] = 0;
			cdf_fix2.data[i * cdf_fix2.cols + j] = 0;
		}
	}
	for (int i = 0; i < 256; i++) {
		line(cdf_fix1, Point(i, 255), Point(i, 255 - cnt_room[i] * 255), Scalar(255));
		line(cdf_fix2, Point(i, 255), Point(i, 255 - cnt_camera[i] * 255), Scalar(255));
	}
	imshow("cdf_fix1", cdf_fix1);
	imshow("cdf_fix2", cdf_fix2);
	waitKey(0);
	destroyAllWindows();
	imwrite("Outpic/hw4_hist1.png", hist1);
	imwrite("Outpic/hw4_hist2.png", hist2);
	imwrite("Outpic/hw4_cdf1.png", cdf1);
	imwrite("Outpic/hw4_cdf2.png", cdf2);
	imwrite("Outpic/hw4_fix1.png", fix1);
	imwrite("Outpic/hw4_fix2.png", fix2);
	imwrite("Outpic/hw4_hist_fix1.png", hist_fix1);
	imwrite("Outpic/hw4_hist_fix2.png", hist_fix2);
	imwrite("Outpic/hw4_cdf_fix1.png", cdf_fix1);
	imwrite("Outpic/hw4_cdf_fix2.png", cdf_fix2);

}
int main() {
	while (1) {
		printf("-----\nhw1\nhw2\nhw3\nhw4\nend\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		if (strcmp(s, "hw1") == 0) {
			hw1();
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
