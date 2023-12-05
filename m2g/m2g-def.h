//
// Created by Admin on 2023/12/4.
//

#ifndef M2G_DEF_H__
#define M2G_DEF_H__

enum Transform {
    TRANS_NONE          = 0,               //   0000
    TRANS_MIRROR_ROT180 = 1,               //   0001
    TRANS_MIRROR        = 2,               //   0010
    TRANS_ROT180        = 3,               //   0011

    TRANS_MIRROR_ROT270 = 4,               //   0100
    TRANS_ROT90         = 5,               //   0101
    TRANS_ROT270        = 6,               //   0110
    TRANS_MIRROR_ROT90  = 7,               //   0111

    TRANSFORM_MASK      =  0x7,
    TRANSFORM_INVERTED_AXES =  1 << 2,
    TRANSFORM_X_FLIP =         1 << 1,
    TRANSFORM_Y_FLIP =         1,
};



#endif //M2G_DEF_H__
