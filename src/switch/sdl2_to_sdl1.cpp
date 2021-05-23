#ifdef USE_SDL2
#include <SDL.h>
#include "sdl2_to_sdl1.h"
#include <switch.h>

static SDL_Window* window = NULL;
static SDL_Texture* texture = NULL;
static SDL_Texture* prescaled = NULL;
static SDL_Renderer* renderer = NULL;
static int prescaled_width = 320;
static int prescaled_height = 240;
static int surface_width = 320;
static int surface_height = 240;

static int currently_docked = -1;

static int x_offset;
static int y_offset;
static int scaled_height;
static int scaled_width;
static int display_width;
static int display_height;
SDL_Surface *switch_screen_surface;
SDL_Surface *alpha_format_source;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int isDocked() {
	switch (appletGetOperationMode()) {
		case AppletOperationMode_Handheld:
			return 0;
			break;
		case AppletOperationMode_Console:
			return 1;
			break;
		default:
			return 0;
	}
}

void updateResolution() {
	int docked = isDocked();
	if ((docked && !currently_docked) || (!docked && currently_docked)) {
		// docked mode has changed, update window size etc.
		if (docked) {
			display_width = 1920;
			display_height = 1080;
			currently_docked = 1;
		} else {
			display_width = 1280;
			display_height = 720;
			currently_docked = 0;
		}
		// remove leftover-garbage on screen
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		SDL_SetWindowSize(window, display_width, display_height);
		SDL_SetVideoModeScaling(0, 0, 0, 0);
	}
}

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, int flags) {
	if (currently_docked == -1) {
		if (isDocked()) {
			display_width = 1920;
			display_height = 1080;
			currently_docked = 1;
		} else {
			display_width = 1280;
			display_height = 720;
			currently_docked = 0;
		}
	}

	if (!renderer) {
		window = SDL_CreateWindow("uae4all2", 0, 0, display_width, display_height, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
		SDL_RenderClear(renderer);
		// only used to get a valid alpha format
		alpha_format_source = SDL_CreateRGBSurfaceWithFormat(0, 64, 64, 32, SDL_PIXELFORMAT_RGBA32);
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
	surface_width = w;
	surface_height = h;
	switch_screen_surface = surface;
	return surface;
}

void SDL_SetVideoModeScaling(int x, int y, float sw, float sh) {
	// scale to fit, preserve aspect ratio
	scaled_height = display_height;
	scaled_width = ((surface_width * display_height) / (float) (surface_height));

	// centering
	x_offset = (display_width - scaled_width) / 2;
	y_offset = (display_height - scaled_height) / 2;
	// find integer prescaled sizes to use later
	int prescale_factor_x = scaled_width / surface_width;
	int prescale_factor_y = scaled_height / surface_height;
	prescaled_width = prescale_factor_x * surface_width;
	prescaled_height = prescale_factor_y * surface_height;
	if (prescaled) {
		SDL_DestroyTexture(prescaled);
		prescaled = NULL;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	prescaled = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, prescaled_width, prescaled_height);
}

void SDL_SetVideoModeBilinear(int value) {
	if (value) {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	} else {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	}
}

void SDL_SetVideoModeSync(int value) {
	if (value) {
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		SDL_GL_SetSwapInterval(1);
	} else {
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
		SDL_GL_SetSwapInterval(0);
	}
}

void SDL_Flip(SDL_Surface *surface) {
	if (surface && renderer && window) {
		updateResolution();
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		
		SDL_SetRenderTarget(renderer, prescaled);
		SDL_Rect dst_rect_prescale = { 0, 0, prescaled_width, prescaled_height };
		SDL_RenderCopy(renderer, texture, NULL, &dst_rect_prescale);

		SDL_SetRenderTarget(renderer, NULL);
		SDL_Rect dst_rect = { x_offset, y_offset, scaled_width , scaled_height };
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderCopy(renderer, prescaled, NULL, &dst_rect);
		SDL_RenderPresent(renderer);
	}
}

void SDL_WarpMouse(Uint16 x, Uint16 y) {
	SDL_WarpMouseInWindow(window, x, y);
}

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode grabmode) {
	if (window) {
		switch (grabmode) {
			case SDL_GRAB_OFF:
				SDL_SetWindowGrab(window, SDL_FALSE);
			 	break;
			case SDL_GRAB_ON:
				SDL_SetWindowGrab(window, SDL_TRUE);
				break;
		}
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_GRABBED)
			return SDL_GRAB_ON;
		else
			return SDL_GRAB_OFF;
	} else {
		return SDL_GRAB_OFF;
	}
}

int Surface_HasBlendMode(const SDL_Surface* surf) {
	return Surface_GetBlendMode(surf) != SDL_BLENDMODE_NONE;
}

inline SDL_BlendMode Surface_GetBlendMode(const SDL_Surface* surf) {
	SDL_BlendMode mode = SDL_BLENDMODE_NONE;
	(void)SDL_GetSurfaceBlendMode((SDL_Surface*)surf, &mode); // ignore return
	return mode;
}

int SDL_VideoModeOK(int w, int h, int bpp, Uint32 flags) {
	if (w==640 && h==480) 
		return 16;
	else 
		return 0;
}
#endif