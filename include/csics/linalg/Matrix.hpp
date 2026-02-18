#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>
#include "csics/linalg/Vec.hpp"
namespace csics::linalg {

template <typename T, std::size_t Rows, std::size_t Cols>
class Matrix {
   public:
    using value_type = T;
    static constexpr std::size_t rows_v = Rows;
    static constexpr std::size_t cols_v = Cols;
    using vec_type = Vec<T, Cols>;

    Matrix() = default;
    template <typename... Args>
        requires(sizeof...(Args) == Rows * Cols) &&
                (std::same_as<Args, T> && ...)
    Matrix(Args... xs) : data_{xs...} {}

    template <std::size_t I, std::size_t J>
    constexpr const T get() const noexcept {
        static_assert(I < Rows && J < Cols, "Index out of bounds");
        return data_[I * Cols + J];
    }

    consteval std::size_t size() { return Rows * Cols; }
    template <std::size_t I>
    consteval T& get() {
        static_assert(I < Rows * Cols, "Index out of bounds");
        return data_[I];
    }

    template <std::size_t I, std::size_t J>
    consteval T& get() {
        static_assert(I < Rows && J < Cols, "Index out of bounds");
        return data_[calculate_index(I, J)];
    }

    consteval std::size_t rows() { return Rows; }

    consteval std::size_t cols() { return Cols; }

    constexpr Vec<T, Cols> get_row(std::size_t i) const noexcept {
        return mat_get_row_impl<Vec<T, Cols>>(i, std::make_index_sequence<Cols>{});
    }

    constexpr Vec<T, Rows> get_col(std::size_t j) const noexcept {
        return mat_get_col_impl<Vec<T, Rows>>(j, std::make_index_sequence<Rows>{});
    }

   private:
    T data_[Rows * Cols];

    constexpr std::size_t calculate_index(std::size_t i,
                                          std::size_t j) const noexcept {
        return i * Cols + j;
    }

    template <StaticVecLike V, std::size_t... Is>
        constexpr auto mat_get_row_impl(std::size_t i, std::index_sequence<Is...>) const noexcept {
        return Vec<typename V::value_type, sizeof...(Is)>{(data_[calculate_index(i, Is)])...};
    }
    template <StaticVecLike V, std::size_t... Is>
    constexpr auto mat_get_col_impl(std::size_t j, std::index_sequence<Is...>) const noexcept {
        return Vec<typename V::value_type, sizeof...(Is)>{(data_[calculate_index(Is, j)])...};
    }
};

template <typename Mat>
concept SmallMatrix = Mat::rows_v <= 4 && Mat::cols_v <= 4;

template <typename Mat>
concept SquareMatrix = Mat::rows_v == Mat::cols_v;

template <typename Mat>
concept SmallSquareMatrix = SmallMatrix<Mat> && SquareMatrix<Mat>;

template <typename MatA, typename MatB>
concept MatrixCompatible = MatA::cols_v == MatB::rows_v;

template <typename Mat>
concept Matrix2x2 = Mat::rows_v == 2 && Mat::cols_v == 2;
template <typename Mat>
concept Matrix3x3 = Mat::rows_v == 3 && Mat::cols_v == 3;

template <SmallMatrix Mat, std::size_t... Is>
constexpr auto mat_add_impl(Mat&& a, Mat&& b, std::index_sequence<>) {
    using T = typename Mat::value_type;
    return Mat{std::array<T, Mat::rows_v * Mat::cols_v>{
        (std::forward<Mat>(a).template get<Is>() + std::forward<Mat>(b).template get<Is>()) ...
    }};
}

template <SmallMatrix Mat>
constexpr auto operator+(Mat&& a, Mat&& b) {
    return mat_add_impl(std::forward<Mat>(a), std::forward<Mat>(b),
                        std::make_index_sequence<Mat::rows_v * Mat::cols_v>{});
}

template <SmallMatrix Mat, std::size_t... Is>
constexpr auto mat_sub_impl(Mat&& a, Mat&& b, std::index_sequence<Is...>) {
    using T = typename Mat::value_type;
    return Mat{std::array<T, Mat::rows_v * Mat::cols_v>{
        (std::forward<Mat>(a).template get<Is>() - std::forward<Mat>(b).template get<Is>()) ...
    }};
}

template <SmallMatrix Mat>
constexpr auto operator-(Mat&& a, Mat&& b) {
    return mat_sub_impl(std::forward<Mat>(a), std::forward<Mat>(b),
                        std::make_index_sequence<Mat::rows_v * Mat::cols_v>{});
}

template <SmallMatrix Mat, std::size_t... Is>
constexpr auto mat_scalar_mul_impl(Mat&& m, typename Mat::value_type s,
                                    std::index_sequence<Is...>) {
    using T = typename Mat::value_type;
    return Mat{std::array<T, Mat::rows_v * Mat::cols_v>{
        (std::forward<Mat>(m).template get<Is>() * s) ...
    }};
}

template <SmallMatrix Mat>
constexpr auto operator*(Mat&& m, typename Mat::value_type s) {
    return mat_scalar_mul_impl(std::forward<Mat>(m), s,
                                std::make_index_sequence<Mat::rows_v * Mat::cols_v>{});
}

template <SmallMatrix Mat>
constexpr auto operator*(typename Mat::value_type s, Mat&& m) {
    return mat_scalar_mul_impl(std::forward<Mat>(m), s,
                                std::make_index_sequence<Mat::rows_v * Mat::cols_v>{});
}


};  // namespace csics::linalg
