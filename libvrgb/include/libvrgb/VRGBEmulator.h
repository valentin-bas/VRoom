#pragma once

#ifndef __VRGB_EMULATOR_H__
#define __VRGB_EMULATOR_H__

#include "Def.h"

namespace vrgb
{
	class CPU;
	class Video;
	class Sound;
	class Cartridge;
	class IGBScreenDrawable;

	class LIBVRGB_API VRGBEmulator
	{
	private:
		CPU			*m_Cpu;
		Video		*m_Video;
		Sound		*m_Sound;

	public:
		VRGBEmulator();
		virtual ~VRGBEmulator();

		void	Clear();
		bool	Init();

		void	Update();
		void	LoadCartridge(Cartridge* cartridge);
		void	SetScreen(IGBScreenDrawable* screen);
	};
}
#endif
