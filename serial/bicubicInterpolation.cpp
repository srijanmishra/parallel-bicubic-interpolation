#include <iostream>
using namespace std;

double cubicInterpolate (double p[4], double x) {
	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

double bicubicInterpolate (double p[4][4], double x, double y) {
	double arr[4];
	arr[0] = cubicInterpolate(p[0], y);
	arr[1] = cubicInterpolate(p[1], y);
	arr[2] = cubicInterpolate(p[2], y);
	arr[3] = cubicInterpolate(p[3], y);
	return cubicInterpolate(arr, x);
}

void zoom (double img[5][5], double newImg[9][9], int width, int height, int f) {
	int i, j, k, l;
	double arr[4][4];
	for(i=0; i<f*width; i++) {
		for(j=0; j<f*height; j++) {
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
					arr[l][k] = img[i/f +l][j/f +k];               
			newImg[i][j] = bicubicInterpolate(arr, (double)(i%f)/f, (double)(j%f)/f);
		}
	}
}

int main () {
	double img[5][5] = { {0, 0, 80, 150, 150}, {0, 0, 80, 150, 150}, {80, 80, 120, 180, 180}, 
			     {150, 150, 180, 250, 250}, {150, 150, 180, 250, 250} };
	double newImg[9][9];

	int i,j;
	zoom(img, newImg, 3, 3, 3);

	for(i=0; i<7; i++) {
		for(j=0; j<7; j++)
			cout<<newImg[i][j];
		cout<<"\n";
	}
	return 0;
}