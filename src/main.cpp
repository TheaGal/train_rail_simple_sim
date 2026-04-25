#include "ascii_canvas.h"
#include "utils.h"

#include <chrono>
#include <cstdint>
#include <thread>


int32_t main()
{
    int32_t pos_x = 0;
    int32_t pos_y = 3;

    TRSS::Ascii_canvas canvas;

    while (true)
    {
        auto dims = TRSS::Utils::get_console_dims();
        canvas.clear(dims.width, dims.height);

        canvas.plot_char(pos_x, pos_y, 'X');
        canvas.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        pos_x++;

        if (pos_x >= dims.width)
            break;  // Finish.
    }

    return 0;
}
