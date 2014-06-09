/* 
 * Wolfenstein 3D visualization system
 * Sergeev Artemiy
 */

#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define MAX_TEXTURES_COUNT 4

#define TEXTURE_SIDE_DEGREE 9 
#define TEXTURE_SIDE (1 << TEXTURE_SIDE_DEGREE)

char TexturesOpen( unsigned long **tex );
void TexturesClose( void );
unsigned long * TexturesGet( unsigned int index );
void TextureDrawVerticalLine( unsigned long *pixels,
                              int start_y,
                              int end_y,
                              int texture_index,
                              int texture_x,
                              int screen_x );
void TextureDrawVerticalLineAsm( unsigned long *pixels,
                                 int start_y,
                                 unsigned int end_y,
                                 int texture_index,
                                 int texture_x,
                                 int screen_x );

#endif /* _TEXTURES_H_ */