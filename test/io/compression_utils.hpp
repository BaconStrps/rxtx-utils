#pragma once
#include <filesystem>
#include <vector>

std::vector<char> run_cmdline(
    const std::string& cmd, // eg: "zlib -d %s -o %s"
    const std::filesystem::path& input_path
);
