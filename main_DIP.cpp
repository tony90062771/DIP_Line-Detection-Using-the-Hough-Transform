#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bmp.h"
#define PI 3.141592

int R[MaxBMPSizeX][MaxBMPSizeY];
int r[MaxBMPSizeX][MaxBMPSizeY];

int houghLines[MaxBMPSizeX][MaxBMPSizeY];

void sobelFilter(const int input[][MaxBMPSizeY], int output[][MaxBMPSizeY], int width, int height) {
    int sobelX[3][3] = {
        {-1, 0, 1}, 
        {-2, 0, 2}, 
        {-1, 0, 1}};
    int sobely[3][3] = {
        {-1, -2, -1}, 
        {0, 0, 0}, 
        {1, 2, 1}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0;
            int sumY = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sumX += input[x + kx][y + ky] * sobelX[ky + 1][kx + 1];
                    sumY += input[x + kx][y + ky] * sobely[ky + 1][kx + 1];
                }
            }
            output[x][y] = (int)(sqrt(sumX * sumX + sumY * sumY));
            if (output[x][y] > 255) {
                output[x][y] = 255;
            } else if (output[x][y] < 0) {
                output[x][y] = 0;
            }
        }
    }
}

void hough(int input[MaxBMPSizeX][MaxBMPSizeY], int iR[MaxBMPSizeX][MaxBMPSizeY], int lines[MaxBMPSizeX][MaxBMPSizeY], int width, int height, int vote_threshold) {
    int max_rho = width * 1.41;
    int max_theta = 180;
    int vote[max_rho][max_theta];
    memset(vote, 0, sizeof(vote));
    
    // Initialize lines image to black
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lines[x][y] = 0;
        }
    }
    
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(input[x][y] > 200) {
                for(int theta = 0; theta < max_theta; theta++) {
                    float radian = theta * PI / 180;
                    int rho = x * cos(radian) + y * sin(radian);
                    if(rho >= 0 && rho < max_rho) {
                        vote[rho][theta]++;
                    }
                }
            }
        }
    }
    
    int count[width][height];
    memset(count, 0, sizeof(count));	// 初始化計數陣列
    for(int rho = 0; rho < max_rho; rho++) {
        for(int theta = 0; theta < max_theta; theta++) {
            if(vote[rho][theta] > vote_threshold) {
                float radian = theta * PI / 180;
                for(int x = 0; x < width; x++) {
                    int y = (rho - x * cos(radian)) / sin(radian);
                    if(y >= 0 && y < height && input[x][y] >= 200 && count[x][y] < 10) {
                        iR[x][y] = 255;
                        lines[x][y] = 255;
                        count[x][y]++;
                    }
                }
                for(int y = 0; y < height; y++) {
                    int x = (rho - y * sin(radian)) / cos(radian);
                    if(x >= 0 && x < width && input[x][y] >= 200 && count[x][y] < 10) {
                        iR[x][y] = 255;
                        lines[x][y] = 255;
                        count[x][y]++;
                    }
                }
            }
        }
    }
}

int main() {
    int width, height;
    
    open_bmp("cameraman.bmp", R, R, R, width, height); 
    sobelFilter(R, r, width, height);
    save_bmp("cameraman_sobel.bmp", r, r, r); 

    int vote_threshold = 120; // 投票閾值
    

    hough(r, R, houghLines, width, height, vote_threshold);
    
    save_bmp("cameraman_hough.bmp", R, R, R);
    save_bmp("cameraman_hough_lines.bmp", houghLines, houghLines, houghLines); // 只包含霍夫直線的圖片
    close_bmp();
    
    printf("Job Finished!\n");
    return 0;
}

