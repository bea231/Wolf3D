/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#include <math.h>
#include "anim.h"
#include "player.h"
#include "world.h"

static player_t player = {0};
static float rotation_angle_sine = 0,
             rotation_angle_cosine = 0;

void PlayerInit( float start_x, float start_z )
{
  rotation_angle_sine = sinf(ROTATE_ANGLE);
  rotation_angle_cosine = cosf(ROTATE_ANGLE);

  player.cosine_phi = 1;
  player.sine_phi = 0;
  player.pos_x = start_x;
  player.pos_z = start_z;
}

void PlayerSetPosition( float x, float z )
{
  player.pos_x = x;
  player.pos_z = z;
}

void PlayerGetPosition( float *x, float *z )
{
  *x = player.pos_x;
  *z = player.pos_z;
}

void PlayerGetRay( int to_x, float *dir_x, float *dir_z )
{
  float scale;
  float const width = (float)AnimGetWidth();
  scale = (to_x - width /2) / width;

  *dir_x = player.sine_phi - player.cosine_phi * scale * CAMERA_WIDTH;
  *dir_z = player.cosine_phi + player.sine_phi * scale * CAMERA_WIDTH;
}
void PlayerGetLookAt( float *at_x, float *at_z )
{
  *at_x = player.sine_phi;
  *at_z = player.cosine_phi;
}

void PlayerRotateRight( void )
{
  float sine = player.sine_phi, cosine = player.cosine_phi;

  player.cosine_phi = cosine * rotation_angle_cosine + sine * rotation_angle_sine;
  player.sine_phi = sine * rotation_angle_cosine - cosine * rotation_angle_sine;
}

void PlayerRotateLeft( void )
{
  float sine = player.sine_phi, cosine = player.cosine_phi;
  player.cosine_phi = cosine * rotation_angle_cosine - sine * rotation_angle_sine;
  player.sine_phi = sine * rotation_angle_cosine + cosine * rotation_angle_sine;
}

void PlayerMoveForward( void )
{
  if (WorldCanGo(1))
  {
    player.pos_x += player.sine_phi * MOVE_STEP;
    player.pos_z += player.cosine_phi * MOVE_STEP;
  }
}

void PlayerMoveBack( void )
{
  if (WorldCanGo(0))
  {
    player.pos_x -= player.sine_phi * MOVE_STEP;
    player.pos_z -= player.cosine_phi * MOVE_STEP;
  }
}