/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#include <Windows.h>
#include "anim.h"
#include "player.h"

#define CLS_NAME "Wolf3DClass"
#define WND_NAME "Wolf3D"

INT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
  HDC hDC;
  PAINTSTRUCT ps;

  switch (msg)
  {
  case WM_CREATE:
    AnimInit(hWnd);
    return 0;
  case WM_SIZE:
    AnimResize(LOWORD(lp), HIWORD(lp));
    AnimRender();
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;
  case WM_KEYDOWN:
    switch (wp)
    {
    case VK_ESCAPE:
      SendMessage(hWnd, WM_CLOSE, 0, 0);
      break;
    case 'A':
    case VK_LEFT:
      PlayerRotateLeft();
      break;
    case 'D':
    case VK_RIGHT:
      PlayerRotateRight();
      break;
    case 'W':
    case VK_UP:
      PlayerMoveForward();
      break;
    case 'S':
    case VK_DOWN:
      PlayerMoveBack();
      break;
    }
    return 0;
  case WM_ERASEBKGND:
    return 0;
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    AnimCopyFrame(hDC);
    EndPaint(hWnd, &ps);
    return 0;
  case WM_CLOSE:
      AnimClose();
      PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, msg, wp, lp);
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, CHAR *CmdLine, INT ShowCmd )
{
  WNDCLASS wc;
  HWND hMainWnd;
  MSG msg;

  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = LoadIcon(NULL, IDI_QUESTION);
  wc.hInstance = hInst;
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = CLS_NAME;
  wc.lpszMenuName = NULL;
  wc.style = CS_HREDRAW | CS_VREDRAW;

  if (!RegisterClass(&wc))
  {
    MessageBox(NULL, "Cannot register window class", "ERROR", MB_OK | MB_ICONERROR);
    return 0;
  }

  AnimPreinit();
  hMainWnd = CreateWindow(CLS_NAME, WND_NAME, WS_OVERLAPPEDWINDOW,
    0, 0, GetSystemMetrics(SM_CXSCREEN), 800,
      NULL, NULL, hInst, NULL);
  
  if (hMainWnd == NULL)
  {
    MessageBox(NULL, "Cannot create window", "ERROR", MB_OK | MB_ICONERROR);
    return 0;
  }

  ShowWindow(hMainWnd, ShowCmd);
  UpdateWindow(hMainWnd);

  while (1)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      AnimResponse();
      AnimRender();
      InvalidateRect(hMainWnd, NULL, FALSE);
    }
  }

  return msg.wParam;
} /* End of 'WinMain' function */