#ifndef DEMO_H
#define DEMO_H

#include "demolib.h"

extern "C" {
    void demo_init(unsigned w, unsigned h);
    bool demo_prepareFrame();
    void demo_drawFrame();

    void demo_evtMouseMove(int x, int y);
    void demo_evtMouseButton(uint8_t button, bool state);
}

#endif
