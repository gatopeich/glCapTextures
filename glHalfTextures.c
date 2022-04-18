/* gatopeich's OpenGL wrapper (c) 2007 (http://gatopeichs.pbwiki.com/)

v1.0: Wraps glTexImage2D to reduce textures that are too big for my 3D card.
Particularly a Savage MX/IX card with max texture size 256x256.
Works in other configurations, at least in Mesa-DRI-Intel 915G.
Can be used to generally reduce texture B/W, possibly improving performance.

Compile with:
$ gcc -Wall -fPIC -shared -Wl,-soname,glHalfTextures.so -ldl -lGLU -o glHalfTextures.so glHalfTextures.c

Run with:
$ LD_PRELOAD=../glHalfTextures/glHalfTextures.so <OpenGL application here>

*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>

static int (*real_glTexImage2D)( GLenum target, GLint level,
                                 GLint internalFormat,
                                 GLsizei width, GLsizei height,
                                 GLint border, GLenum format, GLenum type,
                                 const GLvoid *pixels ) = NULL;

GLAPI void GLAPIENTRY glTexImage2D( GLenum target, GLint level,
                                    GLint internalFormat,
                                    GLsizei width, GLsizei height,
                                    GLint border, GLenum format, GLenum type,
                                    const GLvoid *pixels )
{
    #define max_texture_size 128

    if (real_glTexImage2D == NULL) {
        fprintf(stderr, "glHalfTextures: wrapping glTexImage2D...\n");
        real_glTexImage2D = dlsym(RTLD_NEXT, "glTexImage2D");
        char *errmsg = dlerror();
        if (errmsg  != NULL) {
            fprintf(stderr, "glHalfTextures: dlopen failed : %s\n", errmsg);
            exit(1);
        } else {
            fprintf(stderr, "glHalfTextures: wrapping done, textures now clamped to %dx%d\n", max_texture_size, max_texture_size);
        }
    }

    int max = max_texture_size >> level;
    if( 0==max ) return; /* If level is too high, discard */
    max += 2*border;

    if( width > max || height > max )
    {
        fprintf(stderr, "glTexImage2D: level %d, size=%dx%d%s\n"
                    , level, width, height, border?", with border":"");
        int newWidth, newHeight;
        if( width == height ) newWidth = newHeight = max;
        else if( width > height ) {
            newWidth = max_texture_size+2*border;
            newHeight = max_texture_size*height/width;
        } else {
            newHeight = max_texture_size+2*border;
            newWidth = max_texture_size*width/height;
        }
        /* Use GLU to downscale the image, if needed */
        if( pixels != NULL && target != GL_PROXY_TEXTURE_2D )
        {
            static GLubyte smallImage[max_texture_size*max_texture_size*4*sizeof( GLubyte )];
            int e = gluScaleImage( format, width, height, type, pixels,
                            newWidth, newHeight, type, smallImage );
            if(e) fprintf(stderr, "glHalfTextures: Error #%d in gluScaleImage(): %s\n",e,gluErrorString(e));
            pixels = smallImage;
        }

        width = newWidth;
        height = newHeight;
        fprintf(stderr, "glTexImage2D: new size=%dx%d%s\n",width, height, border?", with border":"");
    }
    real_glTexImage2D( target, level,
                        internalFormat,
                        width, height,
                        border, format, type,
                        pixels );
    int e = glGetError();
    if(e!=GL_NO_ERROR) fprintf(stderr, "glHalfTextures: Error #%d after glTexImage2D(): %s\n",e,gluErrorString(e));
}
