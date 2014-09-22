#pragma once

#ifdef LIBTGB_EXPORT
#define LIBTGB_API __declspec(dllexport)
#else
#define LIBTGB_API __declspec(dllimport)
#endif

class gb;

namespace tgb
{
	class GBRenderer;

	class LIBTGB_API Emulator
	{
	private:
		gb*			_gb;
		GBRenderer*	_renderer;
	public:
		Emulator(GBRenderer& renderer);
		virtual ~Emulator();

		void	Clear();
		bool	LoadCartridge(const char* name);

		void	Update();
		//void	LoadCartridge(Cartridge* cartridge);
		//void	SetScreen(IGBScreenDrawable* screen);
	};
}