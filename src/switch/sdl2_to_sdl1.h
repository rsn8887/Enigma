// SDL 2 to SDL 1 interface by rsn8887
// to port uae4all2 to Switch
// created 10/29/2018
#ifdef USE_SDL2
#ifndef SDL2_TO_SDL1_H
#define SDL2_TO_SDL1_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Surface *switch_screen_surface;
extern SDL_Surface *alpha_format_source;

#define SDL_HWSURFACE 0
#define SDL_DOUBLEBUF 0
#define SDL_CreateThread(x,y) SDL_CreateThread(x,"uae_thread",y)
#define SDLKey SDL_Keycode
#define SDLMod Uint16
#define unicode sym
#define SDL_EnableUNICODE(x) 1
#define SDL_WM_SetCaption(x, y) 1
#define SDL_WM_SetIcon(x, y) 1
#define SDL_FULLSCREEN 0x01000000
#define SDL_DEFAULT_REPEAT_DELAY 1
#define SDL_EnableKeyRepeat(a, b) 1
#define SDL_WM_ToggleFullScreen(a) 1
#define SDL_SetGamma(r, g, b) 1
#define SDL_UpdateRect(a,b,c,d,e) 1
#define SDL_UpdateRects(a,b,c) 1
#define SDL_HWACCEL	0x00000100	/**< Blit uses hardware acceleration */
#define SDL_SRCCOLORKEY	0x00001000	/**< Blit uses a source color key */
#define SDL_RLEACCELOK	0x00002000	/**< Private flag */
#define SDL_SRCALPHA	0x00010000	/**< Blit uses source alpha blending */
#define KMOD_META KMOD_GUI
#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9
#define SDLK_LMETA SDLK_LGUI
#define SDLK_RMETA SDLK_RGUI
#define SDL_keysym SDL_Keysym
#define SDL_DisplayFormat(x) SDL_ConvertSurface(x,switch_screen_surface->format,0)
#define SDL_DisplayFormatAlpha(x) SDL_ConvertSurface(x,alpha_format_source->format,0)
#define SDL_SetAlpha(x,y,z) SDL_SetSurfaceAlphaMod(x,z); SDL_SetSurfaceBlendMode(x, SDL_BLENDMODE_BLEND);
typedef enum {
  SDL_GRAB_QUERY,
  SDL_GRAB_OFF,
  SDL_GRAB_ON
} SDL_GrabMode;

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, int flags);
void SDL_SetVideoModeScaling(int x, int y, float sw, float sh);
void SDL_SetVideoModeBilinear(int value);
void SDL_Flip(SDL_Surface *surface);
void SDL_SetVideoModeSync(int value);
void updateResolution();
int isDocked();
void SDL_WarpMouse(Uint16 x, Uint16 y);
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode grabmode);
int Surface_HasBlendMode(const SDL_Surface* surf);
SDL_BlendMode Surface_GetBlendMode(const SDL_Surface* surf);
int SDL_VideoModeOK(int w, int h, int bpp, Uint32 flags);
#ifdef __cplusplus
}
#endif
#endif
#endif