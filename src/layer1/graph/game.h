#ifndef GRAPH_SCENE_H
#define GRAPH_SCENE_H

#include "service.h"

namespace Graph {

	// Essentially the root of everything
	// Services are supposed to be updated and killed on demand.
	// They are not stored as children for safety reasons.
	class Game : public Node {
	public:
		Game() : Node("Game") { shouldClose = false; }

		void registerService(Service *service);
		void setupServices(); // Inits all services.
		void runServices(); // Updates all services.
		void killService(Service *service); // Removes from registered services and releases the service.
		Service *getService(std::string name);

		void Run(); // Game loop
		void Destroy(); // sends destroy signal to game loop
		void Abort(); // Emergency abort

	protected:
		std::vector<Service *> Services;
		bool shouldClose;
	};

	extern bool _game_shouldClose; // Global var used externally by other services to close the game

}

#endif
