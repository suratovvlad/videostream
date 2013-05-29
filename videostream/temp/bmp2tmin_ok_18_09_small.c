/* 
 * File:   main.cpp
 * Author: Vladislav
 *
 * Created on May 20, 2013, 10:48 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "bmp.h"
#include <ogg/ogg.h>
#include <theora/theoraenc.h>
#include <windows.h>

const char *option_output = "bmpt2.ogv";
int video_fps_numerator = 24;
int video_fps_denominator = 1;
int video_aspect_numerator = 0;
int video_aspect_denominator = 0;
int video_rate = -1;
int video_quality = -1;
ogg_uint32_t keyframe_frequency=0;
int chroma_format = TH_PF_420;

FILE *ogg_fp = NULL;
ogg_stream_state ogg_os;
ogg_packet op;   
ogg_page og;
    
th_enc_ctx      *td;
th_info          ti;

th_comment       tc;

unsigned int w, h;

int theora_write_frame(unsigned char *yuv, int last)
{
	th_ycbcr_buffer ycbcr;
	ogg_packet op;
	ogg_page og;

	unsigned char *yuv_y;
	unsigned char *yuv_u;
	unsigned char *yuv_v;

	unsigned int x, y;

	/* Must hold: yuv_w >= w && yuv_h >= h*/
	unsigned long yuv_w = (w + 15) & ~15;
	unsigned long yuv_h = (h + 15) & ~15;

	ycbcr[0].width = yuv_w;
	ycbcr[0].height = yuv_h;
	ycbcr[0].stride = yuv_w;
	ycbcr[1].width = (chroma_format == TH_PF_444) ? yuv_w : (yuv_w >> 1);
	ycbcr[1].stride = ycbcr[1].width;
	ycbcr[1].height = (chroma_format == TH_PF_420) ? (yuv_h >> 1) : yuv_h;
	ycbcr[2].width = ycbcr[1].width;
	ycbcr[2].stride = ycbcr[1].stride;
	ycbcr[2].height = ycbcr[1].height;

	ycbcr[0].data = yuv_y = (unsigned char*) malloc(ycbcr[0].stride * ycbcr[0].height);
	ycbcr[1].data = yuv_u = (unsigned char*) malloc(ycbcr[1].stride * ycbcr[1].height);
	ycbcr[2].data = yuv_v = (unsigned char*) malloc(ycbcr[2].stride * ycbcr[2].height);

	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			{ yuv_y[x + y * yuv_w] = yuv[3 * (x + y * w) + 0]; }

	if (chroma_format == TH_PF_420) {
		for(y = 0; y < h; y += 2) {
			for(x = 0; x < w; x += 2) {
				yuv_u[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * w) + 1];
				yuv_v[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * w) + 2];
			}
		}
	} else if (chroma_format == TH_PF_444) {
		for(y = 0; y < h; y++) {
			for(x = 0; x < w; x++) {
				yuv_u[x + y * ycbcr[1].stride] = yuv[3 * (x + y * w) + 1];
				yuv_v[x + y * ycbcr[2].stride] = yuv[3 * (x + y * w) + 2];
			}
		}
	} else {  /* TH_PF_422 */
		for(y = 0; y < h; y += 1) {
			for(x = 0; x < w; x += 2) {
				yuv_u[(x >> 1) + y * ycbcr[1].stride] = yuv[3 * (x + y * w) + 1];
				yuv_v[(x >> 1) + y * ycbcr[2].stride] = yuv[3 * (x + y * w) + 2];
			}
		}    
	}

	/* Theora is a one-frame-in,one-frame-out system; submit a frame for compression and pull out the packet */
	if(th_encode_ycbcr_in(td, ycbcr)) { fprintf(stderr, "%s: error: could not encode frame\n", option_output); return 1; }

	if(!th_encode_packetout(td, last, &op)) { fprintf(stderr, "%s: error: could not read packets\n", option_output); return 1; }

	ogg_stream_packetin(&ogg_os, &op);
	while(ogg_stream_pageout(&ogg_os, &og)) {
		fwrite(og.header, og.header_len, 1, ogg_fp);
		fwrite(og.body, og.body_len, 1, ogg_fp);
	}

	free(yuv_y); free(yuv_u); free(yuv_v);

	return 0;
}

unsigned char yuv_clamp(double d)
{
	if(d < 0) { return 0; }
	if(d > 255) { return 255; }
	return d;
}

void rgb24_to_yuv(unsigned char* bmp, unsigned char* yuv)
{
	int i, cnt = w * h;
	unsigned char r, g, b;
	for(i = 0 ; i < cnt ; i++)
	{
		r = *bmp++; g = *bmp++; b = *bmp++;

		*yuv++ = yuv_clamp(0.299 * r + 0.587 * g + 0.114 * b);
		*yuv++ = yuv_clamp((0.436 * 255 - 0.14713 * r - 0.28886 * g + 0.436 * b) / 0.872);
		*yuv++ = yuv_clamp((0.615 * 255 + 0.615 * r - 0.51499 * g - 0.10001 * b) / 1.230);
	}
}
/*
void read_bmp(const char *pathname, unsigned char **bmp_data, int *w, int *h)
{
	FILE *pFile;
	long lSize;
	
	pFile = fopen(pathname , "rb");
	if (pFile == NULL)
	{
		fputs("File error",stderr);
		exit(1);
	}
	
	fseek(pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind(pFile);
	
	unsigned char *buffer = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
	if (buffer == NULL)
	{
		fputs ("Memory error",stderr);
		exit (2);
	}
	
	BITMAPFILEHEADER    BMP;
	BITMAPINFOHEADER	BMPINFO;
	fread (buffer,1,sizeof(BITMAPFILEHEADER),pFile);
	memcpy(&BMP,buffer,sizeof(BITMAPFILEHEADER));
	
	fread (buffer,sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER),pFile);
	memcpy(&BMPINFO, buffer, sizeof(BITMAPINFOHEADER));
	rewind(pFile);
	
	*w = BMPINFO.biWidth;
	*h = BMPINFO.biHeight;
	
	free(buffer);
	buffer = (unsigned char*) malloc(sizeof(unsigned char)*(lSize - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) - 4*256));
    if (fread(buffer,(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)),lSize,pFile))
	{
		fputs("Reading error",stderr);
		exit(3);
	}
	
	fclose (pFile);	
	
	*bmp_data = (unsigned char*)malloc(sizeof(unsigned char)*(lSize - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) - 4*256));
	for (int i = 0; i < sizeof(unsigned char)*(lSize - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) - 4*256); i++)
		bmp_data[0][i] = buffer[i];
	free(buffer);
	
	printf("%i\n", BMPINFO.biBitCount);
	printf("%i\n", BMPINFO.biCompression);
	printf("%i\n", BMPINFO.biSize);
	printf("%i\n", BMPINFO.biSizeImage);
	printf("%i\n", BMPINFO.biClrUsed);
	printf("%i\n", BMPINFO.biPlanes);
	printf("%i\n", BMP.bfType);
}
 * */
/*
void png_read(const char *pathname, unsigned char **yuv)
{
	unsigned char *bmp_data;
	read_bmp(pathname, &bmp_data, (int*)&w, (int*)&h);
	
	*yuv = (unsigned char*)malloc(w * h * 3);
	rgb24_to_yuv(bmp_data, *yuv);
	free(bmp_data);
}
*/
void png_read(unsigned char *rgb, unsigned char **yuv)
{
	//unsigned char *bmp_data;
	//read_bmp(pathname, &bmp_data, (int*)&w, (int*)&h);
	
	*yuv = (unsigned char*)malloc(w * h * 3);
	if (rgb == NULL)
	{
		printf("Empty");
		unsigned char *bmp_data = read_bmp("Background0.bmp", (int*)&w, (int*)&h);
		rgb24_to_yuv(bmp_data, *yuv);
		free(bmp_data);
		return;
	}
	rgb24_to_yuv(rgb, *yuv);

}

int ilog(unsigned _v){int ret;for(ret=0;_v;ret++)_v>>=1;return ret;}

void ENC_Init()
{
	int ret;

	ogg_fp = fopen(option_output, "wb");
	if(!ogg_fp) { fprintf(stderr, "%s: error: %s\n", option_output, "couldn't open output file"); exit(1); }

	srand(time(NULL));
	if(ogg_stream_init(&ogg_os, rand())) {fprintf(stderr, "%s: error: %s\n", option_output, "couldn't create ogg stream state"); exit(1); }

	th_info_init(&ti);    
	ti.frame_width = ((w + 15) >>4)<<4;
	ti.frame_height = ((h + 15)>>4)<<4;
	ti.pic_width = w;
	ti.pic_height = h;
	ti.pic_x = ti.pic_y = 0;
	ti.fps_numerator = video_fps_numerator;
	ti.fps_denominator = video_fps_denominator;
	ti.aspect_numerator = video_aspect_numerator;
	ti.aspect_denominator = video_aspect_denominator;
	ti.colorspace = TH_CS_UNSPECIFIED;
	ti.pixel_fmt = (th_pixel_fmt)chroma_format;
	ti.target_bitrate = video_rate;
	ti.quality = video_quality;
	ti.keyframe_granule_shift=ilog(keyframe_frequency-1);

	td=th_encode_alloc(&ti);  
	th_info_clear(&ti);

	/* setting just the granule shift only allows power-of-two keyframe
	spacing.  Set the actual requested spacing. */
	ret=th_encode_ctl(td,TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE, &keyframe_frequency,sizeof(keyframe_frequency-1));
	if(ret<0){ fprintf(stderr,"Could not set keyframe interval to %d.\n",(int)keyframe_frequency); }

	/* write the bitstream header packets with proper page interleave */
	th_comment_init(&tc);
	/* first packet will get its own page automatically */
	if(th_encode_flushheader(td,&tc,&op)<=0){ fprintf(stderr,"Internal Theora library error.\n"); exit(1); }
	th_comment_clear(&tc);

	ogg_stream_packetin(&ogg_os,&op);
	if(ogg_stream_pageout(&ogg_os,&og)!=1){ fprintf(stderr,"Internal Ogg library error.\n"); exit(1); }
	fwrite(og.header,1,og.header_len,ogg_fp);
	fwrite(og.body,1,og.body_len,ogg_fp);

	/* create the remaining theora headers */
	for(;;){
		ret=th_encode_flushheader(td,&tc,&op);
		if(ret<0){ fprintf(stderr,"Internal Theora library error.\n"); exit(1); }
		else if(!ret)break;
		ogg_stream_packetin(&ogg_os,&op);
	}

	/* Flush the rest of our headers. This ensures the actual data in each stream will start on a new page, as per spec. */
	for(;;){
		int result = ogg_stream_flush(&ogg_os,&og);
		/* can't get here */
		if(result<0){ fprintf(stderr,"Internal Ogg library error.\n"); exit(1); }
		if(result==0)break;
		fwrite(og.header,1,og.header_len,ogg_fp);
		fwrite(og.body,1,og.body_len,ogg_fp);
	}
}

void ENC_Close()
{
	th_encode_free(td);

	if(ogg_stream_flush(&ogg_os, &og)) {
		fwrite(og.header, og.header_len, 1, ogg_fp);
		fwrite(og.body, og.body_len, 1, ogg_fp);
	}            

	if(ogg_fp){
		fflush(ogg_fp);
		if(ogg_fp!=stdout)fclose(ogg_fp);
	}

	ogg_stream_clear(&ogg_os);
}
      
int encode_start()
{
	//int i, n;

	video_quality = 1; // 0..63
	video_rate = 64000; // 1+
	//chroma_format=TH_PF_444; //chroma_format=TH_PF_422;
	if(video_rate>0) video_quality=0;
	if(video_quality==-1) video_quality=48;

	/*Use a default keyframe frequency of 64 for 1-pass (streaming) mode, and 256 for two-pass mode.*/
	if(keyframe_frequency<=0){ keyframe_frequency=64; }
	
	ENC_Init();
	return 0;
}
int encode_stop()
{
	ENC_Close();
}