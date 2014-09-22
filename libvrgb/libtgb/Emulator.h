#pragma once

#ifdef LIBTGB_EXPORT
#define LIBTGB_API __declspec(dllexport)
#else
#define LIBTGB_API __declspec(dllimport)
#endif

class gb;
struct ext_hook;

namespace tgb
{
	class GBRenderer;

	class LIBTGB_API Emulator
	{
	private:
		gb*			_gb;
		GBRenderer*	_renderer;
		ext_hook*	_exthookDef;


	public:
		Emulator(GBRenderer& renderer);
		virtual ~Emulator();

		void	Clear();
		bool	LoadCartridge(const char* name);

		void	Update();

		unsigned char	SeriSend(unsigned char data);

		void	HookExport(unsigned char(*send)(unsigned char), bool(*led)(void));
		void	UnhookExport();
	};
}