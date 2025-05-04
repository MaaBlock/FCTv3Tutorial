//
// Created by Administrator on 2025/5/4.
//
#include "../ThirdParty.h"
#ifndef FREETYPE_FONTCOMMON_H
#define FREETYPE_FONTCOMMON_H
namespace FCT
{
    class Freetype_Font;
    class Freetype_FontCommon
    {
    public:
        FT_Library g_library;
    public:
        void init();
        Freetype_Font* create();
        void tern();
    };

}
#endif //FREETYPE_FONTCOMMON_H
