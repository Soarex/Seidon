#pragma once

extern Seidon::Application* Seidon::CreateApplication();

int main(int argc, char** argv) {
	Seidon::Application* app = Seidon::CreateApplication();
	app->Run();
	delete app;
	return 0;
}