/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "anim.h"
#include "world.h"
#include "player.h"
#include "textures.h"

static char map[MAX_CELL_COUNT_X * MAX_CELL_COUNT_Z];

static char _WorldReadFromFile( char *fileName )
{
  FILE *file;
  unsigned int size = 0, i, row = 0, column = 0;
  char *fileData;

  if (fileName == NULL || fopen_s(&file, fileName, "rb") != 0)
    return 0;

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if ((fileData = malloc(sizeof(char) * size)) == NULL)
    return 0;
  fread(fileData, 1, size, file);
  for (i = 0; 
       i < size; 
       ++i)
  {
    switch(fileData[i])
    {
    case '\n':
      ++row;
      column = 0;
      break;
    case '*':
      PlayerSetPosition((float)(column * CELL_LENGTH + (CELL_LENGTH / 2)),
                        (float)(row * CELL_LENGTH + (CELL_LENGTH / 2)));
    case ' ':
      map[column++ + MAX_CELL_COUNT_X * row] = -1;
      break;
    }
    if (isdigit(fileData[i]))
      map[column++ + MAX_CELL_COUNT_X * row] = fileData[i] - '0' - 1;
  }
  free(fileData);
  return 1;
}

char WorldInit( char *fileName, unsigned int **textures )
{
  PlayerInit(CELL_LENGTH + CELL_LENGTH / 2, CELL_LENGTH + CELL_LENGTH / 2);
  if (!_WorldReadFromFile(fileName))
    return 0;
  TexturesOpen(textures);
  return 1;
}

void WorldClose( void )
{
  TexturesClose();
}

void WorldUpdate( unsigned long *pixels )
{
  float dir_x, dir_z, z_per_x, x_per_z, dist;
  float cam_x, cam_z, at_x, at_z;
  float intersect_x, intersect_z;
  int start_x, start_z, step_x, step_z;
  int search_x, search_z; // Indexes of cells for test line shift
  int x, tex_x;
  char is_intersect, is_v_intersect, iter;

  memset(pixels, 0, AnimGetWidth() * AnimGetHeight() * sizeof(unsigned long));

  PlayerGetPosition(&cam_x, &cam_z);

  start_x = (int)(cam_x) >> CELL_LENGTH_DEGREE;
  start_z = (int)(cam_z) >> CELL_LENGTH_DEGREE;

  for (x = 0; x < (int)AnimGetWidth(); ++x)
  {
    int cell_x = start_x, cell_z = start_z;

    PlayerGetRay(x, &dir_x, &dir_z);

    if (dir_x == 0.f)
      dir_x = 1e-8f;
    if (dir_z == 0.f)
      dir_z = 1e-8f;

    z_per_x = dir_z / dir_x;
    x_per_z = dir_x / dir_z;

    step_x = (dir_x > 0.f) * 2 - 1;
    step_z = (dir_z > 0.f) * 2 - 1;

    is_intersect = 0;
    iter = 0;
    while (1)
    {
      if (++iter > MAX_CELL_COUNT_X * 2)
        break;

      search_x = cell_x + (step_x > 0);
      search_z = cell_z + (step_z > 0);

      intersect_z = cam_z + z_per_x * ((search_x << CELL_LENGTH_DEGREE) - cam_x);
      intersect_x = cam_x + x_per_z * ((search_z << CELL_LENGTH_DEGREE) - cam_z);

      is_v_intersect = 0;
      if ((step_z > 0 && intersect_z <= (search_z << CELL_LENGTH_DEGREE)) ||
          (step_z < 0 && intersect_z >= (search_z << CELL_LENGTH_DEGREE)))
        is_v_intersect = 1;
      
      if (is_v_intersect)
      {
        cell_x += step_x;
        if (map[cell_z * MAX_CELL_COUNT_Z + cell_x] != -1)
        {
          is_intersect = 1;
          intersect_x = (float)(search_x << CELL_LENGTH_DEGREE);
          tex_x = (int)(intersect_z * TEXTURE_SIDE / CELL_LENGTH);
          tex_x &= (TEXTURE_SIDE - 1);
          break;
        }
      }
      else
      {
        cell_z += step_z;
        if (map[cell_z * MAX_CELL_COUNT_Z + cell_x] != -1)
        {
          is_intersect = 1;
          intersect_z = (float)(search_z << CELL_LENGTH_DEGREE);
          tex_x = (int)(intersect_x * TEXTURE_SIDE / CELL_LENGTH);
          tex_x &= (TEXTURE_SIDE - 1);
          break;
        }
      }
    }
    if (!is_intersect)
      continue;

    PlayerGetLookAt(&at_x, &at_z);
    intersect_x -= cam_x;
    intersect_z -= cam_z;
    dist = intersect_x * at_x + intersect_z * at_z;

    dist = (AnimGetHeight() * WALL_HEIGHT * 0.25f) / dist;

    TextureDrawVerticalLine(pixels, 
                            (int)(AnimGetHeight() / 2 - dist),
                            (int)(AnimGetHeight() / 2 + dist),
                            map[cell_z * MAX_CELL_COUNT_Z + cell_x],
                            tex_x,
                            x);
  }

  /*for (x = 0; x < TEXTURE_SIDE; ++x)
    for (tex_x = 0; tex_x < TEXTURE_SIDE; ++tex_x)
      pixels[x * AnimGetWidth() + tex_x] = TexturesGet(0)[x * TEXTURE_SIDE + tex_x];*/



}
