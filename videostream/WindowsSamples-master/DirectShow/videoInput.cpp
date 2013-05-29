#ifdef _WIN32

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#define DEBUG 1
#define _DEBUG 1

#include "videoInput.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ocidl.h>

//for threading
#include <process.h>

/// required DirectShow headers

//#include "dshow_extract.h"
/************************  Extracted from DirectShow headers *************/

struct IPin;
struct IBaseFilter;
struct IEnumFilters;
struct IReferenceClock;

/// strmif.h

typedef 
enum _PinDirection
    {	PINDIR_INPUT	= 0,
	PINDIR_OUTPUT	= PINDIR_INPUT + 1
    } 	PIN_DIRECTION;

typedef 
enum tagPhysicalConnectorType
    {	PhysConn_Video_Tuner	= 1,
	PhysConn_Video_Composite	= PhysConn_Video_Tuner + 1,
	PhysConn_Video_SVideo	= PhysConn_Video_Composite + 1,
	PhysConn_Video_RGB	= PhysConn_Video_SVideo + 1,
	PhysConn_Video_YRYBY	= PhysConn_Video_RGB + 1,
	PhysConn_Video_SerialDigital	= PhysConn_Video_YRYBY + 1,
	PhysConn_Video_ParallelDigital	= PhysConn_Video_SerialDigital + 1,
	PhysConn_Video_SCSI	= PhysConn_Video_ParallelDigital + 1,
	PhysConn_Video_AUX	= PhysConn_Video_SCSI + 1,
	PhysConn_Video_1394	= PhysConn_Video_AUX + 1,
	PhysConn_Video_USB	= PhysConn_Video_1394 + 1,
	PhysConn_Video_VideoDecoder	= PhysConn_Video_USB + 1,
	PhysConn_Video_VideoEncoder	= PhysConn_Video_VideoDecoder + 1,
	PhysConn_Video_SCART	= PhysConn_Video_VideoEncoder + 1,
	PhysConn_Video_Black	= PhysConn_Video_SCART + 1,
	PhysConn_Audio_Tuner	= 0x1000,
	PhysConn_Audio_Line	= PhysConn_Audio_Tuner + 1,
	PhysConn_Audio_Mic	= PhysConn_Audio_Line + 1,
	PhysConn_Audio_AESDigital	= PhysConn_Audio_Mic + 1,
	PhysConn_Audio_SPDIFDigital	= PhysConn_Audio_AESDigital + 1,
	PhysConn_Audio_SCSI	= PhysConn_Audio_SPDIFDigital + 1,
	PhysConn_Audio_AUX	= PhysConn_Audio_SCSI + 1,
	PhysConn_Audio_1394	= PhysConn_Audio_AUX + 1,
	PhysConn_Audio_USB	= PhysConn_Audio_1394 + 1,
	PhysConn_Audio_AudioDecoder	= PhysConn_Audio_USB + 1
    } 	PhysicalConnectorType;

typedef 
enum tagAnalogVideoStandard
    {	AnalogVideo_None	= 0,
	AnalogVideo_NTSC_M	= 0x1,
	AnalogVideo_NTSC_M_J	= 0x2,
	AnalogVideo_NTSC_433	= 0x4,
	AnalogVideo_PAL_B	= 0x10,
	AnalogVideo_PAL_D	= 0x20,
	AnalogVideo_PAL_G	= 0x40,
	AnalogVideo_PAL_H	= 0x80,
	AnalogVideo_PAL_I	= 0x100,
	AnalogVideo_PAL_M	= 0x200,
	AnalogVideo_PAL_N	= 0x400,
	AnalogVideo_PAL_60	= 0x800,
	AnalogVideo_SECAM_B	= 0x1000,
	AnalogVideo_SECAM_D	= 0x2000,
	AnalogVideo_SECAM_G	= 0x4000,
	AnalogVideo_SECAM_H	= 0x8000,
	AnalogVideo_SECAM_K	= 0x10000,
	AnalogVideo_SECAM_K1	= 0x20000,
	AnalogVideo_SECAM_L	= 0x40000,
	AnalogVideo_SECAM_L1	= 0x80000,
	AnalogVideo_PAL_N_COMBO	= 0x100000
    } 	AnalogVideoStandard;

typedef 
enum tagVideoProcAmpProperty
    {	VideoProcAmp_Brightness	= 0,
	VideoProcAmp_Contrast	= VideoProcAmp_Brightness + 1,
	VideoProcAmp_Hue	= VideoProcAmp_Contrast + 1,
	VideoProcAmp_Saturation	= VideoProcAmp_Hue + 1,
	VideoProcAmp_Sharpness	= VideoProcAmp_Saturation + 1,
	VideoProcAmp_Gamma	= VideoProcAmp_Sharpness + 1,
	VideoProcAmp_ColorEnable	= VideoProcAmp_Gamma + 1,
	VideoProcAmp_WhiteBalance	= VideoProcAmp_ColorEnable + 1,
	VideoProcAmp_BacklightCompensation	= VideoProcAmp_WhiteBalance + 1,
	VideoProcAmp_Gain	= VideoProcAmp_BacklightCompensation + 1
    } 	VideoProcAmpProperty;

typedef 
enum tagVideoProcAmpFlags
    {	VideoProcAmp_Flags_Auto	= 0x1,
	VideoProcAmp_Flags_Manual	= 0x2
    } 	VideoProcAmpFlags;

typedef 
enum tagCameraControlProperty
    {	CameraControl_Pan	= 0,
	CameraControl_Tilt	= CameraControl_Pan + 1,
	CameraControl_Roll	= CameraControl_Tilt + 1,
	CameraControl_Zoom	= CameraControl_Roll + 1,
	CameraControl_Exposure	= CameraControl_Zoom + 1,
	CameraControl_Iris	= CameraControl_Exposure + 1,
	CameraControl_Focus	= CameraControl_Iris + 1
    } 	CameraControlProperty;

typedef 
enum tagCameraControlFlags
    {	CameraControl_Flags_Auto	= 0x1,
	CameraControl_Flags_Manual	= 0x2
    } 	CameraControlFlags;

// end of strmif.h

/// control.h

typedef long OAFilterState;
typedef LONG_PTR OAEVENT;
typedef LONG_PTR OAHWND;

/// dmdls.h

typedef LONGLONG REFERENCE_TIME;
typedef REFERENCE_TIME *LPREFERENCE_TIME;

typedef struct tagVIDEOINFOHEADER {
    RECT            rcSource;          // The bit we really want to use
    RECT            rcTarget;          // Where the video should go
    DWORD           dwBitRate;         // Approximate bit data rate
    DWORD           dwBitErrorRate;    // Bit error rate for this stream
    REFERENCE_TIME  AvgTimePerFrame;   // Average time per frame (100ns units)
    BITMAPINFOHEADER bmiHeader;
} VIDEOINFOHEADER;

// end dmdls.h

/// amvideo.h
// Returns the address of the BITMAPINFOHEADER from the VIDEOINFOHEADER
#define HEADER(pVideoInfo) (&(((VIDEOINFOHEADER *) (pVideoInfo))->bmiHeader))

/// vfwmsgs.h

#define VFW_E_NOT_FOUND                  ((HRESULT)0x80040216L)

/// olectl.h

#ifdef _OLEAUT32_
#define WINOLECTLAPI STDAPI
#define WINOLECTLAPI_(type) STDAPI_(type)
#else
#define WINOLECTLAPI EXTERN_C DECLSPEC_IMPORT HRESULT WINAPI
#define WINOLECTLAPI_(type) EXTERN_C DECLSPEC_IMPORT type WINAPI
#endif

WINOLECTLAPI OleCreatePropertyFrame(HWND hwndOwner,UINT x,UINT y,LPCOLESTR lpszCaption,ULONG cObjects,LPUNKNOWN *ppUnk,ULONG cPages,LPCLSID pPageClsID,LCID lcid,DWORD dwReserved,LPVOID pvReserved);

/// Taken from DShow/aviriff.h
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) | (((DWORD)(ch4) & 0xFF00) << 8) | (((DWORD)(ch4) & 0xFF0000) >> 8) | (((DWORD)(ch4) & 0xFF000000) >> 24))

//////////////////

/// uuids.h

#define DEFINE_GUID_Local2(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const GUID name = { l,w1,w2,{ b1,b2,b3,b4,b5,b6,b7,b8 } }

#ifndef OUR_GUID_ENTRY
    #define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    DEFINE_GUID_Local2(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#endif

DEFINE_GUID_Local2(CLSID_SampleGrabber, 0xC1F400A0, 0x3F08, 0x11d3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37);

DEFINE_GUID_Local2(CLSID_NullRenderer,  0xC1F400A4, 0x3F08, 0x11d3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37);

//// Some stuff

// {AC798BE0-98E3-11d1-B3F1-00AA003761C5}
OUR_GUID_ENTRY(LOOK_UPSTREAM_ONLY,
0xac798be0, 0x98e3, 0x11d1, 0xb3, 0xf1, 0x0, 0xaa, 0x0, 0x37, 0x61, 0xc5)

/////// Types

// 73646976-0000-0010-8000-00AA00389B71  'vids' == MEDIATYPE_Video
OUR_GUID_ENTRY(MEDIATYPE_Video,
0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 73647561-0000-0010-8000-00AA00389B71  'auds' == MEDIATYPE_Audio
OUR_GUID_ENTRY(MEDIATYPE_Audio,
0x73647561, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 73747874-0000-0010-8000-00AA00389B71  'txts' == MEDIATYPE_Text
OUR_GUID_ENTRY(MEDIATYPE_Text,
0x73747874, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 7364696D-0000-0010-8000-00AA00389B71  'mids' == MEDIATYPE_Midi
OUR_GUID_ENTRY(MEDIATYPE_Midi,
0x7364696D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// e436eb83-524f-11ce-9f53-0020af0ba770            MEDIATYPE_Stream
OUR_GUID_ENTRY(MEDIATYPE_Stream,
0xe436eb83, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// 73(s)76(v)61(a)69(i)-0000-0010-8000-00AA00389B71  'iavs' == MEDIATYPE_Interleaved
OUR_GUID_ENTRY(MEDIATYPE_Interleaved,
0x73766169, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 656c6966-0000-0010-8000-00AA00389B71  'file' == MEDIATYPE_File
OUR_GUID_ENTRY(MEDIATYPE_File,
0x656c6966, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)


/////////// Subtypes

// e436eb7d-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB24
OUR_GUID_ENTRY(MEDIASUBTYPE_RGB24,
0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// e436eb7e-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB32
OUR_GUID_ENTRY(MEDIASUBTYPE_RGB32,
0xe436eb7e, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// e436eb7b-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB565
OUR_GUID_ENTRY(MEDIASUBTYPE_RGB565,
0xe436eb7b, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// e436eb7c-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB555
OUR_GUID_ENTRY(MEDIASUBTYPE_RGB555,
0xe436eb7c, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// 56555941-0000-0010-8000-00AA00389B71  'AYUV' == MEDIASUBTYPE_AYUV
//
// See the DX-VA header and documentation for a description of this format.
//
OUR_GUID_ENTRY(MEDIASUBTYPE_AYUV,
0x56555941, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 32315659-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_YV12
OUR_GUID_ENTRY(MEDIASUBTYPE_YV12,
0x32315659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 4C504C43-0000-0010-8000-00AA00389B71  'CLPL' == MEDIASUBTYPE_CLPL
OUR_GUID_ENTRY(MEDIASUBTYPE_CLPL,
0x4C504C43, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 56595559-0000-0010-8000-00AA00389B71  'YUYV' == MEDIASUBTYPE_YUYV
OUR_GUID_ENTRY(MEDIASUBTYPE_YUYV,
0x56595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 56555949-0000-0010-8000-00AA00389B71  'IYUV' == MEDIASUBTYPE_IYUV
OUR_GUID_ENTRY(MEDIASUBTYPE_IYUV,
0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 39555659-0000-0010-8000-00AA00389B71  'YVU9' == MEDIASUBTYPE_YVU9
OUR_GUID_ENTRY(MEDIASUBTYPE_YVU9,
0x39555659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 31313459-0000-0010-8000-00AA00389B71  'Y411' == MEDIASUBTYPE_Y411
OUR_GUID_ENTRY(MEDIASUBTYPE_Y411,
0x31313459, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 50313459-0000-0010-8000-00AA00389B71  'Y41P' == MEDIASUBTYPE_Y41P
OUR_GUID_ENTRY(MEDIASUBTYPE_Y41P,
0x50313459, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 32595559-0000-0010-8000-00AA00389B71  'YUY2' == MEDIASUBTYPE_YUY2
OUR_GUID_ENTRY(MEDIASUBTYPE_YUY2,
0x32595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 55595659-0000-0010-8000-00AA00389B71  'YVYU' == MEDIASUBTYPE_YVYU
OUR_GUID_ENTRY(MEDIASUBTYPE_YVYU,
0x55595659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 59565955-0000-0010-8000-00AA00389B71  'UYVY' ==  MEDIASUBTYPE_UYVY
OUR_GUID_ENTRY(MEDIASUBTYPE_UYVY,
0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

// 31313259-0000-0010-8000-00AA00389B71  'Y211' ==  MEDIASUBTYPE_Y211
OUR_GUID_ENTRY(MEDIASUBTYPE_Y211,
0x31313259, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71)

//// Pins

// fb6c4281-0353-11d1-905f-0000c0cc16ba
OUR_GUID_ENTRY(PIN_CATEGORY_CAPTURE,
0xfb6c4281, 0x0353, 0x11d1, 0x90, 0x5f, 0x00, 0x00, 0xc0, 0xcc, 0x16, 0xba)

// fb6c4282-0353-11d1-905f-0000c0cc16ba
OUR_GUID_ENTRY(PIN_CATEGORY_PREVIEW,
0xfb6c4282, 0x0353, 0x11d1, 0x90, 0x5f, 0x00, 0x00, 0xc0, 0xcc, 0x16, 0xba)

// 05589f80-c356-11ce-bf01-00aa0055595a        FORMAT_VideoInfo
OUR_GUID_ENTRY(FORMAT_VideoInfo,
0x05589f80, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a)

///// Capture graph and other interfaces

// BF87B6E1-8C27-11d0-B3F0-00AA003761C5     New Capture graph building
OUR_GUID_ENTRY(CLSID_CaptureGraphBuilder2,
0xBF87B6E1, 0x8C27, 0x11d0, 0xB3, 0xF0, 0x0, 0xAA, 0x00, 0x37, 0x61, 0xC5)

// e436ebb3-524f-11ce-9f53-0020af0ba770           Filter Graph
OUR_GUID_ENTRY(CLSID_FilterGraph,
0xe436ebb3, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70)

// pnp objects and categories
// 62BE5D10-60EB-11d0-BD3B-00A0C911CE86                 ICreateDevEnum
OUR_GUID_ENTRY(CLSID_SystemDeviceEnum,
0x62BE5D10,0x60EB,0x11d0,0xBD,0x3B,0x00,0xA0,0xC9,0x11,0xCE,0x86)

// 860BB310-5D01-11d0-BD3B-00A0C911CE86                 Video capture category
OUR_GUID_ENTRY(CLSID_VideoInputDeviceCategory,
0x860BB310,0x5D01,0x11d0,0xBD,0x3B,0x00,0xA0,0xC9,0x11,0xCE,0x86)

// end uuids.h

//////////////////

// IID_IUnknown
DEFINE_GUID_Local2( IID_IUnknown, 0xA36D02F3, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

DEFINE_GUID_Local2(IID_IPropertyBag,0x55272a00,0x42cb,0x11ce,0x81,0x35,0,0xaa,0,0x4b,0xb8,0x51);
DEFINE_GUID_Local2(IID_ISpecifyPropertyPages,0xb196b28b,0xbab4,0x101a,0xb6,0x9c,0,0xaa,0,0x34,0x1d,0x7);

typedef struct _AMMediaType
{
    GUID majortype;
    GUID subtype;
    BOOL bFixedSizeSamples;
    BOOL bTemporalCompression;
    ULONG lSampleSize;
    GUID formattype;
    IUnknown *pUnk;
    ULONG cbFormat;
    /* [size_is] */ BYTE *pbFormat;
} AM_MEDIA_TYPE;

/**********/

//EXTERN_C const IID IID_IFileSinkFilter;

DEFINE_GUID_Local2(IID_IFileSinkFilter, 0xa2104830, 0x7c70, 0x11cf, 0x8b, 0xce, 0x00, 0xaa, 0x00, 0xa3, 0xf1, 0xa6);

    MIDL_INTERFACE("a2104830-7c70-11cf-8bce-00aa00a3f1a6")
    IFileSinkFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFileName( /* [in] */ LPCOLESTR pszFileName, /* [unique][in] */ const AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetCurFile( /* [out] */ LPOLESTR *ppszFileName, /* [out] */ AM_MEDIA_TYPE *pmt) = 0;
    };

DEFINE_GUID_Local2(IID_IAMCopyCaptureFileProgress, 0x670d1d20, 0xa068, 0x11d0, 0xb3, 0xf0, 0x00, 0xaa, 0x00, 0x37, 0x61, 0xc5);
    
    MIDL_INTERFACE("670d1d20-a068-11d0-b3f0-00aa003761c5")
    IAMCopyCaptureFileProgress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Progress( /* [in] */ int iProgress) = 0;
    };

DEFINE_GUID_Local2(IID_IFilterGraph, 0x56a8689f, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

    MIDL_INTERFACE("56a8689f-0ad4-11ce-b03a-0020af0ba770")
    IFilterGraph : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddFilter( /* [in] */ IBaseFilter *pFilter, /* [string][in] */ LPCWSTR pName) = 0;
        virtual HRESULT STDMETHODCALLTYPE RemoveFilter( /* [in] */ IBaseFilter *pFilter) = 0;
        virtual HRESULT STDMETHODCALLTYPE EnumFilters( /* [out] */ IEnumFilters **ppEnum) = 0;
        virtual HRESULT STDMETHODCALLTYPE FindFilterByName( /* [string][in] */ LPCWSTR pName, /* [out] */ IBaseFilter **ppFilter) = 0;
        virtual HRESULT STDMETHODCALLTYPE ConnectDirect( /* [in] */ IPin *ppinOut, /* [in] */ IPin *ppinIn, /* [unique][in] */ const AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE Reconnect( /* [in] */ IPin *ppin) = 0;
        virtual HRESULT STDMETHODCALLTYPE Disconnect( /* [in] */ IPin *ppin) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetDefaultSyncSource( void) = 0;
    };

DEFINE_GUID_Local2(IID_IEnumMediaTypes, 0x89c31040, 0x846b, 0x11ce, 0x97, 0xd3, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a);

    MIDL_INTERFACE("89c31040-846b-11ce-97d3-00aa0055595a")
    IEnumMediaTypes : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( /* [in] */ ULONG cMediaTypes, /* [size_is][out] */ AM_MEDIA_TYPE **ppMediaTypes, /* [out] */ ULONG *pcFetched) = 0;
        virtual HRESULT STDMETHODCALLTYPE Skip( /* [in] */ ULONG cMediaTypes) = 0;
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Clone( /* [out] */ IEnumMediaTypes **ppEnum) = 0;
    };

DEFINE_GUID_Local2(IID_IMediaSample, 0x56a8689a, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
    
    MIDL_INTERFACE("56a8689a-0ad4-11ce-b03a-0020af0ba770")
    IMediaSample : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPointer( /* [out] */ BYTE **ppBuffer) = 0;
        virtual long STDMETHODCALLTYPE GetSize( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetTime( /* [out] */ REFERENCE_TIME *pTimeStart, /* [out] */ REFERENCE_TIME *pTimeEnd) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetTime( /* [in] */ REFERENCE_TIME *pTimeStart, /* [in] */ REFERENCE_TIME *pTimeEnd) = 0;
        virtual HRESULT STDMETHODCALLTYPE IsSyncPoint( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetSyncPoint( BOOL bIsSyncPoint) = 0;
        virtual HRESULT STDMETHODCALLTYPE IsPreroll( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetPreroll( BOOL bIsPreroll) = 0;
        virtual long STDMETHODCALLTYPE GetActualDataLength( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetActualDataLength( long __MIDL_0010) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetMediaType( AM_MEDIA_TYPE **ppMediaType) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( AM_MEDIA_TYPE *pMediaType) = 0;
        virtual HRESULT STDMETHODCALLTYPE IsDiscontinuity( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetDiscontinuity( BOOL bDiscontinuity) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetMediaTime( /* [out] */ LONGLONG *pTimeStart, /* [out] */ LONGLONG *pTimeEnd) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetMediaTime( /* [in] */ LONGLONG *pTimeStart, /* [in] */ LONGLONG *pTimeEnd) = 0;
    };

typedef struct _PinInfo
{
    IBaseFilter *pFilter;
    PIN_DIRECTION dir;
    WCHAR achName[ 128 ];
} PIN_INFO;

DEFINE_GUID_Local2(IID_IPin, 0x56a86891, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

    MIDL_INTERFACE("56a86891-0ad4-11ce-b03a-0020af0ba770")
    IPin : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( /* [in] */ IPin *pReceivePin, /* [in] */ const AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE ReceiveConnection( /* [in] */ IPin *pConnector, /* [in] */ const AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE ConnectedTo( /* [out] */ IPin **pPin) = 0;
        virtual HRESULT STDMETHODCALLTYPE ConnectionMediaType( /* [out] */ AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryPinInfo( /* [out] */ PIN_INFO *pInfo) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryDirection( /* [out] */ PIN_DIRECTION *pPinDir) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryId( /* [out] */ LPWSTR *Id) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryAccept( /* [in] */ const AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE EnumMediaTypes( /* [out] */ IEnumMediaTypes **ppEnum) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryInternalConnections( /* [out] */ IPin **apPin, /* [out][in] */ ULONG *nPin) = 0;
        virtual HRESULT STDMETHODCALLTYPE EndOfStream( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE BeginFlush( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE EndFlush( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE NewSegment( /* [in] */ REFERENCE_TIME tStart, /* [in] */ REFERENCE_TIME tStop, /* [in] */ double dRate) = 0;
    };

EXTERN_C const IID IID_IEnumPins;
    
    MIDL_INTERFACE("56a86892-0ad4-11ce-b03a-0020af0ba770")
    IEnumPins : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( /* [in] */ ULONG cPins, /* [size_is][out] */ IPin **ppPins, /* [out] */ ULONG *pcFetched) = 0;
        virtual HRESULT STDMETHODCALLTYPE Skip( /* [in] */ ULONG cPins) = 0;
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Clone( /* [out] */ IEnumPins **ppEnum) = 0;
    };

EXTERN_C const IID IID_ISampleGrabberCB;

    MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
    ISampleGrabberCB : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SampleCB( double SampleTime, IMediaSample *pSample) = 0;
        virtual HRESULT STDMETHODCALLTYPE BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen) = 0;
    };

typedef 
enum _FilterState
{	State_Stopped	= 0,
	State_Paused	= State_Stopped + 1,
	State_Running	= State_Paused + 1
} FILTER_STATE;


DEFINE_GUID_Local2(IID_IMediaFilter, 0x56a86899, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
    
    MIDL_INTERFACE("56a86899-0ad4-11ce-b03a-0020af0ba770")
    IMediaFilter : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Run( REFERENCE_TIME tStart) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetState( /* [in] */ DWORD dwMilliSecsTimeout, /* [out] */ FILTER_STATE *State) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetSyncSource( /* [in] */ IReferenceClock *pClock) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetSyncSource( /* [out] */ IReferenceClock **pClock) = 0;
    };

typedef struct _FilterInfo
{
    WCHAR achName[ 128 ];
    IFilterGraph *pGraph;
} FILTER_INFO;

DEFINE_GUID_Local2(IID_IBaseFilter, 0x56a86895, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

    MIDL_INTERFACE("56a86895-0ad4-11ce-b03a-0020af0ba770")
    IBaseFilter : public IMediaFilter
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumPins( /* [out] */ IEnumPins **ppEnum) = 0;
        virtual HRESULT STDMETHODCALLTYPE FindPin( /* [string][in] */ LPCWSTR Id, /* [out] */ IPin **ppPin) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryFilterInfo( /* [out] */ FILTER_INFO *pInfo) = 0;
        virtual HRESULT STDMETHODCALLTYPE JoinFilterGraph( /* [in] */ IFilterGraph *pGraph, /* [string][in] */ LPCWSTR pName) = 0;
        virtual HRESULT STDMETHODCALLTYPE QueryVendorInfo( /* [string][out] */ LPWSTR *pVendorInfo) = 0;
    };

DEFINE_GUID_Local2(IID_IGraphBuilder, 0x56a868a9, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

    MIDL_INTERFACE("56a868a9-0ad4-11ce-b03a-0020af0ba770")
    IGraphBuilder : public IFilterGraph
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( /* [in] */ IPin *ppinOut, /* [in] */ IPin *ppinIn) = 0;
        virtual HRESULT STDMETHODCALLTYPE Render( /* [in] */ IPin *ppinOut) = 0;
        virtual HRESULT STDMETHODCALLTYPE RenderFile( /* [in] */ LPCWSTR lpcwstrFile, /* [unique][in] */ LPCWSTR lpcwstrPlayList) = 0;
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( /* [in] */ LPCWSTR lpcwstrFileName, /* [unique][in] */ LPCWSTR lpcwstrFilterName, /* [out] */ IBaseFilter **ppFilter) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetLogFile( /* [in] */ DWORD_PTR hFile) = 0;
        virtual HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE ShouldOperationContinue( void) = 0;
    };

EXTERN_C const IID IID_IEnumFilters;
    
    MIDL_INTERFACE("56a86893-0ad4-11ce-b03a-0020af0ba770")
    IEnumFilters : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( /* [in] */ ULONG cFilters, /* [out] */ IBaseFilter **ppFilter, /* [out] */ ULONG *pcFetched) = 0;
        virtual HRESULT STDMETHODCALLTYPE Skip( /* [in] */ ULONG cFilters) = 0;
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Clone( /* [out] */ IEnumFilters **ppEnum) = 0;
    };

DEFINE_GUID_Local2(IID_IAMCameraControl, 0xC6E13370, 0x30AC, 0x11d0, 0xA1, 0x8C, 0x00, 0xA0, 0xC9, 0x11, 0x89, 0x56);
    
    MIDL_INTERFACE("C6E13370-30AC-11d0-A18C-00A0C9118956")
    IAMCameraControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRange( 
            /* [in] */ long Property,
            /* [out] */ long *pMin,
            /* [out] */ long *pMax,
            /* [out] */ long *pSteppingDelta,
            /* [out] */ long *pDefault,
            /* [out] */ long *pCapsFlags) = 0;

        virtual HRESULT STDMETHODCALLTYPE Set( /* [in] */ long Property, /* [in] */ long lValue, /* [in] */ long Flags) = 0;
        virtual HRESULT STDMETHODCALLTYPE Get( /* [in] */ long Property, /* [out] */ long *lValue, /* [out] */ long *Flags) = 0;
    };

DEFINE_GUID_Local2(IID_ISampleGrabber, 0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);

    MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
    ISampleGrabber : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE *pType) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE *pType) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( /* [out][in] */ long *pBufferSize, /* [out] */ long *pBuffer) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( /* [retval][out] */ IMediaSample **ppSample) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB *pCallback, long WhichMethodToCallback) = 0;
    };

DEFINE_GUID_Local2(IID_IMediaControl,0x56a868b1,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);
    
    MIDL_INTERFACE("56a868b1-0ad4-11ce-b03a-0020af0ba770")
    IMediaControl : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Run( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetState( /* [in] */ LONG msTimeout, /* [out] */ OAFilterState *pfs) = 0;
        virtual HRESULT STDMETHODCALLTYPE RenderFile( /* [in] */ BSTR strFilename) = 0;
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( /* [in] */ BSTR strFilename, /* [out] */ IDispatch **ppUnk) = 0;
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterCollection( /* [retval][out] */ IDispatch **ppUnk) = 0;
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RegFilterCollection( /* [retval][out] */ IDispatch **ppUnk) = 0;
        virtual HRESULT STDMETHODCALLTYPE StopWhenReady( void) = 0;
    };

DEFINE_GUID(IID_IMediaEvent,0x56a868b6,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);
    
    MIDL_INTERFACE("56a868b6-0ad4-11ce-b03a-0020af0ba770")
    IMediaEvent : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEventHandle( /* [out] */ OAEVENT *hEvent) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetEvent( /* [out] */ long *lEventCode, /* [out] */ LONG_PTR *lParam1, /* [out] */ LONG_PTR *lParam2, /* [in] */ long msTimeout) = 0;
        virtual HRESULT STDMETHODCALLTYPE WaitForCompletion( /* [in] */ long msTimeout, /* [out] */ long *pEvCode) = 0;
        virtual HRESULT STDMETHODCALLTYPE CancelDefaultHandling( /* [in] */ long lEvCode) = 0;
        virtual HRESULT STDMETHODCALLTYPE RestoreDefaultHandling( /* [in] */ long lEvCode) = 0;
        virtual HRESULT STDMETHODCALLTYPE FreeEventParams( /* [in] */ long lEvCode, /* [in] */ LONG_PTR lParam1, /* [in] */ LONG_PTR lParam2) = 0;
    };

DEFINE_GUID(IID_IMediaEventEx,0x56a868c0,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);
    
    MIDL_INTERFACE("56a868c0-0ad4-11ce-b03a-0020af0ba770")
    IMediaEventEx : public IMediaEvent
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetNotifyWindow( /* [in] */ OAHWND hwnd, /* [in] */ long lMsg, /* [in] */ LONG_PTR lInstanceData) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetNotifyFlags( /* [in] */ long lNoNotifyFlags) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetNotifyFlags( /* [out] */ long *lplNoNotifyFlags) = 0;
    };

typedef struct _VIDEO_STREAM_CONFIG_CAPS
{
    GUID guid;
    ULONG VideoStandard;
    SIZE InputSize;
    SIZE MinCroppingSize;
    SIZE MaxCroppingSize;
    int CropGranularityX;
    int CropGranularityY;
    int CropAlignX;
    int CropAlignY;
    SIZE MinOutputSize;
    SIZE MaxOutputSize;
    int OutputGranularityX;
    int OutputGranularityY;
    int StretchTapsX;
    int StretchTapsY;
    int ShrinkTapsX;
    int ShrinkTapsY;
    LONGLONG MinFrameInterval;
    LONGLONG MaxFrameInterval;
    LONG MinBitsPerSecond;
    LONG MaxBitsPerSecond;
} VIDEO_STREAM_CONFIG_CAPS;

typedef struct _AUDIO_STREAM_CONFIG_CAPS
{
    GUID guid;
    ULONG MinimumChannels;
    ULONG MaximumChannels;
    ULONG ChannelsGranularity;
    ULONG MinimumBitsPerSample;
    ULONG MaximumBitsPerSample;
    ULONG BitsPerSampleGranularity;
    ULONG MinimumSampleFrequency;
    ULONG MaximumSampleFrequency;
    ULONG SampleFrequencyGranularity;
} AUDIO_STREAM_CONFIG_CAPS;

DEFINE_GUID_Local2(IID_IAMStreamConfig, 0xC6E13340, 0x30AC, 0x11d0, 0xA1, 0x8C, 0x00, 0xA0, 0xC9, 0x11, 0x89, 0x56);

    MIDL_INTERFACE("C6E13340-30AC-11d0-A18C-00A0C9118956")
    IAMStreamConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFormat( /* [in] */ AM_MEDIA_TYPE *pmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetFormat( /* [out] */ AM_MEDIA_TYPE **ppmt) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities( /* [out] */ int *piCount, /* [out] */ int *piSize) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetStreamCaps( /* [in] */ int iIndex, /* [out] */ AM_MEDIA_TYPE **ppmt, /* [out] */ BYTE *pSCC) = 0;
    };

DEFINE_GUID_Local2(IID_ICaptureGraphBuilder2, 0x93E5A4E0, 0x2D50, 0x11d2, 0xAB, 0xFA, 0x00, 0xA0, 0xC9, 0xC6, 0xE3, 0x8D);

    MIDL_INTERFACE("93E5A4E0-2D50-11d2-ABFA-00A0C9C6E38D")
    ICaptureGraphBuilder2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFiltergraph( /* [in] */ IGraphBuilder *pfg) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetFiltergraph( /* [out] */ IGraphBuilder **ppfg) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetOutputFileName( 
            /* [in] */ const GUID *pType,
            /* [in] */ LPCOLESTR lpstrFile,
            /* [out] */ IBaseFilter **ppf,
            /* [out] */ IFileSinkFilter **ppSink) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE FindInterface( 
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IBaseFilter *pf,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderStream( 
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IUnknown *pSource,
            /* [in] */ IBaseFilter *pfCompressor,
            /* [in] */ IBaseFilter *pfRenderer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ControlStream( 
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IBaseFilter *pFilter,
            /* [in] */ REFERENCE_TIME *pstart,
            /* [in] */ REFERENCE_TIME *pstop,
            /* [in] */ WORD wStartCookie,
            /* [in] */ WORD wStopCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AllocCapFile( /* [in] */ LPCOLESTR lpstr, /* [in] */ DWORDLONG dwlSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyCaptureFile( 
            /* [in] */ LPOLESTR lpwstrOld,
            /* [in] */ LPOLESTR lpwstrNew,
            /* [in] */ int fAllowEscAbort,
            /* [in] */ IAMCopyCaptureFileProgress *pCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindPin( 
            /* [in] */ IUnknown *pSource,
            /* [in] */ PIN_DIRECTION pindir,
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ BOOL fUnconnected,
            /* [in] */ int num,
            /* [out] */ IPin **ppPin) = 0;
    };

DEFINE_GUID_Local2(IID_IAMAnalogVideoDecoder, 0xC6E13350, 0x30AC, 0x11d0, 0xA1, 0x8C, 0x00, 0xA0, 0xC9, 0x11, 0x89, 0x56);
    
    MIDL_INTERFACE("C6E13350-30AC-11d0-A18C-00A0C9118956")
    IAMAnalogVideoDecoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_AvailableTVFormats( /* [out] */ long *lAnalogVideoStandard) = 0;
        virtual HRESULT STDMETHODCALLTYPE put_TVFormat( /* [in] */ long lAnalogVideoStandard) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_TVFormat( /* [out] */ long *plAnalogVideoStandard) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_HorizontalLocked( /* [out] */ long *plLocked) = 0;
        virtual HRESULT STDMETHODCALLTYPE put_VCRHorizontalLocking( /* [in] */ long lVCRHorizontalLocking) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_VCRHorizontalLocking( /* [out] */ long *plVCRHorizontalLocking) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_NumberOfLines( /* [out] */ long *plNumberOfLines) = 0;
        virtual HRESULT STDMETHODCALLTYPE put_OutputEnable( /* [in] */ long lOutputEnable) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_OutputEnable( /* [out] */ long *plOutputEnable) = 0;
    };

DEFINE_GUID_Local2(IID_ICreateDevEnum, 0x29840822, 0x5B84, 0x11D0, 0xBD, 0x3B, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);

    MIDL_INTERFACE("29840822-5B84-11D0-BD3B-00A0C911CE86")
    ICreateDevEnum : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateClassEnumerator( 
            /* [in] */ REFCLSID clsidDeviceClass,
            /* [out] */ IEnumMoniker **ppEnumMoniker,
            /* [in] */ DWORD dwFlags) = 0;
    };

DEFINE_GUID_Local2(IID_IAMVideoProcAmp, 0xC6E13360, 0x30AC, 0x11d0, 0xA1, 0x8C, 0x00, 0xA0, 0xC9, 0x11, 0x89, 0x56);
    
    MIDL_INTERFACE("C6E13360-30AC-11d0-A18C-00A0C9118956")
    IAMVideoProcAmp : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRange( 
            /* [in] */ long Property,
            /* [out] */ long *pMin,
            /* [out] */ long *pMax,
            /* [out] */ long *pSteppingDelta,
            /* [out] */ long *pDefault,
            /* [out] */ long *pCapsFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Set( /* [in] */ long Property, /* [in] */ long lValue, /* [in] */ long Flags) = 0;
        virtual HRESULT STDMETHODCALLTYPE Get( /* [in] */ long Property, /* [out] */ long *lValue, /* [out] */ long *Flags) = 0;
    };
    

DEFINE_GUID_Local2(IID_IAMCrossbar, 0xC6E13380, 0x30AC, 0x11d0, 0xA1, 0x8C, 0x00, 0xA0, 0xC9, 0x11, 0x89, 0x56);

    MIDL_INTERFACE("C6E13380-30AC-11d0-A18C-00A0C9118956")
    IAMCrossbar : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_PinCounts( /* [out] */ long *OutputPinCount, /* [out] */ long *InputPinCount) = 0;
        virtual HRESULT STDMETHODCALLTYPE CanRoute( /* [in] */ long OutputPinIndex, /* [in] */ long InputPinIndex) = 0;
        virtual HRESULT STDMETHODCALLTYPE Route( /* [in] */ long OutputPinIndex, /* [in] */ long InputPinIndex) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_IsRoutedTo( /* [in] */ long OutputPinIndex, /* [out] */ long *InputPinIndex) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_CrossbarPinInfo( 
            /* [in] */ BOOL IsInputPin,
            /* [in] */ long PinIndex,
            /* [out] */ long *PinIndexRelated,
            /* [out] */ long *PhysicalType) = 0;
    };

/// system/ocidl.h

struct ISpecifyPropertyPages;

typedef ISpecifyPropertyPages *LPSPECIFYPROPERTYPAGES;
/*
struct tagCAUUID {
    ULONG cElems;
    GUID *pElems;
};*/
typedef struct tagCAUUID CAUUID;
typedef struct tagCAUUID *LPCAUUID;

/*
struct ISpecifyPropertyPages : public IUnknown {
public:
    virtual HRESULT WINAPI GetPages(CAUUID *pPages) = 0;
};
*/

/************************  End of extractions from DirectShow headers *************/

/******************/

///////////////////////////  HANDY FUNCTIONS  /////////////////////////////

void MyFreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}

	if (mt.pUnk != NULL)
	{
		// Unecessary because pUnk should not be used, but safest.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

void MyDeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	if (pmt == NULL) return;

	MyFreeMediaType(*pmt); 
	CoTaskMemFree(pmt);
}

//////////////////////////////  CALLBACK  ////////////////////////////////

// Callback class
class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	SampleGrabberCallback()
	{
		InitializeCriticalSection(&critSection);

		bufferSetup = newFrame = false;

		freezeCheck = 0;
		latestBufferLength = 0;

		hEvent = CreateEvent(NULL, true, false, NULL);
	}

	~SampleGrabberCallback()
	{
		ptrBuffer = NULL;
		DeleteCriticalSection(&critSection);
		CloseHandle(hEvent);

		if(bufferSetup) delete pixels;
	}	
	
	bool setupBuffer(int numBytesIn)
	{
		if(bufferSetup) return false;

		numBytes    = numBytesIn;
		pixels      = new unsigned char[numBytes];
		bufferSetup = true;
		newFrame    = false;

		latestBufferLength = 0;

		return true;
	}

#pragma region COM stuff

	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
	{
		(void)riid;
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}

	/// This method is meant to have less overhead
	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
	{
		(void)Time;
		if(WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0) return S_OK;
 
		HRESULT hr = pSample->GetPointer(&ptrBuffer);  
    		    	
		if(hr == S_OK)
		{
			latestBufferLength = pSample->GetActualDataLength();		    	

			if(latestBufferLength == numBytes)
			{
				EnterCriticalSection(&critSection);
	      			memcpy(pixels, ptrBuffer, latestBufferLength);	
					newFrame	= true;
					freezeCheck = 1;
				LeaveCriticalSection(&critSection);
				SetEvent(hEvent);
			}else{
				printf("ERROR: SampleCB() - buffer sizes do not match\n");
			}
		}
						
		return S_OK;
	}

	/// This method is meant to have more overhead
	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen) { (void)Time; (void)pBuffer; (void)BufferLen; return E_NOTIMPL; }

#pragma endregion

	int freezeCheck;
	int latestBufferLength;
	int numBytes;

	bool newFrame;
	bool bufferSetup;

	unsigned char * pixels;
	unsigned char * ptrBuffer;
	CRITICAL_SECTION critSection;
	HANDLE hEvent;	
};

//////////////////////////////  VIDEO DEVICE  ////////////////////////////////

//	Should this class also be the callback?		                                                
videoDevice::videoDevice()
{
	pCaptureGraph     = NULL; 
	pGraph            = NULL; 
	pControl          = NULL; 

	pVideoInputFilter = pDestFilter = pGrabberF = NULL;

	pGrabber          = NULL; // Grabs frame
	pMediaEvent       = NULL; 
	streamConf        = NULL;
	pAmMediaType      = NULL;

	//This is our callback class that processes the frame.
	sgCallback           = new SampleGrabberCallback();
	sgCallback->newFrame = false;

        //Default values for capture type
	videoType  = MEDIASUBTYPE_RGB24;
	connection = PhysConn_Video_Composite;
	storeConn  = 0;

	videoSize  = nFramesRunning = 0;
	width = height = tryWidth = tryHeight  = 0;

	nFramesForReconnect = 10000;
	myID = -1;

	tryDiffSize     	= false;
	useCrossbar     	= false;
	readyToCapture  	= false;
	sizeSet			= false;
	setupStarted		= false;
	specificFormat		= false;
	autoReconnect		= false;
	requestedFrameTime = -1;
	 
	memset(wDeviceName, 0, sizeof(WCHAR) * 255);
	memset(nDeviceName, 0, sizeof(char) * 255);
}

/// The only place we are doing new	                                                
void videoDevice::setSize(int w, int h)
{
	if(sizeSet)
	{
		if(verbose)printf("SETUP: Error device size should not be set more than once \n");
		return;
	}

	videoSize = (width = w) * (height = h) * 3;
	sizeSet   = true;
	pixels    = new unsigned char[videoSize];
	pBuffer   = new char[videoSize];

	memset(pixels, 0, videoSize);
	sgCallback->setupBuffer(videoSize);
}

// Borrowed from the SDK, use it to take apart the graph from
//  the capture device downstream to the null renderer                                                                   
void NukeDownstream(IBaseFilter *pBF,  IGraphBuilder *pGraph)
{
	IPin *pP, *pTo;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;
	HRESULT hr = pBF->EnumPins(&pins);
	pins->Reset();

	while (hr == NOERROR)
	{
		hr = pins->Next(1, &pP, &u);
		if (hr == S_OK && pP)
		{
			pP->ConnectedTo(&pTo);
			if (pTo)
			{
				hr = pTo->QueryPinInfo(&pininfo);
				if (hr == NOERROR)
				{
					if (pininfo.dir == PINDIR_INPUT)
					{
						NukeDownstream(pininfo.pFilter, pGraph);
						pGraph->Disconnect(pTo);
						pGraph->Disconnect(pP);
						pGraph->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
					pininfo.pFilter = NULL;
				}
				pTo->Release();
			}
			pP->Release();
		}
	}

	if (pins) pins->Release();
} 

// Also from SDK 	                                                
void videoDevice::destroyGraph()
{
	HRESULT hr = 0; // NULL; // S_OK ?
// 	int FuncRetval=0;
// 	int NumFilters=0;

	int i = 0;
	while (hr == NOERROR)	
	{
		IEnumFilters * pEnum = 0;
		ULONG cFetched;

		// We must get the enumerator again every time because removing a filter from the graph
		// invalidates the enumerator. We always get only the first filter from each enumerator.
		hr = pGraph->EnumFilters(&pEnum);
		if (FAILED(hr)) { if(verbose)printf("SETUP: pGraph->EnumFilters() failed. \n"); return; }

		IBaseFilter * pFilter = NULL;
		if (pEnum->Next(1, &pFilter, &cFetched) == S_OK)
		{
			FILTER_INFO FilterInfo={0};
			hr = pFilter->QueryFilterInfo(&FilterInfo);
			FilterInfo.pGraph->Release();

			int count = 0;
			char buffer[255];
			memset(buffer, 0, 255 * sizeof(char));

			while( FilterInfo.achName[count] != 0x00 ) 
			{
				// FIXME: use wide strings or UTF8 stuff
				buffer[count] = (char)FilterInfo.achName[count];
				count++;
			}

			if(verbose)printf("SETUP: removing filter %s...\n", buffer);
			hr = pGraph->RemoveFilter(pFilter);
			if (FAILED(hr)) { if(verbose)printf("SETUP: pGraph->RemoveFilter() failed. \n"); return; }
			if(verbose)printf("SETUP: filter removed %s  \n",buffer);

			pFilter->Release();
			pFilter = NULL;
		}
		else break;
		pEnum->Release();
		pEnum = NULL;
		i++;
	}

	return;
}

// Attempts to tear down graph and release filters
// Does checking to make sure it only is freeing if it needs to
videoDevice::~videoDevice()
{
	if(!setupStarted)
	{
		if(sgCallback)
		{
			sgCallback->Release();
			delete sgCallback;
		}

		return;
	}

	if(verbose)printf("\nSETUP: Disconnecting device %i\n", myID);

	HRESULT HR = 0; // NULL;

	//Stop the callback and free it
	if( (sgCallback) && (pGrabber) )
	{
		pGrabber->SetCallback(NULL, 1);

		if(verbose)printf("SETUP: freeing Grabber Callback\n");
		sgCallback->Release(); 	
        
		//delete our pixels 
		if(sizeSet){
			 delete[] pixels;
			 delete[] pBuffer;
		}

		delete sgCallback;
	}

	//Check to see if the graph is running, if so stop it. 
 	if( (pControl) )
	{
		HR = pControl->Pause();
		if (FAILED(HR)) if(verbose)printf("ERROR - Could not pause pControl\n");

		HR = pControl->Stop();
		if (FAILED(HR)) if(verbose)printf("ERROR - Could not stop pControl\n");
	}

	//Disconnect filters from capture device
	if( (pVideoInputFilter) ) NukeDownstream(pVideoInputFilter, pGraph);

#define _MY_RELEASE_OBJ(TheObj, TheMsg) \
	if( (TheObj) ) \
	{ \
		if(verbose) printf(TheMsg); \
		(TheObj)->Release(); \
		(TheObj) = 0;\
	}	

	_MY_RELEASE_OBJ(pDestFilter, "SETUP: freeing Renderer \n")
	_MY_RELEASE_OBJ(pVideoInputFilter, "SETUP: freeing Capture Source \n")
	_MY_RELEASE_OBJ(pGrabberF, "SETUP: freeing Grabber Filter  \n")
	_MY_RELEASE_OBJ(pGrabber, "SETUP: freeing Grabber  \n")
	_MY_RELEASE_OBJ(pControl, "SETUP: freeing Control   \n")
	_MY_RELEASE_OBJ(pMediaEvent, "SETUP: freeing Media Event  \n")
	_MY_RELEASE_OBJ(streamConf, "SETUP: freeing Stream  \n")

	if( (pAmMediaType) )
	{
		if(verbose)printf("SETUP: freeing Media Type  \n");
		MyDeleteMediaType(pAmMediaType);  			
	}

	_MY_RELEASE_OBJ(pMediaEvent, "SETUP: freeing Media Event  \n")

	//Destroy the graph
	if( (pGraph) ) destroyGraph();

	//Release and zero our capture graph and our main graph
	_MY_RELEASE_OBJ(pCaptureGraph, "SETUP: freeing Capture Graph \n")
	_MY_RELEASE_OBJ(pGraph, "SETUP: freeing Main Graph \n")

	//delete our pointers
	delete pDestFilter;
	delete pVideoInputFilter;
	delete pGrabberF;
	delete pGrabber;
	delete pControl;
	delete streamConf;
	delete pMediaEvent;
	delete pCaptureGraph;
	delete pGraph;

	if(verbose) printf("SETUP: Device %i disconnected and freed\n\n",myID);
#undef _MY_RELEASE_OBJ
}

//////////////////////////////  VIDEO INPUT  ////////////////////////////////
////////////////////////////  PUBLIC METHODS  ///////////////////////////////

// Create instances of videoDevice and add the various media subtypes to check.                                               
videoInput::videoInput()
{
	comInit();

	devicesFound     = 0;
	callbackSetCount = 0;
	bCallback        = true;

	//setup a max no of device objects
	for(int i=0; i<VI_MAX_CAMERAS; i++)  VDList[i] = new videoDevice();

	if(verbose)printf("\n***** VIDEOINPUT LIBRARY - %2.04f - TFW07 *****\n\n",VI_VERSION);

	//added for the pixelink firewire camera
/*
	// fourcc.h is required for these
 	MEDIASUBTYPE_Y800 = (GUID)FOURCCMap(FCC('Y800'));
 	MEDIASUBTYPE_Y8   = (GUID)FOURCCMap(FCC('Y8'));
 	MEDIASUBTYPE_GREY = (GUID)FOURCCMap(FCC('GREY'));
*/
	//The video types we support
	//in order of preference

	mediaSubtypes[0] 	= MEDIASUBTYPE_RGB24;
	mediaSubtypes[1] 	= MEDIASUBTYPE_RGB32;
	mediaSubtypes[2] 	= MEDIASUBTYPE_RGB555;
	mediaSubtypes[3] 	= MEDIASUBTYPE_RGB565;
	mediaSubtypes[4] 	= MEDIASUBTYPE_YUY2;
	mediaSubtypes[5] 	= MEDIASUBTYPE_YVYU;
	mediaSubtypes[6] 	= MEDIASUBTYPE_YUYV;
	mediaSubtypes[7] 	= MEDIASUBTYPE_IYUV;
	mediaSubtypes[8] 	= MEDIASUBTYPE_UYVY;
	mediaSubtypes[9] 	= MEDIASUBTYPE_YV12;
	mediaSubtypes[10]	= MEDIASUBTYPE_YVU9;
	mediaSubtypes[11] 	= MEDIASUBTYPE_Y411;
	mediaSubtypes[12] 	= MEDIASUBTYPE_Y41P;
	mediaSubtypes[13] 	= MEDIASUBTYPE_Y211;
	mediaSubtypes[14]	= MEDIASUBTYPE_AYUV;
/*
	//non standard
	mediaSubtypes[15]	= MEDIASUBTYPE_Y800;
	mediaSubtypes[16]	= MEDIASUBTYPE_Y8;
	mediaSubtypes[17]	= MEDIASUBTYPE_GREY;	
*/
	//The video formats we support
	formatTypes[VI_NTSC_M]		= AnalogVideo_NTSC_M;
	formatTypes[VI_NTSC_M_J]	= AnalogVideo_NTSC_M_J;
	formatTypes[VI_NTSC_433]	= AnalogVideo_NTSC_433;

	formatTypes[VI_PAL_B]  = AnalogVideo_PAL_B;
	formatTypes[VI_PAL_D]  = AnalogVideo_PAL_D;
	formatTypes[VI_PAL_G]  = AnalogVideo_PAL_G;
	formatTypes[VI_PAL_H]  = AnalogVideo_PAL_H;
	formatTypes[VI_PAL_I]  = AnalogVideo_PAL_I;
	formatTypes[VI_PAL_M]  = AnalogVideo_PAL_M;
	formatTypes[VI_PAL_N]  = AnalogVideo_PAL_N;
	formatTypes[VI_PAL_NC] = AnalogVideo_PAL_N_COMBO;

	formatTypes[VI_SECAM_B]  = AnalogVideo_SECAM_B;
	formatTypes[VI_SECAM_D]  = AnalogVideo_SECAM_D;
	formatTypes[VI_SECAM_G]  = AnalogVideo_SECAM_G;
	formatTypes[VI_SECAM_H]  = AnalogVideo_SECAM_H;
	formatTypes[VI_SECAM_K]  = AnalogVideo_SECAM_K;
	formatTypes[VI_SECAM_K1] = AnalogVideo_SECAM_K1;
	formatTypes[VI_SECAM_L]  = AnalogVideo_SECAM_L;

	propBrightness   = VideoProcAmp_Brightness;
	propContrast     = VideoProcAmp_Contrast;
	propHue          = VideoProcAmp_Hue;
	propSaturation   = VideoProcAmp_Saturation;
	propSharpness    = VideoProcAmp_Sharpness;
	propGamma        = VideoProcAmp_Gamma;
	propColorEnable	 = VideoProcAmp_ColorEnable;
	propWhiteBalance = VideoProcAmp_WhiteBalance;
	propGain         = VideoProcAmp_Gain;

	propBacklightCompensation = VideoProcAmp_BacklightCompensation;

	propPan		= CameraControl_Pan;
	propTilt	= CameraControl_Tilt;
	propRoll	= CameraControl_Roll;
	propZoom	= CameraControl_Zoom;
	propExposure   	= CameraControl_Exposure;
	propIris	= CameraControl_Iris;
	propFocus	= CameraControl_Focus;
}

// static - set whether messages get printed to console or not
void videoInput::setVerbose(bool _verbose) { verbose = _verbose; }

// change to use callback or regular capture
// callback tells you when a new frame has arrived
// but non-callback won't - but is single threaded
void videoInput::setUseCallback(bool useCallback){
	if(callbackSetCount == 0){
		bCallback = useCallback;
		callbackSetCount = 1;
	}else{
		printf("ERROR: setUseCallback can only be called before setup\n");
	}
}

// Set the requested framerate - no guarantee you will get this
void videoInput::setIdealFramerate(int deviceNumber, int idealFramerate){
	if(deviceNumber >= VI_MAX_CAMERAS || VDList[deviceNumber]->readyToCapture) return;

	if(idealFramerate <= 0) return;

	VDList[deviceNumber]->requestedFrameTime = (unsigned long)(10000000 / idealFramerate);
}

// Set the requested framerate - no guarantee you will get this
void videoInput::setAutoReconnectOnFreeze(int deviceNumber, bool doReconnect, int numMissedFramesBeforeReconnect){
	if(deviceNumber >= VI_MAX_CAMERAS) return;

	VDList[deviceNumber]->autoReconnect       = doReconnect;
	VDList[deviceNumber]->nFramesForReconnect = numMissedFramesBeforeReconnect;	
}

// Setup a device with specific size and connection. connection/w/h can be set to -1 if default values required
bool videoInput::setupDevice(int deviceNumber, int w, int h, int connection)
{
	if(deviceNumber >= VI_MAX_CAMERAS || VDList[deviceNumber]->readyToCapture) return false;
	if(w > 0 && h > 0) setAttemptCaptureSize(deviceNumber,w,h);
	if(connection > -1) setPhyCon(deviceNumber, connection);
	return setup(deviceNumber);
}

// Setup the default video format of the device. Must be called after setup
// See #define formats in header file (eg VI_NTSC_M )
bool videoInput::setFormat(int deviceNumber, int format)
{
	if(deviceNumber >= VI_MAX_CAMERAS || !VDList[deviceNumber]->readyToCapture) return false;

	bool returnVal = false;

	if(format >= 0 && format < VI_NUM_FORMATS)
	{
		VDList[deviceNumber]->formatType = formatTypes[format];	
		VDList[deviceNumber]->specificFormat = true;
		
		if(VDList[deviceNumber]->specificFormat)
		{
			HRESULT hr = getDevice(&VDList[deviceNumber]->pVideoInputFilter, deviceNumber, VDList[deviceNumber]->wDeviceName, VDList[deviceNumber]->nDeviceName);

			if(hr != S_OK) return false;

			IAMAnalogVideoDecoder *pVideoDec = NULL;    	
	   		hr = VDList[deviceNumber]->pCaptureGraph->FindInterface(NULL, &MEDIATYPE_Video, VDList[deviceNumber]->pVideoInputFilter, IID_IAMAnalogVideoDecoder, (void **)&pVideoDec);
			
			//in case the settings window some how freed them first
			if(VDList[deviceNumber]->pVideoInputFilter)VDList[deviceNumber]->pVideoInputFilter->Release();  		
			if(VDList[deviceNumber]->pVideoInputFilter)VDList[deviceNumber]->pVideoInputFilter = NULL;  

			if(FAILED(hr)){
				printf("SETUP: couldn't set requested format\n");
			}else{
				long lValue = 0;
				hr = pVideoDec->get_AvailableTVFormats(&lValue);

				if( SUCCEEDED(hr) && (lValue & VDList[deviceNumber]->formatType) )
	   			{
					hr = pVideoDec->put_TVFormat(VDList[deviceNumber]->formatType);

					if( FAILED(hr) ){
						printf("SETUP: couldn't set requested format\n");
					}else{
						returnVal = true;	
					}
			   	}

				pVideoDec->Release();
				pVideoDec = NULL;			   	
			}			
		}		
	}
	
	return returnVal;
}

// Our static function for returning device names - thanks Peter!
// Must call listDevices first.
char videoInput::deviceNames[VI_MAX_CAMERAS][255]={{0}};

char* videoInput::getDeviceName(int devID) { return ( devID < 0 || devID >= VI_MAX_CAMERAS ) ? NULL : deviceNames[devID]; }

// Uses a supplied buffer
bool videoInput::getPixels(int id, unsigned char * dstBuffer, bool flipRedAndBlue, bool flipImage)
{
	if(!isDeviceSetup(id)) return false;

	if(bCallback)
	{
		//callback capture	
		DWORD result = WaitForSingleObject(VDList[id]->sgCallback->hEvent, 1000);
		if( result != WAIT_OBJECT_0) return false;

		//double paranoia - mutexing with both event and critical section
		EnterCriticalSection(&VDList[id]->sgCallback->critSection);

		unsigned char * src = VDList[id]->sgCallback->pixels;
		unsigned char * dst = dstBuffer;
		int height = VDList[id]->height;
		int width  = VDList[id]->width; 

		processPixels(src, dst, width, height, flipRedAndBlue, flipImage);
		VDList[id]->sgCallback->newFrame = false;

		LeaveCriticalSection(&VDList[id]->sgCallback->critSection);	

		ResetEvent(VDList[id]->sgCallback->hEvent);

		return true;
	}

	//regular capture method
	long bufferSize = VDList[id]->videoSize;
	if(VDList[id]->pGrabber->GetCurrentBuffer(&bufferSize, (long *)VDList[id]->pBuffer) != S_OK)
	{
		if(verbose)printf("ERROR: GetPixels() - Unable to grab frame for device %i\n", id);
		return false;
	}

	if (VDList[id]->videoSize == bufferSize)
	{
		unsigned char * src = (unsigned char * )VDList[id]->pBuffer;
		unsigned char * dst = dstBuffer;
		int height = VDList[id]->height;
		int width  = VDList[id]->width; 

		processPixels(src, dst, width, height, flipRedAndBlue, flipImage);
		return true;
	}

	if(verbose)printf("ERROR: GetPixels() - bufferSizes do not match!\n");

	return false;
}

// Returns a buffer
unsigned char * videoInput::getPixels(int id, bool flipRedAndBlue, bool flipImage)
{
	if(isDeviceSetup(id)) getPixels(id, VDList[id]->pixels, flipRedAndBlue, flipImage);

	return VDList[id]->pixels;
}

bool videoInput::isFrameNew(int id)
{
	if(!isDeviceSetup(id)) { return false; }
	if(!bCallback) { return true; }

	bool result = false, freeze = false;

	//again super paranoia!
	EnterCriticalSection(&VDList[id]->sgCallback->critSection);
	result = VDList[id]->sgCallback->newFrame;

	//we need to give it some time at the begining to start up so lets check after 400 frames
	freeze = (VDList[id]->nFramesRunning > 400 && VDList[id]->sgCallback->freezeCheck > VDList[id]->nFramesForReconnect );

	//we increment the freezeCheck var here - the callback resets it to 1 
	//so as long as the callback is running this var should never get too high.
	//if the callback is not running then this number will get high and trigger the freeze action below 
	VDList[id]->sgCallback->freezeCheck++;

	LeaveCriticalSection(&VDList[id]->sgCallback->critSection);	

	VDList[id]->nFramesRunning++;

	if(freeze && VDList[id]->autoReconnect)
	{
		if(verbose)printf("ERROR: Device seems frozen - attempting to reconnect\n"); 

		if( !restartDevice(VDList[id]->myID) )
		{
			if(verbose)printf("ERROR: Unable to reconnect to device\n");
		}
		else
		{
			if(verbose)printf("SUCCESS: Able to reconnect to device\n");
		}
	}

	return result;	
}

// Gives us a little pop up window to adjust settings
// We do this in a seperate thread now
void __cdecl videoInput::basicThread(void * objPtr)
{
	//get a reference to the video device
	//not a copy as we need to free the filter
	videoDevice * vd = *( (videoDevice **)(objPtr) );
	ShowFilterPropertyPages(vd->pVideoInputFilter);	

	//now we free the filter and make sure it set to NULL
	if(vd->pVideoInputFilter)vd->pVideoInputFilter->Release();
	if(vd->pVideoInputFilter)vd->pVideoInputFilter = NULL;

	return;
}

void videoInput::showSettingsWindow(int id)
{
	if(!isDeviceSetup(id)) return;

	HANDLE myTempThread;

	//we reconnect to the device as we have freed our reference to it
	//why have we freed our reference? because there seemed to be an issue with some mpeg devices if we didn't
	if(getDevice(&VDList[id]->pVideoInputFilter, id, VDList[id]->wDeviceName, VDList[id]->nDeviceName) != S_OK) return;

	myTempThread = (HANDLE)_beginthread(basicThread, 0, (void *)&VDList[id]);  
}

// Shutsdown the device, deletes the object and creates a new object so it is ready to be setup again                                          
void videoInput::stopDevice(int id)
{
	if(id < 0 || id >= VI_MAX_CAMERAS) return;

	delete VDList[id];
	VDList[id] = new videoDevice();
}

// Restarts the device with the same settings it was using
bool videoInput::restartDevice(int id)
{
	if(!isDeviceSetup(id)) return false;

	int conn = VDList[id]->storeConn;
	int tmpW = VDList[id]->width;
	int tmpH = VDList[id]->height;

	bool bFormat    = VDList[id]->specificFormat;
	long format     = VDList[id]->formatType;

	int nReconnect	= VDList[id]->nFramesForReconnect;
	bool bReconnect = VDList[id]->autoReconnect;

	unsigned long avgFrameTime = VDList[id]->requestedFrameTime;
	
	stopDevice(id);

	//set our fps if needed
	if( avgFrameTime != -1) { VDList[id]->requestedFrameTime = avgFrameTime; }

	if( !setupDevice(id, tmpW, tmpH, conn) ) return false;

	//reapply the format - ntsc / pal etc

	if( bFormat ) setFormat(id, format);
	if( bReconnect ) setAutoReconnectOnFreeze(id, true, nReconnect);

	return true;
}

// Shuts down all devices, deletes objects and unitializes com if needed
videoInput::~videoInput()
{
	for(int i = 0; i < VI_MAX_CAMERAS; i++) delete VDList[i];

	comUnInit();
}

//////////////////////////////  VIDEO INPUT  ////////////////////////////////
////////////////////////////  PRIVATE METHODS  //////////////////////////////

//keeps track of how many instances of VI are being used
//don't touch
static int comInitCount = 0;

// We only should init com if it hasn't been done so by our apps thread
// Use a static counter to keep track of other times it has been inited
// (do we need to worry about multithreaded apps?)
bool videoInput::comInit()
{
	HRESULT hr = 0; // NULL;

	// no need for us to start com more than once
	if(comInitCount == 0 )
	{
		// Initialize the COM library.
		// CoInitializeEx so videoInput can run in another thread
	#ifdef VI_COM_MULTI_THREADED
		hr = CoInitializeEx(NULL,COINIT_MULTITHREADED);
	#else 
		hr = CoInitialize(NULL);
	#endif
		//this is the only case where there might be a problem
		//if another library has started com as single threaded 
		//and we need it multi-threaded - send warning but don't fail
		if( hr == RPC_E_CHANGED_MODE)
		{
			 if(verbose)printf("SETUP - COM already setup - threaded VI might not be possible\n");
		}
	}

	comInitCount++; 
	return true;
}

// Same as above but to unitialize com, decreases counter and frees com 
// if no one else is using it                                           
bool videoInput::comUnInit()
{
	// decrease the count of instances using com
	if(comInitCount > 0) comInitCount--;

   	if(comInitCount != 0) return false;

	// if there are no instances left - uninitialize com
	CoUninitialize();
	return true;	
}

// This is the size we ask for - we might not get it though :)
void videoInput::setAttemptCaptureSize(int id, int w, int h)
{
	VDList[id]->tryWidth    = w;
	VDList[id]->tryHeight   = h;
	VDList[id]->tryDiffSize = true;	
}

// Set the connection type (maybe move to private?)                                           
void videoInput::setPhyCon(int id, int conn)
{
	switch(conn)
	{
		case 0: VDList[id]->connection = PhysConn_Video_Composite; break;
		case 1:	VDList[id]->connection = PhysConn_Video_SVideo; break;
		case 2:	VDList[id]->connection = PhysConn_Video_Tuner; break;
		case 3: VDList[id]->connection = PhysConn_Video_USB; break;	
		case 4: VDList[id]->connection = PhysConn_Video_1394; break;	
		default: return; //if it is not these types don't set crossbar
		break;
	}

	VDList[id]->storeConn	= conn;
	VDList[id]->useCrossbar	= true;
}

// Check that we are not trying to setup a non-existant device. Then start the graph building!                                           
bool videoInput::setup(int deviceNumber)
{
	devicesFound = getDeviceCount();

	if(deviceNumber>devicesFound-1)
	{	
		if(verbose)printf("SETUP: device[%i] not found - you have %i devices available\n", deviceNumber, devicesFound);
		if(devicesFound>=0) if(verbose)printf("SETUP: this means that the last device you can use is device[%i] \n",  devicesFound-1);
		return false;
	}

	if(VDList[deviceNumber]->readyToCapture)
	{
		if(verbose)printf("SETUP: can't setup, device %i is currently being used\n",VDList[deviceNumber]->myID);
		return false;
	}

	return (start(deviceNumber, VDList[deviceNumber]) == S_OK);
}

// Does both vertical buffer flipping and bgr to rgb swapping
// You have any combination of those.
void videoInput::processPixels(unsigned char * src, unsigned char * dst, int width, int height, bool bRGB, bool bFlip)
{
	int widthInBytes = width * 3;
	int numBytes = widthInBytes * height;
	
	if(!bRGB)
	{
		if(!bFlip) { memcpy(dst, src, numBytes); return; }

		for(int y = 0; y < height; y++)
			memcpy(dst + (y * widthInBytes), src + ( (height -y -1) * widthInBytes), widthInBytes);

		return;
	}

	if(bFlip)
	{
		int x = 0;
		src += (height - 1) * widthInBytes;
			
		for(int i = 0; i < numBytes; i+=3, src += 3, x++)
		{
			if(x >= width)
			{
				x = 0;
				src -= widthInBytes*2;
			}

			*dst++ = *(src+2);
			*dst++ = *(src+1);
			*dst++ = *src;
		}

		return;
	}

	for(int i = 0; i < numBytes; i+=3, src += 3)
	{
		*dst++ = *(src+2);
		*dst++ = *(src+1);
		*dst++ = *src;
	}
}

void videoInput::getMediaSubtypeAsString(GUID type, char * typeAsString)
{
#if defined(_MSC_VER) && (_MSC_VER > 1300)
#define Safe_Sprintf(Value) sprintf_s(tmpStr, 8, Value)
#else
#define Safe_Sprintf(Value) sprintf(tmpStr, Value)
#endif
	char tmpStr[8];
	if( type == MEDIASUBTYPE_RGB24) Safe_Sprintf("RGB24");
	else if(type == MEDIASUBTYPE_RGB32) Safe_Sprintf("RGB32");
	else if(type == MEDIASUBTYPE_RGB555) Safe_Sprintf("RGB555");
	else if(type == MEDIASUBTYPE_RGB565) Safe_Sprintf("RGB565");
	else if(type == MEDIASUBTYPE_YUY2)Safe_Sprintf("YUY2");
	else if(type == MEDIASUBTYPE_YVYU)Safe_Sprintf("YVYU");
	else if(type == MEDIASUBTYPE_YUYV)Safe_Sprintf("YUYV");
	else if(type == MEDIASUBTYPE_IYUV)Safe_Sprintf("IYUV");
	else if(type == MEDIASUBTYPE_UYVY)Safe_Sprintf("UYVY");
	else if(type == MEDIASUBTYPE_YV12)Safe_Sprintf("YV12");
	else if(type == MEDIASUBTYPE_YVU9)Safe_Sprintf("YVU9");
	else if(type == MEDIASUBTYPE_Y411)Safe_Sprintf("Y411");
	else if(type == MEDIASUBTYPE_Y41P)Safe_Sprintf("Y41P");
	else if(type == MEDIASUBTYPE_Y211)Safe_Sprintf("Y211");
	else if(type == MEDIASUBTYPE_AYUV)Safe_Sprintf("AYUV");
/*
	else if(type == MEDIASUBTYPE_Y800)Safe_Sprintf("Y800");
	else if(type == MEDIASUBTYPE_Y8)  Safe_Sprintf("Y8");
	else if(type == MEDIASUBTYPE_GREY)Safe_Sprintf("GREY");
*/
	else Safe_Sprintf("OTHER");

#undef Safe_Sprintf

	memcpy(typeAsString, tmpStr, sizeof(char)*8);
}

static void findClosestSizeAndSubtype(videoDevice * VD, int widthIn, int heightIn, int &widthOut, int &heightOut, GUID & mediatypeOut)
{
	HRESULT hr;

	//find perfect match or closest size
	int nearW = 9999999, nearH = 9999999;
	bool foundClosestMatch 	= true;

	int iCount = 0, iSize = 0;
	hr = VD->streamConf->GetNumberOfCapabilities(&iCount, &iSize);

	if (iSize != sizeof(VIDEO_STREAM_CONFIG_CAPS)) return;

	//For each format type RGB24 YUV2 etc
	for (int iFormat = 0; iFormat < iCount; iFormat++)
	{
		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig;
		hr =  VD->streamConf->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);

		if (SUCCEEDED(hr))
		{
			//his is how many diff sizes are available for the format
			int stepX = scc.OutputGranularityX, stepY = scc.OutputGranularityY;

			int tempW = 999999, tempH = 999999;

			//Don't want to get stuck in a loop
			if(stepX < 1 || stepY < 1) continue;

			//if(verbose)printf("min is %i %i max is %i %i - res is %i %i \n", scc.MinOutputSize.cx, scc.MinOutputSize.cy,  scc.MaxOutputSize.cx,  scc.MaxOutputSize.cy, stepX, stepY);
			//if(verbose)printf("min frame duration is %i  max duration is %i\n", scc.MinFrameInterval, scc.MaxFrameInterval);

			bool exactMatch  = false, exactMatchX = false, exactMatchY = false;

			for(int x = scc.MinOutputSize.cx; x <= scc.MaxOutputSize.cx; x+= stepX)
			{
				//If we find an exact match
				if( widthIn == x ){
					exactMatchX = true;
					tempW = x;			            		
				}
				//Otherwise lets find the closest match based on width
				else if( abs(widthIn-x) < abs(widthIn-tempW) ){
					tempW = x;			            		
				}
			}	
		            
			for(int y = scc.MinOutputSize.cy; y <= scc.MaxOutputSize.cy; y+= stepY)
			{
				//If we find an exact match
				if( heightIn == y)
				{
					exactMatchY = true;
					tempH = y;		    			            		
				}
				//Otherwise lets find the closest match based on height
				else if( abs(heightIn-y) < abs(heightIn-tempH) )
				{
					tempH = y;		    			            		
				}
			}			           		            

			//see if we have an exact match!
			if(exactMatchX && exactMatchY)
			{
				foundClosestMatch = false;
				exactMatch = true;

				widthOut     = widthIn;
				heightOut    = heightIn;
				mediatypeOut = pmtConfig->subtype;	
			}       
			//otherwise lets see if this filters closest size is the closest 
			//available. the closest size is determined by the sum difference
			//of the widths and heights
			else if( abs(widthIn - tempW) + abs(heightIn - tempH)  < abs(widthIn - nearW) + abs(heightIn - nearH) )
			{
				widthOut     = nearW = tempW;
				heightOut    = nearH = tempH;
				mediatypeOut = pmtConfig->subtype;	
			}

			MyDeleteMediaType(pmtConfig);
		            			     		            
			//If we have found an exact match no need to search anymore
			if(exactMatch) break;
		}
	}	
}

static bool setSizeAndSubtype(videoDevice * VD, int attemptWidth, int attemptHeight, GUID mediatype)
{
	VIDEOINFOHEADER *pVih =  reinterpret_cast<VIDEOINFOHEADER*>(VD->pAmMediaType->pbFormat);

	//store current size
//	int tmpWidth  = HEADER(pVih)->biWidth;
//	int tmpHeight = HEADER(pVih)->biHeight;	
	AM_MEDIA_TYPE* tmpType = NULL;
	
	if(VD->streamConf->GetFormat(&tmpType) != S_OK) return false;

	//set new size:
	//width and height
	HEADER(pVih)->biWidth  = attemptWidth;
	HEADER(pVih)->biHeight = attemptHeight;	

	VD->pAmMediaType->formattype = FORMAT_VideoInfo;
	VD->pAmMediaType->majortype  = MEDIATYPE_Video; 
	VD->pAmMediaType->subtype    = mediatype;
	
	//buffer size
	VD->pAmMediaType->lSampleSize = attemptWidth*attemptHeight*3;
	
	//set fps if requested 
	if( VD->requestedFrameTime != -1) pVih->AvgTimePerFrame = VD->requestedFrameTime;

	//okay lets try new size	
	if(VD->streamConf->SetFormat(VD->pAmMediaType) == S_OK)
	{
		if( tmpType != NULL )MyDeleteMediaType(tmpType);
		return true;
	}else{
		VD->streamConf->SetFormat(tmpType);		
		if( tmpType != NULL )MyDeleteMediaType(tmpType);
	}

	return false;
}

// Where all the work happens. Attempts to build a graph for the specified device                                   
int videoInput::start(int devID, videoDevice *VD)
{
#define CHECK_HR(TheMsg) \
	if (FAILED(hr)){ \
		 if(verbose)printf(TheMsg); \
		 stopDevice(devID); \
		 return hr;\
	}

	HRESULT hr       = 0; // NULL;
	VD->myID         = devID;
	VD->setupStarted = true;
	CAPTURE_MODE     = PIN_CATEGORY_CAPTURE; //Don't worry - it ends up being preview (which is faster)
	callbackSetCount = 1;  //make sure callback method is not changed after setup called

	if(verbose)printf("SETUP: Setting up device %i\n",devID);

	// Create the filter graph manager and query for interfaces.
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&VD->pCaptureGraph);
	if (FAILED(hr))	// FAILED is a macro that tests the return value
	{
		if(verbose)printf("ERROR - Could not create the Filter Graph Manager\n");
		return hr;
	}

	// Create the Filter Graph Manager.
	hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void**)&VD->pGraph);
	CHECK_HR("ERROR - Could not add the graph builder!\n");
    
	hr = VD->pCaptureGraph->SetFiltergraph(VD->pGraph);
	CHECK_HR("ERROR - Could not set filtergraph\n")

	hr = VD->pGraph->QueryInterface(IID_IMediaControl, (void **)&VD->pControl);
	CHECK_HR("ERROR - Could not create the Media Control object\n")

	// Find video device and add to graph
	hr = getDevice(&VD->pVideoInputFilter, devID, VD->wDeviceName, VD->nDeviceName);
	CHECK_HR("ERROR - Could not find specified video device\n");

	if(verbose)printf("SETUP: %s\n", VD->nDeviceName);
	hr = VD->pGraph->AddFilter(VD->pVideoInputFilter, VD->wDeviceName);

	//LOOK FOR PREVIEW PIN IF THERE IS NONE THEN WE USE CAPTURE PIN AND THEN SMART TEE TO PREVIEW
	IAMStreamConfig *streamConfTest = NULL;

	hr = VD->pCaptureGraph->FindInterface(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, VD->pVideoInputFilter, IID_IAMStreamConfig, (void **)&streamConfTest);

	if(FAILED(hr))
	{
		if(verbose)printf("SETUP: Couldn't find preview pin using SmartTee\n");
	}
	else{
		 CAPTURE_MODE = PIN_CATEGORY_PREVIEW;
		 streamConfTest->Release();
		 streamConfTest = NULL;
	}

	// CROSSBAR (SELECT PHYSICAL INPUT TYPE)//
	// my own function that checks to see if the device can support a crossbar and if so it routes it.  
	// webcams tend not to have a crossbar so this function will also detect a webcams and not apply the crossbar 
	if(VD->useCrossbar)
	{
		if(verbose)printf("SETUP: Checking crossbar\n");
		routeCrossbar(&VD->pCaptureGraph, &VD->pVideoInputFilter, VD->connection, CAPTURE_MODE);
	}

	//we do this because webcams don't have a preview mode
	hr = VD->pCaptureGraph->FindInterface(&CAPTURE_MODE, &MEDIATYPE_Video, VD->pVideoInputFilter, IID_IAMStreamConfig, (void **)&VD->streamConf);
	CHECK_HR("ERROR: Couldn't config the stream!\n");
	
	//NOW LETS DEAL WITH GETTING THE RIGHT SIZE
	hr = VD->streamConf->GetFormat(&VD->pAmMediaType);
	CHECK_HR("ERROR: Couldn't getFormat for pAmMediaType!\n");

	VIDEOINFOHEADER *pVih =  reinterpret_cast<VIDEOINFOHEADER*>(VD->pAmMediaType->pbFormat);
	int currentWidth	=  HEADER(pVih)->biWidth;
	int currentHeight	=  HEADER(pVih)->biHeight;

	bool customSize = VD->tryDiffSize;
	bool foundSize  = false;

	if(customSize)
	{
		if(verbose)	printf("SETUP: Default Format is set to %i by %i \n", currentWidth, currentHeight);

		char guidStr[8];
		for(int i = 0; i < VI_NUM_TYPES; i++)
		{
			getMediaSubtypeAsString(mediaSubtypes[i], guidStr);

			if(verbose)printf("SETUP: trying format %s @ %i by %i\n", guidStr, VD->tryWidth, VD->tryHeight);

			if( setSizeAndSubtype(VD, VD->tryWidth, VD->tryHeight, mediaSubtypes[i]) )
			{
				VD->setSize(VD->tryWidth, VD->tryHeight);
				foundSize = true;
				break;
			}
		}
		
		//if we didn't find the requested size - lets try and find the closest matching size
		if( foundSize == false ){
			if( verbose )printf("SETUP: couldn't find requested size - searching for closest matching size\n");

			int closestWidth  = -1;
			int closestHeight = -1;
			GUID newMediaSubtype;

			findClosestSizeAndSubtype(VD, VD->tryWidth, VD->tryHeight, closestWidth, closestHeight, newMediaSubtype);
				
			if( closestWidth != -1 && closestHeight != -1){
				getMediaSubtypeAsString(newMediaSubtype, guidStr);

				if(verbose)printf("SETUP: closest supported size is %s @ %i %i\n", guidStr, closestWidth, closestHeight);
				if( setSizeAndSubtype(VD, closestWidth, closestHeight, newMediaSubtype) ){
					VD->setSize(closestWidth, closestHeight);
					foundSize = true;
				}
			}
		}
	}

	//if we didn't specify a custom size or if we did but couldn't find it lets setup with the default settings
	if(!customSize || !foundSize)
	{
		if( VD->requestedFrameTime != -1 )
		{
			pVih->AvgTimePerFrame  = VD->requestedFrameTime;
			hr = VD->streamConf->SetFormat(VD->pAmMediaType);		 
		} 
		VD->setSize(currentWidth, currentHeight);
	}

	//SAMPLE GRABBER (ALLOWS US TO GRAB THE BUFFER)//
	// Create the Sample Grabber.
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void**)&VD->pGrabberF);
	CHECK_HR("Could not Create Sample Grabber - CoCreateInstance()\n")

	hr = VD->pGraph->AddFilter(VD->pGrabberF, L"Sample Grabber");
	CHECK_HR("Could not add Sample Grabber - AddFilter()\n")

	hr = VD->pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&VD->pGrabber);
	CHECK_HR("ERROR: Could not query SampleGrabber\n")

	//Set Params - One Shot should be false unless you want to capture just one buffer
	hr = VD->pGrabber->SetOneShot(FALSE);
	hr = VD->pGrabber->SetBufferSamples( bCallback ? FALSE : TRUE);	

	if(bCallback){
		//Tell the grabber to use our callback function - 0 is for SampleCB and 1 for BufferCB
		//We use SampleCB
		hr = VD->pGrabber->SetCallback(VD->sgCallback, 0); 
		CHECK_HR("ERROR: problem setting callback\n")

		if(verbose)printf("SETUP: Capture callback set\n");
	}
	
	//MEDIA CONVERSION
	//Get video properties from the stream's mediatype and apply to the grabber (otherwise we don't get an RGB image)	
	//zero the media type - lets try this :) - maybe this works?
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt,sizeof(AM_MEDIA_TYPE));
	
	mt.majortype 	= MEDIATYPE_Video;
	mt.subtype 		= MEDIASUBTYPE_RGB24;
	mt.formattype 	= FORMAT_VideoInfo;
	
	//VD->pAmMediaType->subtype = VD->videoType; 
	hr = VD->pGrabber->SetMediaType(&mt);
	
	//lets try freeing our stream conf here too 
	//this will fail if the device is already running
	if(VD->streamConf){
		VD->streamConf->Release();
		VD->streamConf = NULL;
	}else{
		if(verbose)printf("ERROR: connecting device - prehaps it is already being used?\n");
		stopDevice(devID);
		return S_FALSE;
	}

	//NULL RENDERER
	//used to give the video stream somewhere to go to.  
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&VD->pDestFilter));
	CHECK_HR("ERROR: Could not create filter - NullRenderer\n")
	
	hr = VD->pGraph->AddFilter(VD->pDestFilter, L"NullRenderer");	
	CHECK_HR("ERROR: Could not add filter - NullRenderer\n")
	
	//This is where the stream gets put together. 
	hr = VD->pCaptureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, VD->pVideoInputFilter, VD->pGrabberF, VD->pDestFilter);	
	CHECK_HR("ERROR: Could not connect pins - RenderStream()\n")

	//EXP - lets try setting the sync source to null - and make it run as fast as possible
	{
		IMediaFilter *pMediaFilter = 0;
		hr = VD->pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
		if (FAILED(hr)){
			if(verbose)printf("ERROR: Could not get IID_IMediaFilter interface\n");
		}else{
			pMediaFilter->SetSyncSource(NULL);
			pMediaFilter->Release();
		}
	}

	// Run the stream
	hr = VD->pControl->Run();
	CHECK_HR("ERROR: Could not start graph\n")

	// Make sure the device is sending video before we finish
	if(!bCallback)
	{
		long bufferSize = VD->videoSize;

		while( hr != S_OK)
		{
			hr = VD->pGrabber->GetCurrentBuffer(&bufferSize, (long *)VD->pBuffer);
			Sleep(10);
		}
	}

	if(verbose)printf("SETUP: Device is setup and ready to capture.\n\n");
	VD->readyToCapture = true;  
		
	//Release filters - seen someone else do this. Looks like it solved the freezes
	
	// if we release this then we don't have access to the settings
	// we release our video input filter but then reconnect with it each time we need to use it
	VD->pVideoInputFilter->Release();
	VD->pVideoInputFilter = NULL;

	VD->pGrabberF->Release();
	VD->pGrabberF = NULL;

	VD->pDestFilter->Release();
	VD->pDestFilter = NULL;

	return S_OK;
#undef CHECK_HR
} 

// Our static function for finding num devices available etc
int videoInput::listDevices(bool silent)
{
	//COM Library Intialization
	comInit();

	if(!silent)printf("\nVIDEOINPUT SPY MODE!\n\n");   

	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;	
	int deviceCounter = 0;

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));

	if (FAILED(hr)) { comUnInit(); return 0; }

	// Create an enumerator for the video capture category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);

	if(hr == S_OK)
	{
		if(!silent)printf("SETUP: Looking For Capture Devices\n");

		IMoniker *pMoniker = NULL;

		while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));

			if (FAILED(hr))
			{
				pMoniker->Release();
				continue;  // Skip this one, maybe the next one will work.
			} 

			// Find the description or friendly name.
			VARIANT varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"Description", &varName, 0);

			if (FAILED(hr)) hr = pPropBag->Read(L"FriendlyName", &varName, 0);

			if (SUCCEEDED(hr))
			{
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);

				int count = 0;
				int maxLen = sizeof(deviceNames[0])/sizeof(deviceNames[0][0]) - 2;

				while( varName.bstrVal[count] != 0x00 && count < maxLen) {
					deviceNames[deviceCounter][count] = (char)varName.bstrVal[count];
					count++;
				}
				deviceNames[deviceCounter][count] = 0;

		        	if(!silent)printf("SETUP: %i) %s \n",deviceCounter, deviceNames[deviceCounter]);
			}

			pPropBag->Release();
			pPropBag = NULL;

			pMoniker->Release();
			pMoniker = NULL;

			deviceCounter++;
		}   

		pDevEnum->Release();
		pDevEnum = NULL;

		pEnum->Release();
		pEnum = NULL;
	}

	if(!silent)printf("SETUP: %i Device(s) found\n\n", deviceCounter);

	comUnInit();

	return deviceCounter;		
}

// Returns number of good devices
int videoInput::getDeviceCount()
{ 
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;	
	int deviceCounter = 0;
	
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));

	if(FAILED(hr)) return 0;
	    
	// Create an enumerator for the video capture category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
	        
	if(hr == S_OK)
	{
		IMoniker *pMoniker = NULL;

		while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));

			if (FAILED(hr)){
				pMoniker->Release();
				continue;  // Skip this one, maybe the next one will work.
			} 
			 
			pPropBag->Release();
			pPropBag = NULL;
			    
			pMoniker->Release();
			pMoniker = NULL;
			    
			deviceCounter++;
		}   

		pEnum->Release();
		pEnum = NULL;
	}

	pDevEnum->Release();
	pDevEnum = NULL;

	return deviceCounter;	
}
   
// Do we need this?  
//    
// Enumerate all of the video input devices
// Return the filter with a matching friendly name                               
HRESULT videoInput::getDevice(IBaseFilter** gottaFilter, int deviceId, WCHAR * wDeviceName, char * nDeviceName){
	BOOL done = false;
	int deviceCounter = 0;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr)) return hr;

	// Obtain a class enumerator for the video input category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while ((pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) && (!done))
		{
			if(deviceCounter == deviceId)
			{
				// Bind the first moniker to an object
				IPropertyBag *pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
				if (SUCCEEDED(hr))
				{
					// To retrieve the filter's friendly name, do the following:
					VARIANT varName;
					VariantInit(&varName);
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);

					if (SUCCEEDED(hr))
					{		
						
						//copy the name to nDeviceName & wDeviceName
						int count = 0;
						while( varName.bstrVal[count] != 0x00 )
						{
							wDeviceName[count] = varName.bstrVal[count];
							nDeviceName[count] = (char)varName.bstrVal[count];
							count++;
						}

						// We found it, so send it back to the caller
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)gottaFilter);
						done = true;
					}

					VariantClear(&varName);	
					pPropBag->Release();
					pPropBag = NULL;
					pMoniker->Release();
					pMoniker = NULL;
				}
			}
			deviceCounter++;
		}
		pEnumCat->Release();
		pEnumCat = NULL;
	}
	pSysDevEnum->Release();
	pSysDevEnum = NULL;

	// found it, return native error or some other code
	return done ? hr : VFW_E_NOT_FOUND;
}
 
// Show the property pages for a filter. This is stolen from the DX9 SDK
HRESULT videoInput::ShowFilterPropertyPages(IBaseFilter *pFilter)
{
	ISpecifyPropertyPages *pProp;
	HRESULT hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
		// Get the filter's name and IUnknown pointer.
		FILTER_INFO FilterInfo;
		hr = pFilter->QueryFilterInfo(&FilterInfo); 
		IUnknown *pFilterUnk;
		pFilter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		pProp->Release();
		OleCreatePropertyFrame(
			NULL,                   // Parent window
			0, 0,                   // Reserved
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);

		// Clean up.
		if(pFilterUnk)pFilterUnk->Release();
		if(FilterInfo.pGraph)FilterInfo.pGraph->Release(); 
		CoTaskMemFree(caGUID.pElems);
	}
	return hr;
}
   
// Changing the input types
HRESULT videoInput::routeCrossbar(ICaptureGraphBuilder2 **ppBuild, IBaseFilter **pVidInFilter, int conType, GUID captureMode)
{
	//create local ICaptureGraphBuilder2
	ICaptureGraphBuilder2 *pBuild = NULL;
 	pBuild = *ppBuild;
 
 	//create local IBaseFilter
 	IBaseFilter *pVidFilter = NULL;
 	pVidFilter = * pVidInFilter;

	// Search upstream for a crossbar.
	IAMCrossbar *pXBar1 = NULL;
	HRESULT hr = pBuild->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, pVidFilter, IID_IAMCrossbar, (void**)&pXBar1);

	if(FAILED(hr))
	{
		if(verbose)printf("SETUP: You are a webcam or snazzy firewire cam! No Crossbar needed\n");
		return hr;
	}

	bool foundDevice = false;

	if(verbose)printf("SETUP: You are not a webcam! Setting Crossbar\n");
	pXBar1->Release();

	IAMCrossbar *Crossbar;
	hr = pBuild->FindInterface(&captureMode, &MEDIATYPE_Interleaved, pVidFilter, IID_IAMCrossbar, (void **)&Crossbar);

	if(hr != NOERROR)
	{
	        hr = pBuild->FindInterface(&captureMode, &MEDIATYPE_Video, pVidFilter, IID_IAMCrossbar, (void **)&Crossbar);
	}

	LONG lInpin, lOutpin;
	hr = Crossbar->get_PinCounts(&lOutpin , &lInpin); 
				
	BOOL IPin=TRUE; LONG pIndex=0 , pRIndex=0 , pType=0;
		
	while( pIndex < lInpin)
	{
		hr = Crossbar->get_CrossbarPinInfo( IPin , pIndex , &pRIndex , &pType); 
		
		if( pType == conType)
		{
			if(verbose)printf("SETUP: Found Physical Interface");				
					
			switch(conType)
			{
				case PhysConn_Video_Composite: if(verbose)printf(" - Composite\n"); break;
				case PhysConn_Video_SVideo: if(verbose)printf(" - S-Video\n"); break;
				case PhysConn_Video_Tuner: if(verbose)printf(" - Tuner\n");break;
				case PhysConn_Video_USB: if(verbose)printf(" - USB\n"); break;	
				case PhysConn_Video_1394: if(verbose)printf(" - Firewire\n"); break;
			}				
							
			foundDevice = true;
			break;
		}
		pIndex++;
	}

	if(foundDevice)
	{
		BOOL OPin=FALSE; LONG pOIndex=0 , pORIndex=0 , pOType=0;
		while( pOIndex < lOutpin)
		{
			hr = Crossbar->get_CrossbarPinInfo( OPin , pOIndex , &pORIndex , &pOType); 

			if( pOType == PhysConn_Video_VideoDecoder) break;
		}

		Crossbar->Route(pOIndex,pIndex); 
	}
	else
	{
		if(verbose)printf("SETUP: Didn't find specified Physical Connection type. Using Default. \n");	
	}			

	//we only free the crossbar when we close or restart the device, otherwise we are getting a crash
	//if(Crossbar)Crossbar->Release();
	//if(Crossbar)Crossbar = NULL;

	if(pXBar1)pXBar1->Release();
	if(pXBar1)pXBar1 = NULL;

	return hr;
}

/////// Messy part: settings

bool videoInput::setVideoSettingFilterOrCameraPct(bool UseCam, int deviceID, long Property, float pctValue, long Flags)
{
	if( !isDeviceSetup(deviceID) ) return false;

	long a_min, a_max, currentValue, flags, defaultValue, stepAmnt;

	if(UseCam)
	{
		if( !getVideoSettingCamera(deviceID, Property, a_min, a_max, stepAmnt, currentValue, flags, defaultValue) ) return false;
	}
	else
	{
		if( !getVideoSettingFilter(deviceID, Property, a_min, a_max, stepAmnt, currentValue, flags, defaultValue) ) return false;
	}

	pctValue = (pctValue > 1.0f) ? 1.0f : ( (pctValue < 0) ? 0.0f : pctValue);

	float range = (float)a_max - (float)a_min;
	if(range <= 0)return false;	 
	if(stepAmnt == 0) return false;

	long value = (long)( (float)a_min + range * pctValue ); 
	long rasterValue = value;

	//if the range is the stepAmnt then it is just a switch
	//so we either set the value to low or high
	if( range == stepAmnt )
	{
		rasterValue = (pctValue < 0.5) ? a_min : a_max;	
	}
	else
	{
		//we need to rasterize the value to the stepping amnt
		long mod = value % stepAmnt;

		float halfStep 	= (float)stepAmnt * 0.5f;

		rasterValue -= mod - (( mod < halfStep ) ? 0 : stepAmnt);

		printf("RASTER - pctValue is %f - value is %i - step is %i - mod is %i - rasterValue is %i\n", pctValue, value, stepAmnt, mod, rasterValue); 
	}

	return UseCam ?
		setVideoSettingCamera(deviceID, Property, rasterValue, Flags, false) :
		setVideoSettingFilter(deviceID, Property, rasterValue, Flags, false);
}

// Set a video signal setting using IAMVideoProcAmp
bool videoInput::setVideoSettingFilter(int deviceID, long Property, long lValue, long Flags, bool useDefaultValue)
{
	if( !isDeviceSetup(deviceID) )return false;

	HRESULT hr;	
	
	videoDevice * VD = VDList[deviceID];
	
	hr = getDevice(&VD->pVideoInputFilter, deviceID, VD->wDeviceName, VD->nDeviceName);	
	if (FAILED(hr)){
		printf("setVideoSetting - getDevice Error\n");
		return false;
	}

	IAMVideoProcAmp *pAMVideoProcAmp = NULL;
	
	hr = VD->pVideoInputFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pAMVideoProcAmp);
	if(FAILED(hr)){
		printf("setVideoSetting - QueryInterface Error\n");
		if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
		if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;		
		return false;			
	}
	
	if (verbose) printf("Setting video setting %ld.\n", Property);
	long CurrVal, Min, Max, SteppingDelta, Default, CapsFlags, AvailableCapsFlags = 0;
	
	
	pAMVideoProcAmp->GetRange(Property, &Min, &Max, &SteppingDelta, &Default, &AvailableCapsFlags);
	if (verbose) printf("Range for video setting %ld: Min:%ld Max:%ld SteppingDelta:%ld Default:%ld Flags:%ld\n", Property, Min, Max, SteppingDelta, Default, AvailableCapsFlags);
	pAMVideoProcAmp->Get(Property, &CurrVal, &CapsFlags);
	
	if (verbose) printf("Current value: %ld Flags %ld (%s)\n", CurrVal, CapsFlags, (CapsFlags == 1 ? "Auto" : (CapsFlags == 2 ? "Manual" : "Unknown")));
	
	if (useDefaultValue) {
		pAMVideoProcAmp->Set(Property, Default, VideoProcAmp_Flags_Auto);
	}
	else{
		// Perhaps add a check that lValue and Flags are within the range aquired from GetRange above
		pAMVideoProcAmp->Set(Property, lValue, Flags);
	}
	
	if(pAMVideoProcAmp)pAMVideoProcAmp->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;	
			
	return true;
}

bool videoInput::setVideoSettingCamera(int deviceID, long Property, long lValue, long Flags, bool useDefaultValue){
	IAMCameraControl *pIAMCameraControl;
	if(isDeviceSetup(deviceID))
	{
		HRESULT hr;
		hr = getDevice(&VDList[deviceID]->pVideoInputFilter, deviceID, VDList[deviceID]->wDeviceName, VDList[deviceID]->nDeviceName);	
	
		if (verbose) printf("Setting video setting %ld.\n", Property);
		hr = VDList[deviceID]->pVideoInputFilter->QueryInterface(IID_IAMCameraControl, (void**)&pIAMCameraControl);
		if (FAILED(hr)) {
			printf("Error\n");
			return false;
		}
		else
		{
			long CurrVal, Min, Max, SteppingDelta, Default, CapsFlags, AvailableCapsFlags;
			pIAMCameraControl->GetRange(Property, &Min, &Max, &SteppingDelta, &Default, &AvailableCapsFlags);
			if (verbose) printf("Range for video setting %ld: Min:%ld Max:%ld SteppingDelta:%ld Default:%ld Flags:%ld\n", Property, Min, Max, SteppingDelta, Default, AvailableCapsFlags);
			pIAMCameraControl->Get(Property, &CurrVal, &CapsFlags);
			if (verbose) printf("Current value: %ld Flags %ld (%s)\n", CurrVal, CapsFlags, (CapsFlags == 1 ? "Auto" : (CapsFlags == 2 ? "Manual" : "Unknown")));
			if (useDefaultValue) {
				pIAMCameraControl->Set(Property, Default, CameraControl_Flags_Auto);
			}
			else
			{
				// Perhaps add a check that lValue and Flags are within the range aquired from GetRange above
				pIAMCameraControl->Set(Property, lValue, Flags);
			}
			pIAMCameraControl->Release();
			return true;
		}
	}
	return false;
}

bool videoInput::getVideoSettingCamera(int deviceID, long Property, long &min, long &max, long &SteppingDelta, long &currentValue, long &flags, long &defaultValue){
	if( !isDeviceSetup(deviceID) )return false;

	HRESULT hr;	
	
	videoDevice * VD = VDList[deviceID];
	
	hr = getDevice(&VD->pVideoInputFilter, deviceID, VD->wDeviceName, VD->nDeviceName);	
	if (FAILED(hr)){
		printf("setVideoSetting - getDevice Error\n");
		return false;
	}

	IAMCameraControl *pIAMCameraControl = NULL;
	
	hr = VD->pVideoInputFilter->QueryInterface(IID_IAMCameraControl, (void**)&pIAMCameraControl);
	if(FAILED(hr)){
		printf("setVideoSetting - QueryInterface Error\n");
		if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
		if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;		
		return false;			
	}
	
	if (verbose) printf("Setting video setting %ld.\n", Property);
	
	pIAMCameraControl->GetRange(Property, &min, &max, &SteppingDelta, &defaultValue, &flags);
	if (verbose) printf("Range for video setting %ld: Min:%ld Max:%ld SteppingDelta:%ld Default:%ld Flags:%ld\n", Property, min, max, SteppingDelta, defaultValue, flags);
	pIAMCameraControl->Get(Property, &currentValue, &flags);
	
	if(pIAMCameraControl)pIAMCameraControl->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;	
			
	return true;
	
}

// Set a video signal setting using IAMVideoProcAmp
bool videoInput::getVideoSettingFilter(int deviceID, long Property, long &min, long &max, long &SteppingDelta, long &currentValue, long &flags, long &defaultValue)
{
	if( !isDeviceSetup(deviceID) )return false;

	HRESULT hr;	
	
	videoDevice * VD = VDList[deviceID];
	
	hr = getDevice(&VD->pVideoInputFilter, deviceID, VD->wDeviceName, VD->nDeviceName);	
	if (FAILED(hr)){
		printf("setVideoSetting - getDevice Error\n");
		return false;
	}

	IAMVideoProcAmp *pAMVideoProcAmp = NULL;
	
	hr = VD->pVideoInputFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pAMVideoProcAmp);
	if(FAILED(hr)){
		printf("setVideoSetting - QueryInterface Error\n");
		if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
		if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;		
		return false;			
	}
	
	if (verbose) printf("Setting video setting %ld.\n", Property);
	
	pAMVideoProcAmp->GetRange(Property, &min, &max, &SteppingDelta, &defaultValue, &flags);
	if (verbose) printf("Range for video setting %ld: Min:%ld Max:%ld SteppingDelta:%ld Default:%ld Flags:%ld\n", Property, min, max, SteppingDelta, defaultValue, flags);
	pAMVideoProcAmp->Get(Property, &currentValue, &flags);
	
	if(pAMVideoProcAmp)pAMVideoProcAmp->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter->Release();
	if(VD->pVideoInputFilter)VD->pVideoInputFilter = NULL;	
			
	return true;
}	

#endif // WIN32
