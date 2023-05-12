#include "gameshared.h"

MainMemory mainMemory;

#define ALL_KEYS (KEY_MENU | KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)

static volatile KeyPressedEnum _newKeyDown = KEY_NONE;
static volatile KeyPressedEnum _heldKeys = KEY_NONE;
static volatile KeyPressedEnum _lockKeys = KEY_NONE;
static volatile uint8_t _isChrg = 0 ;

// checking key is pressed
void Key_Down(KeyPressedEnum key)
{
	_heldKeys |= key;
	if ( !(_lockKeys & key))
		_newKeyDown |= key;
	_lockKeys |= key;
}


void IsChrg(uint8_t yes)
{
 _isChrg = yes;
}

uint8_t SoC()
{
  return _isChrg;
}

// which key is up
void Key_Up(KeyPressedEnum key)
{
	_heldKeys &= (ALL_KEYS^key);
	_lockKeys &= (ALL_KEYS^key);
}

KeyPressedEnum Keyboard_GetPressedKeys()
{
	KeyPressedEnum result = _newKeyDown;
	_newKeyDown = KEY_NONE;
	return result;
}

KeyPressedEnum Keyboard_GetHeldKeys()
{
	return _heldKeys;
}
