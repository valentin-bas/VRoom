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

#include <iostream>
#include "GBException.h"
#include "Pad.h"
using namespace std;
using namespace vrgb;

bool Pad::externalState[8];
BYTE Pad::gbPadState[8];

void Pad::SetKeys(int * keys)
{
//	for (int i=0; i<8; i++)
//		keysUsed[i] = (wxKeyCode)keys[i];
}

void Pad::ResetStates()
{
	for (int i = 0; i < 8; ++i)
		externalState[i] = false;
}

void Pad::SetState(enum e_gbpad key, bool state)
{
	externalState[key] = state;
}

BYTE Pad::UpdateInput(BYTE valueP1)
{
	if(!BIT5(valueP1))
		return ((valueP1 & 0xF0) |
			(!gbPadState[gbSTART] << 3) | (!gbPadState[gbSELECT] << 2) | (!gbPadState[gbB] << 1) | (!gbPadState[gbA]));

	if(!BIT4(valueP1))
		return ((valueP1 & 0xF0) |
			(!gbPadState[gbDOWN] << 3) | (!gbPadState[gbUP] << 2) | (!gbPadState[gbLEFT] << 1) | (!gbPadState[gbRIGHT]));

	//Desactivar los botones
	return 0xFF;
}

// Devuelve 1 cuando se ha pulsado una tecla
// 0 en caso contrario
int Pad::CheckKeyboard(BYTE * valueP1)
{
	
	int interrupt = 0;
	
	for (int i=0; i<8; i++)
	{
		if ((gbPadState[i] == 0) && (externalState[i] == true))
		{
			interrupt = 1;
		}
		
		gbPadState[i] = externalState[i];
	}
	
	*valueP1 = UpdateInput(*valueP1);
	
	return interrupt;
}
