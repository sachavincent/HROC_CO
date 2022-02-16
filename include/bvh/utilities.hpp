#ifndef BVH_UTILITIES_HPP
#define BVH_UTILITIES_HPP

#include <cstring>
#include <cstdint>
#include <atomic>
#include <memory>
#include <queue>
#include <algorithm>
#include <omp.h>
#include <cmath>
#include <climits>
#include <type_traits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bvh/bounding_box.hpp"

namespace bvh
{

    /// Safe function to reinterpret the bits of the given value as another type.
    template <typename To, typename From>
    To as(From from)
    {
        static_assert(sizeof(To) == sizeof(From));
        To to;
        std::memcpy(&to, &from, sizeof(from));
        return to;
    }

    /// Equivalent to copysign(x, x * y).
    inline float product_sign(float x, float y)
    {
        return as<float>(as<uint32_t>(x) ^ (as<uint32_t>(y) & UINT32_C(0x80000000)));
    }

    /// Equivalent to copysign(x, x * y).
    inline double product_sign(double x, double y)
    {
        return as<double>(as<uint64_t>(x) ^ (as<uint64_t>(y) & UINT64_C(0x8000000000000000)));
    }

    inline float fast_multiply_add(float x, float y, float z)
    {
#ifdef FP_FAST_FMAF
        return std::fmaf(x, y, z);
#else
#pragma STDC FP_CONTRACT ON
        return x * y + z;
#endif
    }

    inline double fast_multiply_add(double x, double y, double z)
    {
#ifdef FP_FAST_FMA
        return std::fma(x, y, z);
#else
#pragma STDC FP_CONTRACT ON
        return x * y + z;
#endif
    }

    /// Returns the mininum of two values.
    /// Guaranteed to return a non-NaN value if the right hand side is not a NaN.
    template <typename T>
    const T &robust_min(const T &x, const T &y)
    {
        return x < y ? x : y;
    }

    /// Returns the maximum of two values.
    /// Guaranteed to return a non-NaN value if the right hand side is not a NaN.
    template <typename T>
    const T &robust_max(const T &x, const T &y)
    {
        return x > y ? x : y;
    }

    template <typename T>
    void atomic_max(std::atomic<T> &x, T y)
    {
        auto z = x.load();
        while (z < y && !x.compare_exchange_weak(z, y))
            ;
    }

    /// Templates that contains signed and unsigned integer types of the given number of bits.
    template <size_t Bits>
    struct SizedIntegerType
    {
        static_assert(Bits <= 8);
        using Signed = int8_t;
        using Unsigned = uint8_t;
    };

    template <>
    struct SizedIntegerType<64>
    {
        using Signed = int64_t;
        using Unsigned = uint64_t;
    };

    template <>
    struct SizedIntegerType<32>
    {
        using Signed = int32_t;
        using Unsigned = uint32_t;
    };

    template <>
    struct SizedIntegerType<16>
    {
        using Signed = int16_t;
        using Unsigned = uint16_t;
    };

    /// Adds the given number of ULPs (Unit in the Last Place) to the floating-point argument.
    template <typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
    T add_ulp_magnitude(T x, unsigned ulps)
    {
        using U = typename SizedIntegerType<sizeof(T) * CHAR_BIT>::Unsigned;
        return std::isfinite(x) ? as<T>(as<U>(x) + ulps) : x;
    }

    /// Computes the (rounded-up) compile-time log in base-2 of an unsigned integer.
    inline constexpr size_t round_up_log2(size_t i, size_t p = 0)
    {
        return (size_t(1) << p) >= i ? p : round_up_log2(i, p + 1);
    }

    /// Returns the number of bits that are equal to zero,
    /// starting from the most significant one.
    template <typename T, std::enable_if_t<std::is_unsigned<T>::value, int> = 0>
    size_t count_leading_zeros(T value)
    {
        static constexpr size_t bit_count = sizeof(T) * CHAR_BIT;
        size_t a = 0;
        size_t b = bit_count;
        auto all = T(-1);
        for (size_t i = 0; i < round_up_log2(bit_count); i++)
        {
            auto m = (a + b) / 2;
            auto mask = all << m;
            if (value & mask)
                a = m + 1;
            else
                b = m;
        }
        return bit_count - b;
    }

    /// Permutes primitives such that the primitive at index i is `primitives[indices[i]]`.
    /// Allows to remove indirections in the primitive intersectors.
    template <typename Primitive>
    std::unique_ptr<Primitive[]> permute_primitives(const Primitive *primitives, const size_t *indices, size_t primitive_count)
    {
        auto primitives_copy = std::make_unique<Primitive[]>(primitive_count);
#pragma omp parallel for
        for (size_t i = 0; i < primitive_count; ++i)
            primitives_copy[i] = primitives[indices[i]];
        return primitives_copy;
    }

    /// Computes the union of all the bounding boxes in the given array.
    static BoundingBox compute_bounding_boxes_union(const std::vector<std::shared_ptr<BoundingBox>> &bboxes)
    {
        AxisBoundingBox bbox = AxisBoundingBox::empty();

        //#pragma omp declare reduction (bbox_extend:BoundingBox::omp_out.extend(omp_in))
        //  initializer(omp_priv = BoundingBox::empty())

        size_t count = bboxes.size();
        //#pragma omp parallel for reduction(bbox_extend : bbox)
        for (size_t i = 0; i < count; ++i)
            bbox.extend(*(bboxes[i].get()));

        return bbox;
    }

} // namespace bvh

#endif
