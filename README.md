# T5GSCLoader

T5GSCLoader is a PS3 module for Black Ops 1 that allows you to load custom game scripts, call C functions from them and make possible connect to others players by patching client/server checksum.

## Build
* Clone the project and open a terminal/cmd on it.
* Type `make release`.
* The module will be created in `bin/release/`.

## Usage
Create a folder in `/dev_hdd0/tmp/T5GSCLoader/mp` or `/dev_hdd0/tmp/T5GSCLoader/zm` and put your script in it. Beside your created folder create a .mod file that has the same name than your mod to enable it. You need to have a file in your mod folder named `main.gsc` with a `main()` function inside to get it load and started (see tmp directory for structure/examples).

## Development
VS Code is used for this project, you can also use Visual Studio if you prefer but you'll need to created a new solution using the VSIX from the SDK and make sure to don't forget to include the required libraries `libc.a libfs_stub.a`.

For a better experience with VS Code and use the tasks:

* Install C/C++ Extension.
* MSYS2 (unix tools).
* Powershell 7.0 (support `&&` operator for tasks) or integrate msys2 shell in vscode.
* Change the console IP in `.vscode/settings` by yours, and be sure you have a ftp server running on console (e.g webMAN).

You can now open the folder in VS Code and press `Ctrl+Shift+B` to build or deploy the module to console, and deploy game scripts.

Deployments tasks will just create or replace any existing file by FTP commands.

The outputs made with a custom `printf` can be seen in Target Manager on "Console Output" menu.

# Reversing
Everything is identical on `XBOX` and `PC`, you can easily adapt for these platforms.

## Opcodes Checksum

The game load and compile scripts each time that a game is started or joined (host or client). Each game scripts will be compiled and will become opcodes, Each opcodes will pass in order through few instructions that will compute a checksum:

```
00596FC4    lwz   r10, 8(r4)    # Load the checksum from struct
00596FCC    slwi  r12, r10, 5   # Shift left by 5 bits (r10 << 5)   
00596FD0    subf  r6, r10, r12  # Substract checksum from the shifted checksum 
00596FD4    add   r5, r6, r21   # Add opcode value to checksum
00596FD8    stw   r5, 8(r4)     # Write checksum to struct
```

The lastest game script loaded is always the map one, right away this one loaded the checksum get copied from the function `Scr_GetChecksum`. On host side the checksum buffer is writed in the GameState that will be sent to clients. On client side, the checksum is read from GameState and compared with the client local value. This is the reason that if you change only a simple character in a GSC you'll not able to join/host with people.

The function signature of `Scr_GetChecksum` on PC is `8B C1 69 C9 38 10 02 00`.

**Note:** The checksum on client scripts (CSC) is computed but never checked on clients.

## Patching Checksum

There is 3 values required for the client check: `checksum`, `programLen`, `substract`.

A struct pointer is passed in `Scr_GetChecksum` and the three values will be copied into it.
I thought about two ways to patch the checksum:

* The first one is to dump the right checksum and just hook `Scr_GetCheckum` to pass the right one after you edited a script, simple and works well. But you'll need to dump each checksum for each map and gamemodes, since the checksum is based on opcodes, a gamemode or a map will also load his own script, so **you'll have to dump ~900 different value** to be able to play on each map/modes in multiplayer.

* The second way is to load custom game scripts, **after the load of the main map script** which is the latest before the copy of the checksum, by this way we can **save the valid checksum** just-in time and replace it in `Scr_GetChecksum_Hook` when our custom game scripts have been loaded.

I used the second one to don't need to worry about dumping checksums per map/gamemodes, **we can auto patch it** like this. Of course this way require additionnal hooks and code.

## Calling C functions

I made a quick exemple to show how to call C functions from game script.
You just have to make your function, and return his opd pointer from `Scr_GetFunction`, like i did for `setMemory` with hex strings. If you want to make your own function working with an entity like `self` ahead the function you need to hook `Scr_GetMethod` and use the second argument of this function (type) to return the according opd.

## Why I made this now?

I made it for some people few months ago, to help and having some memories times by doing C and PPC. Then someone release his own with closed source when he heard that i made it.

Recently, by cleaning my desktop i came across that folder, I said to myself it would be a cool to share this, maybe some people are curious about how game scripts check is made.

Maybe this code will be useful to someone, who knows?


