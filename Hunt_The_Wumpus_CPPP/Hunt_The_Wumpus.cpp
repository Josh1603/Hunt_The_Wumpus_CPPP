#include "Hunt_The_Wumpus.h"

bool is_used_number(vector<int>& used_numbers, int number) {
	for (int used_number : used_numbers) {
		if (used_number == number) {
			return true;
		}
	}
	return false;
}

bool has_hazard_free_connection(Cave* cave) {
	for (Cave* connecting_cave : cave->connecting_caves) {
		if (!connecting_cave->hazard) return true;
	}
	return false;
}

bool connects(Cave* current_cave, Vector<Cave*> caves) {
	for (Cave* cave : caves) {
		if (cave->get_number() == current_cave->get_number()) return true;
	}
	return false;
}

void print_connecting_caves(Vector<Cave*> connecting_caves) {
	int limit = connecting_caves.size() - 1;
	for (int i = 0; i < limit - 1; ++i) {
		cout << connecting_caves[i]->get_number() << ", ";
	}
	cout << connecting_caves[limit - 1]->get_number() << " and " << connecting_caves[limit]->get_number() << ".\n";
}

void Player::move(Cave* i) {
	if (i->hazard) {
		i->hazard->action(this);
	}
	else {
		set_moving(true);
	}
}

void Player::shoot(Cave* i, int direction) {

	Cave* line_of_fire = i;
	int range = get_bow_and_arrow().get_range();

	while (range) {
		if (line_of_fire->hazard && line_of_fire->hazard->get_name() == Hazard_Name::Wumpus) {
			set_wumpus_killer(true);
		}
		line_of_fire = line_of_fire->connecting_caves[direction];
		--range;
	}

	get_bow_and_arrow().shoot();
}

void Wumpus::action(Player* player) {
	player->set_eaten(true);
}

void Bottomless_Pit::action(Player* player) {
	player->set_falling(true);
}

void Giant_Bat::action(Player* player) {
	player->set_flying(true);
}

Level::Level(int cave_count = 20, int cave_connections = 3, int wumpus_count = 1, int bottomless_pit_count = 5, int giant_bat_count = 5) :
	p_cave_count{ cave_count },
	p_cave_connections{ cave_connections },
	p_wumpus_count{ wumpus_count },
	p_bottomless_pit_count{ bottomless_pit_count },
	p_giant_bat_count{ giant_bat_count }
{
	player = new Player();

	generate_caves();
	generate_hazards();
	populate_caves();


	p_current_cave = caves[0];
}

void Level::generate_caves() {

	for (int i = 0; i < p_cave_count; ++i) {
		Cave* cave = new Cave();
		cave->set_number(i);
		caves.push_back(cave);
	}

	for (int i = 0; i < p_cave_count; ++i) {
		vector<int> used_numbers;
		used_numbers.push_back(i);
		for (int j = 0; j < p_cave_connections; ++j) { //Randomly assign connecting caves. Don't allow a cave to connect to itself or the same cave more than once.
			int connecting_cave_number = randint(p_cave_count - 1);
			while (is_used_number(used_numbers, connecting_cave_number)) {
				connecting_cave_number = randint(p_cave_count - 1);
			}
			caves[i]->connecting_caves.push_back(caves[connecting_cave_number]);
			used_numbers.push_back(connecting_cave_number);
		}
	}
}

void Level::generate_hazards() {
	for (int i = 0; i < p_wumpus_count; i++) { hazards.push_back(new Wumpus()); }
	for (int i = 0; i < p_bottomless_pit_count; i++) { hazards.push_back(new Bottomless_Pit()); }
	for (int i = 0; i < p_giant_bat_count; i++) { hazards.push_back(new Giant_Bat()); }

}

void Level::populate_caves() {
	caves[0]->player = player;

	vector<int> used_numbers;
	used_numbers.push_back(0);

	for (Hazard* hazard : hazards) {
		int i = randint(p_cave_count - 1);
		while (is_used_number(used_numbers, i)) {
			i = randint(p_cave_count - 1);
		}
		caves[i]->hazard = hazard;
		if (hazard->get_name() == Hazard_Name::Wumpus) { wumpus_caves.push_back(caves[i]); }
		used_numbers.push_back(i);
	}
}

void Level::move_the_wumpus() {

	Vector<Cave*> new_wumpus_caves;

	for (Cave* cave : wumpus_caves) {

		if (has_hazard_free_connection(cave)) {
			int new_cave = randint(p_cave_connections - 1);
			while (cave->connecting_caves[new_cave]->hazard) { //Randomly selects a connecting cave without a hazard.
				new_cave = randint(p_cave_connections - 1);
			}
			cave->connecting_caves[new_cave]->hazard = cave->hazard;
			cave->hazard = nullptr;
			cout << "\nYou hear the thud of the Wumpus feet, it's on the move!\n\n";
			new_wumpus_caves.push_back(cave->connecting_caves[new_cave]);

			if (cave->connecting_caves[new_cave]->get_number() == p_current_cave->get_number()) {
				player->set_eaten(true);
			}
		}
		else {
			new_wumpus_caves.push_back(cave);
		}

	}
	wumpus_caves.clear();

	for (Cave* cave : new_wumpus_caves) { wumpus_caves.push_back(cave); }

}

Game::Game() {
	level = new Level();

	start_message = "Hunt the Wumpus!\n\nBrave adventurer, alas the time has come for you to hunt the Wumpus! Enter the labyrinth. Navigate the tunnels. Defeat him at your peril! \n\nMove: mX\nShoot: sX\n\n";
	winner_message = "\nA wail and a thud reverberate through the knotted caves.\n\nCongratualations!!! You hath slain the Wumpus!\n\n";
	missed_message = "\nYour arrow whistles through the air and then silence. The hunt continues.\n\n";
	game_over_message = "GAME OVER\n";
	wrong_cave = "\nThat isn't a connecting cave. Try again!\n\n";
	eaten_message = "\nA huge belch echoes through the labyrinth. The Wumpus has eaten you alive!\n\n";
	falling_message = "\nYou plummet eternally into darkness never to be seen again. You have fallen into a bottomless pit!\n\n";
	flying_message = "\nSomething grabs you by the shoulders. Your feet lift off the ground. A giant bat has you now!\nYou struggle as the bat flys manically through the labyrinth. Finally, it releases you from its vice-like grip.\n\n";
}

void Game::run_game() {
	cout << start_message;

	while (true) {

		//print_wumpus_location(); 

		Vector<Cave*> connecting_caves = level->get_current_cave()->connecting_caves;

		cout << "Current cave is " << level->get_current_cave()->get_number() << ". Connecting caves are ";
		print_connecting_caves(connecting_caves);
		cout << "You have " << level->get_player().get_bow_and_arrow().get_ammo() << " arrows.\n";

		for (Cave* cave : connecting_caves) {
			if (cave->hazard) {
				cout << cave->hazard->get_hint();
			}
		}

		cout << "What will you do?\n";

		char command;
		int cave;
		cin >> command >> cave;

		if (command == 'm' || command == 'M') {
			if (connects(level->get_caves()[cave], connecting_caves)) {
				level->get_player().move(level->get_caves()[cave]);
			}
			else {
				cout << wrong_cave;
			}
			if (level->get_player().is_eaten()) {
				cout << eaten_message;
				break;
			}
			if (level->get_player().is_falling()) {
				cout << falling_message;
				break;
			}
			if (level->get_player().is_flying()) {
				cout << flying_message;

				int new_cave = randint(level->get_cave_count() - 1);
				while (new_cave == cave || (level->get_caves()[new_cave]->hazard && level->get_caves()[new_cave]->hazard->name == Hazard_Name::Giant_Bat)) {
					new_cave = randint(level->get_cave_count() - 1);
				}

				if (level->get_caves()[new_cave]->hazard && level->get_caves()[new_cave]->hazard->name == Hazard_Name::Wumpus) {
					cout << eaten_message;
					break;
				}

				if (level->get_caves()[new_cave]->hazard && level->get_caves()[new_cave]->hazard->name == Hazard_Name::Bottomless_Pit) {
					cout << falling_message;
					break;
				}

				cout << "\nYou land in cave " << new_cave << ".\n\n";
				level->get_current_cave()->player = nullptr;// Not a wumpus, bat or pit so must be a regular room.
				level->get_caves()[new_cave]->player = &level->get_player();
				level->set_current_cave(level->get_caves()[new_cave]);

				level->get_player().set_flying(false);
			}

			if (level->get_player().is_moving()) {
				cout << "\nYou follow the tunnel to cave " << cave << ".\n\n";
				level->get_current_cave()->player = nullptr;
				level->get_caves()[cave]->player = &level->get_player();
				level->set_current_cave(level->get_caves()[cave]);
				level->get_player().set_moving(false);
			}

		}

		if ((command == 's' || command == 'S') && level->get_player().get_bow_and_arrow().get_ammo() > 0) {
			if (connects(level->get_caves()[cave], connecting_caves)) {
				int direction = 0;
				while (connecting_caves[direction]->get_number() != cave) {
					++direction;
				}
				level->get_player().shoot(level->get_caves()[cave], direction);
				if (level->get_player().is_wumpus_killer()) {
					cout << winner_message;
					break;
				}
				else {
					cout << missed_message;
					level->move_the_wumpus();

					if (level->get_player().is_eaten()) {
						cout << eaten_message;
						break;
					}
				}
			}
			else {
				cout << wrong_cave;
			}
		}
	}
	cout << game_over_message;

}

void Game::print_state() {
	cout << "Level caves, connected cave and hazards\n\n";

	for (Cave* cave : level->get_caves()) {
		cout << "Cave " << cave->get_number() << ":\n" << "This cave connects to cave " << cave->connecting_caves[0]->get_number() << ", cave " << cave->connecting_caves[1]->get_number() << " and cave " << cave->connecting_caves[2]->get_number() << ".\n";
		if (cave->hazard) {
			switch (cave->hazard->get_name()) {
			case Hazard_Name::Wumpus: cout << "This cave contains the Wumpus!\n"; break;
			case Hazard_Name::Giant_Bat: cout << "This cave contains a Giant Bat.\n"; break;
			case Hazard_Name::Bottomless_Pit: cout << "This cave contains a bottomless pit.\n"; break;
			}
		}
		else {
			cout << "This cave has no hazards.\n";
		}

		cout << '\n';
	}
}

//void Game::print_wumpus_location() { //Used to debug
//	cout << "Wumpus location: " << level.get_wumpus_caves()[0]->get_number() << "\n";
//}

void play_game() {
	Game* game = new Game();
	//game->print_state();
	game->run_game();
	delete game;
}

int main() {
	try
	{
		play_game();
	}
	catch (exception& e)
	{
		cout << "Exception: " << e.what();
		return 1;
	}
	catch (...) {
		cout << "Big ol' error";
		return 2;
	}
}
