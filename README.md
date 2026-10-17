# Graphics-Game
This project is a game done using openGL library, C++. It starts by displaying a cut scene then the game starts featuring players who try to defeat enemies.


Project 1 - 2D Game 
Deadline: Monday, November 2nd, 2026 @ 11:59 PM 
1. Overview 
The project is a 2D side-view tower defense game featuring grid-based player placement, multiple player and enemy types, combat, collectibles, and an animated introductory cutscene. The best game you can use as a reference to visualize the logic and requirements of this project is Plants vs Zombies. 
 
This project description outlines the core mechanics and requirements that must be implemented in your game. Your team is expected to develop its own creative theme and setting that fits these requirements. You must use modern OpenGL (versions 3.3+) to implement this project. Feel free to use any other additional libraries. 
 
You will be graded not only on the implementation of the required mechanics, but also on the creativity of your chosen theme, as well as how logically and creatively your introductory cutscene connects to and establishes the story and setting of your game. You are encouraged to take inspiration from existing 2D games and may reference other games with similar mechanics or gameplay elements. 
 
This is a group project to be completed in teams of strictly 3-4 students; given the scope of the project and the other deadlines you will have during the semester, it is not feasible for a single team member to complete the entire project alone. Team members may be from different tutorials and/or majors. You are strongly encouraged to begin working on the project as early as possible, as it includes a significant number of requirements and is not realistically achievable within only a few days before the deadline. 
 

2. Cutscene 
The game begins with a fully animated, 10-20 second cutscene that establishes the story, theme, and setting before transitioning into the main gameplay.  
 
The introductory cutscene must include at least one player and one enemy, and should briefly establish what the players are defending and why it is important to the story. 
 
The cutscene must also include a short dialogue or interaction between the players and enemies that helps introduce the conflict and setting. Text may be used as subtitles to support the dialogue and storytelling, but the cutscene must not rely solely on text to convey the story (meaning that you must include some animation and not just display text on the screen) 
 
The players and enemies must be visibly animated and interacting on screen. Be creative with how you introduce your characters, setting, and conflict, and make sure the cutscene feels like a natural introduction to your chosen game theme. 
 
3. Game Logic 
 
3.1 Playfield 
The playfield is a 2D grid viewed from the side. Rows are horizontal lanes, and each lane is an independent battle line in which players and enemies meet. 
•	Home base (safe zone): Located at the left end of the playfield; this is the player's starting position. It must look distinct and remain clearly visible against the environment. 
•	Enemy entry: Enemies emerge on the grid at the right most edge, in a randomly chosen lane, and advance horizontally toward the home base. 
 
 
 
•	Players advance: Once placed, any player (except the defense players) will automatically begin moving from the left most end of the chosen horizontal lane until they reach the right, advancing across the screen while encountering and fighting enemies. 
•	Fight strategy: Both players and enemies will automatically begin their respective attacking, shooting, or defensive actions as soon as they are placed on the grid (player) or appear on screen (enemy). 
•	Player-Enemy encounter: When a player and an enemy meet and collide, both should stop moving and continue attacking each other until one of them is defeated. Gameplay should continue as the player advances through successive waves of enemies toward the end of the level. 
•	The game must contain a minimum of three distinct lanes. 
•	Once a player is placed on a lane, they cannot switch to another lane. 
•	Once an enemy spawns on a lane, it must remain on that same lane and cannot switch to another lane. 
•	All player and enemy movement must therefore remain within the lane on which the character was placed or spawned. 
•	Find below an extremely simplified snippet that illustrates the grid layout, as well as enemy/player movement and basic attacks. 
•	Disclaimer: The game scene components—including players, enemies, environmental elements, the Home Base, and other objects—must not consist of a single primitive shape alone. Any snippets included in this description are provided only to help you understand the specified required game mechanics and logic. They are not intended to represent the final level of modeling or detail expected in your scene. Every component of the scene must include additional modeling details appropriate to its design and theme (realistic shapes are required). Failure to add sufficient detail to the required components will result in significant grade deductions. 
 
 
 
 
What is deduced from this snippet: 
•	There are 4 lanes (you don’t have to number them): players/enemies belong to a lane and can only move horizontally within a lane. 
•	There are 3 players (represented as circles): players are placed on the grid, and immediately start attacking (attacks are represented as dots) and move towards the right side (as indicated by the arrow at the top left of the scene) as soon as they are placed, even if there are no enemies (as in lane number 4). 
•	There are 3 enemies (represented as triangles): enemies appear at random positions from the right most side of the screen and immediately start attacking and move towards the left side (as indicated by the arrow at the top right of the scene) as soon as they appear, even if there are no players (as in lane number 3). So, a wise move would be placing a player in lane number 3 to defeat the enemy that has spawned there before the enemy reaches the home base. 
•	Enemy and player attacks overlap: as in lane number 2, attacks may overlap in which case, they will both keep attacking until one of them defeats the other. 
 
3.2 Player Logic 
There are three types of players: 
 
●	Melee Player: Attacks enemies using close-range weapons. 
●	Ranged Player: Attacks enemies using long-range weapons. 
●	Defense Player: The only player type that does not move after being placed on the grid. It remains in position and focuses on defending until it is defeated. 
 
Each player type must have a distinct visual design that logically represents its role. Players must not be differentiated solely by changing their colors. The modeling, shape, equipment, and overall appearance of each player should be clearly different and appropriate for its respective role. 
 
In order to place a player onto the scene, the player must first be purchased using coins/collectibles. Initially, all player types are locked except for the Melee Player. The game starts with enough coins/collectibles to purchase and place at least three Melee Players onto the scene. 
 
Find in the next page an extremely simplified snippet that illustrates locked and unlocked players. P.S. The number of coins here is not accurate; refer to the notes column of the players’ table in page number 9 to understand the distribution of coins. 
 
 
 
 
 
 
 
 
 
 
•	What is deduced from this snippet: 
•	There are 3 player cards: one of them is clearly unlocked and so it can be selected and placed in the scene, while the other two are clearly locked (greyed out) so we can’t select or interact with them. 
•	The game starts with 30 coins: which is enough to purchase 3 instances of the unlocked player card. We clearly can’t purchase any of the other two players since we don’t have enough coins. 
•	There is an enemy already spawned at the bottom lane: as the enemies don’t wait for the players to be placed on the grid. So, a wise move would be placing a player in the bottom lane to defeat the enemy that has spawned there before the enemy reaches the home base. 
Additional coins/collectibles are obtained by defeating enemies. As the player collects enough coins/collectibles, the other player types are progressively unlocked and become available for purchase and placement on the grid.  
To place a player on the grid, the player must first select a lane. The entire horizontal lane is visibly highlighted when the mouse hovers over it, and a left click selects the lane, locking the highlight on the selected lane, and another left click deselects it, unlocking the highlight on a selected lane. 
 
The player then selects a character from the active player card UI and chooses which lane to place that player on. Once the lane is selected, the player automatically appears at the leftmost position of the selected lane and begins advancing toward the right, attacking enemies encountered along the way. The Defense Player is the only exception: once placed, it remains stationary on the selected lane and focuses on defending until it is defeated. 
 
Find below an extremely simplified snippet that illustrates a highlighted lane in the grid due to a mouse hover/click event. 
 
 
 
Defending enemy attacks is also done per lane, where: 
●	Hovering the mouse over the grid highlights the entire horizontal lane under the cursor. 
●	Clicking toggles that lane between selected (highlighted) and deselected (not highlighted). Multiple lanes can be selected at once. 
●	Pressing W / Up Arrow makes every player in all the selected lanes jump; while pressing S / Down Arrow makes them duck. 
●	Jumping avoids low attacks and ducking avoids high attacks. 
●	Defense players are stationary and are not affected by jump or duck. 
●	When a player is attacked, varying shades of red should be applied to the player model to visually indicate the damage being received. The intensity of the red should increase as the player's health decreases, continuing until the player is defeated. You may choose to scale the player model instead, where the player is scaled down with every hit until it disappears after being defeated. 
●	When a player is defeated, it must be removed from the scene. 
●	Players may temporarily overlap or pass through one another while moving along the same lane. For example, a Melee Player may pass by a stationary Defense Player without affecting either player. 
●	If a player is placed on the same lane as a Defense Player, the Defense Player provides protection from enemy attacks as long as the player remains behind the Defense Player. Once the player moves past the Defense Player, or if the Defense Player is defeated, the player will become vulnerable to enemy attacks again. The player must then avoid incoming attacks using the available jumping or ducking mechanics. 
●	All player types (except the defense player) deal 1 point of damage per attack to enemies.  
 
 
 
 
 
 
 
The table below also shows some more additional information for the players, including animations and health. 
 
Type 	Movement 	Attack 	Durability 	Notes 
Melee 	Advances towards the right automatically; jumps and ducks on command 	Close-range weapon that animates in the character's hand 	5 lives 	The basic starter player, affordable with the starting coins 
Defense 	Stationary; the only player type 
that does not move 	None; acts as a blocker that absorbs enemy 
attacks 	7 hits 	Defends the home base as well as other players behind them; unlocked through 3x the collectibles required to unlock the Melee Player 
Long Range 	Advances towards the right automatically; jumps and ducks on command 	Fires a projectile; the weapon animates as it hits the enemy and disappears on collision 	5 lives 	Never harms other players; unlocked through 2x the collectibles required to unlock the Melee 
Player 
 
Note: A player's remaining health must always be visible, below or above each player. 
 
 
 
 
 
 
 
 
 
3.3 Enemy Logic 
There are three enemy types, each with a different difficulty level and attack behavior: 
 
●	Easy - Upper Ranged Enemy: Moves at a normal speed and attacks the player from a distance using attacks that come from above (the player must duck to avoid these). Starts with 5 lives. 
●	Medium - Lower Ranged Enemy: Moves at a normal speed and attacks the player from a distance using attacks that come from below (the player must jump to avoid these). Starts with 5 lives. 
●	Hard - Melee Enemy: Runs toward the player and attacks at close range. Starts with 7 lives. 
●	All enemies deal 1 point of damage per attack, except for the Hard - Melee Enemy, which deals 2 points of damage per attack. 
●	Enemies will appear in random lanes throughout the game and will be introduced across three waves with increasing difficulty: 
 
Wave 	Minimum Number of Enemies 	Enemy types 
Wave 1 	3 	Easy only 
Wave 2 	5 	A mix of easy and medium 
Wave 3 	10 	A mix of easy, medium, and hard 
 
●	A wave begins as soon as the first enemy of that wave appears and ends as soon as the final enemy of that wave appears. Once the wave has ended, no additional enemies from that wave will spawn, and the player must defeat the remaining enemies on screen to proceed to the next wave until all waves are covered. 
●	Defeated enemies drop collectibles. 
●	The collectibles are collected when the player collides with them. 
 
 
 
●	As with the players, all three enemy types must have clearly distinct visual designs. Their differences must be represented through their models, shapes, equipment, and overall appearance, rather than simply changing their colors. Their designs should also logically communicate their respective attack types and difficulty levels. 
●	When an enemy is attacked, varying shades of red should be applied to the enemy model to visually indicate the damage being received. The intensity of the red should increase as the enemy’s health decreases, continuing until the enemy is defeated. You may choose to scale the enemy model instead, where the enemy is scaled down with every hit until it disappears after being defeated. 
●	An enemy’s remaining health must always be visible, below or above each enemy. 
 
3.4 Winning & Losing Conditions 
●	The player wins the game by defeating all enemies across all three waves. 
●	The player loses the game if at least one enemy reaches the Home Base from any lane. In this case, everything in the scene stops (the rest of the enemies/players should not continue moving or attacking). 
 
 
4. Visuals 
The chosen game theme must be logically and realistically consistent across all aspects of the game. The cutscene, story, environment, players, enemies, weapons, and overall visual design should all belong to the same setting and make sense within the context of the chosen theme. 
For example, the characters' appearances and weapons must be appropriate for the world they inhabit, and the conflict introduced in the cutscene must logically explain the gameplay that follows. The environment must also reinforce the same setting rather than feeling disconnected from the characters or story. 
 
4.1 Enemies & Players 
•	Everything related to the player must look completely different from everything related to the enemy. Recoloring alone does not meet this rule. 
•	The player home base must be distinct from, and clearly visible against, the game environment. Recoloring alone does not meet this rule. 
•	The three player types must be visually distinct from each other, and so must the three enemy types. Recoloring alone does not meet this rule. 
•	The current health of every player and enemy must be visible at all times during gameplay. Health may be displayed using one of these options: 
a.	A visible health bar 
b.	Numerical health values 
c.	Visual primitives 
The chosen method must make it immediately clear how much health each player and enemy has remaining. You may use this image as reference to help you understand what is meant by having multiple health bars for every player appear on screen: https://www.theappguruz.com/app/uploads/2017/03/health-bars-age-of-empires.png  
 
4.2 Game Scene 
The game scene must include at least five different environmental elements that accurately represent the chosen setting. These elements should be integrated naturally into the environment rather than added as unrelated decorations. 
 
For example, depending on the chosen theme, these could include: 
●	Garden: trees, fences, flowers, bushes, benches, lamps, garden tools, or fountains. 
●	City: buildings, streetlights, traffic signs, parked cars, trash bins, or benches. 
●	Forest: trees, rocks, mushrooms, bushes, logs, or plants. 
●	Castle: towers, walls, banners, torches, gates, or stone structures. 
●	Laboratory: computers, machinery, pipes, containers, warning signs, or laboratory equipment. 
●	Beach: palm trees, umbrellas, beach chairs, rocks, shells, or boats. 
 
The specific elements must be chosen according to the team's own game theme and should help make the setting visually recognizable. The theme must be realistically consistent. 
The player's current state must always be clearly visible through the game's UI. 
●	The player's total number of collected points/collectibles must be displayed and correctly updated at the top of the screen. 
●	The UI must include player cards, where each card represents a player type and indicates whether that player is currently unlocked and available for placement. 
●	Locked players must be displayed using grayscale values and must not respond to mouse hover events or other interactive selection behavior. 
●	Unlocked players must be displayed in their normal colors and must respond to mouse hover events, allowing them to be selected for placement on the grid. 
●	Once the player has collected enough points to meet the required unlock cost, the corresponding locked player card must automatically become unlocked. 
●	If the player's available points/collectibles later fall below the required unlock cost, the corresponding player card must automatically return to its locked state. 
 
4.3 Game Menus 
The game must contain three menus: 
1.	Main Menu: Appears when the game is launched, before the introductory cutscene. It must provide options to: 
○	Start the game (triggers the cut scene, then the game starts) 
○ Exit the game (closes the application) 
○ Open the sound settings (bonus) 
2.	Pause Menu: Triggered during gameplay using the escape button, and must provide the options to: 
○	Continue the game 
○ Restart the game 
○ Return to the Main Menu 
3.	Win/Lose Menu: Appears when the player wins or loses and must provide the options to: 
○	Restart the game  
○ Return to the Main Menu 
 
5. Bonus: Sound & Audio 
Sound and audio features are optional bonus elements. The Main Menu should include sound settings that allow the player to increase the volume, decrease the volume, or mute all sound. 
In order to receive this bonus, you must include all of the following points: 
●	Background music 
●	Sound effects for at least 4 user interactions, such as: 
○ Enemy/player attacks and hits 
○ Character deaths 
○ Lane selection 
○ Player placement 
○ Collectible collection 
○ Winning and losing 
●	A dedicated Sound Settings menu accessible from the Main Menu that can correctly modify existing sounds. 
 
 
6. Video Recording: 
You are asked to submit a video recording for your screen while you are playing the game. The video should follow the following regulations: 
 
●	Your video should be from 2 – 5 minutes 
●	Your video should have voiceover explaining: 
○	Description of the game 
○ The aim of the game 
○ How to play it 
●	The video must be arranged in the following sequence: 
○	University name & logo 
○ Course name and semester 
○ Project name 
○ Students’ names 
○ Rest of the video content 
○ Thank you 
●	Finally, note that your video can be published online, so do a good job. 
7. Submission Guidelines 
●	Files with compilation errors will not be graded. It is zero. 
●	Submit your project files through this form. 
●	Only .h, .cpp, … files should be submitted in a .zip archive and uploaded to a drive link. 
●	Please keep the naming convention for all of your submitted files to easily reach them [Team name]-[Name of one team member].zip 
●	It is preferable to submit the video as a separate link using the same form mentioned above. 
●	Grant us access to the drive link. 
●	The project submission deadline is on Monday, November 2nd, 2026 @ 11:59 PM 
 
 
