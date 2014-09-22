#include "Emulator.h"

#include "gb_core\gb.h"
#include "GBRenderer.h"

#include <Windows.h>
#include <cstdio>

using namespace tgb;

Emulator::Emulator(GBRenderer& renderer)
	: _gb(nullptr)
	, _exthookDef(nullptr)
	, _renderer(&renderer)
{
}


Emulator::~Emulator()
{
}

void	Emulator::Clear()
{
	if (_gb != nullptr)
	{
		delete _gb;
		_gb = nullptr;
	}
	if (_exthookDef != nullptr)
	{
		delete _exthookDef;
		_exthookDef = nullptr;
	}
}

bool	Emulator::LoadCartridge(const char* romName)
{
	FILE *file;
	int size;
	BYTE *dat;
	char *p;
	char *originalBuf;
	char *buf;

	if (_exthookDef == nullptr)
	{
		_exthookDef = new ext_hook();
		if (_exthookDef == nullptr)
			return false;
	}
	originalBuf = (char*)malloc(strlen(romName) +1);
	if (originalBuf == nullptr)
		return false;
	buf = originalBuf;
	strcpy(buf, romName);
	p = strrchr(buf, '.');
	if (p)
		while (*p != '\0') *(p++) = tolower(*p);

	if (strstr(buf, ".gb") || strstr(buf, ".gbc"))
	{
		file = fopen(buf, "rb");
		if (!file) return false;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
		dat = (BYTE*)malloc(size);
		fread(dat, 1, size, file);
		fclose(file);
	}
	else
		return false;

	//if ((num == 1) && (!render[1])){
	//	render[1] = new dx_renderer(hWnd_sub, hInstance);
	//	render[1]->set_render_pass(config->render_pass);
	//	load_key_config(1);
	//}
	if (_gb == nullptr)
	{
		_gb = new gb(_renderer, true, false); //renderer, unused bool lcd, enable sound
		if (_gb == nullptr)
			return false;

		_gb->set_target(NULL);

		//if (g_gb[num ? 0 : 1]
		//{
		//	g_gb[0]->set_target(g_gb[1]);
		//	g_gb[1]->set_target(g_gb[0]);
		//}

		//if (config->sound_enable[4])
		//{
		//	g_gb[num]->get_apu()->get_renderer()->set_enable(0, config->sound_enable[0] ? true : false);
		//	g_gb[num]->get_apu()->get_renderer()->set_enable(1, config->sound_enable[1] ? true : false);
		//	g_gb[num]->get_apu()->get_renderer()->set_enable(2, config->sound_enable[2] ? true : false);
		//	g_gb[num]->get_apu()->get_renderer()->set_enable(3, config->sound_enable[3] ? true : false);
		//}
		//else
		//{
		_gb->get_apu()->get_renderer()->set_enable(0, false);
		_gb->get_apu()->get_renderer()->set_enable(1, false);
		_gb->get_apu()->get_renderer()->set_enable(2, false);
		_gb->get_apu()->get_renderer()->set_enable(3, false);
		//}
	}
	//else
	//{
	//	if (g_gb[num]->get_rom()->has_battery())
	//		save_sram(g_gb[num]->get_rom()->get_sram(), g_gb[num]->get_rom()->get_info()->ram_size, num);
	//}

	int tbl_ram[] = { 1, 1, 1, 4, 16, 8 };//0と1は保険
	char sram_name[256], cur_di[256], sv_dir[256];
	BYTE *ram;
	int ram_size = 0x2000 * tbl_ram[dat[0x149]];
	char *suffix = ".sav";

	{
		char tmp_sram[256];
		strcpy(tmp_sram, sram_name);

		// そのまま、先頭のピリオドから拡張子に、さらにそれを拡張子RAM化、の3通りしらべないかん…
		int i;
		for (i = 0; i<3; i++){
			//if (i == 1) strcpy(strstr(tmp_sram, "."), ".sav");
			//else if (i == 2) strcpy(strstr(tmp_sram, "."), "ram");
			//
			//FILE *fs = fopen(tmp_sram, "rb");
			//if (fs){
			//	ram = (BYTE*)malloc(ram_size);
			//	fread(ram, 1, ram_size, fs);
			//	fseek(fs, 0, SEEK_END);
			//	//if (ftell(fs) & 0xff){
			//	//	int tmp;
			//	//	fseek(fs, -4, SEEK_END);
			//	//	fread(&tmp, 4, 1, fs);
			//	//	if (render[num])
			//	//		render[num]->set_timer_state(tmp);
			//	//}
			//	fclose(fs);
			//	break;
			//}
		}
		if (i == 3){
			ram = (BYTE*)malloc(ram_size);
			memset(ram, 0, ram_size);
		}
	}

	//if (config->gb_type == 1)
	//	dat[0x143] &= 0x7f;
	//else if (config->gb_type >= 3)
	//	dat[0x143] |= 0x80;

	_gb->set_use_gba(false);
	_gb->load_rom(dat, size, ram, ram_size);

	free(dat);
	free(ram);
	free(originalBuf);
	return true;
}

void	Emulator::Update()
{
	if (_gb == nullptr)
		return;
	for (int line = 0; line < 154; line++)
	{
		_gb->run();
	}
	_gb->set_skip(0); //to skip frame (0 = no skip)
}

void	Emulator::HookExport(unsigned char(*sendFct)(unsigned char), bool(*ledStatut)(void))
{
	_exthookDef->send = sendFct;
	_exthookDef->led = ledStatut;

	_gb->hook_extport(_exthookDef);
}

void	Emulator::UnhookExport()
{
	_gb->unhook_extport();
}