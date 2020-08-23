# Hunt_The_Wumpus_CPPP
An exercise from Bjarne Stroustrups Programming Principles and Practice

/* Brief:
 Implement a version of the game "Hunt the Wumpus" (Invented by Gregory Yob)
 Aim of the game: Slay the Wumpus using a bow and arrow
 Environment: A series of numbered interconnected underground caves. Each cave is connected to 3 others.
 Ammo: 5 bows and an arrow (Arrows have a range of 3 rooms)
 Hazards: Wumpus / Bottomless Pits / Giant Bats
 Wumpus -> Eats you -> Hint: "I smell the Wumpus!"
 Bottomless Pits -> Fall forever -> Hint: "I feel a breeze"
 Giant Bats -> Pick you up and take you to another room -> Hint: "I hear a bat"
 Hints are given when one of the hazards are in an adjoining room.
 When you shoot, the Wumpus wakes up and moves to a room adjoining the one he was in.
 */

 // Room message example "You are in room 12. There are tunnels to rooms 1, 13, amd 4; move or shoot?"
 // Answer example "m13" -> Move to room 13. "s13-4" -> Shoot into rooms 13 and 4.

 // Issues: Due to way caves are generated in this version, it is possible that some caves become inaccessible for the player. 
 // This is because the connected caves for a given cave are assigned randomly, thus if any one cave is never randomly selected as a connected cave it won't actually be accessible!
 // Also with this game design the player cannot move backwards. Not necessarily an issue as it is as if they are dropped into each cave from a tunnel above it which they are then unable to re-enter.
 // I haven't set limits on things like room amount, hazard amount etc. or defined any sort of acceptable ratio.
 // I only allowed the player to shoot into one cave at a time. I prefer it that way, otherwise it's too easy to kill the Wumpus!
