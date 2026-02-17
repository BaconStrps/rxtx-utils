#include <gtest/gtest.h>

#include <csics/serialization/JSONSerializer.hpp>

class TestClass {
   private:
    int a;
    double b;
    std::string c;

   public:
    TestClass(int a, double b, std::string c) : a(a), b(b), c(c) {}
    static consteval auto fields() {
        using namespace csics;
        return serialization::fields(serialization::field("a", &TestClass::a),
                                     serialization::field("b", &TestClass::b),
                                     serialization::field("c", &TestClass::c));
    }
};

TEST(CSICSSerializationTests, JSONBasicSerialization) {
    using namespace csics::serialization;

    JSONSerializer serializer;
    char buffer[256];
    TestClass c(42, 3.14, "Hello, world!");

    constexpr std::string_view expected =
        R"({"a":42,"b":3.14,"c":"Hello, world!"})";

    csics::io::BufferView bv(buffer, sizeof(buffer));
    serialize(serializer, bv, c);
    std::string_view result(buffer, sizeof(buffer) - bv.size());
    std::cout << "Serialized JSON: " << std::string_view(buffer, sizeof(buffer)) << std::endl;
    EXPECT_EQ(result, expected);
};

TEST(CSICSSerializationTests, JSONArraySerialization) {
    using namespace csics::serialization;

    JSONSerializer serializer;
    char buffer[256];
    std::vector<int> vec = {1, 2, 3, 4, 5};

    constexpr std::string_view expected = R"([1,2,3,4,5])";

    csics::io::BufferView bv(buffer, sizeof(buffer));
    serialize(serializer, bv, vec);
    std::string_view result(buffer, sizeof(buffer) - bv.size());
    std::cout << "Serialized JSON: " << std::string_view(buffer, sizeof(buffer)) << std::endl;
    EXPECT_EQ(result, expected);
};
