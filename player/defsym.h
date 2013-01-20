// hacky symbol magic
#ifndef defsym
#   error "need to define defsym to use defsym.h"
#endif

// define the symbols that are imported
defsym(demo_init,         void, unsigned, unsigned);
defsym(demo_prepareFrame, bool);
defsym(demo_drawFrame,    void);
