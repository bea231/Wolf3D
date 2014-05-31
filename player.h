/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#define ROTATE_ANGLE (2 * 3.14159265f / 180.f)
#define MOVE_STEP 0.3f

#define CAMERA_WIDTH 1.f

typedef struct tag_player
{
  float sine_phi,
        cosine_phi;
  float pos_x, pos_z;
} player_t;

void PlayerInit( float start_x, float start_z );
void PlayerSetPosition( float x, float z );
void PlayerGetPosition( float *x, float *z );
void PlayerGetRay( int to_x, float *dir_x, float *dir_z );
void PlayerGetLookAt( float *at_x, float *at_z );
void PlayerRotateRight( void );
void PlayerRotateLeft( void );
void PlayerMoveForward( void );
void PlayerMoveBack( void );

#endif /* _PLAYER_H_ */
