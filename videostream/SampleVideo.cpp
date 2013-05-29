#include <windows.h>
#include <stdio.h>
#include <theora/theora.h>
#include <theora/codec.h>
#include <theora/theoradec.h>
#include <theora/theoraenc.h>



#include "videoInput.h"

// ширина/высота изображения
const int ImageWidth  = 320;
const int ImageHeight = 240;

// Преобразованное в формат RGBA изображение (для отображения на экране)
unsigned char Img[ImageWidth * ImageHeight * 4];

//Conversioned from RGB to YUV (for save to file)
unsigned char ImgYUV[ImageWidth * ImageHeight * 3];

struct _yuvBuff
{
	unsigned _yWidth, _yHeight;
	unsigned _uWidth, _uHeight;
	unsigned _vWidth, _vHeight;
	unsigned char *_y, *_u, *_v;
	_yuvBuff(unsigned yWidth, unsigned yHeight, unsigned uWidth, unsigned uHeight, unsigned vWidth, unsigned vHeight):
				_yWidth(yWidth), _yHeight(yHeight),
				_uWidth(uWidth), _uHeight(uHeight),
				_vWidth(vWidth), _vHeight(vHeight)
				{
					_y = new unsigned char[_yWidth * _yHeight];
					_u = new unsigned char[_uWidth * _uHeight];
					_v = new unsigned char[_vWidth * _vHeight];
					//Интересно, что будет, если не округлять??
				}
	~_yuvBuff()
	{
		delete _y;
		delete _u;
		delete _v;
		_yWidth = _uWidth = _vWidth = 0;
		_yHeight = _uHeight = _vHeight = 0;
	}
};



unsigned char YUV420[ImageWidth * ImageHeight + ImageWidth * ImageHeight / 4 + ImageWidth * ImageHeight / 4];

// Изображение, получаемое с видеокамеры, в rgb-формате
unsigned char CamImg[ImageWidth * ImageHeight * 3];

// оконная функция (предварительное объявление)
LRESULT CALLBACK MyFunc(HWND, UINT, WPARAM, LPARAM);

// имя оконного класса
char szWinName[] = "VideoClass";

HWND hWnd;

// индекс подключенной видеокамеры - от 0 до videoInput::listDevices()-1
int dev = 0;

// интерфейс для получения кадров
videoInput VI;

// запуск получения кадров с видеокамеры
void InitVideo()
{
	// int numDevices = videoInput::listDevices();
	VI.setUseCallback(true);
	VI.setupDevice(dev, ImageWidth, ImageHeight, VI_COMPOSITE); 
}

#ifdef _MSC_VER
// для MSVC точка входа одна
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
#else
// для GCC - другая
int main()
#endif
{
	MSG msg;
	WNDCLASSA wcl;
	// 0. Регистрируем оконный класс

	wcl.hInstance = NULL;
	wcl.lpszClassName = szWinName;
	wcl.lpfnWndProc = MyFunc;
	wcl.style = 0;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;

	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;

	wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

	if(!RegisterClassA(&wcl)) return 0;

	// 1. Создаём окно

	hWnd = CreateWindowA(szWinName, "Video", 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 700, HWND_DESKTOP, NULL, 
		NULL, NULL);

	// 2. Отображаем окно на экран
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// 3. Запускаем таймер (20 мсек, 50 кадров в секунду)
	SetTimer(hWnd, 1, 20, NULL);

	// 4. стартуем получение кадров с камеры
	InitVideo();

	// 5. Стандартный цикл обработки событий
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void PaintIt(HDC dc)
{
	// создаём временный контекст
	HDC hMemDC = CreateCompatibleDC(dc);

	// создаём совместимую картинку
	HBITMAP hTmpBmp = CreateCompatibleBitmap(dc, ImageWidth, ImageHeight);

	// заполняем информацию о картинке
	BITMAPINFO BitmapInfo;

	memset(&BitmapInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));

	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = ImageWidth;
	BitmapInfo.bmiHeader.biHeight = ImageHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = 0;
	BitmapInfo.bmiHeader.biSizeImage = ImageWidth * ImageHeight * 32;

	// заполняем картинку
	unsigned short ucount = 0, vcount = 0;
	unsigned short ubuff = 0, vbuff = 0;
	_yuvBuff *YUV420Buff = new _yuvBuff(ImageWidth, ImageHeight, ImageWidth/2, ImageHeight/2, ImageWidth/2, ImageHeight/2);
	for(int y = 0 ; y < ImageHeight ; y++)
	{
		for(int x = 0 ; x < ImageWidth ; x++)
		{
			int Ofs = y * ImageWidth + x;
			Img[ Ofs * 4 + 0 ] = CamImg[Ofs * 3 + 2];
			Img[ Ofs * 4 + 1 ] = CamImg[Ofs * 3 + 1];
			Img[ Ofs * 4 + 2 ] = CamImg[Ofs * 3 + 0];
			Img[ Ofs * 4 + 3 ] = 0;
			
			//Convertion to YUV
			ImgYUV[ Ofs * 3 + 0 ] = 0.299 * CamImg[ Ofs * 3 + 2] + 0.587 * CamImg[ Ofs * 3 + 1] + 0.114 * CamImg[ Ofs * 3 + 0];
			ImgYUV[ Ofs * 3 + 1 ] = -0.14713 * CamImg[ Ofs * 3 + 2] + (-0.28886) * CamImg[ Ofs * 3 + 1] + 0.436 * CamImg[ Ofs * 3 + 0] +128;
			ImgYUV[ Ofs * 3 + 2 ] = 0.615 * CamImg[ Ofs * 3 + 2] + (-0.51499) * CamImg[ Ofs * 3 + 1] + (-0.10001) * CamImg[ Ofs * 3 + 0] + 128;
			
			//Convertion to YUV420
			YUV420Buff->_y[Ofs] = ImgYUV[ Ofs * 3 + 0];
			if (ucount + vcount == 8)
			{
				YUV420Buff->_u[Ofs/4] = ubuff/4;
				YUV420Buff->_v[Ofs/4] = vbuff/4;
				ucount = vcount = 0;
				ubuff = vbuff = 0;
			}
			else
			{
				ubuff += ImgYUV[ Ofs * 3 + 1 ];
				vbuff += ImgYUV[ Ofs * 3 + 2 ];
				++ucount, ++vcount;
			}
		 
			
		}
	}
	/*unsigned j = 0;
	for (unsigned i = 0; i < YUV420Buff->_yWidth * YUV420Buff->_yHeight; ++i)
		YUV420[j++] = YUV420Buff->_y[i];
	for (unsigned i = 0; i < YUV420Buff->_uWidth * YUV420Buff->_uHeight; ++i)
		YUV420[j++] = YUV420Buff->_u[i];
	for (unsigned i = 0; i < YUV420Buff->_vWidth * YUV420Buff->_vHeight; ++i)
		YUV420[j++] = YUV420Buff->_v[i];*/
	
		

	// копирум содержимое Img[] в наш BITMAP
	SetDIBits(hMemDC, hTmpBmp, 0, ImageHeight, &Img[0], &BitmapInfo, DIB_RGB_COLORS);

	// выбираем BITMAP на hMemDC
	SelectObject(hMemDC, hTmpBmp);

	// копируем кусок контекста hMemDC (там выбран BITMAP) на "экран" dc
	BitBlt(dc, 0, 0, ImageWidth, ImageHeight, hMemDC, 0, 0, SRCCOPY);

	// чистим всё
	DeleteDC(hMemDC);
	DeleteObject(hTmpBmp);
}

void Procedure()
{
	th_info ti;
	th_info_init(&ti);
	ti.frame_width = ImageWidth * 16;
    ti.frame_height = ImageHeight * 16;
    ti.pic_width = ImageWidth;
    ti.pic_height = ImageHeight;
    ti.pic_x = 0;
    ti.pic_y = 0;
    //ti.fps_numerator = 0;
    //ti.fps_denominator = 0;
    //ti.aspect_numerator = 1;
    //ti.aspect_denominator = 1;
    ti.colorspace = TH_CS_UNSPECIFIED;
	ti.pixel_fmt = TH_PF_420;
	
	yuv_buffer temp;
	temp.y_width = ImageWidth * 16;
	temp.y_height = ImageHeight * 16;
	temp.uv_width = ImageWidth * 4;
	temp.uv_height = ImageHeight * 4;
	temp.y = YUV420Buff->_y;
	temp.u = YUV420Buff->_u;
	temp.v = YUV420Buff->_v;
}

void UpdateTime(float dt)
{
	// проверяем наличие кадра
	if( VI.isFrameNew(dev) )	
	{
		// копируем содержимое кадра
		VI.getPixels(dev, CamImg, true);			
	}
}

LRESULT CALLBACK MyFunc(HWND this_hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		UpdateTime(0.02f);
		InvalidateRect(hWnd, NULL, 0);
		break;

	case WM_PAINT:
		// перерисовываем только всё окно
		if(this_hwnd == hWnd)
		{
			PAINTSTRUCT ps;
			HDC ThisDC = BeginPaint(hWnd, &ps);
			PaintIt(ThisDC);
			Procedure();
			EndPaint(hWnd, &ps);
		}

		break;
	}

	return DefWindowProc(this_hwnd, message, wParam, lParam);
}
