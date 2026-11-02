# Combat and game integration

main.cpp is now connected to the repaired Game class and the full TowerDefense
executable builds. See BUILDING.md for exact build/run commands and controls.

## Implemented behavior

- One Game owns the grid, players, waves, collision, combat and economy.
- Automatic melee/ranged attacks, projectile collisions, lane separation, defense
  protection and correct high/low evasion are implemented.
- Movement, attack animation, damage feedback and evasion operate independently.
  Attack cooldowns measure time between attack starts.
- Game uses fixed 1/120-second simulation steps and resolves projectile collisions
  before cleanup. A real enemy reaching the base ends the simulation immediately.
- Collectible collisions award each reward once. Purchases charge only after
  successful placement. Card unlocking tracks earned collectible coins.
- Game::reset clears players, both projectile types, waves, drops, timers, economy,
  selections, pause and terminal states.
- Rendering uses the existing sprite frames and artwork, with character health
  bars, red damage tint, animated jump/duck, projectiles and collectibles.
- The 12-second animated intro transitions to gameplay and can be skipped.
- Existing menu artwork, button positions, lane selection, hover highlights,
  pause menu and win/loss menus are retained.

## main.cpp connection

main.cpp now calls Game::init once, then Game::update and Game::render. It does not
create or advance a second WaveManager or Economy. Purchases use purchasePlayer;
W/Up and S/Down call the selected-lane evasion methods. Restart uses Game::reset,
and results use Game::hasWon/hasLost. The old testing shortcuts and placement-array
rendering have been removed. Graphics resources are released before GLFW teardown.

Game::render accepts optional hovered-lane and hovered-card indices. Existing
callers that use render() still work. Game::placePlayer remains available for
scripted/direct placement without charging coins; normal purchases should use
Game::purchasePlayer.

Game::init starts the intro. Game::reset starts a fresh gameplay session without
replaying it; call startCutscene separately when starting from the main menu.

## Verification

- CombatSystemTests: 51 passed.
- GameLogicTests: 46 passed.
- EnemyTests: 115 passed.
- WaveManagerTests: 127 passed.
- Full TowerDefense executable: builds successfully.
- OpenGL render verification: passed.
- Application startup smoke test: passed from an unrelated working directory with
  compiler directories removed from PATH, using the bundled assets and runtime DLLs.

The checks cover builds, simulation behavior and rendering/startup. They do not
replace a full manual playthrough for balance and overall game experience.
