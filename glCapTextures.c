/* gatopeich's OpenGL wrapper (c) 2012 (http://gatopeichs.pbwiki.com/)

v1.0: Wraps glTexImage2D to reduce textures that are too big for my 3D card.
Particularly a Savage MX/IX card with max texture size 256x256.
Works in other configurations, at least in Mesa-DRI-Intel 915G.
Can be used to generally reduce texture B/W, possibly improving performance.

Compile with:
$ gcc [-m32] -Wall -shared -lGLU -Wl,-wrap,glTexImage2D -o glCapTextures.so32 glCapTextures.c

Run with:
$ LD_PRELOAD=../glCapTextures/glCapTextures.so <OpenGL application here>

*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define glTexImage2D __real_glTexImage2D
#include <GL/gl.h>
#include <GL/glu.h>

#define max_texture_size 64

GLAPI void GLAPIENTRY __wrap_glTexImage2D( GLenum target, GLint level,
                                    GLint internalFormat,
                                    GLsizei width, GLsizei height,
                                    GLint border, GLenum format, GLenum type,
                                    const GLvoid *pixels )
{
    int max = max_texture_size >> level;
    if( max<1 ) return; /* If level is too high, discard */

    if( width > max || height > max )
    {
        // fprintf(stderr, "glTexImage2D: level %d, size=%dx%d%s\n", level, width, height, border?", with border":"");
        int newWidth, newHeight;
        if( width == height )
            newWidth = newHeight = max;
        else if( width >= height ) {
            newWidth = max_texture_size;
            newHeight = max_texture_size*height/width;
        } else {
            newHeight = max_texture_size;
            newWidth = max_texture_size*width/height;
        }
        /* Use GLU to downscale the image, if needed */
        if( pixels != NULL && target != GL_PROXY_TEXTURE_2D )
        {
            static GLubyte smallImage[max_texture_size*max_texture_size*4*sizeof( GLubyte )];
            int e = gluScaleImage( format, width, height, type, pixels, newWidth, newHeight, type, smallImage );
            if(e)
                fprintf(stderr, "glCapTextures: Error #%d in gluScaleImage(): %s\n",e,gluErrorString(e));
            else
                pixels = smallImage;
        }

        width = newWidth;
        height = newHeight;
        // fprintf(stderr, "glTexImage2D: new size=%dx%d%s\n",width, height, border?", with border":"");
    }

    __real_glTexImage2D( target, level, internalFormat, width, height, border, format, type, pixels );
    // int e = glGetError();
    // if(e!=GL_NO_ERROR)
    //     fprintf(stderr, "glCapTextures: Error #%d after glTexImage2D(): %s\n",e,gluErrorString(e));
}
