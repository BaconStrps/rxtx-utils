
#pragma once
#include <cstddef>
#include <memory>
#include <vector>
bool binary_arr_eq(std::byte* arr1, std::byte* arr2, std::size_t size);

std::vector<uint8_t> generate_random_bytes(std::size_t size);
FILE* get_command_pipe(const std::string& command);
    

