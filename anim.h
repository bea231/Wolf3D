/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#ifndef _ANIM_H_
#define _ANIM_H_

#include <windows.h>

typedef struct tag_anim anim_t;
struct tag_anim
{
  HWND hWnd;
  HDC hDC;
  HBITMAP hBm;
  int W, H;
  float GlobalTime;
  float GlobalDelta;
  float FPS;

  BYTE Kbd[256];

  unsigned long *pixels;
};
/* Before init animation system function */
VOID AnimPreinit( VOID );
/* Init animation system function */
VOID AnimInit( HWND hWnd );
/* Close animation system function */
VOID AnimClose( VOID );
/* Resize system windows function */
VOID AnimResize( INT W, INT H );
/* Responde system parametrs function */
VOID AnimResponse( VOID );
/* Render frame function */
VOID AnimRender( VOID );
/* Copy frame to window function */
VOID AnimCopyFrame( HDC hDC );

unsigned int AnimGetWidth( VOID );
unsigned int AnimGetHeight( VOID );

#endif /* _ANIM_H_ */
