#include <windows.h>
#include <stdio.h>

#include "videoInput.h"
#define ID_START_BUTTON			102
#define ID_STOP_BUTTON			103

bool RECORD = false;

// ширина/высота изображения
const int ImageWidth  = 160;
const int ImageHeight = 120;


// Преобразованное в формат RGBA изображение (для отображения на экране)
unsigned char Img[ImageWidth * ImageHeight * 4];

extern unsigned int w, h;
#ifdef __cplusplus 
extern "C"
{
	/* a C functions */
	int encode_start(); 
	int encode_stop();
	void png_read(unsigned char *rgb, unsigned char **yuv);
	int theora_write_frame(unsigned char *yuv, int last);
}
#endif

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
int _main()
#endif
{
	w = ImageWidth, h = ImageHeight;
	encode_start();
	//memset(&CamImg, 0, sizeof(CamImg));
	
	
	
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

	HWND hStartRecord = CreateWindowA(
									"BUTTON", // Window class
									"Start", // Title
									WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
									650,
									200,
									100,
									50,
									hWnd,
									(HMENU)ID_START_BUTTON, // для WM_COMMAND
									NULL /*hThisInst*/,
									NULL
								);
	
	HWND hStopRecord = CreateWindowA(
									"BUTTON", // Window class
									"Stop", // Title
									WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
									650,
									100,
									100,
									50,
									hWnd,
									(HMENU)ID_STOP_BUTTON, // для WM_COMMAND
									NULL /*hThisInst*/,
									NULL
								);
	
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

	for(int y = 0 ; y < ImageHeight ; y++)
	{
		for(int x = 0 ; x < ImageWidth ; x++)
		{
			int Ofs = y * ImageWidth + x;
			Img[ Ofs * 4 + 0 ] = CamImg[Ofs * 3 + 2];
			Img[ Ofs * 4 + 1 ] = CamImg[Ofs * 3 + 1];
			Img[ Ofs * 4 + 2 ] = CamImg[Ofs * 3 + 0];
			Img[ Ofs * 4 + 3 ] = 0;
		}
	}		

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

void UpdateTime(float dt)
{
	// проверяем наличие кадра
	if( VI.isFrameNew(dev) )	
	{
		// копируем содержимое кадра
		VI.getPixels(dev, CamImg, true);			
	}
}

void Record(bool toStop, int last)
{
	unsigned char *rgb = (unsigned char*)malloc(w * h * 3);
	for(int y = 0 ; y < ImageHeight ; y++)
	{
		for(int x = 0 ; x < ImageWidth ; x++)
		{
			int Ofs = y * ImageWidth + x;
			rgb[ Ofs * 3 + 0 ] = CamImg[ImageWidth*ImageHeight*3 - 1 - (Ofs * 3 + 2)];
			rgb[ Ofs * 3 + 1 ] = CamImg[ImageWidth*ImageHeight*3 - 1 - (Ofs * 3 + 1)];
			rgb[ Ofs * 3 + 2 ] = CamImg[ImageWidth*ImageHeight*3 - 1 - (Ofs * 3 + 0)];
		}
	}
	
	unsigned char *yuv;
	png_read(rgb, &yuv);
	if(theora_write_frame(yuv, last)) { fprintf(stderr,"Encoding error.\n"); exit(1); }
	free(yuv);
	free(rgb);
	RECORD = !toStop;
}

LRESULT CALLBACK MyFunc(HWND this_hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_DESTROY:
		if (RECORD)	Record(true, 1);	
		encode_stop();
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		UpdateTime(0.02f);
		InvalidateRect(hWnd, NULL, 0);
		break;

	case WM_PAINT:
	{	// перерисовываем только всё окно
		if(this_hwnd == hWnd)
		{
			PAINTSTRUCT ps;
			HDC ThisDC = BeginPaint(hWnd, &ps);
			PaintIt(ThisDC);
			//encode_start();
			if (RECORD)
				Record(false, 0);
			EndPaint(hWnd, &ps);
		}		
		break;
	}
	case WM_COMMAND:
	{
		case ID_START_BUTTON:
			if (!RECORD) Record(false, 0);
			break;
		case ID_STOP_BUTTON:
			if (RECORD) Record(true, 1);
			break;
		break;
	}
	}

	return DefWindowProc(this_hwnd, message, wParam, lParam);
}
