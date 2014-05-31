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
  unsigned long *texture, *cur_pixel;
  int tex_start, tex_end, tex_step, tex_y, tex_y_shifted;
  unsigned int wnd_height = AnimGetHeight(), wnd_width = AnimGetWidth();
  int i;

  tex_start = 0;
  tex_end = TEXTURE_SIDE - 1;
  tex_y = start_y;
  if (start_y < 0)
  {
    tex_start = (int)(-(float)start_y * TEXTURE_SIDE / ((float)end_y - start_y));
    start_y = 0;
  }
  if (end_y >= (int)wnd_height)
  {
    tex_end = (int)((-tex_y + (float)wnd_height) * TEXTURE_SIDE / ((float)end_y - tex_y));
    end_y = wnd_height - 1;
  }

  cur_pixel = pixels + screen_x + start_y * wnd_width;

  texture = textures[texture_index] + texture_x;
  tex_step = ((tex_end - tex_start) << 16) / (end_y - start_y);
  tex_y_shifted = tex_start << 16;
  for (i = start_y; i <= end_y; ++i)
  {
    tex_y = tex_y_shifted >> 16;
    *cur_pixel = texture[tex_y << TEXTURE_SIDE_DEGREE];
    cur_pixel += wnd_width;
    tex_y_shifted += tex_step;
  }
}