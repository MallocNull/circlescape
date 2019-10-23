#include "okuu.h"

#define OKUU_MFALL(F, P, D) \
    F(P##D##i_t, D##i) \
    F(P##D##ui_t, D##ui) \
    F(P##D##f_t, D##f)
#define OKUU_MFGALL(F, P) \
    F(P##gi_t, GLint, i) \
    F(P##gui_t, GLuint, ui) \
    F(P##gf_t, GLfloat, f)

/** MATRIX GEN FUNCS **/

#define OKUU_IDENT4(T, S) \
    inline T okuu_ident##S(void) { \
        return (T) {{ \
            {1, 0, 0, 0}, \
            {0, 1, 0, 0}, \
            {0, 0, 1, 0}, \
            {0, 0, 0, 1}, \
        }}; \
    }
OKUU_MFALL(OKUU_IDENT4, mat, 4)

#define OKUU_IDENT3(T, S) \
    inline T okuu_ident##S(void) { \
        return (T) {{ \
            {1, 0, 0}, \
            {0, 1, 0}, \
            {0, 0, 1}, \
        }}; \
    }
OKUU_MFALL(OKUU_IDENT3, mat, 3)

#define OKUU_IDENT2(T, S) \
    inline T okuu_ident##S(void) { \
        return (T) {{ \
            {1, 0}, \
            {0, 1}, \
        }}; \
    }
OKUU_MFALL(OKUU_IDENT2, mat, 2)

mat4f_t okuu_ortho_ex
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom,
     GLfloat near, GLfloat far)
{
    // TODO this
    return (mat4f_t) {{{}}};
}

inline mat4f_t okuu_ortho
    (GLfloat left, GLfloat right,
     GLfloat top, GLfloat bottom)
{
    return okuu_ortho_ex(left, right,top, bottom, -1, 1);
}

/** END MATRIX GEN FUNCS **/

/** MATRIX OPERATIONS **/

#define OKUU_TRANSW(T, D, P, S) \
    T* okuu_transw##S(T* in) { \
        int i, j; P swap; \
        for(i = 0; i < D; ++i) { \
            for(j = 0; j < D; ++j) { \
                swap = in->mat[i][j]; \
                in->mat[i][j] = in->mat[D - i - 1][D - j - 1]; \
                in->mat[D - i - 1][D - j - 1] = swap; \
            } \
        } \
        return in; \
    }
#define OKUU_TRANSW_MALL(T, D) \
    OKUU_TRANSW(T##D##i_t, D, GLint, D##i) \
    OKUU_TRANSW(T##D##ui_t, D, GLuint, D##ui) \
    OKUU_TRANSW(T##D##f_t, D, GLfloat, D##f)
OKUU_TRANSW_MALL(mat, 4)
OKUU_TRANSW_MALL(mat, 3)
OKUU_TRANSW_MALL(mat, 2)

#define OKUU_TRANS_SQ(T, S) \
    inline T okuu_trans##S(T in) { \
        return *okuu_transw##S(&in); \
    }
OKUU_MFALL(OKUU_TRANS_SQ, mat, 4)
OKUU_MFALL(OKUU_TRANS_SQ, mat, 3)
OKUU_MFALL(OKUU_TRANS_SQ, mat, 2)

#define OKUU_TRANS(R, C, P, TS) \
    mat##R##x##C##TS##_t \
        okuu_trans##C##x##R##TS(mat##C##x##R##TS##_t in) \
    { \
        int i, j; mat##R##x##C##TS##_t out; \
        for(i = 0; i < R; ++i) \
            for(j = 0; j < C; ++j) \
                out.mat[j][i] = in.mat[i][j]; \
        return out; \
    }
#define OKUU_TRANS_MALL(R, C) \
    OKUU_TRANS(R, C, GLint, i) \
    OKUU_TRANS(R, C, GLuint, ui) \
    OKUU_TRANS(R, C, GLfloat, f)
OKUU_TRANS_MALL(4, 3)
OKUU_TRANS_MALL(3, 4)
OKUU_TRANS_MALL(4, 2)
OKUU_TRANS_MALL(2, 4)
OKUU_TRANS_MALL(3, 2)
OKUU_TRANS_MALL(2, 3)

/** END MATRIX OPERATIONS **/

/** DEBUG FUNCTIONS **/

#define OKUU_PRINTM(T, P, S) \
    void okuu_printm##S(void* mat, int rows, int cols) { \
        int i, j; \
        for(i = 0; i < rows; ++i) \
            for(j = 0; j < cols; ++j) \
                printf("%f ", ((T*)mat)->gl[i*cols + j]); \
    }
OKUU_MFGALL(OKUU_PRINTM, mat)

/** END DEBUG FUNCTIONS **/