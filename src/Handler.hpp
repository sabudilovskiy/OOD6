//
// Created by Andrei Budilovskii on 14.03.2023.
//

#pragma once
#include <functional>
#include <string_view>

std::function<bool(std::string_view)> CreateAll();