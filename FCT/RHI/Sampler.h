#ifndef FCT_RHI_SAMPLER_H
#define FCT_RHI_SAMPLER_H
namespace FCT
{
    namespace RHI
    {
        enum class FilterMode {
            Nearest,
            Linear
        };

        enum class AddressMode {
            Repeat,
            MirroredRepeat,
            ClampToEdge,
            ClampToBorder,
            MirrorClampToEdge
        };

        enum class CompareOp {
            Never,
            Less,
            Equal,
            LessOrEqual,
            Greater,
            NotEqual,
            GreaterOrEqual,
            Always
        };

        enum class BorderColor {
            TransparentBlack,
            OpaqueBlack,
            OpaqueWhite
        };

        class Sampler {
        public:
            virtual ~Sampler() = default;

            virtual void setFilter(FilterMode magFilter, FilterMode minFilter, FilterMode mipmapFilter) {
                m_magFilter = magFilter;
                m_minFilter = minFilter;
                m_mipmapFilter = mipmapFilter;
            }

            virtual void setAddressMode(AddressMode u, AddressMode v, AddressMode w) {
                m_addressModeU = u;
                m_addressModeV = v;
                m_addressModeW = w;
            }

            virtual void setAnisotropy(bool enable, float maxAnisotropy) {
                m_anisotropyEnable = enable;
                m_maxAnisotropy = maxAnisotropy;
            }

            virtual void setCompare(bool enable, CompareOp op) {
                m_compareEnable = enable;
                m_compareOp = op;
            }

            virtual void setLodRange(float minLod, float maxLod, float bias) {
                m_minLod = minLod;
                m_maxLod = maxLod;
                m_mipLodBias = bias;
            }

            virtual void setBorderColor(BorderColor color) {
                m_borderColor = color;
            }

            virtual void setUnnormalizedCoordinates(bool unnormalized) {
                m_unnormalizedCoordinates = unnormalized;
            }

            virtual void create() = 0;

            virtual void setLinear() {
                setFilter(FilterMode::Linear, FilterMode::Linear, FilterMode::Linear);
                setAddressMode(AddressMode::Repeat, AddressMode::Repeat, AddressMode::Repeat);
                setAnisotropy(false, 1.0f);
                setCompare(false, CompareOp::Never);
                setLodRange(0.0f, 1000.0f, 0.0f);
                setBorderColor(BorderColor::OpaqueBlack);
                setUnnormalizedCoordinates(false);
            }

            virtual void setPoint() {
                setFilter(FilterMode::Nearest, FilterMode::Nearest, FilterMode::Nearest);
                setAddressMode(AddressMode::Repeat, AddressMode::Repeat, AddressMode::Repeat);
                setAnisotropy(false, 1.0f);
                setCompare(false, CompareOp::Never);
                setLodRange(0.0f, 1000.0f, 0.0f);
                setBorderColor(BorderColor::OpaqueBlack);
                setUnnormalizedCoordinates(false);
            }

            virtual void setAnisotropic(float maxAnisotropy = 16.0f) {
                setFilter(FilterMode::Linear, FilterMode::Linear, FilterMode::Linear);
                setAddressMode(AddressMode::Repeat, AddressMode::Repeat, AddressMode::Repeat);
                setAnisotropy(true, maxAnisotropy);
                setCompare(false, CompareOp::Never);
                setLodRange(0.0f, 1000.0f, 0.0f);
                setBorderColor(BorderColor::OpaqueBlack);
                setUnnormalizedCoordinates(false);
            }

            virtual void setLinearClamp() {
                setFilter(FilterMode::Linear, FilterMode::Linear, FilterMode::Linear);
                setAddressMode(AddressMode::ClampToEdge, AddressMode::ClampToEdge, AddressMode::ClampToEdge);
                setAnisotropy(false, 1.0f);
                setCompare(false, CompareOp::Never);
                setLodRange(0.0f, 1000.0f, 0.0f);
                setBorderColor(BorderColor::OpaqueBlack);
                setUnnormalizedCoordinates(false);
            }

            virtual void setShadowMap() {
                setFilter(FilterMode::Linear, FilterMode::Linear, FilterMode::Linear);
                setAddressMode(AddressMode::ClampToBorder, AddressMode::ClampToBorder, AddressMode::ClampToBorder);
                setAnisotropy(false, 1.0f);
                setCompare(true, CompareOp::Less);
                setLodRange(0.0f, 1000.0f, 0.0f);
                setBorderColor(BorderColor::OpaqueWhite);
                setUnnormalizedCoordinates(false);
            }

        protected:
            FilterMode m_magFilter = FilterMode::Linear;
            FilterMode m_minFilter = FilterMode::Linear;
            FilterMode m_mipmapFilter = FilterMode::Linear;

            AddressMode m_addressModeU = AddressMode::Repeat;
            AddressMode m_addressModeV = AddressMode::Repeat;
            AddressMode m_addressModeW = AddressMode::Repeat;

            float m_mipLodBias = 0.0f;
            bool m_anisotropyEnable = false;
            float m_maxAnisotropy = 1.0f;

            bool m_compareEnable = false;
            CompareOp m_compareOp = CompareOp::Never;

            float m_minLod = 0.0f;
            float m_maxLod = 1000.0f;

            BorderColor m_borderColor = BorderColor::OpaqueBlack;
            bool m_unnormalizedCoordinates = false;
        };
    }
}
#endif //FCT_RHI_SAMPLER_H