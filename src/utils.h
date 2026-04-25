#pragma once

#include <cstdint>
namespace TRSS
{
namespace Utils
{

struct Dimensions
{
    uint32_t width;
    uint32_t height;
};

Dimensions get_console_dims();

}  // namespace Utils
}  // namespace TRSS