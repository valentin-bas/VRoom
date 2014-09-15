#include <CPU.h>
#include <Video.h>
#include <Sound.h>
#include <Cartridge.h>

#include <SFML/System.hpp>

#include "Renderer.h"

using namespace vrgb;

int main(int argc, char* argv[])
{
	CPU			*cpu;
	Video		*video;
	Sound		*sound;
	Cartridge	*cartridge;

	Renderer	*renderer;

	sf::Clock	clock;

	renderer = new Renderer();
	if (renderer == nullptr)
		return -1;
	if (!renderer->Init())
		return -1;

	video = new Video(nullptr);
	sound = new Sound();
	cpu = new CPU(video, sound);
	cartridge = new Cartridge("tetris.gb");

	sound->SetEnabled(false);
	cpu->Reset();
	cpu->LoadCartridge(cartridge);
	video->SetScreen(renderer);

	const float desiredFps = 60.0f;
	const float desiredMs = 1000.0f / desiredFps;

	float lastElapsedTime = 16.0f;
	while (renderer->IsOpen())
    {
		clock.restart();
		renderer->DrawFps(1000 / lastElapsedTime);
		cpu->UpdatePad();
		cpu->ExecuteOneFrame();
		const float elapsedTime = (float)clock.getElapsedTime().asMilliseconds();
		if (elapsedTime < desiredMs)
			sf::sleep(sf::microseconds((desiredMs - elapsedTime) / 1000));
		lastElapsedTime = clock.getElapsedTime().asMilliseconds();

    }


	delete cartridge;
	delete cpu;
	delete sound;
	delete video;
	delete renderer;
	return 0;
}