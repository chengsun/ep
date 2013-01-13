#ifndef DEMO_H
#define DEMO_H

#include "demolib.h"

extern "C" {
    void demo_init(unsigned w, unsigned h);
    void demo_prepareFrame();
    void demo_drawFrame();
}

#endif
