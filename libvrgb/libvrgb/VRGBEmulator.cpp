#include "VRGBEmulator.h"

#include "CPU.h"
#include "Video.h"
#include "Sound.h"
#include "Cartridge.h"
#include "IGBScreenDrawable.h"

using namespace vrgb;

VRGBEmulator::VRGBEmulator()
: m_Video(nullptr)
, m_Sound(nullptr)
, m_Cpu(nullptr)
{
}


VRGBEmulator::~VRGBEmulator()
{
	Clear();
}

void	VRGBEmulator::Clear()
{
	SAFE_DELETE(m_Video);
	SAFE_DELETE(m_Sound);
	SAFE_DELETE(m_Cpu);
}

bool	VRGBEmulator::Init()
{
	Clear();

	m_Video = new Video(nullptr);
	CHECK_ALLOC(m_Video);
	m_Sound = new Sound();
	CHECK_ALLOC(m_Sound);
	m_Cpu = new CPU(m_Video, m_Sound);
	CHECK_ALLOC(m_Cpu);

	m_Sound->SetEnabled(false);
	m_Cpu->Reset();
	return true;
}

void	VRGBEmulator::Update()
{
	m_Cpu->UpdatePad();
	m_Cpu->ExecuteOneFrame();
}

void	VRGBEmulator::LoadCartridge(Cartridge* cartridge)
{
	m_Cpu->LoadCartridge(cartridge);
}

void	VRGBEmulator::SetScreen(IGBScreenDrawable* screen)
{
	m_Video->SetScreen(screen);
}