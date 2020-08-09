#ifndef RENDERER_INPUT_H
#define RENDERER_INPUT_H

#include "runtime.h"

namespace Renderer {

	class Input : public Graph::Service {
	public:
		inline Input() : Service("InputService") {  }

		bool Setup();
		bool Update();

		bool isKeyPressed(int keycode);
		bool isKeyPressedOnce(int keycode); // useful for gui
		bool isMouseButtonPressed(int button);
		bool isMouseButtonPressedOnce(int button);
		void getMousePos(int *x, int *y);

	private:
		bool keys[SDL_NUM_SCANCODES];
		bool keys_processed[SDL_NUM_SCANCODES];
		bool mouse_buttons[5];
		bool mouse_buttons_processed[5];
		int mouse_pos[2];
	};

}

#endif
