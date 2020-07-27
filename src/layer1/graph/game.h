#ifndef GRAPH_SCENE_H
#define GRAPH_SCENE_H

#include "node.h"
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

	protected:
		std::vector<Service *> Services;
		bool shouldClose;
	};

}

#endif