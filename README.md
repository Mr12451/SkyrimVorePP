# SkyrimVore++
My attempt to make a new vore mod for Skyrim SE/AE.

This mod is not affiliated with Devourment and doesn't use it's code.

The goal is to make it have as little dependencies as possible, and to write most of the code in c++, because it's faster and has more functionality than Papyrus.

### Progress
Currently in pre-alpha state. Lacks some essential functionality and shouldn't be used for playthroughs.

No releases are currently provided, because the mod is not release-ready yet.

However, you can build it from source yourself.


## Building from source
### Requirements
* Visual Studio 2022
* [CMake](https://cmake.org/download/)
* vcpkg
* (for Skyrim) [SKSE](https://skse.silverlock.org/), [RaceMenu](https://www.nexusmods.com/skyrimspecialedition/mods/19080), [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [Pandora (recommened)](https://github.com/Monitor221hz/Pandora-Behaviour-Engine-Plus/releases/latest) or [Nemesis (untested)](https://www.nexusmods.com/skyrimspecialedition/mods/60033) for generating animations as you would for any other mod which requires them
* (Recommended) [SSE display tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/34705) and [SSE Engine Fixes](https://www.nexusmods.com/skyrimspecialedition/mods/17230), but read the Bugs section first

(optional) This mod uses sliders for belly/wg visuals, same as Devourment. So, you need to build a body with sliders in [BodySlide and Outfit Studio](https://www.nexusmods.com/skyrimspecialedition/mods/201). You can use 3BA + converted body and outfits from Devourment.

### Building
* `git clone https://github.com/Mr12451/SkyrimVorePP --recursive`
* make a new empty mod in your mod manager
* Set the SamplePluginOutputDir to your new mod in CMakePresets.json
* manually copy the contents of `Dist` folder to your new mod
* switch build preset to ALL-WITH-AUTO-DEPLOYMENT
* build
* run game

## Bugs
### Characters fade out when camera is too close
Use [SSE display tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/34705) and set `DisableActorFade` and `DisablePlayerFade` to `true` in `SSEDisplayTweaks.ini`

### Sliders freeze/stop changing
This happens with certain sliders, like SSBBW3 body/SSBBW2 body. Once the bug happens, all sliders, not just these, stop changing.

(not sure) but I think this happens because of EngineFixes. You can fix this in 2 ways:

1. Change the problematic sliders to different ones
2. Set `MemoryManager` and `ScaleformAllocator` to `false` in `EngineFixes.toml`

### Vore expressions not working
Download Expressive Facial Animation [Female Version](https://www.nexusmods.com/skyrimspecialedition/mods/19181) [Male Version](https://www.nexusmods.com/skyrimspecialedition/mods/19532)


## Plugins
### Devourment Dummy Plugin
A version of Devourment with all the features and scripts removed. Does nothing on it's own. Will be used as a dependency for some other plugins in the future.

Has sound files from Devourment in it. When loaded alongside SkyrimVore++, my mod will use them. The code that plays the sounds is my own, obviously, only the sound files are from Devourment. They're better then nothing, so they'll serve as a placeholder for the time being.

### DialogueReGherk
A version of DialogueReGherk (devourment dialogue plugin) that is compatible with SkyrimVorePP. Requires Devourment Dummy Plugin and SkyUI for MCM

.esp is untouched, scripts are rewritten to use my Papyrus API
Credits go to gherking on Discord for the original dialogue plugin

## Development roadmap
### Implemented
* Swallowing and regurgitation
* All vore types - Lethal, Safe, Healing; OV, AV, CV, BV, UB
* Full tour
* Vore data saving/loading
* Basic ui
* Highly customizable belly visuals
* Struggle visuals
* Highly customizable Weight gain
* Proper damage and death handling
* Assault (Combat) on vore
* Wait/Sleep/Travel/Timescale/Global time multiplier support
* A better way to hide actors during vore (needs testing)
* Disposal skeleton (for humanoids)
* Prey persistence (needs testing)
* Acid levels
* INI settings
* Prey size calculation using havok data (idk how to calculate kMOPP and kConvexVertices, so for some objects I use bounds instead)
* Item vore
* Add size gain
* Add dragon soul on digestion (look into dragon quest)
* Sounds (plugin)
* Fixed Talk to prey/pred
* Devourment Dialogue plugin support
* Papyrus API
* Player as Prey
* Basic reformation
* Inventory vore (horrible imlementation, but it works)
* Limit WG to certain NPC categories
* Pred/prey faction distribution (aka my own SPID)
* NPC auto-diposal
* Realistic digestion of consumed items
* Vore gives stats/skill xp
### Plans
#### Currently working on
* Animations
* Spells/Perks/Vore leveling
* Vore personalities and preferences for NPCs (need a whole new dialogue plugin)
* Capacity limits
* Struggle damage
#### For the future
* Find more people for new dialogue plugin
* Basic Animations
* Struggle minigame for prey and ?pred?
* Swallow minigame
* NPC Vore in combat
* Non-combat NPC vore (for the new dialogue plugin: random npc vore, approach)
* Non-combat NPC voire routine?
* Basic VFX
* inventory vore for player (hotkey or smth)
* Item digestion to other items, item disposal, fix items disappearing on release
* Compatibility with needs / survival mods
* vampire / dragon quest compatibility
* Compatibility with futa mods
* Player-as-prey internal view - attach camera to belly node? also disable actor fade out on closeup (hook cam update)
* Become your killer - look into [this mod](https://www.nexusmods.com/skyrimspecialedition/mods/62934)
* Compatibility with devourment world interactions
* Reformation of skulls / something similar
* Vampirism and becoming a werewolf should give pred abilities, devouring an npc will count as vampire feeding and werewolf soul absorbtion (config option)
* Compatibility with ostim?
* MCM
#### Other
* Fix menu disappearing on cell change
* Don't forget about updating commonlib
* maybe make switching digestion dependent on acid level
* More skeletons and skeleton scaling
* MB look at display tweeks for better menu even handling for inventory vore
* Rework digestion during time skip for better multiprey and voreception support