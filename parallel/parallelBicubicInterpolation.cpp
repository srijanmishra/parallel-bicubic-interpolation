#include <iostream>
#include <mpi.h>
#define cimg_use_jpeg
#include "CImg.h"
using namespace std;
using namespace cimg_library;

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

void zoom (CImg<double> img, CImg<double> *z_img, int f, int tid, int NCPUs) {
	int i, j, k, l, c;
	double arr[4][4];
	double color[3];

	int loop_min    =(int)((long)(tid + 0) * (long)((*z_img).width())/(long)NCPUs);
	int loop_max    =(int)((long)(tid + 1) * (long)((*z_img).width())/(long)NCPUs);

	for(i=loop_min; i<loop_max; i++) {
		for(j=0; j<(*z_img).height(); j++) {
			//For R,G,B
			for(c=0; c<3; c++) {
				for(l=0; l<4; l++)
					for(k=0; k<4; k++)
						arr[l][k] = img(i/f +l, j/f +k, 0, c);		
				color[c] = bicubicInterpolate(arr, (double)(i%f)/f, (double)(j%f)/f);
			}
			(*z_img).draw_point(i,j,color);
		}
	}
}

int main(int argc, char *argv[])
{
	int factor = 3;
	CImg<double> img("phi.jpg");
	CImg<double> z_img( factor*img.width()-factor+1, factor*img.height()-factor+1, 1, 3, 0 );

	int tid, nthreads;
	double timeInitial, timeCurrent, timeTotal;

 	MPI_Init(&argc, &argv);
	timeInitial  = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &tid);
	MPI_Comm_size(MPI_COMM_WORLD, &nthreads);

	img.append(img.get_column(img.width()-1), 'x', 0);
	img.append(img.get_row(img.height()-1), 'y', 0);
	img = img.get_column(0).append(img, 'x', 0);
	img = img.get_row(0).append(img, 'y', 0);

	zoom(img, &z_img, factor, tid, nthreads);

	timeCurrent  = MPI_Wtime();
	timeTotal  = timeCurrent - timeInitial;
	cout<<timeTotal<<endl;

	CImgDisplay draw_disp1(img,"Original Image");
	CImgDisplay draw_disp(z_img,"Zoomed Image");
	while(!draw_disp.is_closed())
		draw_disp.wait();

	MPI_Finalize();

	return 0;
}
