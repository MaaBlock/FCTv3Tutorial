//
// Created by Administrator on 2025/3/5.
//
#include "../MutilThreadBase/RefCount.h"
#include "./IRenderTarget.h"
#include "./Format.h"

#ifndef FCT_IMAGE_H
#define FCT_IMAGE_H

namespace FCT {
    class Context;

    class Image : public RefCount, public IRenderTarget {
    public:
        Image() {

        }

        virtual ~Image() = default;

        virtual void create(int width, int height,Format format = Format::R8G8B8A8_UNORM) = 0;

        virtual void initTexture() { return; }

        virtual void initTarget() = 0;

        virtual void bind(Context* ctx) = 0;

        int getWidth() const { return m_width; }

        int getHeight() const { return m_height; }

        Format getFormat() const { return m_format; }
    protected:
        int m_width;
        int m_height;
        Format m_format;
    };
} // namespace FCT

#endif //FCT_IMAGE_H