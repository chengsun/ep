#include "demolib.h"

#include <GL/glew.h>
#include <SDL.h>
#include <cstdio>
#include <ctime>
#include <getopt.h>


static void (*demo_init)(unsigned w, unsigned h) = 0;
static void (*demo_prepareFrame)() = 0;
static void (*demo_drawFrame)() = 0;


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

#define loadsym(symname, ...) \
    symname = reinterpret_cast<void (*)(__VA_ARGS__)>(GetProcAddress(demoLib, #symname)); \
    if (!symname) { \
        log("ERROR: Symbol \"%s\" undefined in demo library \"%s\"", #symname, libName); \
        missingSymbol = true; \
    }

static void pl_loadDemoLib(const char *libName)
{
    HMODULE demoLib = LoadLibrary(libName);
    if (!demoLib) {
        log("ERROR: Couldn't load demo library \"%s\": %s", libName, formattedError(GetLastError()).get());
        exit(1);
    }
    bool missingSymbol = false;

    loadsym(demo_init, unsigned, unsigned);
    loadsym(demo_prepareFrame);
    loadsym(demo_drawFrame);

    if (missingSymbol) exit(1);
}


#elif defined(LINUX)

#include <dlfcn.h>

#define loadsym(symname, ...) \
    symname = reinterpret_cast<void (*)(__VA_ARGS__)>(dlsym(demoLib, #symname)); \
    if (!symname) { \
        log("ERROR: Symbol \"%s\" undefined in demo library \"%s\"", #symname, libName); \
        missingSymbol = true; \
    }

static void pl_loadDemoLib(const char *libName)
{
    void *demoLib = dlopen(libName, RTLD_NOW);
    if (!demoLib) {
        log("ERROR: Couldn't load demo library \"%s\": %s", libName, dlerror());
        exit(1);
    }
    bool missingSymbol = false;

    loadsym(demo_init, unsigned, unsigned);
    loadsym(demo_prepareFrame);
    loadsym(demo_drawFrame);

    if (missingSymbol) exit(1);
}
#undef loadsym

#else
#   error "Compiling for unknown platform"
#endif


static SDL_Surface *screen;

static void pl_setVideoMode(int w, int h)
{
    log("Setting %dx%d GL video mode", w, h);
    screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL);
    if (screen) {
        log("Set %dx%d GL video mode", screen->w, screen->h);
    } else {
        log("ERROR: Couldn't set %dx%d GL video mode: %s", w, h, SDL_GetError());
        exit(1);
    }
}

static const char *demoLibrary = "demo.so";

int main(int argc, char *argv[])
{
    log("Player starting");

    log("Parsing switches");
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
            log("-d/--demo-library [lib.so]  Demo library location");
            log("-h/--help                   Help");
            exit(c == 'h' ? 0 : 1);
        }
    }

    log("Initialising SDL video");
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);
    SDL_WM_SetCaption("Demo player", "player");

    log("Initialising GL");
    pl_setVideoMode(600, 480);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        log("ERROR: GLEW initialisation failed: %s", glewGetErrorString(err));
        exit(1);
    }
    log("GLEW version: %s", glewGetString(GLEW_VERSION));

    log("OpenGL vendor: %s", glGetString(GL_VENDOR));
    log("OpenGL renderer: %s", glGetString(GL_RENDERER));
    log("OpenGL version: %s", glGetString(GL_VERSION));

#ifndef GLEW_NO_GLU
    log("GLU version: %s", gluGetString(GLU_VERSION));
#else
    log("GLU not present");
#endif

    log("Loading demo library \"%s\"", demoLibrary);
    pl_loadDemoLib(demoLibrary);

    log("Initialising demo");
    demo_init(screen->w, screen->h);

    log("Beginning render loop");
    bool done = false;
    unsigned nFrames = 0, oldTicks = 0;
    while (!done) {
        demo_prepareFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
            }
        }

        Uint8 *keys = SDL_GetKeyState(NULL);
        if (keys[SDLK_ESCAPE]) {
            done = true;
        }

        demo_drawFrame();
        SDL_GL_SwapBuffers();

        nFrames++;
        unsigned newTicks = SDL_GetTicks();
        if (newTicks - oldTicks > 2000) {
            log("%.1f fps (%d frames in %d ms)",
                nFrames * 1000.f / (newTicks - oldTicks),
                nFrames, newTicks - oldTicks);
            oldTicks = newTicks;
            nFrames = 0;
        }
    }

    return 0;
}