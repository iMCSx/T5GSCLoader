#include maps\_utility;
#include common_scripts\utility;
#include maps\_hud_util;

#include maps\zm\mod\utils;

main()
{
    applyPatches();
    level thread onPlayerConnect();
}

onPlayerConnect()
{
    for(;;)
    {
        level waittill("connected", player);
        player thread onPlayerSpawned();
    }
}

onPlayerSpawned()
{
    self endon("disconnect");
    for(;;)
    {
        self waittill("spawned_player");
        self iprintln("Hello from ^1injected ^7GSC");
    }
}