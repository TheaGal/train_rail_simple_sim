#include "ascii_canvas.h"

#include <cstdint>
#include <iostream>


namespace TRSS
{

void Ascii_canvas::clear(uint32_t width, uint32_t height)
{
    // Calc buffer size.
    size_t buffer_size{ (width + 1) * height };
    m_buf.resize(buffer_size);

    // Clear canvas.
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (i % (width + 1) == 0)
            m_buf.at(i) = '\n';
        else
            m_buf.at(i) = ' ';
    }

    m_cur_width = width;
    m_cur_height = height;
}

void Ascii_canvas::plot_char(int32_t x, int32_t y, char c)
{
    if (x < 0 || y < 0 || x >= m_cur_width || y >= m_cur_height)
        return;  // Failed to plot.

    m_buf.at((m_cur_width + 1) * y + x) = c;
}

void Ascii_canvas::render()
{
    std::cout << m_buf << std::endl;
}

}  // namespace TRSS