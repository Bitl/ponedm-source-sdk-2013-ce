# PoneDM
Based off of [Source SDK 2013 CE](https://github.com/Nbc66/source-sdk-2013-ce).
Inspired by [Bear Party Adventure](https://store.steampowered.com/app/1274450/Bear_Party_Adventure/), PoneDM is a shitpost mod taken too seriously. 
Created by the same person who made Intellectual Techno Hell and FIREFIGHT RELOADED, PoneDM is an experimental multiplayer arena shooter that pits you against other colorful horses in various arenas. 
You can fight against bots, or you can fight with other players around the world. 
You will be able to traverse many areas by bunny-hopping and rocket jumping through them as you try to completely destroy the enemy team, or against other players in free-for-all romps. 
You can also customize your horse with various colors and mane/tail styles. 

This mod was explicitly built so I can refresh my knowledge of the Source Engine and experiment with various aspects I haven't yet experimented with in other mods I worked on in the past. 
I also wanted to create an engaging experience like Intellectual Techno Hell: a shitposty feeling mod that is fun to play while also balanced enough to be taken seriously.

## Compiling

### WINDOWS:

To be able to build PoneDM you will need to download:
* Visual Studio 2013 or later
* C++ Build Tools for Visual Studio 2013 (Not required if you already have Visual Studio 2013)
* Multibyte MFC Library for Visual Studio 2013

Read this for more details: https://developer.valvesoftware.com/wiki/Source_SDK_2013#Source_SDK_2013_on_Windows

You must also run this command in your CMD with administrator privileges in order to get VPC to build a project:
* REG ADD "HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\10.0\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}" /v DefaultProjectExtension /t REG_SZ /d vcproj /f

Note that the Microsoft Speech API is only required for certain SDK tools.

Creating a solution is as simple as running mp/src/creategameprojects.bat, then opening up the solution in Visual Studio.

### LINUX:
Read this first to install the steam runtime and other necessary components.\
https://developer.valvesoftware.com/wiki/Source_SDK_2013#Source_SDK_2013_on_Linux \
After installing it and other dependencies, do the 3 steps below.

1. CD to your PoneDM git directory, I.E.\
```cd ~/projects/ponedm-source-sdk-2013-ce```

1. Run the steam-runtime. NOTE: Make sure you have the steam runtime installed in /valve.\
```bash ./sandbox_setup.sh```

3. Go back to this directory and run:\
```bash ./build_ponedm_linux.sh```\
This will update the repo automatically. If you would like to update the repo without building it, run:\
```bash ./update_ponedm_linux.sh```

## Credits
* Nbc66 & GamerDude27 for the Source SDK 2013 CE base used. (https://github.com/Nbc66/source-sdk-2013-ce)
* Open Fortress team and Rykah for the basis of the player coloring code and the gore system. (https://openfortress.fun, used source code branch before GitHub delisting)
* Momentum Mod team for the engine patch and sv_skyname reloading. (https://github.com/momentum-mod/game/blob/develop/mp/src/game/shared/momentum/util/engine_patch.cpp)
* VectoredThrust for the name of the pony model (Pantone Shift).
* Mattyhex for the Celestia Medium Redux font. (http://www.mattyhex.net/CMR/)
* Facepunch Studios for the c_models used for the weapons in Garry's Mod.
* Ozxybox and GamerDude27 for the c_models tutorial on VDC. (https://developer.valvesoftware.com/wiki/Hand_Viewmodels)
* Kolessios for the SourceBots source code used as the basis for the bots. (https://github.com/kolessios/sourcebots)
* Wave Software for the list of names used in their MLP Name Generator. (https://github.com/wavesoftware/mlp-name-generator/blob/develop/lib/data.js)
* Aionaever for the Toggling RPG Laser tutorial on VDC. (https://developer.valvesoftware.com/wiki/Toggling_RPG_Guidance)
* Drift. for the dm_aerowalk map. (https://gamebanana.com/maps/194479) 
This map was licenced under the Attribution-NonCommercial-NoDerivatives 4.0 International license. (https://creativecommons.org/licenses/by-nc-nd/4.0/) 
dm_aerowalk has not been modified. A weapon was added to the map, but the weapon was added through the mod's Mapadd system which allows items to be added to maps without modifying them. (listed below)
* Team Fortress 2 Vintage team for the reverse-engineered bot name script code used for bot names and the weapon randomizer. (https://github.com/TF2V/TF2Vintage/blob/1.0/src/game/server/tf/bot/tf_bot_manager.cpp#L514)
* GamerDude27 for the Dircord RPC Tutorial. (https://developer.valvesoftware.com/wiki/Implementing_Discord_RPC)
* Taito for the Gatling Gun models from Half-Life 2 Survivor Ver 2.0.
* Railgun model comes from Alien Swarm.
* Railgun firing sound comes from Team Fortress 2.
* SirMasters for the Mapadd implementation. (https://github.com/sirmastercombat/SirMasters_Mod) This is used to add items to maps (without editing the maps themselves) by using script files.
* Additional fixes for Linux done by [Cyanide/BlendedAppleSeeds](https://github.com/Bitl/ponedm-source-sdk-2013-ce/pull/1)
* Soundscape fix for Linux by [TotallyMehis](https://github.com/ValveSoftware/source-sdk-2013/pull/448/files)
* Model panel based off old [TF2 Classic code](https://github.com/danielmm8888/TF2Classic/blob/d070129a436a8a070659f0267f6e63564a519a47/src/game/client/tf/vgui/controls/tf_advmodelpanel.cpp)

PoneDM includes models from PPM/2 (https://gitlab.com/DBotThePony/PPM). These models were created and modified by the following people
* [Durpy](https://steamcommunity.com/id/xDeRpYx/)
* [DBotThePony](https://steamcommunity.com/id/DBotThePony)
* [Unkn](https://steamcommunity.com/id/12263)
* [Digivee](https://www.deviantart.com/digivee)
* [DarkSunriseHD](https://steamcommunity.com/id/InfinityNetworksDarkSunrise/)
* [KP-ShadowSquirrel](https://www.deviantart.com/kp-shadowsquirrel)

This project also uses sounds from [Dynamic Surroundings](https://github.com/OreCruncher/DynamicSurroundings/blob/master/CREDITS.md#sounds).

Sources for sounds:
https://freesound.org/people/craigsmith/sounds/479674/ R13-02-Scared Neighs.wav by craigsmith

All rights go to their respective holders.

### Codename: Project Ahuizotl

[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com)
