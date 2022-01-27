# AviUtl-RPC 
![rich presence example](https://user-images.githubusercontent.com/56076195/151333122-b7f83afd-fb80-4191-a368-b43edea5a9a8.png)
  
![configuration window](https://user-images.githubusercontent.com/56076195/151332948-63068f62-5563-40f6-a889-b9d8091cd79b.png)
  
An AviUtl plugin, that shows status (RPC) like 'Playing AviUtl' in Discord

## Features
When idling, editing, and encoding, this will show current status in user profiles in Discord.  
In addition to that, this will show also the file name you are now editing. You can disable this feature in configuration window.  

## Installing
1. Go 'Releases' page in GitHub and find newest releases. Download aviutl_rpc_[version number].zip in 'Assets'    
2. Unzip the file you downloaded in Step.1    
3. Place `aviutl_rpc.auf` and `discord_game_sdk.dll` in the same directory where `aviutl.exe` is placed. Restart AviUtl app if you have already opened it.  

## How to use
### Enable/Disable the plugin
If it's been installed successfully, you can find `Show AviUtl Discord RPC` option in 'View' menu in AviUtl (may be different and this is depend on which language plugin you applyed)  
You can enable/disable this plugin and showing filenames in the configuration window.  
Settings will be stored in profiles. The plugin will be disabled as default in the project files you edited before installing it.
If you change these settings in idling state, the plugin will use them in the projects you edit after this

## When you have troubled
When you have troubled or faced some errors which is caused by the plugin, feel free to post an issue in GitHub.  
If you can't do it, you can contact with me through Discord. Discord user name is `うさみょん(myon2019)/mtripg6666tdr#4470`.

## Have feature requests?
You can post it in the same way mentioned above.

## Self building
Open `AviUtl-RPC.vcxproj` in Visual Studio, and build.  
However you should follow these steps before building:
1. Make 'Binaries' directory in the root of this project.
2. Place `discord_game_sdk.dll`と`discord_game_sdk.dll.lib` in the directory. These files are contained in `Discord Game SDK`, which you can download in [Discord Developer Portal](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)

## License and disclaimer
See [LICENSE](../LICENSE).  
This file is also packed in release zip files.  
This README was translated by mtripg6666tdr. Sorry for my bad English.

## Copyright
(c) mtripg6666tdr
