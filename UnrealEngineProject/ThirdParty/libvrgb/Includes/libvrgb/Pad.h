/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PAD_H__
#define __PAD_H__

#include "Def.h"

namespace vrgb
{
	class LIBVRGB_API Pad
	{
	private:
		static bool externalState[8];
		static BYTE gbPadState[8];
	public:
		enum e_gbpad { gbUP, gbDOWN, gbLEFT, gbRIGHT, gbA, gbB, gbSELECT, gbSTART };

		static void	SetKeys(int * keys);
		static int	CheckKeyboard(BYTE * valueP1);
		static BYTE	UpdateInput(BYTE valueP1);
		static void	ResetStates();
		static void	SetState(enum e_gbpad key, bool state);
	};
}

#endif
