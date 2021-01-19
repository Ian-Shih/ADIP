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
int cnt = 0;
int minstep = 999;
int mazecnt[20][20] = { 0 };
int best[20][20] = { 0 };
int qu_times = 0;
int map_times = 0;
std::ostringstream name;
void D4map_path(Mat InMat, int x, int y, int step, int p1, int p2 = 80, int p3 = 80) {
	//x,y:位置
	//step:步數
	//p1,p2,p3:可走的灰階值
	mazecnt[0][0] = 1;
	int i, j;
	//走到終點
	if (x == 19 && y == 19 || step > minstep) {
		if (step < minstep) {
			minstep = step;
			for (i = 0; i < 20; i++) {
				for (j = 0; j < 20; j++) {
					best[i][j] = mazecnt[i][j];
				}
			}
		}
	}
	else {
		if (y + 1 < 20 && !mazecnt[x][y + 1] && (InMat.at<uchar>(x, y + 1) == p1 || InMat.at<uchar>(x, y + 1) == p2 || InMat.at<uchar>(x, y + 1) == p3)) {
			mazecnt[x][y + 1] = 1;
			D4map_path(InMat, x, y + 1, step + 1, p1, p2, p3);
			mazecnt[x][y + 1] = 0;
		}
		if (x + 1 < 20 && !mazecnt[x + 1][y] && (InMat.at<uchar>(x + 1, y) == p1 || InMat.at<uchar>(x + 1, y) == p2 || InMat.at<uchar>(x + 1, y) == p3)) {
			mazecnt[x + 1][y] = 1;
			D4map_path(InMat, x + 1, y, step + 1, p1, p2, p3);
			mazecnt[x + 1][y] = 0;
		}
		if (y - 1 > 0 && !mazecnt[x][y - 1] && (InMat.at<uchar>(x, y - 1) == p1 || InMat.at<uchar>(x, y - 1) == p2 || InMat.at<uchar>(x, y - 1) == p3)) {
			mazecnt[x][y - 1] = 1;
			D4map_path(InMat, x, y - 1, step + 1, p1, p2, p3);
			mazecnt[x][y - 1] = 0;
		}
		if (x - 1 > 0 && !mazecnt[x - 1][y] && (InMat.at<uchar>(x - 1, y) == p1 || InMat.at<uchar>(x - 1, y) == p2 || InMat.at<uchar>(x - 1, y) == p3)) {
			mazecnt[x - 1][y] = 1;
			D4map_path(InMat, x - 1, y, step + 1, p1, p2, p3);
			mazecnt[x - 1][y] = 0;
		}
	}
}
void D8map_path(Mat InMat, int x, int y, int step, int p1, int p2 = 80, int p3 = 80) {
	//x,y:位置
	//step:步數
	//p1,p2,p3:可走的灰階值
	mazecnt[0][0] = 1;
	int i, j;
	//走到終點
	if (x == 19 && y == 19 || step > minstep) {
		if (step < minstep) {
			minstep = step;
			for (i = 0; i < 20; i++) {
				for (j = 0; j < 20; j++) {
					best[i][j] = mazecnt[i][j];
				}
			}
		}
	}
	else {
		if (x + 1 < 20 && y + 1 < 20 && !mazecnt[x + 1][y + 1] && (InMat.at<uchar>(x + 1, y + 1) == p1 || InMat.at<uchar>(x + 1, y + 1) == p2 || InMat.at<uchar>(x + 1, y + 1) == p3)) {
			mazecnt[x + 1][y + 1] = 1;
			D8map_path(InMat, x + 1, y + 1, step + 1, p1, p2, p3);
			mazecnt[x + 1][y + 1] = 0;
		}
		if (x + 1 < 20 && y - 1 > 0 && !mazecnt[x + 1][y - 1] && (InMat.at<uchar>(x + 1, y - 1) == p1 || InMat.at<uchar>(x + 1, y - 1) == p2 || InMat.at<uchar>(x + 1, y - 1) == p3)) {
			mazecnt[x + 1][y - 1] = 1;
			D8map_path(InMat, x + 1, y - 1, step + 1, p1, p2, p3);
			mazecnt[x + 1][y - 1] = 0;
		}
		if (x - 1 > 0 && y - 1 > 0 && !mazecnt[x - 1][y - 1] && (InMat.at<uchar>(x - 1, y - 1) == p1 || InMat.at<uchar>(x - 1, y - 1) == p2 || InMat.at<uchar>(x - 1, y - 1) == p3)) {
			mazecnt[x - 1][y - 1] = 1;
			D8map_path(InMat, x - 1, y - 1, step + 1, p1, p2, p3);
			mazecnt[x - 1][y - 1] = 0;
		}
		if (x - 1 > 0 && y + 1 < 20 && !mazecnt[x - 1][y + 1] && (InMat.at<uchar>(x - 1, y + 1) == p1 || InMat.at<uchar>(x - 1, y + 1) == p2 || InMat.at<uchar>(x - 1, y + 1) == p3)) {
			mazecnt[x - 1][y + 1] = 1;
			D8map_path(InMat, x - 1, y + 1, step + 1, p1, p2, p3);
			mazecnt[x - 1][y + 1] = 0;
		}
		if (y + 1 < 20 && !mazecnt[x][y + 1] && (InMat.at<uchar>(x, y + 1) == p1 || InMat.at<uchar>(x, y + 1) == p2 || InMat.at<uchar>(x, y + 1) == p3)) {
			mazecnt[x][y + 1] = 1;
			D8map_path(InMat, x, y + 1, step + 1, p1, p2, p3);
			mazecnt[x][y + 1] = 0;
		}
		if (x + 1 < 20 && !mazecnt[x + 1][y] && (InMat.at<uchar>(x + 1, y) == p1 || InMat.at<uchar>(x + 1, y) == p2 || InMat.at<uchar>(x + 1, y) == p3)) {
			mazecnt[x + 1][y] = 1;
			D8map_path(InMat, x + 1, y, step + 1, p1, p2, p3);
			mazecnt[x + 1][y] = 0;
		}
		if (y - 1 > 0 && !mazecnt[x][y - 1] && (InMat.at<uchar>(x, y - 1) == p1 || InMat.at<uchar>(x, y - 1) == p2 || InMat.at<uchar>(x, y - 1) == p3)) {
			mazecnt[x][y - 1] = 1;
			D8map_path(InMat, x, y - 1, step + 1, p1, p2, p3);
			mazecnt[x][y - 1] = 0;
		}
		if (x - 1 > 0 && !mazecnt[x - 1][y] && (InMat.at<uchar>(x - 1, y) == p1 || InMat.at<uchar>(x - 1, y) == p2 || InMat.at<uchar>(x - 1, y) == p3)) {
			mazecnt[x - 1][y] = 1;
			D8map_path(InMat, x - 1, y, step + 1, p1, p2, p3);
			mazecnt[x - 1][y] = 0;
		}
	}
}
void DMmap_path(Mat InMat, int x, int y, int step, int p1, int p2 = 80, int p3 = 80) {
	//x,y:位置
	//step:步數
	//p1,p2,p3:可走的灰階值
	mazecnt[0][0] = 1;
	int i, j;
	//走到終點
	if (x == 19 && y == 19 || step > minstep) {
		if (step < minstep) {
			minstep = step;
			for (i = 0; i < 20; i++) {
				for (j = 0; j < 20; j++) {
					best[i][j] = mazecnt[i][j];
				}
			}
		}
	}
	else {
		//須判斷其x,y分量皆無法走才能走斜邊
		if (!(y + 1 < 20 && !mazecnt[x][y + 1] && (InMat.at<uchar>(x, y + 1) == p1 || InMat.at<uchar>(x, y + 1) == p2 || InMat.at<uchar>(x, y + 1) == p3)) &&
			!(x + 1 < 20 && !mazecnt[x + 1][y] && (InMat.at<uchar>(x + 1, y) == p1 || InMat.at<uchar>(x + 1, y) == p2 || InMat.at<uchar>(x + 1, y) == p3))) {
			if (x + 1 < 20 && y + 1 < 20 && !mazecnt[x + 1][y + 1] && (InMat.at<uchar>(x + 1, y + 1) == p1 || InMat.at<uchar>(x + 1, y + 1) == p2 || InMat.at<uchar>(x + 1, y + 1) == p3)) {
				mazecnt[x + 1][y + 1] = 1;
				DMmap_path(InMat, x + 1, y + 1, step + 1, p1, p2, p3);
				mazecnt[x + 1][y + 1] = 0;
			}
		}
		if (!(y + 1 < 20 && !mazecnt[x][y + 1] && (InMat.at<uchar>(x, y + 1) == p1 || InMat.at<uchar>(x, y + 1) == p2 || InMat.at<uchar>(x, y + 1) == p3)) &&
			!(x - 1 > 0 && !mazecnt[x - 1][y] && (InMat.at<uchar>(x - 1, y) == p1 || InMat.at<uchar>(x - 1, y) == p2 || InMat.at<uchar>(x - 1, y) == p3))) {
			if (x - 1 > 0 && y + 1 < 20 && !mazecnt[x - 1][y + 1] && (InMat.at<uchar>(x - 1, y + 1) == p1 || InMat.at<uchar>(x - 1, y + 1) == p2 || InMat.at<uchar>(x - 1, y + 1) == p3)) {
				mazecnt[x - 1][y + 1] = 1;
				DMmap_path(InMat, x - 1, y + 1, step + 1, p1, p2, p3);
				mazecnt[x - 1][y + 1] = 0;
			}
		}
		if (!(y - 1 > 0 && !mazecnt[x][y - 1] && (InMat.at<uchar>(x, y - 1) == p1 || InMat.at<uchar>(x, y - 1) == p2 || InMat.at<uchar>(x, y - 1) == p3)) &&
			!(x + 1 < 20 && !mazecnt[x + 1][y] && (InMat.at<uchar>(x + 1, y) == p1 || InMat.at<uchar>(x + 1, y) == p2 || InMat.at<uchar>(x + 1, y) == p3))) {
			if (x + 1 < 20 && y - 1 > 0 && !mazecnt[x + 1][y - 1] && (InMat.at<uchar>(x + 1, y - 1) == p1 || InMat.at<uchar>(x + 1, y - 1) == p2 || InMat.at<uchar>(x + 1, y - 1) == p3)) {
				mazecnt[x + 1][y - 1] = 1;
				DMmap_path(InMat, x + 1, y - 1, step + 1, p1, p2, p3);
				mazecnt[x + 1][y - 1] = 0;
			}
		}
		if (!(y - 1 > 0 && !mazecnt[x][y - 1] && (InMat.at<uchar>(x, y - 1) == p1 || InMat.at<uchar>(x, y - 1) == p2 || InMat.at<uchar>(x, y - 1) == p3)) &&
			!(x - 1 > 0 && !mazecnt[x - 1][y] && (InMat.at<uchar>(x - 1, y) == p1 || InMat.at<uchar>(x - 1, y) == p2 || InMat.at<uchar>(x - 1, y) == p3))) {
			if (x - 1 > 0 && y - 1 > 0 && !mazecnt[x - 1][y - 1] && (InMat.at<uchar>(x - 1, y - 1) == p1 || InMat.at<uchar>(x - 1, y - 1) == p2 || InMat.at<uchar>(x - 1, y - 1) == p3)) {
				mazecnt[x - 1][y - 1] = 1;
				DMmap_path(InMat, x - 1, y - 1, step + 1, p1, p2, p3);
				mazecnt[x - 1][y - 1] = 0;
			}
		}
		if (y + 1 < 20 && !mazecnt[x][y + 1] && (InMat.at<uchar>(x, y + 1) == p1 || InMat.at<uchar>(x, y + 1) == p2 || InMat.at<uchar>(x, y + 1) == p3)) {
			mazecnt[x][y + 1] = 1;
			DMmap_path(InMat, x, y + 1, step + 1, p1, p2, p3);
			mazecnt[x][y + 1] = 0;
		}
		if (x + 1 < 20 && !mazecnt[x + 1][y] && (InMat.at<uchar>(x + 1, y) == p1 || InMat.at<uchar>(x + 1, y) == p2 || InMat.at<uchar>(x + 1, y) == p3)) {
			mazecnt[x + 1][y] = 1;
			DMmap_path(InMat, x + 1, y, step + 1, p1, p2, p3);
			mazecnt[x + 1][y] = 0;
		}

		if (y - 1 > 0 && !mazecnt[x][y - 1] && (InMat.at<uchar>(x, y - 1) == p1 || InMat.at<uchar>(x, y - 1) == p2 || InMat.at<uchar>(x, y - 1) == p3)) {
			mazecnt[x][y - 1] = 1;
			DMmap_path(InMat, x, y - 1, step + 1, p1, p2, p3);
			mazecnt[x][y - 1] = 0;
		}
		if (x - 1 > 0 && !mazecnt[x - 1][y] && (InMat.at<uchar>(x - 1, y) == p1 || InMat.at<uchar>(x - 1, y) == p2 || InMat.at<uchar>(x - 1, y) == p3)) {
			mazecnt[x - 1][y] = 1;
			DMmap_path(InMat, x - 1, y, step + 1, p1, p2, p3);
			mazecnt[x - 1][y] = 0;
		}
	}
}
float W(float x)
{	//Bicubic函數
	float a = -0.5;
	if (x < 0) {
		x *= (-1);
	}
	if (x <= 1.0)
	{
		return (a + 2) * pow(x, 3) - (a + 3) * pow(x, 2) + 1;
	}
	else if (x < 2.0)
	{
		return a * pow(x, 3) - 5 * a * pow(x, 2) + 8 * a * x - 4 * a;
	}
	else
		return 0;
}
int quantize(Mat InMat, Mat* OutMat) {
	int size = InMat.rows * InMat.cols;
	int i, j, k;
	double mse[8], psnr[8];
	int t1, t2, t3;
	for (i = 0; i < 8; i++) {
		OutMat[i] = Mat(InMat.rows, InMat.cols, CV_8UC1);
		mse[i] = 0;
		psnr[i] = 0;
	}
	for (k = 0; k < 8; k++) {
		mse[k] = 0;
		t1 = pow(2, 8 - k - 1);	//閥值
		t2 = pow(2, 8) - 1;	//255
		t3 = pow(2, k + 1) - 1;	//總共幾類(從0開始所以-1)
		for (i = 0; i < InMat.rows; i++) {
			for (j = 0; j < InMat.cols; j++) {
				OutMat[k].data[i * InMat.cols + j] = InMat.data[i * InMat.cols + j] / t1 * t2 / t3;
				mse[k] += pow((OutMat[k].data[i * InMat.cols + j] - InMat.data[i * InMat.cols + j]), 2);
			}
		}
		//計算mse&psnr
		mse[k] /= (double)size;
		psnr[k] = 10 * log10(pow(255, 2) / mse[k]);
		printf("\n%d-bit mse: %f\n", k + 1, mse[k]);
		printf("%d-bit psnr: %f\n", k + 1, psnr[k]);
		imshow("", OutMat[k]);
		qu_times++;
		imwrite(format("Outpic/qu%d.png", qu_times), OutMat[k]);
		waitKey(0);
		destroyAllWindows();
	}
	return 0;
}
Mat bilinear(Mat InMat, double Mag) {
	Mat tmp((int)(InMat.rows * Mag), (int)(InMat.cols * Mag), CV_8UC1);
	int i, j, m, n;
	clock_t   time_start, time_end;
	time_start = clock();
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			tmp.data[(int)(i * Mag) * tmp.cols + (int)(j * Mag)] = InMat.data[i * InMat.cols + j];
		}
	}
	//雙線性插值公式
	for (i = 0; i < InMat.rows - 1; i++) {
		for (j = 0; j < InMat.cols - 1; j++) {
			int x1, x2, y1, y2, F11, F12, F21, F22;
			x1 = i * Mag;
			y1 = j * Mag;
			x2 = (i + 1) * Mag;
			y2 = (j + 1) * Mag;
			F11 = tmp.data[x1 * tmp.cols + y1];
			F12 = tmp.data[x1 * tmp.cols + y2];
			F21 = tmp.data[x2 * tmp.cols + y1];
			F22 = tmp.data[x2 * tmp.cols + y2];
			for (m = i * Mag; m <= (i + 1) * Mag; m++) {
				for (n = j * Mag; n <= (j + 1) * Mag; n++) {
					tmp.data[m * tmp.cols + n] = (F11 * (x2 - m) * (y2 - n) +
						F21 * (m - x1) * (y2 - n) +
						F12 * (x2 - m) * (n - y1) +
						F22 * (m - x1) * (n - y1)) / ((x2 - x1) * (y2 - y1));
				}
			}
		}
	}
	time_end = clock();
	printf("bi:%lf\n", (time_end - time_start) / (double)(CLOCKS_PER_SEC));
	return tmp;
}
Mat nearest_neighbor(Mat InMat, double Mag) {
	Mat tmp(InMat.rows * Mag, InMat.cols * Mag, CV_8UC1);
	int i, j, m, n;
	clock_t   time_start, time_end;
	time_start = clock();
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			tmp.data[(int)(i * Mag) * tmp.cols + (int)(j * Mag)] = InMat.data[i * InMat.cols + j];
		}
	}
	for (i = 0; i < InMat.rows; i++) {
		for (j = 0; j < InMat.cols; j++) {
			int x[4];
			x[0] = i * Mag;
			x[1] = j * Mag;
			x[2] = (i + 1) * Mag;
			x[3] = (j + 1) * Mag;
			for (m = x[0]; m < x[2]; m++) {
				for (n = x[1]; n < x[3]; n++) {
					double dis[4];
					dis[0] = pow((m - x[0]), 2) + pow((n - x[1]), 2);
					dis[1] = pow((m - x[0]), 2) + pow((x[3] - n), 2);
					dis[2] = pow((x[2] - m), 2) + pow((n - x[1]), 2);
					dis[3] = pow((x[2] - m), 2) + pow((x[3] - n), 2);
					if (m > (InMat.rows - 1) * Mag) {
						dis[2] = INFINITY;
						dis[3] = INFINITY;
					}
					if (n > (InMat.cols - 1) * Mag) {
						dis[1] = INFINITY;
						dis[3] = INFINITY;
					}
					//找出附近最短距離點的灰階值
					if (dis[0] <= dis[1] && dis[0] <= dis[2] && dis[0] <= dis[3]) {
						tmp.data[m * tmp.cols + n] = tmp.data[x[0] * tmp.cols + x[1]];
					}
					else if (dis[1] <= dis[0] && dis[1] <= dis[2] && dis[1] <= dis[3]) {
						tmp.data[m * tmp.cols + n] = tmp.data[x[0] * tmp.cols + x[3]];
					}
					else if (dis[2] <= dis[1] && dis[2] <= dis[0] && dis[2] <= dis[3]) {
						tmp.data[m * tmp.cols + n] = tmp.data[x[2] * tmp.cols + x[1]];
					}
					else if (dis[3] <= dis[1] && dis[3] <= dis[2] && dis[3] <= dis[0]) {
						tmp.data[m * tmp.cols + n] = tmp.data[x[2] * tmp.cols + x[3]];
					}
				}
			}
		}
	}
	time_end = clock();
	printf("nn:%lf\n", (time_end - time_start) / (double)(CLOCKS_PER_SEC));
	return tmp;
}
Mat bicubic(Mat InMat, double Mag) {
	Mat tmp(InMat.rows * Mag, InMat.cols * Mag, CV_8UC1);
	int i, j, m, n;
	clock_t   time_start, time_end;
	time_start = clock();
	int xx[4], yy[4];
	double x, y, u, v;
	//Bicubic公式
	for (i = 0; i < InMat.rows * Mag; i++) {
		for (j = 0; j < InMat.cols * Mag; j++) {
			x = i / Mag;
			y = j / Mag;
			for (m = 0; m < 4; m++) {
				xx[m] = int(x) + m - 1;
				yy[m] = int(y) + m - 1;
			}
			float dist_x[4], dist_y[4], dist[4][4];
			if ((xx[0] >= 0) && (xx[3] < InMat.rows) && (yy[0] >= 0) && (yy[3] < InMat.cols)) {
				for (m = 0; m < 4; m++) {
					dist_x[m] = W(x - xx[m]);
					dist_y[m] = W(y - yy[m]);
				}
				int sum = 0;
				for (m = 0; m < 4; m++) {
					for (n = 0; n < 4; n++) {
						dist[m][n] = dist_x[m] * dist_y[n];
						sum += InMat.data[xx[m] * InMat.cols + yy[n]] * dist[m][n];
					}
				}
				tmp.data[i * tmp.cols + j] = sum;
			}
		}
	}
	time_end = clock();
	printf("cu:%lf\n", (time_end - time_start) / (double)(CLOCKS_PER_SEC));
	return tmp;
}
Mat shrinking(Mat InMat) {
	Mat tmp(InMat.rows / 2, InMat.cols / 2, CV_8UC1);
	int m, n;
	//縮小
	for (m = 0; m < InMat.rows; m += 2) {
		for (n = 0; n < InMat.cols; n += 2) {
			tmp.data[m / 2 * tmp.cols + n / 2] = InMat.data[m * InMat.cols + n] / 4 +
				InMat.data[(m + 1) * InMat.cols + n] / 4 +
				InMat.data[m * InMat.cols + n + 1] / 4 +
				InMat.data[(m + 1) * InMat.cols + n + 1] / 4;
		}
	}
	cnt++;
	return tmp;
}
int plotandcal(Mat InMat, int mazecnt[20][20]) {
	//繪製路線圖
	int tmp = 0;
	int i, j;
	Mat stepmap(20, 20, CV_8UC1);

	for (i = 0; i < 20; i++) {
		for (j = 0; j < 20; j++) {
			printf("%4d", InMat.at<uchar>(i, j));
		}
		printf("\n");
	}
	printf("\n");
	for (i = 0; i < 20; i++) {
		for (j = 0; j < 20; j++) {
			printf("%4d", mazecnt[i][j]);
			stepmap.at<uchar>(i, j) = mazecnt[i][j] * 255;
		}
		printf("\n");
	}
	namedWindow("stepmap", 0);
	resizeWindow("stepmap", 256, 256);
	imshow("stepmap", stepmap);
	map_times++;
	imwrite(format("Outpic/map%d.png", map_times), stepmap);
	waitKey(0);
	destroyAllWindows();

	return 0;
}
int hw2_1_1and2() {
	int width = 256;
	int height = 256;
	int size = width * height;
	int i, j;
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
	Mat tmp(height, width, CV_8UC1);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			tmp.data[i * tmp.cols + j] = lenaMat.data[i * lenaMat.cols + j];
		}
	}
	Mat OutMat(10, 50, CV_8UC1);
	//切出眼睛部分
	for (i = 130; i < 140; i++) {
		for (j = 125; j < 175; j++) {
			OutMat.data[(i - 130) * OutMat.cols + j - 125] = lenaMat.data[i * lenaMat.cols + j];
		}
	}
	Mat OutMat2(20, 100, CV_8UC1);
	//放大
	for (i = 130; i < 140; i++) {
		for (j = 125; j < 175; j++) {
			OutMat2.data[(i - 130) * 2 * OutMat2.cols + (j - 125) * 2] = OutMat.data[(i - 130) * OutMat.cols + j - 125];
			OutMat2.data[(i - 130) * 2 * OutMat2.cols + (j - 125) * 2 + 1] = OutMat.data[(i - 130) * OutMat.cols + j - 125];
			OutMat2.data[((i - 130) * 2 + 1) * OutMat2.cols + (j - 125) * 2] = OutMat.data[(i - 130) * OutMat.cols + j - 125];
			OutMat2.data[((i - 130) * 2 + 1) * OutMat2.cols + (j - 125) * 2 + 1] = OutMat.data[(i - 130) * OutMat.cols + j - 125];
		}
	}
	//貼回原圖
	for (i = 125; i < 145; i++) {
		for (j = 100; j < 200; j++) {
			tmp.data[i * tmp.cols + j] = OutMat2.data[(i - 125) * OutMat2.cols + j - 100];
		}
	}
	namedWindow("hw2_1_1", 0);
	resizeWindow("hw2_1_1", 256, 256);
	moveWindow("hw2_1_1", 100, 100);
	imshow("hw2_1_1", tmp);
	imwrite("Outpic/hw2_1_1.png", tmp);
	waitKey(0);

	Mat	test1(128, 128, CV_8UC1);
	test1 = shrinking(tmp);
	namedWindow("shrinking", 0);
	resizeWindow("shrinking", 128, 128);
	moveWindow("shrinking", 100, 100);
	imshow("shrinking", test1);
	imwrite("Outpic/hw2_1_2_shrinking.png", test1);

	Mat	test2(128, 128, CV_8UC1);
	blur(tmp, tmp, Size(5, 5));
	test2 = shrinking(tmp);
	namedWindow("blur", 0);
	resizeWindow("blur", 128, 128);
	moveWindow("blur", 100, 300);
	imshow("blur", test2);
	imwrite("Outpic/hw2_1_2_blur.png", test2);
	waitKey(0);

	destroyAllWindows();
	return 0;
}
int hw2_1_3() {
	double width = 256;
	double height = 256;
	double Mag = 2.5;
	double size = width * height;
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
	Mat nn2_5, bi2_5, nn2_5div2, bi2_5div2, nn1_25, bi1_25, cu2_5, cu2_5div2, cu1_25;
	Mat nndiv2_mul2_5, bidiv2_mul2_5, cudiv2_mul2_5, Indiv2;

	nn2_5 = nearest_neighbor(lenaMat, Mag);
	nn2_5div2 = shrinking(nn2_5);
	imshow("nn2_5div2", nn2_5div2);
	imwrite("Outpic/nn2_5div2.png", nn2_5div2);
	bi2_5 = bilinear(lenaMat, Mag);
	bi2_5div2 = shrinking(bi2_5);
	imshow("bi2_5div2", bi2_5div2);
	imwrite("Outpic/bi2_5div2.png", bi2_5div2);
	cu2_5 = bicubic(lenaMat, Mag);
	cu2_5div2 = shrinking(cu2_5);
	imshow("cu2_5div2", cu2_5div2);
	imwrite("Outpic/cu2_5div2.png", cu2_5div2);
	waitKey(0);

	Indiv2 = shrinking(lenaMat);
	nndiv2_mul2_5 = nearest_neighbor(Indiv2, Mag);
	imshow("nndiv2_mul2_5", nndiv2_mul2_5);
	imwrite("Outpic/nndiv2_mul2_5.png", nndiv2_mul2_5);
	bidiv2_mul2_5 = bilinear(Indiv2, Mag);
	imshow("bidiv2_mul2_5", bidiv2_mul2_5);
	imwrite("Outpic/bidiv2_mul2_5.png", bidiv2_mul2_5);
	cudiv2_mul2_5 = bicubic(Indiv2, Mag);
	imshow("cudiv2_mul2_5", cudiv2_mul2_5);
	imwrite("Outpic/cudiv2_mul2_5.png", cudiv2_mul2_5);
	waitKey(0);


	nn1_25 = nearest_neighbor(lenaMat, 1.25);
	imshow("nn1_25", nn1_25);
	imwrite("Outpic/nn1_25.png", nn1_25);
	bi1_25 = bilinear(lenaMat, 1.25);
	imshow("bi1_25", bi1_25);
	imwrite("Outpic/bi1_25.png", bi1_25);
	cu1_25 = bicubic(lenaMat, 1.25);
	imshow("cu1_25", cu1_25);
	imwrite("Outpic/cu1_25.png", cu1_25);
	waitKey(0);

	destroyAllWindows();
	return 0;


}
int	hw2_2() {
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
	Mat lena_bp[8], baboon_bp[8];
	quantize(lenaMat, lena_bp);
	fclose(lenaFile);
	delete(lenai);
	char OriFile2[] = "Inpic/baboon_256.raw";
	FILE* baboonFile;
	baboonFile = fopen(OriFile2, "rb");
	if (baboonFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* baboon = new unsigned char[size];
	fread(baboon, 1, size, baboonFile);
	Mat baboonMat(height, width, CV_8UC1, baboon);
	quantize(baboonMat, baboon_bp);
	fclose(baboonFile);
	delete(baboon);
	return 0;
}
int hw2_3() {
	int width = 256;
	int height = 256;
	int size = width * height;
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
	Mat	test1(256, 256, CV_8UC1);
	Mat	stest1(128, 128, CV_8UC1);
	stest1 = shrinking(lenaMat);
	namedWindow("stest1", 0);
	resizeWindow("stest1", 128, 128);
	imshow("stest1", stest1);
	imwrite("Outpic/hw2_3_lena.png", stest1);
	char OriFile2[] = "Inpic/baboon_256.raw";
	FILE* baboonFile;
	baboonFile = fopen(OriFile2, "rb");
	if (baboonFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* baboon = new unsigned char[size];
	fread(baboon, 1, size, baboonFile);
	Mat baboonMat(height, width, CV_8UC1, baboon);
	Mat	stest2(128, 128, CV_8UC1);
	stest2 = shrinking(baboonMat);
	namedWindow("stest2", 0);
	resizeWindow("stest2", 128, 128);
	imshow("stest2", stest2);
	imwrite("Outpic/hw2_3_baboon.png", stest2);
	waitKey(0);
	destroyAllWindows();
	Mat lena_bp[8], baboon_bp[8];
	quantize(stest1, lena_bp);
	quantize(stest2, baboon_bp);
	return 0;
}
int hw2_5() {
	int width = 20;
	int height = 20;
	int size = width * height;
	//-----------------------Read File-----------------------//
	char OriFile[] = "Inpic/map.raw";
	FILE* lenaFile;
	lenaFile = fopen(OriFile, "rb");
	if (lenaFile == NULL) {
		puts("Loading File Error!");
		system("PAUSE");
		exit(0);
	}
	unsigned char* mapi = new unsigned char[size];
	fread(mapi, 1, size, lenaFile);
	Mat mapMat(height, width, CV_8UC1, mapi);
	int i, j, si, sj, step;
	while (1) {
		printf("-----\nD4{80}\nD4{80,160}\nD4{80,160,255}\nD8{80}\nD8{80,160}\nD8{80,160,255}\nDm{80}\nDm{80,160}\nDm{80,160,255}\nend\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		step = 0;
		si = 0;
		sj = 0;
		minstep = 999;
		for (i = 0; i < 20; i++) {
			for (j = 0; j < 20; j++) {
				mazecnt[i][j] = 0;
				best[i][j] = 0;
			}
		}
		if (strcmp(s, "D4{80}") == 0) {
			printf("\nD4{80}\n");
			D4map_path(mapMat, si, sj, step, 80);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "D4{80,160}") == 0) {
			printf("\nD4{80,160}\n");
			D4map_path(mapMat, si, sj, step, 80, 160);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "D4{80,160,255}") == 0) {
			printf("\nD4{80,160,255}\n");
			D4map_path(mapMat, si, sj, step, 80, 160, 255);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "D8{80}") == 0) {
			printf("\nD8{80}\n");
			D8map_path(mapMat, si, sj, step, 80);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "D8{80,160}") == 0) {
			printf("\nD8{80,160}\n");
			D8map_path(mapMat, si, sj, step, 80, 160);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "D8{80,160,255}") == 0) {
			printf("\nD8{80,160,255}\n");
			D8map_path(mapMat, si, sj, step, 80, 160, 255);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "Dm{80}") == 0) {
			printf("\nDm{80}\n");
			DMmap_path(mapMat, si, sj, step, 80);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "Dm{80,160}") == 0) {
			printf("\nDm{80,160}\n");
			DMmap_path(mapMat, si, sj, step, 80, 160);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
		}
		else if (strcmp(s, "Dm{80,160,255}") == 0) {
			printf("\nDm{80,160,255}\n");
			DMmap_path(mapMat, si, sj, step, 80, 160, 255);
			plotandcal(mapMat, best);
			printf("\nminstep=%d\n", minstep);
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
int main()
{
	while (1) {
		printf("-----\n hw2_1_1and2\n hw2_1_3\n hw2_2\n hw2_3\n hw2_5\n end\n-----\n");
		printf("Enter the question number to select output result\n");
		char s[20];
		scanf("%s", s);
		printf("\n");
		if (strcmp(s, "hw2_1_1and2") == 0) {
			hw2_1_1and2();
		}
		else if (strcmp(s, "hw2_1_3") == 0) {
			hw2_1_3();
		}
		else if (strcmp(s, "hw2_2") == 0) {
			hw2_2();
		}
		else if (strcmp(s, "hw2_3") == 0) {
			hw2_3();
		}
		else if (strcmp(s, "hw2_5") == 0) {
			hw2_5();
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