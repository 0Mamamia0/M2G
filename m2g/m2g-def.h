
#pragma once


namespace m2g {

    enum Transform {
        TRANS_NONE = 0,               //   0000
        TRANS_MIRROR_ROT180 = 1,               //   0001
        TRANS_MIRROR = 2,               //   0010
        TRANS_ROT180 = 3,               //   0011

        TRANS_MIRROR_ROT270 = 4,               //   0100
        TRANS_ROT90 = 5,               //   0101
        TRANS_ROT270 = 6,               //   0110
        TRANS_MIRROR_ROT90 = 7,               //   0111

        TRANSFORM_MASK = 0x7,
        TRANSFORM_INVERTED_AXES = 1 << 2,
        TRANSFORM_X_FLIP = 1 << 1,
        TRANSFORM_Y_FLIP = 1,
    };

    enum FontStyle {
        PLAIN = 0,
        BOLD = 1,
        ITALIC = 2,
        UNDERLINED = 4,
    };

    enum FontSize {
        SMALL = 8,
        MEDIUM = 0,
        LARGE = 16,
    };

    enum FontFace {
        SYSTEM = 0,
        MONOSPACE = 32,
        PROPORTIONAL = 64,
    };

    enum {
        FONT_STATIC_TEXT = 0,
        FONT_INPUT_TEXT = 1,
    };


    enum Anchor {
        HCENTER = 1,
        VCENTER = 2,
        LEFT = 4,
        RIGHT = 8,
        TOP = 16,
        BOTTOM = 32,
        BASELINE = 64,
    };


}




