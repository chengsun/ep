#ifndef DEMO_H
#define DEMO_H

#include "demolib.h"

extern "C" {
    void demo_init(unsigned w, unsigned h);
    bool demo_prepareFrame();
    void demo_drawFrame();
}

#endif
