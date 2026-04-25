#include "utils.h"

#include <sys/ioctl.h>  // ioctl() and TIOCGWINSZ
#include <unistd.h>     // for STDOUT_FILENO


namespace TRSS
{
namespace Utils
{

Dimensions get_console_dims()
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    return { .width = size.ws_col, .height = size.ws_row };
}

}  // namespace Utils
}  // namespace TRSS
