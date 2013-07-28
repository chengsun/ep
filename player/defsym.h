// hacky symbol magic
#ifndef defsym
#   error "need to define defsym to use defsym.h"
#endif

// define the symbols that are imported
defsym(true,  demo_init,         void, unsigned, unsigned);
defsym(true,  demo_prepareFrame, bool);
defsym(true,  demo_drawFrame,    void);
defsym(false, demo_evtMouseButton,    void, uint8_t, bool);
defsym(false, demo_evtMouseMove,    void, int, int);
