
#import "xDistanceTransform.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

template <class T>
inline T abs(const T &x) { return (x > 0 ? x : -x); };

template <class T>
inline int sign(const T &x) { return (x >= 0 ? 1 : -1); };

template <class T>
inline T square(const T &x) { return x*x; };

template <class T>
inline T bound(const T &x, const T &min, const T &max) {
	return (x < min ? min : (x > max ? max : x));
}

template <class T>
inline bool check_bound(const T &x, const T&min, const T &max) {
	return ((x < min) || (x > max));
}


xDistanceTransform::xDistanceTransform(void)
{
}

xDistanceTransform::~xDistanceTransform(void)
{
}

int	  xDistanceTransform::IDX(int x, int y)
{
	return x*m_iHeight + y;
}


float * xDistanceTransform::dt(float *f, int n) {
	float *d = new float[n];
	int *v = new int[n];
	float *z = new float[n+1];
	int k = 0;
	v[0] = 0;
	z[0] = -INF;
	z[1] = +INF;
	for (int q = 1; q <= n-1; q++) {
		float s  = ((f[q]+square(q))-(f[v[k]]+square(v[k])))/(2*q-2*v[k]);
		while (s <= z[k]) {
			k--;
			s  = ((f[q]+square(q))-(f[v[k]]+square(v[k])))/(2*q-2*v[k]);
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = +INF;
	}

	k = 0;
	for (int q = 0; q <= n-1; q++) {
		while (z[k+1] < q)
			k++;
		d[q] = square(q-v[k]) + f[v[k]];
	}

	delete [] v;
	delete [] z;
	return d;
}

/* dt of 2d function using squared distance */
void xDistanceTransform::dt(float *im, int width, int height) 
{

	float *f = new float[max(width,height)];

	// transform along columns
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			f[y] = im[IDX(x, y)];
		}
		float *d = dt(f, height);
		for (int y = 0; y < height; y++) {
			im[IDX(x, y)] = d[y];
		}
		delete [] d;
	}

	// transform along rows
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			f[x] = im[IDX(x, y)];
		}
		float *d = dt(f, width);
		for (int x = 0; x < width; x++) {
			im[IDX(x, y)] = d[x];
		}
		delete [] d;
	}

	delete [] f;
}


/* dt of binary image using squared distance */
IplImage * xDistanceTransform::GetDT(IplImage *im, Byte on)
{
	int width = im->width;
	int height = im->height;

	IplImage *pResult = cvCreateImage(cvGetSize(im), IPL_DEPTH_8U,1);

	m_iWidth = width;
	m_iHeight = height;
	
	float *out = new float[width*height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int avg = (Byte)im->imageData[(y*im->widthStep)+x];

			if (avg==0)
				out[IDX(x, y)] = 0;
			else
				out[IDX(x, y)] = INF;
		}
	}

	dt(out, width, height);
	
	int iDist;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			iDist =	sqrt(out[IDX(x,y)])	;
			if(iDist > 255){
				iDist = 255;
			}
			pResult->imageData[(y*pResult->widthStep)+x] =(Byte)iDist;
		}
	}
	
	delete [] out;

	return pResult;
}
