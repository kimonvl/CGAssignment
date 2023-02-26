#include "Input.h"
#include <SDL2/SDL.h>

static SDL_Window* s_window = nullptr;

void Input::Init(void* window)
{
	s_window = (SDL_Window*)window;
}

bool Input::IsKeyPressed(int key)
{
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	return keyboardState[key] == 1;
}

bool Input::IsMouseButtonPressed(int button)
{
	return false;
}
