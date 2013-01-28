#include "demolib.h"

#include <GL/glew.h>
#include <SDL.h>
#include <cstdio>
#include <ctime>
#include <getopt.h>


#define defsym(symname, ret, ...) \
    static ret (*symname)(__VA_ARGS__) = 0;
#include "defsym.h"
#undef defsym


#if defined(WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <memory>

class LocalFree_deleter {
public:
    template <class T>
    void operator()(T *p) {
        LocalFree(p);
    }
};

static std::unique_ptr<char, LocalFree_deleter> formattedError(DWORD err)
{
    char *lpMsgBuf;
    FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR> (&lpMsgBuf),
            0, NULL);
    return std::unique_ptr<char, LocalFree_deleter>(lpMsgBuf);
}

#define defsym(symname, ret, ...) \
    symname = reinterpret_cast<ret (*)(__VA_ARGS__)>(GetProcAddress(demoLib, #symname)); \
    if (!symname) { \
        LOG("ERROR: Symbol \"%s\" undefined in demo library \"%s\"", #symname, libName); \
        missingSymbol = true; \
    }

static void pl_loadDemoLib(const char *libName)
{
    HMODULE demoLib = LoadLibrary(libName);
    if (!demoLib) {
        LOG("ERROR: Couldn't load demo library \"%s\": %s", libName, formattedError(GetLastError()).get());
        exit(1);
    }
    bool missingSymbol = false;

#   include "defsym.h"

    if (missingSymbol) exit(1);
}

#undef defsym


#elif defined(LINUX)

#include <dlfcn.h>

#define defsym(symname, ret, ...) \
    symname = reinterpret_cast<ret (*)(__VA_ARGS__)>(dlsym(demoLib, #symname)); \
    if (!symname) { \
        LOG("ERROR: Symbol \"%s\" undefined in demo library \"%s\"", #symname, libName); \
        missingSymbol = true; \
    }

static void pl_loadDemoLib(const char *libName)
{
    void *demoLib = dlopen(libName, RTLD_NOW);
    if (!demoLib) {
        LOG("ERROR: Couldn't load demo library \"%s\": %s", libName, dlerror());
        exit(1);
    }
    bool missingSymbol = false;

#   include "defsym.h"

    if (missingSymbol) exit(1);
}

#undef defsym

#else
#   error "Compiling for unknown platform"
#endif


static SDL_Surface *screen;

static void pl_setVideoMode(int w, int h)
{
    LOG("Setting %dx%d GL video mode", w, h);
    screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL);
    if (screen) {
        LOG("Set %dx%d GL video mode", screen->w, screen->h);
    } else {
        LOG("ERROR: Couldn't set %dx%d GL video mode: %s", w, h, SDL_GetError());
        exit(1);
    }
}

static const char *demoLibrary = "demo.so";

int main(int argc, char *argv[])
{
    LOG("Player starting");

    LOG("Parsing switches");
    int c, opt_idx;
    static const struct option long_opts[] = {
        {"help", no_argument, 0, 'h'},
        {"demo-library", required_argument, 0, 'd'}
    };
    while ((c = getopt_long(argc, argv, "hd:", long_opts, &opt_idx)) != -1) {
        switch (c) {
        case 'd':
            demoLibrary = optarg;
            break;
        case 0:
            if (long_opts[opt_idx].flag != 0) break;
        case 'h':
        default:
            LOG("-d/--demo-library [lib.so]  Demo library location");
            LOG("-h/--help                   Help");
            exit(c == 'h' ? 0 : 1);
        }
    }

    LOG("Initialising SDL video");
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);
    SDL_WM_SetCaption("Demo player", "player");

    LOG("Initialising GL");
    pl_setVideoMode(600, 480);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOG("ERROR: GLEW initialisation failed: %s", glewGetErrorString(err));
        exit(1);
    }
    LOG("GLEW version: %s", glewGetString(GLEW_VERSION));

    LOG("OpenGL vendor: %s", glGetString(GL_VENDOR));
    LOG("OpenGL renderer: %s", glGetString(GL_RENDERER));
    LOG("OpenGL version: %s", glGetString(GL_VERSION));

#ifndef GLEW_NO_GLU
    LOG("GLU version: %s", gluGetString(GLU_VERSION));
#else
    LOG("GLU not present");
#endif

    LOG("Loading demo library \"%s\"", demoLibrary);
    pl_loadDemoLib(demoLibrary);

    LOG("Initialising demo");
    demo_init(screen->w, screen->h);

    LOG("Beginning render loop");
    unsigned nFrames = 0, oldTicks = 0;
    while (1) {
        if (!demo_prepareFrame()) {
            break;
        }

        bool done = false;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    LOG("Window closed");
                    done = true;
                    break;
            }
        }
        if (done) break;

        Uint8 *keys = SDL_GetKeyState(NULL);
        if (keys[SDLK_ESCAPE]) {
            LOG("Escape pressed");
            break;
        }

        demo_drawFrame();
        SDL_GL_SwapBuffers();

        nFrames++;
        unsigned newTicks = SDL_GetTicks();
        if (newTicks - oldTicks > 2000) {
            LOG("%.1f fps (%d frames in %d ms)",
                nFrames * 1000. / (newTicks - oldTicks),
                nFrames, newTicks - oldTicks);
            oldTicks = newTicks;
            nFrames = 0;
        }
    }

    LOG("Exiting gracefully");
    return 0;
}
