#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>
#include <conio.h>
#pragma warning(disable : 4996)
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <locale.h>
#include <time.h>

int id = 1, max_hp = 0;
const int backpack_size_X = 15, backpack_size_Y = 15;
struct item {
	int id;
	char name[50];
	int durability;
	int item_type; // 0 - armor, 1 - weapon
	int slot; // 0 - head, 1 - body, 2 - legs, 3 - feet, 4 - weapon
	int slot_type; // 0 - left, 1 - right, 2 - oburêczna
	// bronie
	int dmg;
	// zbroje
	int hp;
	int armor;
	int size_X;
	int size_Y;
};
struct backpack {
	item* item;
	int id;
};
struct champion {
	char name[50];
	int hp;
	int xp;
	int armor;
	int stength;
	int posX;
	int posY;
	int inv_slots[9];
	// 0 - head, 1 - body, 2 - l_legs, 3 - r_leg, 4- l_feet,  5 - r_feet, 6 - l_hand, 7 - r_hand, 8 - weapon;
	item* head;
	item* chestplate;
	item* left_leggins;
	item* right_leggins;
	item* left_boot;
	item* right_boot;
	item* left_hand;
	item* right_hand;
	item* weapon;
	backpack** backpack;
};
struct skrytka {
	item* item;
	int chest_xp;
	int pos_x;
	int pos_y;
};
struct pulapka {
	int trap_dmg;
	int pos_x;
	int pos_y;
};
struct enemy {
	char name[32];
	int hp;
	int max_hp;
	int dmg;
	int armor;
	int xp_reward;
	int pos_x;
	int pos_y;
	bool alive;
	int typeofmovement;
};
struct map {
	int width;
	int height;
	pulapka* traps;
	skrytka* chests;
	enemy* enemies;
	char** map;
};
typedef void(*lvlupfunc)(champion& postac);
item* generate(int type, item* current) {
	// 0 - armor, 1 - weapon
	switch (type) {
	case 0: // armor
		current->id = id++;
		strcpy(current->name, "Zbroja");
		current->durability = rand() % 100 + 1;
		current->item_type = 0;
		current->slot = rand() % 5;
		current->slot_type = rand() % 3;
		current->hp = rand() % 20 + 1;
		current->armor = rand() % 10 + 1;
		current->size_X = rand() % 3 + 1;
		current->size_Y = rand() % 3 + 1;
		break;
	case 1: // weapon
		current->id = id++;
		strcpy(current->name, "Broñ");
		current->durability = rand() % 100 + 1;
		current->item_type = 1;
		current->slot = rand() % 2;
		current->slot_type = rand() % 2;
		current->dmg = rand() % 15 + 5;
		current->size_X = rand() % 2 + 1;
		current->size_Y = rand() % 3 + 1;
		break;
	default:
		return nullptr;
	}
	return current;
}
champion* create_champion(int type, champion* current, int backpack_size_X, int backpack_size_Y, const char* name) {
	{
		// 0 -  human, 1 - orc, 2 - troll, 3 - dwarf;
		switch (type) {
		case 0: // Human
			current->hp = 100;
			current->xp = 0;
			current->armor = 5;
			current->stength = 5;
			break;
		case 1: // Orc
			current->hp = 110;
			current->xp = 0;
			current->armor = 3;
			current->stength = 8;
			break;
		case 2: // Goblin
			current->hp = 80;
			current->xp = 0;
			current->armor = 2;
			current->stength = 4;
			break;
		case 3: // Dwarf
			current->hp = 90;
			current->xp = 0;
			current->armor = 8;
			current->stength = 5;
			break;
		default: // Domyœlnie human
			current->hp = 100;
			current->xp = 0;
			current->armor = 5;
			current->stength = 5;
			break;
		}
		strncpy(current->name, name, sizeof(current->name) - 1);
		current->name[sizeof(current->name) - 1] = '\0';
		for (int i = 0; i < 9; ++i) current->inv_slots[i] = 0;
		current->posX = 0;
		current->posY = 0;
		current->head = nullptr;
		current->chestplate = nullptr;
		current->left_leggins = nullptr;
		current->right_leggins = nullptr;
		current->left_boot = nullptr;
		current->right_boot = nullptr;
		current->left_hand = nullptr;
		current->right_hand = nullptr;
		current->weapon = nullptr;
		current->backpack = (backpack**)malloc(backpack_size_X * sizeof(backpack*));
		for (int i = 0; i < backpack_size_X; ++i) {
			current->backpack[i] = (backpack*)malloc(backpack_size_Y * sizeof(backpack));
			for (int j = 0; j < backpack_size_Y; ++j) {
				current->backpack[i][j].item = nullptr;
				current->backpack[i][j].id = 0;
			}
		}
	}
	return current;
}
void add_item_backpack(champion* current, item* current_i) {
	if (current == nullptr || current_i == nullptr) return;
	for (int i = 0; i <= backpack_size_X - current_i->size_X; ++i) {
		for (int j = 0; j <= backpack_size_Y - current_i->size_Y; ++j) {
			bool wolne = true;
			for (int dx = 0; dx < current_i->size_X; ++dx) {
				for (int dy = 0; dy < current_i->size_Y; ++dy) {
					if (current->backpack[i + dx][j + dy].id != 0) {
						wolne = false;
						break;
					}
				}
				if (!wolne) break;
			}
			if (wolne) {
				// Wstaw przedmiot do wszystkich zajmowanych pól
				for (int dx = 0; dx < current_i->size_X; ++dx) {
					for (int dy = 0; dy < current_i->size_Y; ++dy) {
						current->backpack[i + dx][j + dy].id = current_i->id;
						if (dx == 0 && dy == 0) {
							current->backpack[i + dx][j + dy].item = current_i; // lewy górny róg
						}
						else {
							current->backpack[i + dx][j + dy].item = nullptr;
						}
					}
				}
				return;
			}
		}
	}
	printf("Plecak jest pe³ny lub brak miejsca na przedmiot o rozmiarze %dx%d!\n", current_i->size_X, current_i->size_Y);
}
void show_backpack(champion* current) {
	printf("Zawartoœæ plecaka:\n");
	for (int i = 0; i < backpack_size_X; ++i) {
		for (int j = 0; j < backpack_size_Y; ++j) {
			printf("%3d ", current->backpack[i][j].id);
		}
		printf("\n");
	}
}
void equip_item(champion* bohater, item* przedmiot) {
	if (przedmiot->item_type == 0) { // armor
		bool zalozono = false;
		switch (przedmiot->slot) {
		case 0:
			if (bohater->head == nullptr) {
				bohater->head = przedmiot;
				zalozono = true;
			}
			break;
		case 1:
			if (bohater->chestplate == nullptr) {
				bohater->chestplate = przedmiot;
				zalozono = true;
			}
			break;
		case 2:
			if (bohater->left_leggins == nullptr) {
				bohater->left_leggins = przedmiot;
				zalozono = true;
			}
			break;
		case 3:
			if (bohater->right_leggins == nullptr) {
				bohater->right_leggins = przedmiot;
				zalozono = true;
			}
			break;
		case 4:
			if (bohater->left_boot == nullptr) {
				bohater->left_boot = przedmiot;
				zalozono = true;
			}
			break;
		case 5:
			if (bohater->right_boot == nullptr) {
				bohater->right_boot = przedmiot;
				zalozono = true;
			}
			break;
		}
		if (zalozono) {
			printf("Za³o¿ono zbrojê na odpowiedni slot.\n");
		}
		else {
			printf("Slot na zbrojê zajêty, dodano do plecaka.\n");
			add_item_backpack(bohater, przedmiot);
		}
	}
	else if (przedmiot->item_type == 1) { // weapon
		bool zalozono = false;
		if (przedmiot->slot == 0) {
			if (bohater->left_hand == nullptr) {
				bohater->left_hand = przedmiot;
				zalozono = true;
			}
		}
		else {
			if (bohater->right_hand == nullptr) {
				bohater->right_hand = przedmiot;
				zalozono = true;
			}
		}
		if (zalozono) {
			printf("Za³o¿ono broñ do odpowiedniej rêki.\n");
		}
		else {
			printf("Slot na broñ zajêty, dodano do plecaka.\n");
			add_item_backpack(bohater, przedmiot);
		}
	}
}
void move_backpack_to_equipment(champion* bohater) {
	while (true) {
		system("cls");
		printf("====== Przenieœ przedmiot z plecaka do ekwipunku ======\n");
		printf("Podaj ID przedmiotu do za³o¿enia (0 - wyjœcie):\n");
		show_backpack(bohater);

		printf("Twój wybór (ID): ");
		int wybor = 0;
		scanf("%d", &wybor);
		getchar();
		if (wybor == 0) return;
		item* to_equip = nullptr;
		for (int i = 0; i < backpack_size_X; ++i) {
			for (int j = 0; j < backpack_size_Y; ++j) {
				if (bohater->backpack[i][j].id == wybor && bohater->backpack[i][j].item != nullptr) {
					to_equip = bohater->backpack[i][j].item;
					break;
				}
			}
			if (to_equip) break;
		}
		if (!to_equip) {
			printf("Nie znaleziono przedmiotu o podanym ID!\n");
			getch();
			continue;
		}
		equip_item(bohater, to_equip);
		for (int i = 0; i < backpack_size_X; ++i) {
			for (int j = 0; j < backpack_size_Y; ++j) {
				if (bohater->backpack[i][j].id == wybor) {
					bohater->backpack[i][j].id = 0;
					bohater->backpack[i][j].item = nullptr;
				}
			}
		}
		printf("Przeniesiono przedmiot do ekwipunku (jeœli slot by³ wolny).\n");
		getch();
		return;
	}
}
void remove_item_from_backpack(champion* bohater) {
	while (true) {
		system("cls");
		printf("====== Usuñ przedmiot z plecaka ======\n");
		printf("Podaj ID przedmiotu do usuniêcia (0 - wyjœcie):\n");
		show_backpack(bohater);

		printf("Twój wybór (ID): ");
		int wybor = 0;
		scanf("%d", &wybor);
		getchar();
		if (wybor == 0) return;
		item* to_remove = nullptr;
		for (int i = 0; i < backpack_size_X; ++i) {
			for (int j = 0; j < backpack_size_Y; ++j) {
				if (bohater->backpack[i][j].id == wybor && bohater->backpack[i][j].item != nullptr) {
					to_remove = bohater->backpack[i][j].item;
					break;
				}
			}
			if (to_remove) break;
		}
		if (!to_remove) {
			printf("Nie znaleziono przedmiotu o podanym ID!\n");
			getch();
			continue;
		}
		for (int i = 0; i < backpack_size_X; ++i) {
			for (int j = 0; j < backpack_size_Y; ++j) {
				if (bohater->backpack[i][j].id == wybor) {
					bohater->backpack[i][j].id = 0;
					bohater->backpack[i][j].item = nullptr;
				}
			}
		}
		free(to_remove);
		printf("Przedmiot zosta³ usuniêty z plecaka.\n");
		getch();
		return;
	}
}
const char* get_armor_slot_name(int slot) {
	switch (slot) {
	case 0: return "He³m";
	case 1: return "Napierœnik";
	case 2: return "Lewy nagolennik";
	case 3: return "Prawy nagolennik";
	case 4: return "Lewy but";
	case 5: return "Prawy but";
	default: return "Nieznany";
	}
}
const char* get_weapon_slot_name(int slot) {
	switch (slot) {
	case 0: return "Lewa rêka";
	case 1: return "Prawa rêka";
	default: return "Nieznany";
	}
}
void found_item(champion* bohater, item** przedmiot_ptr) {
	if (*przedmiot_ptr == nullptr) return;
	item* przedmiot = *przedmiot_ptr;
	printf("Znalaz³eœ przedmiot!\n");
	printf("Nazwa: %s\n", przedmiot->name);
	printf("ID: %d\n", przedmiot->id);
	if (przedmiot->item_type == 0) {
		printf("Typ: Zbroja (%s)\n", get_armor_slot_name(przedmiot->slot));
		printf("Wytrzyma³oœæ: %d\n", przedmiot->durability);
		printf("HP: %d, Armor: %d\n", przedmiot->hp, przedmiot->armor);
	}
	else {
		printf("Typ: Broñ (%s)\n", get_weapon_slot_name(przedmiot->slot));
		printf("Wytrzyma³oœæ: %d\n", przedmiot->durability);
		printf("DMG: %d\n", przedmiot->dmg);
	}
	printf("Rozmiar: %dx%d\n", przedmiot->size_X, przedmiot->size_Y);

	printf("Co chcesz zrobiæ z tym przedmiotem?\n");
	printf("1 - Za³ó¿\n");
	printf("2 - Dodaj do plecaka\n");
	printf("3 - Odrzuæ\n");
	printf("Twój wybór: ");
	int wybor = 0;
	scanf("%d", &wybor);
	getchar();

	if (wybor == 1) {
		equip_item(bohater, przedmiot);
	}
	else if (wybor == 2) {
		add_item_backpack(bohater, przedmiot);
		printf("Dodano do plecaka.\n");
	}
	else if (wybor == 3) {
		printf("Przedmiot zosta³ odrzucony.\n");
		free(przedmiot);
		*przedmiot_ptr = nullptr;
		return;
	}
	else {
		printf("Nieprawid³owy wybór. Przedmiot zosta³ dodany do plecaka.\n");
		add_item_backpack(bohater, przedmiot);
	}
	*przedmiot_ptr = nullptr;
}
void move_equipment_to_backpack(champion* bohater) {
	while (true) {
		system("cls");
		printf("====== Przenieœ przedmiot z ekwipunku do plecaka ======\n");
		printf("Wybierz numer slotu do przeniesienia (0 - g³owa, 1 - korpus, 2 - lewy nagolennik, 3 - prawy nagolennik, 4 - lewy but, 5 - prawy but, 6 - lewa rêka, 7 - prawa rêka, 8 - broñ, 9 - wyjœcie):\n");

		printf("0. G³owa:        %s\n", bohater->head ? bohater->head->name : "brak");
		printf("1. Korpus:       %s\n", bohater->chestplate ? bohater->chestplate->name : "brak");
		printf("2. Lewy nagolennik: %s\n", bohater->left_leggins ? bohater->left_leggins->name : "brak");
		printf("3. Prawy nagolennik: %s\n", bohater->right_leggins ? bohater->right_leggins->name : "brak");
		printf("4. Lewy but:     %s\n", bohater->left_boot ? bohater->left_boot->name : "brak");
		printf("5. Prawy but:    %s\n", bohater->right_boot ? bohater->right_boot->name : "brak");
		printf("6. Lewa rêka:    %s\n", bohater->left_hand ? bohater->left_hand->name : "brak");
		printf("7. Prawa rêka:   %s\n", bohater->right_hand ? bohater->right_hand->name : "brak");
		printf("8. Broñ:         %s\n", bohater->weapon ? bohater->weapon->name : "brak");
		printf("9. Wyjœcie\n");

		printf("Twój wybór: ");
		int wybor = 0;
		scanf("%d", &wybor);
		getchar();

		item* to_move = nullptr;
		switch (wybor) {
		case 0: to_move = bohater->head; break;
		case 1: to_move = bohater->chestplate; break;
		case 2: to_move = bohater->left_leggins; break;
		case 3: to_move = bohater->right_leggins; break;
		case 4: to_move = bohater->left_boot; break;
		case 5: to_move = bohater->right_boot; break;
		case 6: to_move = bohater->left_hand; break;
		case 7: to_move = bohater->right_hand; break;
		case 8: to_move = bohater->weapon; break;
		case 9: return;
		default: printf("Nieprawid³owy wybór!\n"); getch(); continue;
		}

		if (!to_move) {
			printf("Brak przedmiotu w wybranym slocie!\n");
			getch();
			continue;
		}

		add_item_backpack(bohater, to_move);

		switch (wybor) {
		case 0: bohater->head = nullptr; break;
		case 1: bohater->chestplate = nullptr; break;
		case 2: bohater->left_leggins = nullptr; break;
		case 3: bohater->right_leggins = nullptr; break;
		case 4: bohater->left_boot = nullptr; break;
		case 5: bohater->right_boot = nullptr; break;
		case 6: bohater->left_hand = nullptr; break;
		case 7: bohater->right_hand = nullptr; break;
		case 8: bohater->weapon = nullptr; break;
		}
		printf("Przeniesiono przedmiot do plecaka.\n");
		getch();
	}
}
void remove_equipped_item(champion* bohater, item** slot) {
	if (slot && *slot) {
		free(*slot);
		*slot = nullptr;
	}
}
void lvl_up_normal(champion& player) {
	player.hp = 5 + max_hp;
	max_hp = player.hp;
	player.armor += 1;
	player.stength += 2;
	player.xp -= 100;
}
void lvl_up_plus_5(champion& player) {
	player.hp = 5 + max_hp;
	max_hp = player.hp;
	player.armor += 5;
	player.stength += 5;
	player.xp -= 100;
}
void lvl_up_plus_3(champion& player) {
	player.hp = 3 + max_hp;
	max_hp = player.hp;
	player.armor += 3;
	player.stength += 3;
	player.xp -= 100;
}

void fight(champion* player, enemy* foe, int start_hp) {
	printf("Rozpoczynasz walkê z przeciwnikiem!\n");

	int total_armor = player->armor;
	if (player->head)         total_armor += player->head->armor;
	if (player->chestplate)   total_armor += player->chestplate->armor;
	if (player->left_leggins) total_armor += player->left_leggins->armor;
	if (player->right_leggins)total_armor += player->right_leggins->armor;
	if (player->left_boot)    total_armor += player->left_boot->armor;
	if (player->right_boot)   total_armor += player->right_boot->armor;
	int weapon_dmg = 0;
	if (player->left_hand && player->left_hand->item_type == 1)
		weapon_dmg += player->left_hand->dmg;
	if (player->right_hand && player->right_hand->item_type == 1)
		weapon_dmg += player->right_hand->dmg;
	if (weapon_dmg == 0 && player->weapon)
		weapon_dmg = player->weapon->dmg;
	if (weapon_dmg == 0)
		weapon_dmg = 1;

	int hp_before = player->hp;

	while (player->hp > 0 && foe->hp > 0) {
		// Obra¿enia gracza
		int player_dmg = (int)(
			log2(1.0 + player->stength + weapon_dmg) * 5.0 - log2(1.0 + foe->armor) * 2.5+ (rand() % 3 + 3));
		if (player_dmg < 1) player_dmg = 1;
		if (player->left_hand && player->left_hand->item_type == 1) {
			player->left_hand->durability--;
			if (player->left_hand->durability <= 0) {
				printf("Twoja broñ w lewej rêce siê zniszczy³a!\n");
				remove_equipped_item(player, &player->left_hand);
			}
		}
		if (player->right_hand && player->right_hand->item_type == 1) {
			player->right_hand->durability--;
			if (player->right_hand->durability <= 0) {
				printf("Twoja broñ w prawej rêce siê zniszczy³a!\n");
				remove_equipped_item(player, &player->right_hand);
			}
		}
		if (player->weapon && weapon_dmg == player->weapon->dmg) {
			player->weapon->durability--;
			if (player->weapon->durability <= 0) {
				printf("Twoja broñ siê zniszczy³a!\n");
				remove_equipped_item(player, &player->weapon);
			}
		}

		// Obra¿enia przeciwnika
		int foe_dmg = (int)(
			log2(1.0 + foe->dmg) * 5.0 - log2(1.0 + total_armor) * 2.5+ (rand() % 3 + 3));
		if (foe_dmg < 1) foe_dmg = 1;
		// Zu¿ycie zbroi po otrzymaniu obra¿eñ
		if (player->head) {
			player->head->durability--;
			if (player->head->durability <= 0) {
				printf("Twój he³m siê zniszczy³!\n");
				remove_equipped_item(player, &player->head);
			}
		}
		if (player->chestplate) {
			player->chestplate->durability--;
			if (player->chestplate->durability <= 0) {
				printf("Twój napierœnik siê zniszczy³!\n");
				remove_equipped_item(player, &player->chestplate);
			}
		}
		if (player->left_leggins) {
			player->left_leggins->durability--;
			if (player->left_leggins->durability <= 0) {
				printf("Twoje lewe nagolenniki siê zniszczy³y!\n");
				remove_equipped_item(player, &player->left_leggins);
			}
		}
		if (player->right_leggins) {
			player->right_leggins->durability--;
			if (player->right_leggins->durability <= 0) {
				printf("Twoje prawe nagolenniki siê zniszczy³y!\n");
				remove_equipped_item(player, &player->right_leggins);
			}
		}
		if (player->left_boot) {
			player->left_boot->durability--;
			if (player->left_boot->durability <= 0) {
				printf("Twój lewy but siê zniszczy³!\n");
				remove_equipped_item(player, &player->left_boot);
			}
		}
		if (player->right_boot) {
			player->right_boot->durability--;
			if (player->right_boot->durability <= 0) {
				printf("Twój prawy but siê zniszczy³!\n");
				remove_equipped_item(player, &player->right_boot);
			}
		}


		printf("Zadajesz %d obra¿eñ. ", player_dmg);
		foe->hp -= player_dmg;
		if (foe->hp <= 0) break;
		printf("%Przeciwnik zadaje Ci %d obra¿eñ.\n", foe_dmg);
		player->hp -= foe_dmg;
		printf("Twoje HP: %d, HP przeciwnika: %d\n", player->hp, foe->hp);
		printf("Naciœnij dowolny klawisz, aby kontynuowaæ...\n");
		getch();
	}
	if (player->hp > 0) {
		int xp = rand() % 21 + 17;
		printf("Pokona³eœ przeciwnika! Zdobywasz %d XP.\n", xp);
		player->xp += xp;
		int hp_po = player->hp;
		int utracone = hp_before - hp_po;
		if (utracone > 0) {
			int przywroc = (int)(utracone * 0.6);
			player->hp += przywroc;
			printf("Odzyskujesz %d HP (60%% utraconego zdrowia). Twoje HP: %d\n", przywroc, player->hp);
		}
		foe->alive = false;
		while (player->xp >= 100) {
			printf("Awans! Statystyki zosta³y zwiêkszone o\n");
			lvlupfunc func_ptr = nullptr;
			int c = rand() % 3;
			if (c == 0) {
				func_ptr = lvl_up_normal;
				printf(" normalna wartoœæ.");
			}
			else if (c == 1) {
				func_ptr = lvl_up_plus_3;
				printf(" 3 ka¿da.");
			}
			else if (c == 2) {
				func_ptr = lvl_up_plus_5;
				printf(" 5 ka¿da.");
			}
			func_ptr(*player);
		}
	}
	else {
		printf("Zosta³eœ pokonany przez %s!\n", foe->name);
		printf("Koniec gry.\n");
		exit(0);
	}
	printf("Naciœnij dowolny klawisz, aby wróciæ do gry...\n");
	getch();
}
void show_stats(champion* bohater) {
	printf("====== Statystyki bohatera: %s ======\n", bohater->name);
	printf("HP: %d\n", bohater->hp);
	printf("XP: %d\n", bohater->xp);
	printf("Pancerz: %d\n", bohater->armor);
	printf("Si³a: %d\n", bohater->stength);
	printf("Pozycja: (%d, %d)\n", bohater->posX, bohater->posY);
}
void show_equipment(champion* bohater) {
	printf("====== Ekwipunek bohatera: %s ======\n", bohater->name);

	printf("G³owa:        ");
	if (bohater->head)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->head->name, bohater->head->id, bohater->head->durability, bohater->head->armor, bohater->head->hp);
	else
		printf("brak\n");

	printf("Korpus:       ");
	if (bohater->chestplate)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->chestplate->name, bohater->chestplate->id, bohater->chestplate->durability, bohater->chestplate->armor, bohater->chestplate->hp);
	else
		printf("brak\n");

	printf("Lewy nagolennik: ");
	if (bohater->left_leggins)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->left_leggins->name, bohater->left_leggins->id, bohater->left_leggins->durability, bohater->left_leggins->armor, bohater->left_leggins->hp);
	else
		printf("brak\n");

	printf("Prawy nagolennik: ");
	if (bohater->right_leggins)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->right_leggins->name, bohater->right_leggins->id, bohater->right_leggins->durability, bohater->right_leggins->armor, bohater->right_leggins->hp);
	else
		printf("brak\n");

	printf("Lewy but:     ");
	if (bohater->left_boot)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->left_boot->name, bohater->left_boot->id, bohater->left_boot->durability, bohater->left_boot->armor, bohater->left_boot->hp);
	else
		printf("brak\n");

	printf("Prawy but:    ");
	if (bohater->right_boot)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, Armor: %d, HP: %d)\n", bohater->right_boot->name, bohater->right_boot->id, bohater->right_boot->durability, bohater->right_boot->armor, bohater->right_boot->hp);
	else
		printf("brak\n");

	printf("Lewa rêka:    ");
	if (bohater->left_hand)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, DMG: %d)\n", bohater->left_hand->name, bohater->left_hand->id, bohater->left_hand->durability, bohater->left_hand->dmg);
	else
		printf("brak\n");

	printf("Prawa rêka:   ");
	if (bohater->right_hand)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, DMG: %d)\n", bohater->right_hand->name, bohater->right_hand->id, bohater->right_hand->durability, bohater->right_hand->dmg);
	else
		printf("brak\n");

	printf("Broñ:         ");
	if (bohater->weapon)
		printf("%s (ID: %d, Wytrzyma³oœæ: %d, DMG: %d)\n", bohater->weapon->name, bohater->weapon->id, bohater->weapon->durability, bohater->weapon->dmg);
	else
		printf("brak\n");
}
void compare_item_with_equipped(champion* bohater, item* przedmiot) {
	if (!przedmiot) {
		printf("Brak przedmiotu do porównania.\n");
		return;
	}

	item* equipped = nullptr;
	if (przedmiot->item_type == 0) { // armor
		switch (przedmiot->slot) {
		case 0: equipped = bohater->head; break;
		case 1: equipped = bohater->chestplate; break;
		case 2: equipped = bohater->left_leggins; break;
		case 3: equipped = bohater->right_leggins; break;
		case 4: equipped = bohater->left_boot; break;
		case 5: equipped = bohater->right_boot; break;
		}
		printf("Porównanie zbroi (%s):\n", get_armor_slot_name(przedmiot->slot));
		printf("Plecak:  HP: %d, Armor: %d, Wytrzyma³oœæ: %d\n", przedmiot->hp, przedmiot->armor, przedmiot->durability);
		if (equipped)
			printf("Za³o¿ony: HP: %d, Armor: %d, Wytrzyma³oœæ: %d\n", equipped->hp, equipped->armor, equipped->durability);
		else
			printf("Za³o¿ony: brak\n");
	}
	else if (przedmiot->item_type == 1) { // weapon
		if (przedmiot->slot == 0)
			equipped = bohater->left_hand;
		else
			equipped = bohater->right_hand;
		printf("Porównanie broni (%s):\n", get_weapon_slot_name(przedmiot->slot));
		printf("Plecak:  DMG: %d, Wytrzyma³oœæ: %d\n", przedmiot->dmg, przedmiot->durability);
		if (equipped)
			printf("Za³o¿ony: DMG: %d, Wytrzyma³oœæ: %d\n", equipped->dmg, equipped->durability);
		else
			printf("Za³o¿ony: brak\n");
	}
	printf("Naciœnij dowolny klawisz, aby kontynuowaæ...\n");
	getch();
}
void print_char_map(char** char_map, int width, int height) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			printf("%c", char_map[y][x]);
		}
		printf("\n");
	}
}
void start(int& champion_type, int width, int height) {
	printf("########################################################################################################################");
	for (int i = 0; i < 34; i++) printf("b\n");
	printf("Skalibruj rozmiar okna, tak aby # by³o na samej górze ekranu, a b by³o po prawej stronie. \n");
	printf("Nacisnij dowolny przycisk aby kontynuowaæ: ");
	getch();
	system("cls");
	// Górna krawêdŸ
	printf("########################################################################################################################");
	printf("\n");

	// Œrodek ramki: 28 wierszy
	for (int i = 0; i < 28 - 5; ++i) {
		printf("#");
		if (i == 9)		  printf("	Witaj w grze RPG!                                                                                              ");
		else if (i == 10) printf("	Zacznijmy od wybrania postaci                                                                                  ");
		else if (i == 11) printf("	Ka¿dy typ, posiada nieco inne statystyki pocz¹tkowe:                                                           ");
		else if (i == 12) printf("		0 - cz³owiek    | HP: 100 | Si³a: 5 | Pancerz: 5                                                       ");
		else if (i == 13) printf("		1 - orc         | HP: 110 | Si³a: 8 | Pancerz: 3                                                       ");
		else if (i == 14) printf("		2 - goblin      | HP:  80 | Si³a: 4 | Pancerz: 2                                                       ");
		else if (i == 15) printf("		3 - krasnolud   | HP:  90 | Si³a: 5 | Pancerz: 8                                                       ");
		else {
			for (int j = 0; j < 118; ++j) printf(" ");
		}
		printf("#\n");
	}

	// Dolna krawêdŸ
	printf("########################################################################################################################");
	printf("\n");

	// Teraz komunikaty i wybór typu postaci
	printf("\nWprowadŸ swój wybór, uwaga nie bêdzie mo¿na go zmieniæ.\n");
	printf("Wpisanie innej liczby przypisze automatycznie typ: cz³owiek.\n");
	printf("Wybrany typ postaci: ");
	scanf("%d", &champion_type);
	getchar();
	system("cls");

	switch (champion_type) {
	case 0:
		printf("           ______\n");
		printf("        .-\"      \"-.\n");
		printf("       /            \\\n");
		printf("      |              |\n");
		printf("      |,  .-.  .-.  ,|\n");
		printf("      | )(__/  \\__)( |\n");
		printf("      |/     /\\     \\|\n");
		printf("      (_     ^^     _)\n");
		printf("       \\__|IIIIII|__/\n");
		printf("        | \\IIIIII/ |\n");
		printf("        \\          /\n");
		printf("         `--------`\n");
		break;
	case 1:
		printf("        -'''''-\n");
		printf("     {           }\n");
		printf("   |\\{ - O | O - }/|\n");
		printf("   \\ \\   ),=,(  /  / \n");
		printf("   (_/\\         /\\_)\n");
		printf("      /_ V+++V _\\ \n");
		printf("       \\___,___/\n");
		break;

	case 2:
		printf("             ,      ,\n");
		printf("            /(.-\"\"-.)\\\n");
		printf("        |\\  \\/      \\/  /|\n");
		printf("        | \\ / =.  .= \\ / |\n");
		printf("        \\( \\   o\\/o   / )/\n");
		printf("         \\_, '-/  \\-' ,_/\n");
		printf("           /   \\__/   \\\n");
		printf("           \\ \\__/\\__/ /\n");
		printf("         ___\\ \\|--|/ /___\n");
		printf("       /`    \\      /    `\\\n");
		printf("      /       '----'       \\\n");
		break;

	case 3:
		printf("        _.._..,_,_\n");
		printf("       (          )\n");
		printf("        ]~,\"-.-~~[\n");
		printf("      .=])' (;  ([\n");
		printf("      | ]:: '    [\n");
		printf("      '=]): .)  ([\n");
		printf("        |:: '    |\n");
		printf("         ~~----~~\n");
		break;

	default:
		break;
	}
	printf("\n\n Teraz czas aby nazwaæ naszego bohatera: ");
}
void generate_map_and_enemies(map** mapa, int map_width, int map_height, int ENEMY_COUNT, int liczba_skrytek, int liczba_pulapek, int liczba_przeszkod) {
	if (*mapa) {
		for (int i = 0; i < map_height; ++i) free((*mapa)->map[i]);
		free((*mapa)->map);
		for (int i = 0; i < liczba_skrytek; ++i)
			if ((*mapa)->chests[i].item) free((*mapa)->chests[i].item);
		free((*mapa)->chests);
		free((*mapa)->traps);
		free((*mapa)->enemies);
		free(*mapa);
	}

	*mapa = (map*)malloc(sizeof(map));
	(*mapa)->width = map_width;
	(*mapa)->height = map_height;

	// Mapa znakowa
	(*mapa)->map = (char**)malloc(map_height * sizeof(char*));
	for (int y = 0; y < map_height; ++y) {
		(*mapa)->map[y] = (char*)malloc(map_width * sizeof(char));
		for (int y = 0; y < map_height; ++y) {
			(*mapa)->map[y] = (char*)malloc(map_width * sizeof(char));
			for (int x = 0; x < map_width; ++x) {
				(*mapa)->map[y][x] = '.';
			}
		}

	}

	// Pu³apki
	(*mapa)->traps = (pulapka*)malloc(liczba_pulapek * sizeof(pulapka));
	for (int i = 0; i < liczba_pulapek; ++i) {
		int x, y;
		do {
			x = rand() % map_width;
			y = rand() % map_height;
		} while ((x == 0 && y == 0) || (*mapa)->map[y][x] != '.');
		(*mapa)->traps[i].pos_x = x;
		(*mapa)->traps[i].pos_y = y;
		(*mapa)->traps[i].trap_dmg = rand() % 10 + 1;
		(*mapa)->map[y][x] = '.';
	}

	// Skrytki
	(*mapa)->chests = (skrytka*)malloc(liczba_skrytek * sizeof(skrytka));
	for (int i = 0; i < liczba_skrytek; ++i) {
		int x, y;
		do {
			x = rand() % map_width;
			y = rand() % map_height;
		} while ((x == 0 && y == 0) || (*mapa)->map[y][x] != '.');
		(*mapa)->chests[i].pos_x = x;
		(*mapa)->chests[i].pos_y = y;
		(*mapa)->chests[i].chest_xp = rand() % 10 + 1;
		(*mapa)->chests[i].item = (item*)malloc(sizeof(item));
		int typ = rand() % 2;
		generate(typ, (*mapa)->chests[i].item);
		(*mapa)->map[y][x] = 'C';
	}

	// Przeciwnicy
	(*mapa)->enemies = (enemy*)malloc(ENEMY_COUNT * sizeof(enemy));
	for (int i = 0; i < ENEMY_COUNT; ++i) {
		int x, y;
		do {
			x = rand() % map_width;
			y = rand() % map_height;
		} while ((x == 0 && y == 0) || (*mapa)->map[y][x] != '.');
		(*mapa)->enemies[i].pos_x = x;
		(*mapa)->enemies[i].pos_y = y;
		(*mapa)->enemies[i].alive = true;
		(*mapa)->enemies[i].hp = rand() % 30 + 30; // 30-59 HP
		(*mapa)->enemies[i].max_hp = (*mapa)->enemies[i].hp;
		(*mapa)->enemies[i].dmg = rand() % 8 + 3;  // 3-10 DMG
		(*mapa)->enemies[i].armor = rand() % 4 + 1; // 1-4 Armor
		(*mapa)->enemies[i].xp_reward = rand() % 20 + 10; // 10-29 XP
		(*mapa)->enemies[i].typeofmovement = rand() % 2; //
		snprintf((*mapa)->enemies[i].name, sizeof((*mapa)->enemies[i].name), "Przeciwnik%d", i + 1);
		(*mapa)->map[y][x] = 'E';
	}

	// Przeszkody
	for (int i = 0; i < liczba_przeszkod; ++i) {
		int x, y;
		do {
			x = rand() % map_width;
			y = rand() % map_height;
		} while ((x == 0 && y == 0) || (*mapa)->map[y][x] != '.');
		(*mapa)->map[y][x] = '|';
	}
}
void move_enemies_towards_player(map* mapa, champion* player, int ENEMY_COUNT) {
	for (int i = 0; i < ENEMY_COUNT; ++i) {
		if (!mapa->enemies[i].alive) continue;
		int ex = mapa->enemies[i].pos_x;
		int ey = mapa->enemies[i].pos_y;
		int px = player->posX;
		int py = player->posY;
		int dx = px - ex;
		int dy = py - ey;
		if (!(ex == px && ey == py)) {
			mapa->map[ey][ex] = '.';
		}
		switch (mapa->enemies[i].typeofmovement) {
		case 0: //Przeciwnik sie nie rusza
			break;
		case 1: //Przeciwnik idzie w naszym kierunku
			if (abs(dx) > abs(dy)) {
				if (dx > 0 && mapa->map[ey][ex + 1] == '.') ex++;
				else if (dx < 0 && mapa->map[ey][ex - 1] == '.') ex--;
			}
			else if (abs(dy) > 0) {
				if (dy > 0 && mapa->map[ey + 1][ex] == '.') ey++;
				else if (dy < 0 && mapa->map[ey - 1][ex] == '.') ey--;
			}
		}
		mapa->enemies[i].pos_x = ex;
		mapa->enemies[i].pos_y = ey;
		if (!(ex == px && ey == py)) {
			mapa->map[ey][ex] = 'E';
		}
	}
}
//^Eksperymentalna, cos dzia³a³o, popracowac jeszcze bo sie czasami przeciwnycy dubluj¹ .1
//^Nie mam pojecia .2
void save_item(FILE* f, item* it) {
	if (!it) {
		fprintf(f, "NULL\n");
		return;
	}
	fprintf(f, "%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		it->id, it->name, it->durability, it->item_type, it->slot, it->slot_type,
		it->dmg, it->hp, it->armor, it->size_X, it->size_Y, 0, 0);
}
void save_backpack(FILE* f, champion* ch) {
	int saved_ids[backpack_size_X * backpack_size_Y] = { 0 };
	int saved_count = 0;
	for (int i = 0; i < backpack_size_X; ++i) {
		for (int j = 0; j < backpack_size_Y; ++j) {
			item* it = ch->backpack[i][j].item;
			int id = ch->backpack[i][j].id;
			if (it && id != 0) {
				int already = 0;
				for (int k = 0; k < saved_count; ++k)
					if (saved_ids[k] == id) already = 1;
				if (!already) {
					save_item(f, it);
					saved_ids[saved_count++] = id;
				}
			}
		}
	}
	fprintf(f, "END_BACKPACK\n");
}
void save_equipment(FILE* f, champion* ch) {
	save_item(f, ch->head);
	save_item(f, ch->chestplate);
	save_item(f, ch->left_leggins);
	save_item(f, ch->right_leggins);
	save_item(f, ch->left_boot);
	save_item(f, ch->right_boot);
	save_item(f, ch->left_hand);
	save_item(f, ch->right_hand);
	save_item(f, ch->weapon);
	fprintf(f, "END_EQUIPMENT\n");
}
void save_champion(FILE* f, champion* ch) {
	fprintf(f, "%s,%d,%d,%d,%d,%d,%d\n", ch->name, ch->hp, ch->xp, ch->armor, ch->stength, ch->posX, ch->posY);
	save_equipment(f, ch);
	save_backpack(f, ch);
	fprintf(f, "END_CHAMPION\n");
}
void save_map(FILE* f, map* m, int ENEMY_COUNT, int liczba_skrytek, int liczba_pulapek, int liczba_przeszkod) {
	fprintf(f, "%d,%d,%d,%d,%d,%d\n", m->width, m->height, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);

	// Przeciwnicy
	for (int i = 0; i < ENEMY_COUNT; ++i) {
		enemy* e = &m->enemies[i];
		fprintf(f, "%s,%d,%d,%d,%d,%d,%d,%d,%d\n", e->name, e->hp, e->max_hp, e->dmg, e->armor, e->xp_reward, e->pos_x, e->pos_y, e->alive ? 1 : 0);
	}
	fprintf(f, "END_ENEMIES\n");

	// Pu³apki
	for (int i = 0; i < liczba_pulapek; ++i) {
		pulapka* p = &m->traps[i];
		fprintf(f, "%d,%d,%d\n", p->trap_dmg, p->pos_x, p->pos_y);
	}
	fprintf(f, "END_TRAPS\n");

	// Skrytki
	for (int i = 0; i < liczba_skrytek; ++i) {
		skrytka* s = &m->chests[i];
		fprintf(f, "%d,%d,%d\n", s->chest_xp, s->pos_x, s->pos_y);
		save_item(f, s->item);
	}
	fprintf(f, "END_CHESTS\n");

	// Mapa znakowa
	for (int y = 0; y < m->height; ++y) {
		for (int x = 0; x < m->width; ++x) {
			fputc(m->map[y][x], f);
		}
		fputc('\n', f);
	}
	fprintf(f, "END_MAP\n");
}
void save_game(const char* filename, champion* ch, map* m, int ENEMY_COUNT, int liczba_skrytek, int liczba_pulapek, int liczba_przeszkod) {
	FILE* f = fopen(filename, "w");
	if (!f) {
		printf("Nie mo¿na otworzyæ pliku do zapisu!\n");
		return;
	}
	save_champion(f, ch);
	save_map(f, m, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
	fclose(f);
	printf("Gra zosta³a zapisana do pliku %s\n", filename);
	getch();
}
item* load_item(FILE* f) {
	static char buf[256];
	if (!fgets(buf, sizeof(buf), f)) return nullptr;
	if (strncmp(buf, "NULL", 4) == 0) return nullptr;
	item* it = (item*)malloc(sizeof(item));
	sscanf(buf, "%d,%49[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",
		&it->id, it->name, &it->durability, &it->item_type, &it->slot, &it->slot_type,
		&it->dmg, &it->hp, &it->armor, &it->size_X, &it->size_Y);
	return it;
}
void load_equipment(FILE* f, champion* ch) {
	ch->head = load_item(f);
	ch->chestplate = load_item(f);
	ch->left_leggins = load_item(f);
	ch->right_leggins = load_item(f);
	ch->left_boot = load_item(f);
	ch->right_boot = load_item(f);
	ch->left_hand = load_item(f);
	ch->right_hand = load_item(f);
	ch->weapon = load_item(f);
	char buf[64];
	fgets(buf, sizeof(buf), f); // END_EQUIPMENT
}
void load_backpack(FILE* f, champion* ch) {
	// Najpierw wyczyœæ plecak
	for (int i = 0; i < backpack_size_X; ++i)
		for (int j = 0; j < backpack_size_Y; ++j) {
			ch->backpack[i][j].item = nullptr;
			ch->backpack[i][j].id = 0;
		}
	// Wczytaj przedmioty do tablicy tymczasowej
	item* items[backpack_size_X * backpack_size_Y] = { 0 };
	int ids[backpack_size_X * backpack_size_Y] = { 0 };
	int count = 0;
	char buf[256];
	while (fgets(buf, sizeof(buf), f)) {
		if (strncmp(buf, "END_BACKPACK", 12) == 0) break;
		if (strncmp(buf, "NULL", 4) == 0) continue;
		item* it = (item*)malloc(sizeof(item));
		sscanf(buf, "%d,%49[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",
			&it->id, it->name, &it->durability, &it->item_type, &it->slot, &it->slot_type,
			&it->dmg, &it->hp, &it->armor, &it->size_X, &it->size_Y);
		items[count] = it;
		ids[count] = it->id;
		count++;
	}
	// Rozmieœæ przedmioty w plecaku
	for (int n = 0; n < count; ++n) {
		item* it = items[n];
		int placed = 0;
		for (int i = 0; i <= backpack_size_X - it->size_X && !placed; ++i) {
			for (int j = 0; j <= backpack_size_Y - it->size_Y && !placed; ++j) {
				int free = 1;
				for (int dx = 0; dx < it->size_X; ++dx)
					for (int dy = 0; dy < it->size_Y; ++dy)
						if (ch->backpack[i + dx][j + dy].id != 0) free = 0;
				if (free) {
					for (int dx = 0; dx < it->size_X; ++dx)
						for (int dy = 0; dy < it->size_Y; ++dy) {
							ch->backpack[i + dx][j + dy].id = it->id;
							ch->backpack[i + dx][j + dy].item = (dx == 0 && dy == 0) ? it : nullptr;
						}
					placed = 1;
				}
			}
		}
	}
}
champion* load_champion(FILE* f) {
	char buf[256];
	champion* ch = (champion*)malloc(sizeof(champion));
	fgets(buf, sizeof(buf), f);
	sscanf(buf, "%49[^,],%d,%d,%d,%d,%d,%d",
		ch->name, &ch->hp, &ch->xp, &ch->armor, &ch->stength, &ch->posX, &ch->posY);
	ch->backpack = (backpack**)malloc(backpack_size_X * sizeof(backpack*));
	for (int i = 0; i < backpack_size_X; ++i)
		ch->backpack[i] = (backpack*)malloc(backpack_size_Y * sizeof(backpack));
	load_equipment(f, ch);
	load_backpack(f, ch);
	fgets(buf, sizeof(buf), f); // END_CHAMPION
	return ch;
}
map* load_map(FILE* f, int* ENEMY_COUNT, int* liczba_skrytek, int* liczba_pulapek, int* liczba_przeszkod) {
	char buf[512];
	int width, height;
	fgets(buf, sizeof(buf), f);
	sscanf(buf, "%d,%d,%d,%d,%d,%d", &width, &height, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
	map* m = (map*)malloc(sizeof(map));
	m->width = width;
	m->height = height;

	// Przeciwnicy
	m->enemies = (enemy*)malloc((*ENEMY_COUNT) * sizeof(enemy));
	for (int i = 0; i < *ENEMY_COUNT; ++i) {
		fgets(buf, sizeof(buf), f);
		enemy* e = &m->enemies[i];
		sscanf(buf, "%31[^,],%d,%d,%d,%d,%d,%d,%d,%d",
			e->name, &e->hp, &e->max_hp, &e->dmg, &e->armor, &e->xp_reward, &e->pos_x, &e->pos_y, (int*)&e->alive);
	}
	fgets(buf, sizeof(buf), f); // END_ENEMIES

	// Pu³apki
	m->traps = (pulapka*)malloc((*liczba_pulapek) * sizeof(pulapka));
	for (int i = 0; i < *liczba_pulapek; ++i) {
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%d,%d,%d", &m->traps[i].trap_dmg, &m->traps[i].pos_x, &m->traps[i].pos_y);
	}
	fgets(buf, sizeof(buf), f); // END_TRAPS

	// Skrytki
	m->chests = (skrytka*)malloc((*liczba_skrytek) * sizeof(skrytka));
	for (int i = 0; i < *liczba_skrytek; ++i) {
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%d,%d,%d", &m->chests[i].chest_xp, &m->chests[i].pos_x, &m->chests[i].pos_y);
		m->chests[i].item = load_item(f);
	}
	fgets(buf, sizeof(buf), f); // END_CHESTS

	// Mapa znakowa
	m->map = (char**)malloc(height * sizeof(char*));
	for (int y = 0; y < height; ++y) {
		fgets(buf, sizeof(buf), f);
		m->map[y] = (char*)malloc(width + 1);
		strncpy(m->map[y], buf, width);
		m->map[y][width] = '\0';
	}
	fgets(buf, sizeof(buf), f); // END_MAP
	return m;
}
void load_game(const char* filename, champion** ch, map** m, int* ENEMY_COUNT, int* liczba_skrytek, int* liczba_pulapek, int* liczba_przeszkod) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("Nie mo¿na otworzyæ pliku do wczytania!\n");
		getch();
		return;
	}
	*ch = load_champion(f);
	*m = load_map(f, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
	fclose(f);
	printf("Gra zosta³a wczytana z pliku %s\n", filename);
	getch();
}
int main() {
	srand((unsigned int)time(NULL));
	setlocale(LC_ALL, "Polish");
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	//Ustawienia pocz¹tkowe
	const int map_width = 30;
	const int map_height = 20;
	int champion_type = 0;
	int ENEMY_COUNT = rand() % 4 + 7;           // 7-10 przeciwników
	int liczba_skrytek = rand() % 3 + 14;       // 3-25 skrytek
	int liczba_pulapek = rand() % 7 + 23;        // pu³apek
	int liczba_przeszkod = rand() % 24 + 53;    // 13-26 przeszkód
	int player_move_counter = 0;
	char input_name[50];
	champion* postac = nullptr;
	map* mapa = nullptr;
	start(champion_type, map_width, map_height);
	fgets(input_name, sizeof(input_name), stdin);
	size_t len = strlen(input_name);
	if (len > 0 && input_name[len - 1] == '\n') {
		input_name[len - 1] = '\0';
	}
	//Tworzenie postaci i generowanie pierwszej mapy
	postac = (champion*)malloc(sizeof(champion));
	create_champion(champion_type, postac, backpack_size_X, backpack_size_Y, input_name);
	generate_map_and_enemies(&mapa, map_width, map_height, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
	postac->posX = 0;
	postac->posY = 0;
	max_hp = postac->hp;
	//G³ówna pêtla gry
	bool running = true;
	while (running) {
		system("cls");
		//Wyœwietlanie mapy z postaci¹ i przeciwnikami
		for (int y = 0; y < map_height; ++y) {
			for (int x = 0; x < map_width; ++x) {
				if (x == postac->posX && y == postac->posY) {
					printf("P");
				}
				else {
					bool enemy_here = false;
					if (mapa && mapa->enemies) {
						for (int i = 0; i < ENEMY_COUNT; ++i) {
							if (mapa->enemies[i].alive && mapa->enemies[i].pos_x == x && mapa->enemies[i].pos_y == y) {
								printf("E");
								enemy_here = true;
								break;
							}
						}
					}
					if (!enemy_here) {
						printf("%c", mapa->map[y][x]);
					}
				}
			}
			printf("\n");
		}
		printf("Sterowanie: W/S/A/D, Q - wyjœcie, E - ekwipunek, P - plecak, Z - statystyki, G - zapis gry, L - Pomoc w raie utkniecia, M - wczytanie save\n");
		//Sprawdzenie, czy gracz znalaz³ skrytkê
		for (int i = 0; i < liczba_skrytek; ++i) {
			if (mapa->chests[i].pos_x == postac->posX && mapa->chests[i].pos_y == postac->posY && mapa->chests[i].item != nullptr) {
				found_item(postac, &mapa->chests[i].item);
				mapa->map[postac->posY][postac->posX] = '.';
				break;
			}
		}

		// Sprawdzenie, czy gracz wszed³ na pu³apkê
		for (int i = 0; i < liczba_pulapek; ++i) {
			if (mapa->traps[i].pos_x == postac->posX && mapa->traps[i].pos_y == postac->posY) {
				int dmg = mapa->traps[i].trap_dmg;
				printf("Wpad³eœ w pu³apkê! Otrzymujesz %d obra¿eñ!\n", dmg);
				postac->hp -= dmg;
				// Zu¿ycie zbroi po pu³apce
				if (postac->head) {
					postac->head->durability--;
					if (postac->head->durability <= 0) {
						printf("Twój he³m siê zniszczy³!\n");
						remove_equipped_item(postac, &postac->head);
					}
				}
				if (postac->chestplate) {
					postac->chestplate->durability--;
					if (postac->chestplate->durability <= 0) {
						printf("Twój napierœnik siê zniszczy³!\n");
						remove_equipped_item(postac, &postac->chestplate);
					}
				}
				if (postac->left_leggins) {
					postac->left_leggins->durability--;
					if (postac->left_leggins->durability <= 0) {
						printf("Twoje lewe nagolenniki siê zniszczy³y!\n");
						remove_equipped_item(postac, &postac->left_leggins);
					}
				}
				if (postac->right_leggins) {
					postac->right_leggins->durability--;
					if (postac->right_leggins->durability <= 0) {
						printf("Twoje prawe nagolenniki siê zniszczy³y!\n");
						remove_equipped_item(postac, &postac->right_leggins);
					}
				}
				if (postac->left_boot) {
					postac->left_boot->durability--;
					if (postac->left_boot->durability <= 0) {
						printf("Twój lewy but siê zniszczy³!\n");
						remove_equipped_item(postac, &postac->left_boot);
					}
				}
				if (postac->right_boot) {
					postac->right_boot->durability--;
					if (postac->right_boot->durability <= 0) {
						printf("Twój prawy but siê zniszczy³!\n");
						remove_equipped_item(postac, &postac->right_boot);
					}
				}
				if (postac->hp <= 0) {
					printf("Zgin¹³eœ od pu³apki!\nKoniec gry.\n");
					exit(0);
				}
				mapa->traps[i].pos_x = -1;
				mapa->traps[i].pos_y = -1;
				getch();
				break;
			}
		}

		//Walka z przeciwnikiem
		bool enemy_defeated = false;
		int defeated_index = -1;
		if (mapa && mapa->enemies) {
			for (int i = 0; i < ENEMY_COUNT; ++i) {
				if (mapa->enemies[i].alive && mapa->enemies[i].pos_x == postac->posX && mapa->enemies[i].pos_y == postac->posY) {
					fight(postac, &mapa->enemies[i], postac->hp);
					mapa->map[mapa->enemies[i].pos_y][mapa->enemies[i].pos_x] = '.';
					enemy_defeated = true;
					defeated_index = i;
					break;
				}
			}
		}

		//Wzmocnienie pozosta³ych przeciwników po pokonaniu jednego
		if (enemy_defeated) {
			for (int i = 0; i < ENEMY_COUNT; ++i) {
				if (i != defeated_index && mapa->enemies[i].alive) {
					mapa->enemies[i].hp += 5;
					mapa->enemies[i].max_hp += 6;
					mapa->enemies[i].dmg += 3;
					mapa->enemies[i].armor += 2;
					mapa->enemies[i].xp_reward += 9;
				}
			}
		}

		//Sprawdzenie, czy wszyscy przeciwnicy zostali pokonani
		bool all_defeated = true;
		if (mapa && mapa->enemies) {
			for (int i = 0; i < ENEMY_COUNT; ++i) {
				if (mapa->enemies[i].alive) {
					all_defeated = false;
					break;
				}
			}
		}
		if (all_defeated) {
			printf("Pokona³eœ wszystkich przeciwników! Generujê now¹ mapê...\n");
			getch();
			ENEMY_COUNT = rand() % 4 + 7; // losuj now¹ liczbê przeciwników
			generate_map_and_enemies(&mapa, map_width, map_height, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
			postac->posX = 0;
			postac->posY = 0;
			continue;
		}

		//Obs³uga sterowania
		char ruch = getch();
		if (ruch == 'q' || ruch == 'Q') {
			running = false;
		}
		else if (ruch == 'w' || ruch == 'W') {
			if (postac->posY > 0 && mapa->map[postac->posY - 1][postac->posX] != '|') postac->posY--;
			player_move_counter++;
			if (player_move_counter % 2 == 0) {
				move_enemies_towards_player(mapa, postac, ENEMY_COUNT);
			}
		}
		else if (ruch == 's' || ruch == 'S') {
			if (postac->posY < map_height - 1 && mapa->map[postac->posY + 1][postac->posX] != '|') postac->posY++;
			player_move_counter++;
			if (player_move_counter % 2 == 0) {
				move_enemies_towards_player(mapa, postac, ENEMY_COUNT);
			}
		}
		else if (ruch == 'a' || ruch == 'A') {
			if (postac->posX > 0 && mapa->map[postac->posY][postac->posX - 1] != '|') postac->posX--;
			player_move_counter++;
			if (player_move_counter % 2 == 0) {
				move_enemies_towards_player(mapa, postac, ENEMY_COUNT);
			}
		}
		else if (ruch == 'd' || ruch == 'D') {
			if (postac->posX < map_width - 1 && mapa->map[postac->posY][postac->posX + 1] != '|') postac->posX++;
			player_move_counter++;
			if (player_move_counter % 2 == 0) {
				move_enemies_towards_player(mapa, postac, ENEMY_COUNT);
			}
		}
		else if (ruch == 'g' || ruch == 'G') {
			save_game("savegame.txt", postac, mapa, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
		}
		else if (ruch == 'l' || ruch == 'L') {
			printf("Generowanie mapy od nowa\n");
			getch();
			ENEMY_COUNT = rand() % 4 + 7; // losuj now¹ liczbê przeciwników
			generate_map_and_enemies(&mapa, map_width, map_height, ENEMY_COUNT, liczba_skrytek, liczba_pulapek, liczba_przeszkod);
			postac->posX = 0;
			postac->posY = 0;
			continue;
		}
		else if (ruch == 'm' || ruch == 'M') {
			load_game("savegame.txt", &postac, &mapa, &ENEMY_COUNT, &liczba_skrytek, &liczba_pulapek, &liczba_przeszkod);
			max_hp = postac->hp;
		}
		//Menu ekwipunku
		else if (ruch == 'e' || ruch == 'E') {
			while (true) {
				system("cls");
				show_equipment(postac);
				printf("\n1 - Przenieœ przedmiot do plecaka\n");
				printf("2 - Wróæ do gry\n");
				printf("Twój wybór: ");
				int wybor = 0;
				scanf("%d", &wybor);
				getchar();
				if (wybor == 1) {
					move_equipment_to_backpack(postac);
				}
				else {
					break;
				}
			}
		}
		//Menu plecaka
		else if (ruch == 'p' || ruch == 'P') {
			while (true) {
				system("cls");
				show_backpack(postac);
				printf("\n1 - Za³ó¿ przedmiot z plecaka\n");
				printf("2 - Usuñ przedmiot z plecaka\n");
				printf("3 - Porównaj przedmiot z ekwipunkiem\n");
				printf("4 - Wróæ do gry\n");
				printf("Twój wybór: ");
				int wybor = 0;
				scanf("%d", &wybor);
				getchar();
				if (wybor == 1) {
					move_backpack_to_equipment(postac);
				}
				else if (wybor == 2) {
					remove_item_from_backpack(postac);
				}
				else if (wybor == 3) {
					printf("Podaj ID przedmiotu do porównania: ");
					int id_do_porownania = 0;
					scanf("%d", &id_do_porownania);
					getchar();
					item* to_compare = nullptr;
					for (int i = 0; i < backpack_size_X; ++i) {
						for (int j = 0; j < backpack_size_Y; ++j) {
							if (postac->backpack[i][j].id == id_do_porownania && postac->backpack[i][j].item != nullptr) {
								to_compare = postac->backpack[i][j].item;
								break;
							}
						}
						if (to_compare) break;
					}
					if (!to_compare) {
						printf("Nie znaleziono przedmiotu o podanym ID!\n");
						getch();
					}
					else {
						compare_item_with_equipped(postac, to_compare);
					}
				}
				else {
					break;
				}
			}
		}
		//Wyœwietlanie statystyk
		else if (ruch == 'z' || ruch == 'Z') {
			system("cls");
			show_stats(postac);
			printf("\nNaciœnij dowolny klawisz, aby wróciæ do gry...");
			getch();
		}
	}

	if (mapa) {
		for (int i = 0; i < map_height; ++i) free(mapa->map[i]);
		free(mapa->map);
		for (int i = 0; i < liczba_skrytek; ++i)
			if (mapa->chests[i].item) free(mapa->chests[i].item);
		free(mapa->chests);
		free(mapa);
	}
	for (int i = 0; i < backpack_size_X; ++i) {
		free(postac->backpack[i]);
	}
	free(postac->backpack);
	free(postac);

	return 0;
}



