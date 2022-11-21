# AviUtl-RPC 
[![GitHub all releases](https://img.shields.io/github/downloads/mtripg6666tdr/AviUtl-RPC/total)](https://github.com/mtripg6666tdr/AviUtl-RPC/releases)  
[READMEは日本語版もご利用いただけます](../README.md)

![rich presence example](https://user-images.githubusercontent.com/56076195/151333122-b7f83afd-fb80-4191-a368-b43edea5a9a8.png)
  
![configuration window](https://user-images.githubusercontent.com/56076195/151332948-63068f62-5563-40f6-a889-b9d8091cd79b.png)
  
An AviUtl plugin, showing status (RPC) like 'Playing AviUtl' on Discord

## Feature
When idling, editing, and encoding, this will show the current status in your profile on Discord.  
In addition to that, this will also show the file name you are now editing. You can disable this in the configuration window.  

## Install
1. Go 'Releases' page on GitHub and find the newest one. Download aviutl_rpc_en_[version number].zip in 'Assets'    
2. Unzip the file  
3. Place `aviutl_rpc_en.auf` and `discord_game_sdk.dll` in the same folder as where `aviutl.exe` is placed. Then restart AviUtl if you have already opened it.  

## How to use
### Enable/disable the plugin
If it's been installed successfully, you can find `Show AviUtl Discord RPC` option in 'View' menu in AviUtl (may be different and this is depend on which language plugin you applied)  
You can enable/disable this plugin and showing filenames in the configuration window.  
Settings will be stored in profiles. The plugin will be disabled as default in the project files you edited before installing it.  
If you change these settings in idling state, this will use them in new projects.

## When you have troubled
When you have troubled or faced some errors that are caused by this, feel free to create an issue on GitHub.  

## Have feature requests?
You can post it in the same way when you have troubled.

## Self building
Open `AviUtl-RPC.vcxproj` in Visual Studio, and build.  
However you should follow these steps before building:
1. Create 'Binaries' directory in the root of this project.
2. Place `discord_game_sdk.dll` and `discord_game_sdk.dll.lib` in the directory. These files are contained in `Discord Game SDK`, which you can download in [Discord Developer Portal](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)

## License and disclaimer
See [LICENSE](../LICENSE).  
This file is also packed in release zip files.  
This README was translated by mtripg6666tdr. Sorry for my bad English.

## Copyright
(c) mtripg6666tdr
