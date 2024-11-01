# SkyrimVore++
My attempt to make a new vore mod for Skyrim SE/AE, not affiliated with Devourment.
The goal is to make it have as little dependencies as possible, and to write most of the code in c++, because it's faster.

### Progress
Currently in pre-alpha state. Lacks some essential functionality and shouldn't be used for playthroughs.
No releases are currently provided, because the mod is not release-ready yet.
However, you can build it from source yourself.

## Building from source
### Requirements
* Visual Studio 2022
* vcpkg
* (for Skyrim) [SKSE](https://skse.silverlock.org/), [RaceMenu](https://www.nexusmods.com/skyrimspecialedition/mods/19080), [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

(optional) This mod uses sliders for belly/wg visuals, same as Devourment. So, you need to build a body with sliders in [BodySlide and Outfit Studio](https://www.nexusmods.com/skyrimspecialedition/mods/201). You can use 3BA + the body and outfits from Devourment.

### Building
* git clone --recursive this repo
* make a new empty mod in your mod manager
* in CMakePresets.json, set the SamplePluginOutputDir to your new mod
* switch build preset to ALL-WITH-AUTO-DEPLOYMENT
* build
