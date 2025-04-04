//
// Created by Administrator on 2025/4/3.
//

#ifndef FLAGS_H
#define FLAGS_H

#include <type_traits>

namespace FCT
{
    //learning from vulkan-hpp
    template <typename BitType>
    class Flags {
    public:
        using MaskType = typename std::underlying_type<BitType>::type;

        constexpr Flags() noexcept : m_mask(0) {}
        constexpr Flags(BitType bit) noexcept : m_mask(static_cast<MaskType>(bit)) {}
        constexpr Flags(Flags<BitType> const & rhs) noexcept = default;
        constexpr explicit Flags(MaskType flags) noexcept : m_mask(flags) {}

        constexpr bool operator<(Flags<BitType> const & rhs) const noexcept { return m_mask < rhs.m_mask; }
        constexpr bool operator<=(Flags<BitType> const & rhs) const noexcept { return m_mask <= rhs.m_mask; }
        constexpr bool operator>(Flags<BitType> const & rhs) const noexcept { return m_mask > rhs.m_mask; }
        constexpr bool operator>=(Flags<BitType> const & rhs) const noexcept { return m_mask >= rhs.m_mask; }
        constexpr bool operator==(Flags<BitType> const & rhs) const noexcept { return m_mask == rhs.m_mask; }
        constexpr bool operator!=(Flags<BitType> const & rhs) const noexcept { return m_mask != rhs.m_mask; }

        constexpr bool operator!() const noexcept { return !m_mask; }

        constexpr Flags<BitType> operator&(Flags<BitType> const & rhs) const noexcept { return Flags<BitType>(m_mask & rhs.m_mask); }
        constexpr Flags<BitType> operator|(Flags<BitType> const & rhs) const noexcept { return Flags<BitType>(m_mask | rhs.m_mask); }
        constexpr Flags<BitType> operator^(Flags<BitType> const & rhs) const noexcept { return Flags<BitType>(m_mask ^ rhs.m_mask); }
        constexpr Flags<BitType> operator~() const noexcept { return Flags<BitType>(~m_mask); }

        Flags<BitType> & operator=(Flags<BitType> const & rhs) noexcept = default;
        Flags<BitType> & operator|=(Flags<BitType> const & rhs) noexcept { m_mask |= rhs.m_mask; return *this; }
        Flags<BitType> & operator&=(Flags<BitType> const & rhs) noexcept { m_mask &= rhs.m_mask; return *this; }
        Flags<BitType> & operator^=(Flags<BitType> const & rhs) noexcept { m_mask ^= rhs.m_mask; return *this; }

        explicit constexpr operator bool() const noexcept { return !!m_mask; }
        explicit constexpr operator MaskType() const noexcept { return m_mask; }

    private:
        MaskType m_mask;
    };
}
#endif //FLAGS_H