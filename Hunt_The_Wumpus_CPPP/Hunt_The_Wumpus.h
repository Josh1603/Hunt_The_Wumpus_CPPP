#include "../../std_lib_facilities.h"
#include<random>

// Exercise 12 Hunt the Wumpus from Bjarne Stroustrup's C++ Programming Principles and Practice

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

 //TODO: Divide into Header and CPP files

//References for Cave.
struct Player;
struct Hazard;

struct Cave {
	Vector<Cave*> connecting_caves;
	Hazard* hazard = nullptr;
	Player* player = nullptr;
	void set_number(int number) { p_number = number; };
	int get_number() { return p_number; }
private:
	int p_number = 0;
};

struct Bow_and_arrow {
	void shoot() { --ammo; };
	int get_ammo() { return ammo; }
	int get_range() { return range; };
private:
	int ammo = 5;
	int range = 3;
};

struct Player {

	bool is_eaten() { return p_eaten; }
	bool is_falling() { return p_falling; }
	bool is_flying() { return p_flying; }
	bool is_moving() { return p_moving; }
	bool is_wumpus_killer() { return p_wumpus_killer; }

	void set_eaten(bool eaten) { p_eaten = eaten; }
	void set_falling(bool falling) { p_falling = falling; }
	void set_flying(bool flying) { p_flying = flying; }
	void set_moving(bool moving) { p_moving = moving; }
	void set_wumpus_killer(bool killer) { p_wumpus_killer = killer; }

	Bow_and_arrow get_bow_and_arrow() { return bow_and_arrow; }

	void move(Cave* i); // Move Player to an adjoining cave
	void shoot(Cave* i, int direction); // Shoot into a cave

private:
	Bow_and_arrow bow_and_arrow;
	bool p_eaten = false;
	bool p_falling = false;
	bool p_flying = false;
	bool p_moving = false;
	bool p_wumpus_killer = false;

};

enum class Hazard_Name {
	Empty, Wumpus, Bottomless_Pit, Giant_Bat
};

struct Hazard {
	Hazard_Name name = Hazard_Name::Empty;
	virtual string get_hint() = 0;
	virtual Hazard_Name get_name() = 0;
	virtual void action(Player* p) = 0;
};

struct Wumpus : Hazard {
	void action(Player* p); // Eat player
	string get_hint() { return hint; }
	Hazard_Name get_name() { return name; }
private:
	Hazard_Name name = Hazard_Name::Wumpus;
	string hint = "You can smell the Wumpus!\n";
};

struct Bottomless_Pit : Hazard {
	void action(Player* p); // Consume player
	string get_hint() { return hint; }
	Hazard_Name get_name() { return name; }
private:
	Hazard_Name name = Hazard_Name::Bottomless_Pit;
	string hint = "You can feel a breeze.\n";
};

struct Giant_Bat : Hazard {
	void action(Player* p); // Move player
	string get_hint() { return hint; }
	Hazard_Name get_name() { return name; }
private:
	Hazard_Name name = Hazard_Name::Giant_Bat;
	string hint = "You can hear a bat.\n";
};

struct Level {
	Level(int cave_count, int cave_connections, int wumpus_count, int bottomless_pit_count, int giant_bat_count);
	void generate_caves();
	void generate_hazards();
	void populate_caves();
	void move_the_wumpus();

	int get_cave_count() { return p_cave_count; }
	Cave* get_current_cave() { return p_current_cave; }
	void set_current_cave(Cave* current_cave) { p_current_cave = current_cave; }
	Vector<Cave*>& get_caves() { return caves; }
	Vector<Cave*>& get_wumpus_caves() { return wumpus_caves; }
	Player& get_player() { return player; } //Must pass by reference as state can be modified.
	Vector<Hazard*>& get_hazards() { return hazards; }

private:
	int p_cave_count;
	int p_cave_connections;
	int p_wumpus_count;
	int p_bottomless_pit_count;
	int p_giant_bat_count;
	Cave* p_current_cave;
	Vector<Cave*> caves;
	Vector<Cave*> wumpus_caves;
	Player player;
	Vector<Hazard*> hazards;
};

struct Game {
	Game();
	void run_game();
	void print_state();
	void print_wumpus_location();
	~Game() {
		for (Cave* cave : level.get_caves()) {
			delete cave;
		}

		for (Hazard* hazard : level.get_hazards()) {
			delete hazard;
		}
	};
	Level level;
	string start_message;
	string winner_message;
	string missed_message;
	string game_over_message;
	string wrong_cave;
	string eaten_message;
	string falling_message;
	string flying_message;
};
