#include "../headers/config.h"

void c_config::init_config()
{
    static bool init = false;

    if (!init)
    {

        init = true;
    }
}
