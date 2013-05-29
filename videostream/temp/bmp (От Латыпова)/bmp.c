#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

void write_bmp(const char *fname, int w,int h,unsigned char *img) {
	FILE *f = fopen(fname,"wb");
	unsigned char bfh[54] = {0x42, 0x4d,
	/* bfSize [2]*/ 54, 0, 0, 0, /**/
	/* reserved [6]*/ 0, 0, 0, 0, /**/
	/* biOffBits [10]*/ 54, 0, 0, 0, /**/
	/* biSize [14]*/ 40, 0, 0, 0, /**/
	/* width [18]*/ 0, 0, 0, 0, /**/
	/* height [22]*/ 0, 0, 0, 0, /**/
	/* planes [26]*/ 1, 0, /**/
	/* bitcount [28]*/ 24, 0,/**/
	/* compression [30]*/ 0, 0, 0, 0, /**/
	/* size image [34]*/ 0, 0, 0, 0, /**/
	/* xpermeter [38]*/ 0, 0, 0, 0, /**/
	/* ypermeter [42]*/ 0, 0, 0, 0, /**/
	/* clrused [46]*/ 0, 0, 0, 0, /**/
	/* clrimportant [50]*/ 0, 0, 0, 0 /**/};
	int realw = w * 3, rem = w % 4, isz = (realw + rem) * h, fsz = isz + 54;
	//bfh.bfSize = fsz;
	bfh[2] = (fsz & 0xFF); bfh[3] = (fsz >> 8) & 0xFF; bfh[4] = (fsz >> 16) & 0xFF; bfh[5] = (fsz >> 24) & 0xFF;
	//bfh.biSize = isz
	bfh[34] = (isz & 0xFF); bfh[35] = (isz >> 8) & 0xFF; bfh[36] = (isz >> 16) & 0xFF; bfh[37] = (isz >> 24) & 0xFF;
	//bfh.biWidth = w;
	bfh[18] = (w & 0xFF); bfh[19] = (w >> 8) & 0xFF; bfh[20] = (w >> 16) & 0xFF; bfh[21] = (w >> 24) & 0xFF;
	//bfh.biHeight = h;
	bfh[22] = (h & 0xFF); bfh[23] = (h >> 8) & 0xFF; bfh[24] = (h >> 16) & 0xFF; bfh[25] = (h >> 24) & 0xFF;
	
	// xpels/ypels
//	bfh[38] = 19; bfh[39] = 11;
//	bfh[42] = 19; bfh[43] = 11;
	
	fwrite((void*)&bfh[0], 54,1,f);
	unsigned char* bstr = (unsigned char*)malloc(realw), *remstr = 0; 
	if(rem != 0) { remstr = (unsigned char*)malloc(rem); memset(remstr,0,rem); }
	int i, j, k;
	for(j = h-1 ; j > -1 ; j--){
		for(i = 0 ; i < w ; i++)
			for(k = 0 ; k < 3 ; k++) { bstr[i*3+k] = img[(j*realw+i*3)+(2-k)]; }
		fwrite(bstr,realw,1,f); if (rem != 0) { fwrite(remstr,rem,1,f); }
	}
	free(bstr); if(remstr) free(remstr);
	fclose(f);
}

unsigned char *read_bmp(char *fname,int* _w,int* _h) {
	unsigned char head[54];
	FILE *f = fopen(fname,"rb");
	fread((void *)&head[0],1,54,f);
	*_w = head[18] + ( ((int)head[19]) << 8) + ( ((int)head[20]) << 16) + ( ((int)head[21]) << 24);
	*_h = head[22] + ( ((int)head[23]) << 8) + ( ((int)head[24]) << 16) + ( ((int)head[25]) << 24);
//	printf("w = %d, h = %d\n", *_w, *_h);
	//if(((head.biBitCount != 24) && (head.biBitCount != 32)) || (head.biCompression != 0)) { return 0; }
	const int fileSize = ((*_w)*3+((*_w)%4))*(*_h);
	unsigned char *img = (unsigned char*)malloc((*_w)*(*_h)*3), *data = (unsigned char*)malloc(fileSize);
	fseek(f,54,SEEK_SET); fread(data,1,fileSize,f);
	int i,j,k,rev_j;
	for(j = 0, rev_j = (*_h) - 1; j < (*_h) ; j++, rev_j--) {
		for(i = 0 ; i < (*_w) ; i++) {
			int fpos = j*((*_w) * 3 + (*_h) % 4) + i*3, pos = rev_j * (*_w) * 3 + i*3;
			for(k = 0 ; k < 3 ; k++) img[pos+k] = data[fpos+(2-k)];
		}
	}
	free(data);
	return img;
}

void put_pixel(unsigned char* img, int w, int h, int x, int y, int col)
{
//	printf("pix{%d, %d}\n", x, y);

	(void)h;

	int ofs = (y * w + x) * 3;

	unsigned char r = (col) & 0xFF;
	unsigned char g = (col >> 8 ) & 0xFF;
	unsigned char b = (col >> 16) & 0xFF;

	img[ofs + 0] = r;
	img[ofs + 1] = g;
	img[ofs + 2] = b;
}

void hline(unsigned char* img, int w, int h, int x, int y1, int y2, int col)
{
	if(y1 < 0) { y1 = 0; }
	if(y2 < 0) { y2 = 0; }

	if(y1 >= h) { y1 = h - 1; }
	if(y2 >= h) { y2 = h - 1; }

//	printf("hline y1 = %d, y2 = %d\n", y1, y2);

	int y;
	for(y = y1; y <= y2 ; y++)
	{
		put_pixel(img, w, h, x, y, col);
	}
}

void clear_img(unsigned char* img, int w, int h)
{
	memset(img, 0, w * h * 3);
}

unsigned char* plot_array(float* arr, int len, int H, float MinY, float MaxY, int color, int use_lines)
{
	unsigned char* img = (unsigned char*)malloc(len * H * 3);

	clear_img(img, len, H);

	int i, y1, y2;

	float CC = ((float)H)/ (MaxY - MinY);

	y2 = H - 1 - (int) ( ( 0 - MinY) * CC ); // (int) (((MaxY - MinY) / 2.0f) * (float)H);

	for(i = 0 ; i < len ; i++)
	{
		y1 = H - 1 - (int) ( (arr[i] - MinY) * CC );

//		printf("{%d} = (%d , %d)\n", i, y1, y2);

		if(use_lines) { hline(img, len, H, i, y1 < y2 ? y1 : y2, y1 < y2 ? y2 : y1, color); }
		if(!use_lines) { put_pixel(img, len, H, i, y1, color); }
	}

	return img;
}

void save_array_as_bmp(const char* fname, float* arr, int len, int H, float MinY, float MaxY, int color, int use_lines)
{
	unsigned char* img = plot_array(arr, len, H, MinY, MaxY, color, use_lines);

	write_bmp(fname, len, H, img);

	free(img);
}
