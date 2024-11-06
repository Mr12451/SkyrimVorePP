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
* manually copy `Interface` folder to your new mod
* switch build preset to ALL-WITH-AUTO-DEPLOYMENT
* build
* run game

## Development roadmap
### Implemented
* Swallowing and regurgitation
* All vore types - Lethal, Safe, Healing; OV, AV, CV, BV, UB
* Full tour
* Vore data saving/loading
* Basic ui
* Belly visuals (a little wonky)
* Basic WG
### Plans
* Proper damage and death handling
* A better way to hide actors during vore
* INI settings
* Improve code for struggle sliders
* Basic reformation
* Fix Talk to prey/pred
* Basic dialogue
* Disposal
* Basic Animations
* Basic Sounds
* Basic VFX
* Find more people
* Compatibility with futa mods
* Compatibility with needs mods
* Compatibility with survival mods
* Add size gain
* Optimize WG
* NPC Vore in combat
* Non-combat NPC vore
* Vore personalities and preferences for NPCs
* Item/Object vore
* Capacity limits
* Acid levels
* Spells/Perks/Vore leveling
* Struggle minigame
* Become your killer
* MCM