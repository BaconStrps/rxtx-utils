#pragma once
#include <cmath>
#include <csics/linalg/Vec.hpp>

namespace csics::linalg {

class Dot {
   public:
    template <typename T>
    constexpr auto operator()(const T& a, const T& b) const noexcept {
        return apply(a, b);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& a, const VecU& b) {
        return std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((a.template get<Is>(a) * b.template get<Is>(b)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{});
    };
};

class Cross {
   public:
    template <typename T>
    constexpr auto operator()(T&& a, T&& b) const noexcept {
        return apply(std::forward<T>(a), std::forward<T>(b));
    }

    template <Vec3Like Vec3U>
    static auto apply(Vec3U&& a, Vec3U&& b) {
        return Vec3U(a.y() * b.z() - a.z() * b.y(),
                     a.z() * b.x() - a.x() * b.z(),
                     a.x() * b.y() - a.y() * b.x());
    };
};

class Mag {
   public:
    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        return apply(v);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& v) {
        return std::sqrt(std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((v.template get<Is>(v) * v.template get<Is>(v)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{}));
    }
};

class Abs {
   public:
    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        return apply(v);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& v) {
        return std::sqrt(std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((v.template get<Is>(v) * v.template get<Is>(v)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{}));
    }
};

constexpr Dot dot;
constexpr Cross cross;

};  // namespace csics::linalg
