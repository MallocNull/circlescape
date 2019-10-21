#ifndef CSC_OKUU_H
#define CSC_OKUU_H

#include <GL/glew.h>

/** PRIMITIVES **/

#define OKUU_MTYPE(T, R, C) \
    typedef union { \
        T mat[R][C]; \
        T gl[R*C]; \
    }

#define OKUU_MTALL(R, C, N) \
    OKUU_MTYPE(GLint, R, C) N##i_t; \
    OKUU_MTYPE(GLuint, R, C) N##ui_t; \
    OKUU_MTYPE(GLfloat, R, C) N##f_t;

OKUU_MTALL(4, 4, mat4);
OKUU_MTALL(3, 3, mat3);
OKUU_MTALL(2, 2, mat2);

OKUU_MTALL(4, 1, vec4);
OKUU_MTALL(3, 1, vec3);
OKUU_MTALL(2, 1, vec2);

/** END PRIMITIVES **/

/** MATRIX GEN FUNCS **/

#define OKUU_MGALL(M) \
    M(mat4i_t, i) \
    M(mat4ui_t, ui) \
    M(mat4f_t, f)

#define OKUU_IDENT(T, S) \
    inline T okuu_ident##S(void) { \
        return (T) {{ \
            {1, 0, 0, 0}, \
            {0, 1, 0, 0}, \
            {0, 0, 1, 0}, \
            {0, 0, 0, 1}, \
        }}; \
    }

OKUU_MGALL(OKUU_IDENT)

mat4f_t okuu_ortho_ex
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom,
     GLfloat near, GLfloat far);

inline mat4f_t okuu_ortho
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom)
{
    return okuu_ortho_ex(left, right,top, bottom, -1, 1);
}


/** END MATRIX GEN FUNCS **/

/** MATRIX OPERATIONS **/

/** END MATRIX OPERATIONS **/

#endif
