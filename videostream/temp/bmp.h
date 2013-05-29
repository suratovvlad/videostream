#ifdef __cplusplus
extern "C"
{
#endif

void write_bmp(const char *fname, int w,int h,unsigned char *img);

unsigned char *read_bmp(const char *fname,int* _w,int* _h);

unsigned char* plot_array(float* arr, int len, int H, float MinY, float MaxY, int color, int use_lines);

void clear_img(unsigned char* img, int w, int h);

void save_array_as_bmp(const char* fname, float* arr, int len, int H, float MinY, float MaxY, int color, int use_lines);

#ifdef __cplusplus
} // extern "C"
#endif
