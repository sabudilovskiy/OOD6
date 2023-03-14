//
// Created by Andrei Budilovskii on 14.03.2023.
//

#pragma once
#include <string_view>
#include <string>
#include <array>
namespace OS {
    inline std::string exec(std::string_view cmd) {
        std::array<char, 16> buffer{};
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
}