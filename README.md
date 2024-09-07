# H2M Plugin Loader

This is a plugin loader for H2M, designed for adding/testing new features to H2M without having to touch the original .exe

## How plugins that hook/detour game functions work on servers:
* **Both Client and Server Running the Plugin:** This is the preferred way, results in the most consitent behavior since both sides are synced.
* **Only Server Running the Plugin:** The client can connect but may experience glitches, for example when using a plugin that removes map boundaries, the client will expierence jittering when going out of bounds due to conflicts between the client and server states.
* **Only Client Running the Plugin:** The plugin will not work because the server doesn't recognize the modifications.

## How to setup
1. Go to the [Releases](https://github.com/REVLIIS/H2M-Plugin-Loader/releases) section in the sidebar and download the latest `.zip` file. Or click [here](https://github.com/REVLIIS/H2M-Plugin-Loader/releases/download/v1.0.0/H2MPluginLoader.zip) for a direct download.
2. Unzip the contents of the `.zip` into your H2M root folder (the same folder where your h2m-mod.exe is located).
3. Add your DLLs to the plugins folder e.g. `\Call of Duty Modern Warfare Remastered\plugins\Silverr\wrist twists.dll`
4. Run h2m-mod.exe (or your server's startup .bat file if you're a server hoster) and if you setup everything correctly, your console window should look something like this:
![image](https://github.com/user-attachments/assets/3b640755-9398-4d85-90a9-a59c4cff4d04)

## For mod developers:
You can enable a file watcher that monitors the plugin folder for any changes, allowing you easily test DLL changes in-game without having to restart your game, or having to manually detach and inject the DLL.

### Hotreload Feature
You can enable the HotReload feature in pluginloader.ini, when enabled, a message should appear in your H2M console saying so.

* On File Add: When a new plugin is added to the plugins folder, the loader will attempt to inject the DLL into the game.
* On File Delete: If a loaded plugin is deleted, the tool will attempt to detach the corresponding DLL from the game.
* On File Change: If an existing DLL is replaced or modified, the tool will attempt to hotreload the DLL.

## For the ultra nerds

### Build Requirements
Compile with [TDM-GCC](https://jmeubank.github.io/tdm-gcc/).

### Build Instructions
1. Clone repo
2. Make sure TDM-GCC is installed and added to your PATH variables.
3. Run cmds in project root:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
cmake --build .
```
