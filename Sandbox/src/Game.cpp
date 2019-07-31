#include <Seidon.h>

using namespace Seidon;

class Game : public Application {
public:
	Game() {

	}

	~Game() {

	}
};

Application* Seidon::CreateApplication() {
	return new Game();
}