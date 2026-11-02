# Build and run on Windows

Use the installed MSYS2 UCRT64 compiler. The older C:\MinGW compiler on your
normal PATH should not be mixed with the existing build-ucrt directory.

From the Graphics-Game folder in PowerShell:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
cmake -S . -B build-ucrt -G "MinGW Makefiles" -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe
cmake --build build-ucrt -j 4
.\build-ucrt\TowerDefense.exe
```

For later builds, just repeat the PATH line and build command.
CMake includes Game.cpp and the other game sources in TowerDefenseCore, then links
main.cpp into TowerDefense. GLAD 2 is compiled once; the older GLAD 1 source is not
included. STB_IMAGE_IMPLEMENTATION remains in one source file.

The build copies assets and the MinGW runtime DLLs beside TowerDefense.exe. You
can launch that executable from Explorer or another working directory. To move
it elsewhere, keep the assets folder and copied DLLs beside it.

## Controls

- Start: play the introductory cutscene, then enter gameplay.
- Space or Enter: skip the intro.
- Click lanes: toggle selection. Multiple lanes can be selected.
- Select a lane, then click a character card to purchase/place it in that lane.
- M: purchase a melee player in the current placement lane.
- W / Up: jump with mobile players in all selected lanes.
- S / Down: duck with mobile players in all selected lanes.
- Escape: pause/resume. The pause menu also offers restart and main menu.
- Win/loss menus: restart or return to the main menu.

Only melee starts unlocked. Ranged unlocks after earning 20 collectible coins;
defense unlocks after earning 30. Costs are 10/20/30. Locked cards appear dimmed.
Win/loss screens now use the real game conditions, not W/L/B/H testing shortcuts.

## Verification

```powershell
ctest --test-dir build-ucrt --output-on-failure
.\build-ucrt\RenderSmokeTest.exe
.\build-ucrt\TowerDefense.exe --smoke-test
```

The four regression suites contain 339 passing checks. The render test checks
OpenGL rendering and writes review frames into build-ucrt. The application smoke
test opens a hidden window, renders the menu/intro/gameplay and exits automatically.
The actual executable has also been launched from an empty directory with the
compiler removed from PATH, verifying bundled asset/DLL discovery.
