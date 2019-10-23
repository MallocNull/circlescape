#ifndef CSC_OKUU_H
#define CSC_OKUU_H

#include <stdio.h>
#include <GL/glew.h>

/** PRIMITIVES **/

#define OKUU_MGTYPE(T) \
    typedef union { \
        T** mat; \
        T* gl; \
    }
OKUU_MGTYPE(GLint) matgi_t;
OKUU_MGTYPE(GLuint) matgui_t;
OKUU_MGTYPE(GLfloat) matgf_t;

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

OKUU_MTALL(3, 4, mat4x3);
OKUU_MTALL(4, 3, mat3x4);
OKUU_MTALL(2, 4, mat4x2);
OKUU_MTALL(4, 2, mat2x4);
OKUU_MTALL(2, 3, mat3x2);
OKUU_MTALL(3, 2, mat2x3);

OKUU_MTALL(4, 1, vec4);
OKUU_MTALL(3, 1, vec3);
OKUU_MTALL(2, 1, vec2);

/** END PRIMITIVES **/

/** MATRIX GEN FUNCS **/

#define OKUU_IDENT_PROTO(S) \
    mat##S##_t okuu_ident##S(void);
#define OKUU_IDENT_ALL(D) \
    OKUU_IDENT_PROTO(D##i) \
    OKUU_IDENT_PROTO(D##ui) \
    OKUU_IDENT_PROTO(D##f)
OKUU_IDENT_ALL(4)
OKUU_IDENT_ALL(3)
OKUU_IDENT_ALL(2)

mat4f_t okuu_ortho_ex
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom,
     GLfloat near, GLfloat far);

mat4f_t okuu_ortho
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom);

/** END MATRIX GEN FUNCS **/

/** MATRIX OPERATIONS **/

#define OKUU_SQTRANSW_ALL(F, D) \
    F(D##i) \
    F(D##ui) \
    F(D##f)

#define OKUU_TRANSW_PROTO(S) \
    mat##S##_t* okuu_transw##S(mat##S##_t* in);
OKUU_SQTRANSW_ALL(OKUU_TRANSW_PROTO, 4)
OKUU_SQTRANSW_ALL(OKUU_TRANSW_PROTO, 3)
OKUU_SQTRANSW_ALL(OKUU_TRANSW_PROTO, 2)

#define OKUU_SQTRANS_PROTO(S) \
    mat##S##_t okuu_trans##S(mat##S##_t in);
OKUU_SQTRANSW_ALL(OKUU_SQTRANS_PROTO, 4)
OKUU_SQTRANSW_ALL(OKUU_SQTRANS_PROTO, 3)
OKUU_SQTRANSW_ALL(OKUU_SQTRANS_PROTO, 2)

#define OKUU_TRANS_PROTO(S, R, C) \
    mat##R##x##C##S##_t \
        okuu_trans##C##x##R##S(mat##C##x##R##S##_t in);
#define OKUU_TRANS_ALL(R, C) \
    OKUU_TRANS_PROTO(i, R, C) \
    OKUU_TRANS_PROTO(ui, R, C) \
    OKUU_TRANS_PROTO(f, R, C)
OKUU_TRANS_ALL(4, 3)
OKUU_TRANS_ALL(3, 4)
OKUU_TRANS_ALL(4, 2)
OKUU_TRANS_ALL(2, 4)
OKUU_TRANS_ALL(3, 2)
OKUU_TRANS_ALL(2, 3)

/** END MATRIX OPERATIONS **/

/** DEBUG FUNCTIONS **/

void okuu_printmi(void* mat, int rows, int cols);
void okuu_printmui(void* mat, int rows, int cols);
void okuu_printmf(void* mat, int rows, int cols);

/** END DEBUG FUNCTIONS **/

#endif
