//
// Created by Administrator on 2025/5/4.
//
#include "../headers.h"
namespace FCT
{
    void Freetype_FontCommon::init()
    {
        if (FT_Init_FreeType(&g_library))
            fout << "Could not initialize FreeType library" << std::endl;
    }
    FCT::Freetype_Font *FCT::Freetype_FontCommon::create()
    {
        return new Freetype_Font(this);
    }

    void FCT::Freetype_FontCommon::tern()
    {
        FT_Done_FreeType(g_library);
    }
}