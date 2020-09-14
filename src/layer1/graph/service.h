#ifndef GRAPH_SERVICE_H
#define GRAPH_SERVICE_H

#include "node.h"

namespace Graph {

	class Service : public Node {
	public:
		Service(std::string name) : Node(name) { Initialized = false; }
		virtual ~Service() {  }

		virtual bool Setup() { Initialized = true; return true; }   // True = success, False = failed
		virtual bool Update() { return true; }  // True = success, False = failed
		virtual void Release() {  } 		// Destroys the service.

		inline bool isInitialized() { return Initialized; }
		inline void setInitialized() { Initialized = true; }

	protected:
		bool Initialized;
	};

}

#endif
