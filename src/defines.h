#pragma once

#include <cstdint>

#define global_variable static;

#define KB(x) ((uint64_t)x * 1024)
#define MB(x) ((uint64_t)1024 * KB(1))
#define GB(x) ((uint64_t)1024 * MB(1))