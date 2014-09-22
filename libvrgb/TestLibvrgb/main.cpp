#include <CPU.h>
#include <Video.h>
#include <Sound.h>
#include <Cartridge.h>

#include <Emulator.h>

#include <SFML/System.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Renderer.h"
#include "TGBRenderer.h"

using namespace vrgb;

#define VRGBEMULATOR 0

int main(int argc, char* argv[])
{
	sf::Clock	clock;

#if VRGBEMULATOR
	CPU			*cpu;
	Video		*video;
	Sound		*sound;
	Cartridge	*cartridge;

	Renderer	*renderer;

	renderer = new Renderer();
	if (renderer == nullptr)
		return -1;
	if (!renderer->Init())
		return -1;

	video = new Video(nullptr);
	sound = new Sound();
	cpu = new CPU(video, sound);
	cartridge = new Cartridge("agame.gb");

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
			sf::sleep(sf::milliseconds((desiredMs - elapsedTime) / 1000));
		lastElapsedTime = clock.getElapsedTime().asMilliseconds();
    }


	delete cartridge;
	delete cpu;
	delete sound;
	delete video;
	delete renderer;

#else
	TGBRenderer*	renderer;

	renderer = new TGBRenderer();
	if (renderer == nullptr)
		return -1;
	if (!renderer->Init())
		return -1;

	tgb::Emulator	emulator(*renderer);

	if (!emulator.LoadCartridge("agame.gb"))
		return -1;

	const float desiredFps = 60.0f;
	const float desiredMs = 1000.0f / desiredFps;

	float lastElapsedTime = 16.0f;
	while (renderer->IsOpen())
	{
		clock.restart();
		renderer->DrawFps(1000 / lastElapsedTime);
		emulator.Update();
		renderer->Update();
		const float elapsedTime = (float)clock.getElapsedTime().asMilliseconds();
		if (elapsedTime < desiredMs)
			sf::sleep(sf::milliseconds((desiredMs - elapsedTime) / 1000));
		lastElapsedTime = clock.getElapsedTime().asMilliseconds();
	}
	delete renderer;
#endif
	return 0;
}