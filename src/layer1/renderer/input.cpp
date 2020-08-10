#include "pch/pch.h"

#include "layer1/graph/game.h"

#include "input.h"

namespace Graph {

	extern bool _game_shouldClose;

}

namespace Renderer {

	bool Input::Setup()
	{
		int i;

		for (i = 0; i < SDL_NUM_SCANCODES; i++) {
			keys[i] = 0;
			keys_processed[i] = 0;
		}

		for (i = 0; i < 5; i++) {
			mouse_buttons[i] = 0;
			mouse_buttons_processed[i] = 0;
		}

		return true;
	}

	bool Input::Update()
	{
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch(event.type) {
			case SDL_KEYDOWN:
				keys[event.key.keysym.scancode] = 1;
				break;
			case SDL_KEYUP:
				keys[event.key.keysym.scancode] = 0;
				keys_processed[event.key.keysym.scancode] = 0;
				break;
			case SDL_MOUSEMOTION:
				mouse_pos[0] = event.motion.x;
				mouse_pos[1] = event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button-1] = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button-1] = 0;
				mouse_buttons_processed[event.button.button-1] = 0;
				break;
			case SDL_QUIT:
				Graph::_game_shouldClose = 1;
				break;
			default:
				break;
			}
		}

		return true;
	}

	bool Input::isKeyPressed(int keycode)
	{
		if (keycode > SDL_NUM_SCANCODES)
			return false;

		return keycode == keys[keycode];
	}

	bool Input::isKeyPressedOnce(int keycode)
	{
		if (isKeyPressed(keycode) && !keys_processed[keycode]) {
			keys_processed[keycode] = true;
			return true;
		}

		return false;
	}

	bool Input::isMouseButtonPressed(int button)
	{
		if (button - 1 > 4)
			return false;

		return mouse_buttons[button - 1];
	}

	bool Input::isMouseButtonPressedOnce(int button)
	{
		if (isMouseButtonPressed(button) & !mouse_buttons_processed[button]) {
			mouse_buttons_processed[button] = true;
			return true;
		}

		return false;
	}

	void Input::getMousePos(int *x, int *y)
	{
		if (!x || !y)
			return;

		*x = mouse_pos[0];
		*y = mouse_pos[1];
	}

}
