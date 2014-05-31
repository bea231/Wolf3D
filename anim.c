/* FILE NAME:  ANIM.C
 * PROGRAMMER: AS5
 * DATE:       03.06.2010
 * PURPOSE:    Animation pattern.
 *             Animation implimentation module.
 */

#include <stdio.h>

#include "textures.h"
#include "anim.h"
#include "world.h"

anim_t anim;
INT64 AnimStartTime, AnimOldTime, AnimLastTime, AnimTimesPerSec, AnimFrameCount, AnimFPSPrintTime;

/* Make RGB value from 3 bytes macro */
#define RRGB(r, g, b) \
  (((unsigned long)(unsigned char)(r) << 16) | ((unsigned long)(unsigned char)(g) << 8) | (unsigned char)(b))

static unsigned long * _LoadBMP( char *fileName )
{
  HBITMAP hImageBm = LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  HDC hDC = CreateCompatibleDC(NULL);
  BITMAP bm;
  BITMAPINFO bmpInfo;
  unsigned long *pixels;

  if (hImageBm == NULL)
    return NULL;
  GetObject(hImageBm, sizeof(bm), &bm);

  pixels = malloc(sizeof(unsigned long) * bm.bmWidth * bm.bmHeight);
  if (!pixels)
    return NULL;

  SelectObject(hDC, hImageBm);
  bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmpInfo.bmiHeader.biWidth = bm.bmWidth;
  bmpInfo.bmiHeader.biHeight = -bm.bmHeight;
  bmpInfo.bmiHeader.biPlanes = 1;
  bmpInfo.bmiHeader.biBitCount = 32;
  bmpInfo.bmiHeader.biCompression = BI_RGB;
  bmpInfo.bmiHeader.biSizeImage = 0;
  GetDIBits(hDC, hImageBm, 0, bm.bmHeight, pixels , &bmpInfo , DIB_RGB_COLORS );
  return pixels;
}

/* Before init animation system function */
VOID AnimPreinit( VOID )
{
  unsigned long *textures[MAX_TEXTURES_COUNT] = {0};

  memset(&anim, 0, sizeof(anim_t));
  textures[0] = _LoadBMP("data/tex01.bmp");
  textures[1] = _LoadBMP("data/tex02.bmp");
  textures[2] = _LoadBMP("data/tex03.bmp");
  textures[2] = _LoadBMP("data/tex04.bmp");
  WorldInit("level1.txt", textures);
  free(textures[0]);
  free(textures[1]);
  free(textures[2]);
  free(textures[3]);
}

/* Init animation system function */
VOID AnimInit( HWND hWnd )
{
  HDC hDC;
  LARGE_INTEGER Time;
  BITMAPINFOHEADER bmi;

  anim.hWnd = hWnd;
  hDC = GetDC(hWnd);
  anim.hDC = CreateCompatibleDC(hDC);
  /* Allocate memory for new background size */
  bmi.biSize = sizeof(BITMAPINFOHEADER);
  bmi.biWidth = 10;
  bmi.biHeight = -10;
  bmi.biBitCount = 32;
  bmi.biPlanes = 1;
  bmi.biCompression = BI_RGB;
  bmi.biSizeImage = 10 * 10 * 4;
  bmi.biXPelsPerMeter = bmi.biYPelsPerMeter = 1;
  bmi.biClrUsed = 0;
  bmi.biClrImportant = 0;
  anim.hBm = CreateDIBSection(NULL, (BITMAPINFO *)&bmi, DIB_RGB_COLORS,
      (VOID **)&anim.pixels, NULL, 0); 
  anim.H = anim.W = 10;
  SelectObject(anim.hDC, anim.hBm);
  ReleaseDC(hWnd, hDC);

  QueryPerformanceCounter(&Time);
  AnimFPSPrintTime = AnimStartTime = AnimOldTime = AnimLastTime = Time.QuadPart; 
  QueryPerformanceFrequency(&Time);
  AnimTimesPerSec = Time.QuadPart;
}

/* Close animation system function */
VOID AnimClose( VOID )
{
  ReleaseDC(anim.hWnd, anim.hDC);
  DeleteObject(anim.hBm);
  memset(&anim, 0, sizeof(anim));
}

/* Resize system windows function */
VOID AnimResize( INT W, INT H )
{
  HDC hDC;
  BITMAPINFOHEADER bmi;

  hDC = GetDC(anim.hWnd);
  DeleteObject(anim.hBm);
  /* Allocate memory for new background size */
  bmi.biSize = sizeof(BITMAPINFOHEADER);
  bmi.biWidth = W;
  bmi.biHeight = -H;
  bmi.biBitCount = 32;
  bmi.biPlanes = 1;
  bmi.biCompression = 0;
  bmi.biSizeImage = W * H * 4;
  bmi.biXPelsPerMeter = bmi.biYPelsPerMeter = 1;
  bmi.biClrUsed = 0;
  bmi.biClrImportant = 0;
  anim.hBm = CreateDIBSection(NULL, (BITMAPINFO *)&bmi, DIB_RGB_COLORS,
      (VOID **)&anim.pixels, NULL, 0); 
  ReleaseDC(anim.hWnd, hDC);

  anim.W = W;
  anim.H = H;

  SelectObject(anim.hDC, anim.hBm);
}

VOID _SetFpsTitle( VOID )
{
  char buf[64];

  sprintf_s(buf, 64, "FPS=%5.2f", anim.FPS);
  SetWindowText(anim.hWnd, buf);
}

/* Responde system parametrs function */
VOID AnimResponse( VOID )
{
  LARGE_INTEGER Time;
  int i;

  QueryPerformanceCounter(&Time);
  anim.GlobalTime = (float)(Time.QuadPart - AnimStartTime) / AnimTimesPerSec;
  anim.GlobalDelta = (float)(Time.QuadPart - AnimOldTime) / AnimTimesPerSec;
  if (Time.QuadPart - AnimFPSPrintTime > 20000)
  {
    _SetFpsTitle();
    AnimFPSPrintTime = Time.QuadPart;
  }
  if (Time.QuadPart - AnimLastTime > AnimTimesPerSec)
  {
    anim.FPS = (float)AnimFrameCount / (Time.QuadPart - AnimLastTime) * AnimTimesPerSec;
    AnimLastTime = Time.QuadPart;
    AnimFrameCount = 0;
  }
  AnimOldTime = Time.QuadPart;

  GetKeyboardState(anim.Kbd);
  for (i = 0; i < sizeof(anim.Kbd); i++)
    anim.Kbd[i] >>= 7;
}

/* Render frame function */
VOID AnimRender( VOID )
{
  WorldUpdate(anim.pixels);
}

/* Copy frame to window function */
VOID AnimCopyFrame( HDC hDC )
{
  SelectObject(anim.hDC, anim.hBm);
  BitBlt(hDC, 0, 0, anim.W, anim.H, anim.hDC, 0, 0, SRCCOPY);
  ++AnimFrameCount;
}

unsigned int AnimGetWidth( VOID )
{
  return anim.W;
}
unsigned int AnimGetHeight( VOID )
{
  return anim.H;
}
