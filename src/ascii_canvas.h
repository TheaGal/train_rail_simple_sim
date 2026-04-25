#pragma once

#include <cstdint>
#include <string>


namespace TRSS
{

class Ascii_canvas
{
public:
    void clear(uint32_t width, uint32_t height);

    void plot_char(int32_t x, int32_t y, char c);

    void render();

private:
    std::string m_buf;
    uint32_t m_cur_width;
    uint32_t m_cur_height;
};

}  // namespace TRSS