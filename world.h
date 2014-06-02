/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#define MAX_CELL_COUNT_X 3
#define MAX_CELL_COUNT_Z 3

#define CELL_LENGTH_DEGREE    4
#define CELL_LENGTH           (1L<<CELL_LENGTH_DEGREE)
#define WALL_HEIGHT           CELL_LENGTH * 1.3f
#define MIN_WALL_DIST         0.8

char WorldInit( char *fileName, unsigned int **textures );
void WorldClose( void );
char WorldCanGo( char forward );
void WorldUpdate( unsigned long *pixels );



#endif /* _WORLD_H_ */