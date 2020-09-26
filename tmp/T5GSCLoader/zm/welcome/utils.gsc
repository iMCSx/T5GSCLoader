#include maps\_utility;
#include common_scripts\utility;
#include maps\_hud_util;

applyPatches()
{
    setMemory("0x10042000", "12345678");
    setMemory("0x10042010", "4578656D706C65206F6620737472696E67");
}