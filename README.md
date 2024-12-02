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
#### Sliders freeze/stop changing
This happens with certain sliders, like SSBBW3 body/SSBBW2 body
Once the bug happens, all sliders, not just these, stop changing
(not sure) but I think this happens because of EngineFixes
You can fix this in 2 ways
1. Change the problematic sliders to different ones
2. Set `MemoryManager` and `ScaleformAllocator` to false in `EngineFixes.toml`

#### Vore expressions not working
Download Expressive Facial Animation [Female Version](https://www.nexusmods.com/skyrimspecialedition/mods/19181) [Male Version](https://www.nexusmods.com/skyrimspecialedition/mods/19532)


## Plugins
### Devourment Dummy Plugin
A version of Devourment with all the features and scripts removed. Does nothing on it's own. Will be used as a dependency for some other plugins in the future.

Has sound files from Devourment in it. When loaded alongside SkyrimVore++, my mod will use them. The code that plays the sounds is my own, obviously, only the sound files are from Devourment. They're better then nothing, so they'll serve as a placeholder for the time being.

## Development roadmap
### Implemented
* Swallowing and regurgitation
* All vore types - Lethal, Safe, Healing; OV, AV, CV, BV, UB
* Full tour
* Vore data saving/loading
* Basic ui
* Belly visuals (a little wonky)
* Struggle visuals
* Basic WG
* Proper damage and death handling
* Assault (Combat) on vore
* Wait/Sleep/Travel/Timescale/Global time multiplier support
* A better way to hide actors during vore (needs testing)
* Disposal skeleton (for humanoids)
* Prey persistence (needs testing)
* Acid levels (maybe make switching digestion dependent on acid level)
* INI settings
* Prey size calculation using havok data (idk how to calculate kMOPP and kConvexVertices, so I use bounds instead)
* Item vore (very unoptimized and untested)
* Add size gain
* Add dragon soul on digestion (look into dragon quest)
* Sounds (plugin)
* Fixed Talk to prey/pred
### Plans
#### Currently working on
* Dialogue plugin
* Player as Prey
* Inventory (items from container) vore
* Basic reformation
#### For the future
* Basic Animations
* Basic VFX
* Limit WG to certain NPC categories
* Player-as-prey internal view
* Ui - hide debug info
* Struggle minigame for prey and pred
* Struggle damage
* Swallow minigame
* Find more people
* Compatibility with futa mods
* Compatibility with needs mods
* Compatibility with survival mods
* Compatibility with ostim?
* Compatibility with devourment sound mod
* Better WG (food, sprint, etc. should affect WG/Loss)
* NPC Vore in combat
* Non-combat NPC vore
* Vore personalities and preferences for NPCs
* Capacity limits
* Spells/Perks/Vore leveling
* Vore gives stats/skill xp
* Become your killer
* MCM
* Fix menu disappearing on cell change