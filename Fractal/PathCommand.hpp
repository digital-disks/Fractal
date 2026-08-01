#pragma once

#include <vector>

struct PathCommand {
    char type;
    std::vector<float> args;
};