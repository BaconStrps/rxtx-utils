#pragma once

// #include <csics/io/Buffer.hpp>
#include <concepts>
#include <string_view>

#include "../Buffer.hpp"

namespace csics::serialization {

enum class SerializationStatus { Ok };

template <typename T>
concept Field = requires(T t) {
    typename T::name_type;
    typename T::value_type;
} && requires(T t) {
    { t.name } -> std::convertible_to<std::string_view>;
    { t.ptr } -> std::same_as<typename T::value_type*>;
};

template <typename T>
concept FieldList =
    requires(T t) {
        typename std::tuple_size<T>::value;
        { std::get<0>(t) } -> Field;
    } &&
    []<std::size_t... Is>(
        std::index_sequence<Is...>) {  // ensure all elements in the tuple are
                                       // Fields
        return (Field<std::tuple_element_t<Is, T>> && ...);
    }(std::make_index_sequence<std::tuple_size<T>::value>{});

template <typename S>
concept Serializer = requires(S s, io::BufferView bv, std::string_view key) {
    { s.begin_obj(bv) } -> std::same_as<SerializationStatus>;
    { s.end_obj(bv) } -> std::same_as<SerializationStatus>;
    { s.begin_array(bv) } -> std::same_as<SerializationStatus>;
    { s.end_array(bv) } -> std::same_as<SerializationStatus>;
    { s.key(bv, key) } -> std::same_as<SerializationStatus>;
    { S::key_overhead() } -> std::convertible_to<std::size_t>;
    { S::obj_overhead() } -> std::convertible_to<std::size_t>;
    { S::array_overhead() } -> std::convertible_to<std::size_t>;
    { S::meta_overhead() } -> std::convertible_to<std::size_t>;
    { S::template value_overhead<int>() } -> std::convertible_to<std::size_t>;
    {
        S::template value_overhead<double>()
    } -> std::convertible_to<std::size_t>;
    { S::template value_overhead<bool>() } -> std::convertible_to<std::size_t>;
    {
        S::template value_overhead<std::string_view>()
    } -> std::convertible_to<std::size_t>;
    { s.value(bv, int{}) } -> std::same_as<SerializationStatus>;
    { s.value(bv, double{}) } -> std::same_as<SerializationStatus>;
    { s.value(bv, bool{}) } -> std::same_as<SerializationStatus>;
    { s.value(bv, std::string_view{}) } -> std::same_as<SerializationStatus>;
} && std::default_initializable<S>;

template <typename T>
concept StructSerializable = requires(T t) {
    { T::fields() } -> FieldList;
};

template <typename T>
concept ArraySerializable = requires(T t) {
    typename T::value_type;
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.data() } -> std::same_as<typename T::value_type*>;
};

template <typename T, typename S>
concept PrimitiveSerializable = requires(T t, S s, io::BufferView bv) {
    { s.value(bv, t) } -> std::same_as<SerializationStatus>;
} && Serializer<S>;

template <typename T, typename S>
concept Serializable = requires(T t, S s, io::BufferView bv) {
    { t.serialize(s, bv) } -> std::same_as<SerializationStatus>;
};

struct serializer {
    template <Serializer S, StructSerializable T>
    SerializationStatus operator()(S& s, io::BufferView bv, T&& obj) const {
        auto fields = T::fields();
        s.begin_obj(bv);
        std::apply(
            [&](auto&&... field) {
                (..., (s.key(bv, field.name),
                       this->operator()(
                           s, bv, obj.*(field.ptr))));  // Serialize each field
            },
            fields);
        s.end_obj(bv);
        return SerializationStatus::Ok;
    }

    template <Serializer S, ArraySerializable T>
    SerializationStatus operator()(S& s, io::BufferView bv, T&& arr) const {
        SerializationStatus status;
        s.begin_array(bv);
        for (std::size_t i = 0; i < arr.size(); ++i) {
            if ((status = this->operator()(s, bv, arr.data()[i])) !=
                SerializationStatus::Ok) {
                return status;
            }
        }
        s.end_array(bv);
        return SerializationStatus::Ok;
    }

    template <Serializer S, PrimitiveSerializable<S> T>
    SerializationStatus operator()(S& s, io::BufferView bv, T&& value) const {
        return s.value(bv, value);
    }

    template <Serializer S,
              Serializable<S> T>  // Fallback for types with a serialize method
    SerializationStatus operator()(S& s, io::BufferView bv, T&& obj) const {
        return obj.serialize(s, bv);
    }

    template <Serializer S, typename T>
    SerializationStatus operator()(S&, io::BufferView, T&&) const {
        static_assert(sizeof(T) == 0,
                      "Type is not serializable");
        return SerializationStatus::Ok;  // Unreachable, but satisfies return type
    };

    template <Serializer S, typename T>
    SerializationStatus operator()(io::BufferView, T&&) const {
        static_assert(sizeof(T) == 0,
                      "Type is not serializable");
        return SerializationStatus::Ok;  // Unreachable, but satisfies return type
    };
};

}  // namespace csics::serialization
