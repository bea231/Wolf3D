/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#include <stdlib.h>
#include <string.h>
#include "textures.h"
#include "anim.h"

static unsigned long *textures[MAX_TEXTURES_COUNT] = {0};
static unsigned long default_tex[TEXTURE_SIDE * TEXTURE_SIDE];

char TexturesOpen( unsigned long **tex )
{
  unsigned int i;
  char result = 1;

  for (i = 0; i < TEXTURE_SIDE * TEXTURE_SIDE; ++i)
    *(default_tex + i) = 8355711;

  for (i = 0; i < MAX_TEXTURES_COUNT; ++i)
  {
    textures[i] = (unsigned long *)malloc(sizeof(unsigned long) * TEXTURE_SIDE * TEXTURE_SIDE);
    if (tex == NULL || tex[i] == NULL)
    {
      textures[i] = default_tex;
      result = 0;
      continue;
    }
    memcpy(textures[i], tex[i], sizeof(unsigned long) * TEXTURE_SIDE * TEXTURE_SIDE);
  }
  return 1;
}

void TexturesClose( void )
{
  unsigned int i;

  for (i = 0; i < MAX_TEXTURES_COUNT; ++i)
    if (textures[i] != default_tex)
      free(textures[i]);
}

unsigned long * TexturesGet( unsigned int index )
{
  if (index > MAX_TEXTURES_COUNT)
    return NULL;
  return textures[index];
}

void TextureDrawVerticalLine( unsigned long *pixels,
                              int start_y,
                              int end_y,
                              int texture_index,
                              int texture_x,
                              int screen_x )
{
  unsigned long *texture, *cur_pixel, *next_pixel;
  int tex_start, tex_step, tex_y_shifted, tex_y1_shifted;
  int i;
  int const wnd_height = (int)AnimGetHeight(), wnd_width = (int)AnimGetWidth(),
            delta = (end_y - start_y);
  char const clip_start = (start_y < 0), clip_end = (end_y >= (int)wnd_height);

  tex_start = (((-start_y) << 16) / delta * clip_start) << TEXTURE_SIDE_DEGREE;
  tex_step = (1 << (TEXTURE_SIDE_DEGREE + 16)) / delta;
  start_y *= !clip_start;
  end_y -= (end_y - wnd_height) * clip_end;

  cur_pixel = pixels + screen_x;
  next_pixel = pixels + screen_x + (wnd_height - 1) * wnd_width;
  for (i = 0; i < start_y; ++i)
  {
    *cur_pixel = 0;
    *next_pixel = 0;
    cur_pixel += wnd_width;
    next_pixel -= wnd_width;
  }

  next_pixel = cur_pixel + wnd_width;

  texture = textures[texture_index] + texture_x;
  tex_y1_shifted = (tex_y_shifted = tex_start) + tex_step;
  for (i = start_y; i < end_y - 1; i += 2)
  {
    *cur_pixel = texture[(tex_y_shifted >> 16) << TEXTURE_SIDE_DEGREE];
    *next_pixel = texture[(tex_y1_shifted >> 16) << TEXTURE_SIDE_DEGREE];
    cur_pixel += wnd_width * 2;
    next_pixel += wnd_width * 2;
    tex_y_shifted += tex_step * 2;
    tex_y1_shifted += tex_step * 2;
  }
}

void TextureDrawVerticalLineAsm( unsigned long *pixels,
                                 int start_y,
                                 unsigned int end_y,
                                 int texture_index,
                                 int texture_x,
                                 int screen_x )
{
  unsigned int const wnd_height = AnimGetHeight(), wnd_width = AnimGetWidth(),
                     delta = (end_y - start_y),
                     wnd_width2 = wnd_width * 8,
                     wnd_width3 = wnd_width * 4;
  unsigned int const clip_start = (start_y < 0), clip_end = (end_y >= wnd_height), not_clip_start = start_y > 0;
  unsigned long const * texture = textures[texture_index] + texture_x;
  unsigned long *cur_pixel = pixels + screen_x;

  _asm
  {
    mov ebx, delta

    mov eax, 1 << (TEXTURE_SIDE_DEGREE + 10h) ; calculate testure step
    xor edx, edx
    div ebx
    shl eax, 1
    push eax

    mov eax, start_y ; calculate texture start
    neg eax
    shl eax, 10h
    xor edx, edx
    div ebx
    shl eax, TEXTURE_SIDE_DEGREE
    mul clip_start
    push eax
    
    cmp ebx, wnd_height
    mov ebx, end_y
    jl not_clamp
    mov ebx, wnd_height
  not_clamp:
    dec ebx
    mov end_y, ebx

    mov eax, start_y ; clamp start_y
    mul not_clip_start
    mov ebx, eax

    mov eax, dword ptr [cur_pixel]

    xor ecx, ecx
    cmp ebx, ecx ; black loop
    jg black_loop
    jmp tex_prepare
  black_loop:   ; draw black pixels
    mov dword ptr [eax], 0
    add eax, wnd_width3

    inc ecx
    cmp ebx, ecx
    jg black_loop
    jmp tex_prepare
  tex_prepare:
    mov ebx, [esp] ; tex_start to tex_y_shifted
    mov edi, [esp + 4]
    add edi, ebx ; tex_y_shifted1
    push edi

    mov edx, eax
    add edx, wnd_width3

  tex_loop:
    mov esi, [esp + 4]
    sar esi, 10h
    shl esi, TEXTURE_SIDE_DEGREE
    mov ebx, dword ptr [texture]
    mov edi, dword ptr [ebx + esi * 4]
    mov dword ptr [eax], edi

    mov esi, [esp]
    sar esi, 10h
    shl esi, TEXTURE_SIDE_DEGREE
    mov ebx, dword ptr [texture]
    mov edi, dword ptr [ebx + esi * 4]
    mov dword ptr [edx], edi

    add eax, wnd_width2
    add edx, wnd_width2

    pop esi ; tex_shift2
    pop edi ; tex_shift
    mov ebx, dword ptr[esp]
    add esi, ebx
    add edi, ebx
    push edi
    push esi

    inc ecx
    inc ecx
    cmp ecx, end_y
  jl tex_loop
    cmp ecx, wnd_height
    jge finish
  bottom_black_loop:
    mov dword ptr [eax], 0
    mov dword ptr [edx], 0
    add eax, wnd_width2
    add edx, wnd_width2
    inc ecx
    inc ecx
    cmp ecx, wnd_height
    jl bottom_black_loop
  finish:
   pop eax
   pop eax
   pop eax
  }
}
