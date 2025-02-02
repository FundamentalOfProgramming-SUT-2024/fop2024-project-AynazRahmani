#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define regular 20
#define enchant 21
#define treasure 22

#define regular_room_color 20
#define enchant_room_color 21
#define treasure_room_color 22
#define error_color 1
#define main_color 2
#define green_color 3
#define top_users_color 7
#define yellow 4
#define blue 5
#define green 6
#define gold_color 23
#define spell_color 24
#define food_color 25
#define weapon_color 26
#define enemy_color 27
#define win_color 28
#define lose_color 29
#define pause_color 30

#define Deamon 40
#define Fire 41
#define Giant 42
#define Snake 43
#define Undeed 44 

#define mace 45
#define knife 46
#define wand 47
#define arrow 48
#define blade 49

int END = 0;

typedef struct location {
    int x;
    int y;
} location;

typedef struct ROOM {
    int shown;
    /*
    already shown -> 2
    must be shown -> 1
    hidden -> 0
    */
    int theme;
    location corner;
    int height;
    int width;
    int has_door;
    location door;
    location window;

    int pillar;
    location pillar_loc;

    int speed_spell;
    location speed_spell_loc;
    int health_spell;
    location health_spell_loc;
    int damage_spell;
    location damage_spell_loc;
    
    int gold;
    location gold_loc;
    int black_gold;
    location black_gold_loc;
    
    int mace_weapon;
    location mace_weapon_loc;
    int knife_weapon;
    location knife_weapon_loc;
    int wand_weapon;
    location wand_weapon_loc;
    int arrow_weapon;
    location arrow_weapon_loc;
    int blade_weapon;
    location blade_weapon_loc;
    
    int food;
    location food_loc;
    
    int staircase;
    location staircase_loc;
    
    int trap;
    location trap_loc;

    int end;
    location end_loc;

    int monster;
    location monster_loc;
} ROOM;

ROOM room[25];

typedef struct CORRIDER {
    int n; //number of pieces
    location piece[3000];
    int shown[1000]; //1 if it's shown. 0 if not.
} CORRIDER;

CORRIDER corrider[24];

typedef struct PLAYER {
    int id;

    location position;
    int color;
    int level;
    int health;
    int gold;
    int black_gold;

    int speed_spell;
    int health_spell;
    int damage_spell;

    int mace_weapon;
    int knife_weapon;
    int wand_weapon;
    int arrow_weapon;
    int blade_weapon;
    int current_weapon;

    int food;
    int magic_food;
    int fullness;

} PLAYER;

PLAYER player;

typedef struct USER {
    int rank;
    char username[100];
    int score;
    int gold;
    int number_of_games;
    int experience;
} USER;

typedef struct DEAMON {
    location position;
    int health;
    int move;
    int number_of_moves;
} DEAMON;

DEAMON deamon;

typedef struct FIRE {
    location position;
    int health;
    int move;
    int number_of_moves;
} FIRE;

FIRE fire;

typedef struct GIANT {
    location position;
    int health;
    int move;
} GIANT;

GIANT giant;

typedef struct SNAKE {
    location position;
    int health;
    int move;
} SNAKE;

SNAKE snake;

typedef struct UNDEED {
    location position;
    int health;
    int move;
} UNDEED;

UNDEED undeed;

//menu functions: /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void play_music (const char* filename);
const char *lorestani = "lorestani.mp3";
const char *andy = "andy.mp3";
const char *birthday = "birthday.mp3";
const char *sandy = "sandy.mp3";
const char *mokhtar = "mokhtar.mp3";
int default_music = 1;
void number_of_users();
int NUMBER_OF_USERS;
void border_make ();
void ROGUE ();
void welcome_page ();
void signup_page ();
void make_user_file (int user_id, char* username);
int email_validation (char *email, int len);
void random_password (char *str);
void signin_page ();
void new_pass ();
void guest_mode ();
void pre_game ();
void game ();
void scoreboard ();
void settings ();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// game functions: ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mapfloor1();
void featuresfloor1();
void mapfloor2();
void featuresfloor2();
void mapfloor3();
void featuresfloor3();
void mapfloor4();
void featuresfloor4();
void mapfloor5();
void featuresfloor5();

void input_handle (int ch);
void room_maker (location loc, int doorside, int windowside, int room_number);
/*
doorside = 0      up
doorside = 1      right
doorside = 2      down
doorside = 3      left
*/
void copy_room (int i_copy, int i);
void print_room (ROOM room);
void corrider_maker (int i);
void print_corrider (int i);
location random_location_maker (int section);
/*
1  2  3
0  5  4
*/
void input_handle (int input);
void player_move (location new_position);
int current_room ();
int is_in_room (int i); //returns 1 if player is inside room[i]. returns 2 if it's on the door. else -> returns 0.
int current_section (); //returns the number of player's current section
int is_going_to(); //returns -1 if player is in a room and returns a room number if the player is going toward it.
int is_inside(); //returns 1 if the player is in a room and returns 0 if else.
int is_on_door(); //returns 1 if the player is on a door. else -> 0;
int is_trap (location loc);

void game ();
void the_end ();
void change_info (int id, int gold, int number_of_games, int number_of_wins);
void save_game ();
void right_box ();
void draw_box ();
void message_box (char* message);
void info_box ();
void weapon_box ();
void spell_box ();
void food_box ();
void fullness_box ();
void fullness_bar (int f);
void hunger_health_handle ();
// void jangoolak ();

void show_map (int level);
void print_room_in_window (ROOM room, WINDOW* window);
void print_corrider_in_window (int i, WINDOW* window);

void deamon_move ();
void fire_move ();
void giant_move ();
void snake_move ();
void undeed_move ();
int is_point (int y, int x);

void weapon_choose ();
int weapon_blink;
int spell_blink;
int food_blink;
void weapon_use ();
void spell_use ();
int fast_speed;
void food_use ();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main () {
    setlocale (LC_ALL,"");
    initscr();
    keypad(stdscr, TRUE);
    
    //play_music (music);

    border_make ();
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(error_color, COLOR_RED, COLOR_BLACK);
    init_pair(main_color, COLOR_BLUE, COLOR_BLACK);
    init_pair(green_color, COLOR_GREEN, COLOR_BLACK);
    init_pair(top_users_color, COLOR_YELLOW, COLOR_BLACK);

    init_pair(treasure_room_color, COLOR_YELLOW, COLOR_BLACK);
    init_pair(regular_room_color, COLOR_CYAN, COLOR_BLACK);
    init_pair(enchant_room_color, COLOR_RED, COLOR_BLACK);

    init_pair(gold_color, COLOR_BLACK, COLOR_YELLOW);
    init_pair(spell_color, COLOR_BLACK, COLOR_GREEN);
    init_pair(food_color, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(weapon_color, COLOR_BLACK, COLOR_WHITE);
    init_pair(enemy_color, COLOR_BLACK, COLOR_RED);

    init_pair(yellow, COLOR_YELLOW, COLOR_BLACK);
    init_pair(blue, COLOR_BLUE, COLOR_BLACK);
    init_pair(green, COLOR_GREEN, COLOR_BLACK);

    init_pair(win_color, COLOR_BLACK, COLOR_GREEN);
    init_pair(lose_color, COLOR_BLACK, COLOR_RED);
    init_pair(pause_color, COLOR_BLACK, COLOR_CYAN);

    number_of_users();

    player.health = 5;
    player.mace_weapon = 1;
    player.gold = 0;
    player.color = blue;
    player.fullness = 10;
    deamon.health = 5;
    fire.health = 10;
    giant.health = 15;
    snake.health = 20;
    undeed.health = 30;
    player.current_weapon = mace;
    weapon_blink = 0;
    spell_blink = 0;
    food_blink = 0;
    deamon.move = 1;
    fire.move = 1;
    giant.move = 1;
    snake.move = 1;
    undeed.move = 1;
    fast_speed = 0;
    deamon.number_of_moves = 15;
    fire.number_of_moves = 15;
    player.magic_food = 1;
    for (int i = 0; i < 24; i++) {
        room[i].has_door = 1;
    }
    room[24].has_door = 0;

    while (true) {
        attron (COLOR_PAIR(main_color));
        ROGUE();
        attroff (COLOR_PAIR(main_color));
        break;
    }

    refresh();
    endwin ();
    return 0;

}

void play_music (const char* filename) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return;
    }
    if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Quit ();
        return;
    }
    Mix_Music *music = Mix_LoadMUS (filename);
    if (!music) {
        Mix_CloseAudio ();
        SDL_Quit ();
        return;
    }
    Mix_PlayMusic (music, -1);
}

void number_of_users() {
    FILE* number;
    number = fopen ("number_of_users.txt", "r");
    char str[5];
    fgets (str, 5, number);
    NUMBER_OF_USERS = str[0] - '0';
    // str[strlen(str) - 1] = '\0';
    // if (strlen (str) == 1) 
    //     NUMBER_OF_USERS = (str[0] - '0');
    // else if (strlen(str) == 2) 
    //     NUMBER_OF_USERS = ((str[0] - '0') * 10 + (str[1] - '0'));
    // else if (strlen(str) == 3)
    //     NUMBER_OF_USERS = ((str[0] - '0') * 100 + (str[1] - '0') * 10 + (str[2] - '0'));
    // else if (strlen(str) == 4)
    //     NUMBER_OF_USERS = ((str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + (str[3] - '0'));
    fclose (number);
}

void border_make () {
    attron (COLOR_PAIR(main_color));
    int row, col;
    getmaxyx (stdscr, row, col);
    attron (A_STANDOUT);
    for (int i = 0; i <col; i++){
        mvprintw (0, i, " ");
        mvprintw (row - 1, i, " ");
    }
    for (int i = 0; i <row; i++){
        mvprintw (i, 0, " ");
        mvprintw (i, col - 1, " ");
    }
    attroff (A_STANDOUT);
    attroff (COLOR_PAIR(main_color));
}

void ROGUE () {
    clear ();
    border_make ();
    noecho ();
    curs_set(FALSE);
    const char rogue[10] = "ROGUE";
    attron (A_BOLD | A_STANDOUT | A_BLINK);
    mvprintw (LINES / 2 - 3, (COLS - strlen(rogue)) / 2, "ROGUE");
    attroff (A_BOLD | A_STANDOUT | A_BLINK);
    mvprintw (LINES / 2 + 1, (COLS - 26) / 2, "press any key to continue.");
    getch ();
    welcome_page ();
    refresh ();
    endwin ();
}

void welcome_page () {
    while (TRUE) {
        clear ();
        border_make ();
        noecho();
        curs_set(FALSE);
        const char wlc[20] = "WELCOME!";
        attron (A_STANDOUT | A_BOLD);
        mvprintw (LINES / 2 - 9, (COLS - strlen(wlc)) / 2, "WELCOME!");
        attroff (A_STANDOUT | A_BOLD);
        const char cmd[8][100] = {"1.Sign in", "in case you already have an acount", "2.Sign up", "in case you don't have an acount", "3.Guest mode", "your legendary game won't be saved in this case :(", "4.Exit", "go to previous page"};
        for (int i = 0; i < 8; i++) {
            if (i % 2 == 0) {
                attron (A_BOLD);
                mvprintw (LINES / 2 + ((3 * i) / 2) - 6, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
                attroff (A_BOLD);
            }
            else {
                mvprintw (LINES / 2 + ((3 * (i - 1)) / 2) - 5, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
            }
        }
        mvprintw (LINES / 2 + 8, (COLS - 31) / 2, "enter the corresponding number.");

        int c = getch ();
        
        switch (c) {
            case '1':
                signin_page ();
                break;
            case '2':
                signup_page ();
                break;
            case '3':
                guest_mode ();
                break;
            case '4':
                return;
                break;
        }
    }
    refresh();
    endwin ();    
}

void signup_page () {
    while (TRUE) {
        clear ();
        border_make ();
    
        curs_set(FALSE);
        noecho();
        attron (A_BOLD | A_STANDOUT);
        mvprintw (2, (COLS - 7) / 2, "Sign up");
        attroff (A_BOLD | A_STANDOUT);

        char rand_pass[9];
        random_password (rand_pass);
        move (LINES / 2, (COLS - 29) / 2);
        printw ("suggested password: %s", rand_pass);

        mvprintw (LINES / 2 - 5, (COLS - 9) / 2, "username:");
        mvprintw (LINES / 2 - 2, (COLS - 9) / 2, "password:");
        mvprintw (LINES / 2 + 2, (COLS - 14) / 2, "email address:");

        move (LINES / 2 - 4, (COLS) / 8);
        curs_set(TRUE);
        echo();
        char username[100];
        scanw ("%s", username);
        curs_set(FALSE);
        noecho();

        
        move (LINES / 2 - 1, (COLS) / 8);
        curs_set(TRUE);
        echo();
        char password[100];
        scanw ("%s", password);
        curs_set(FALSE);
        noecho();

        
        move (LINES / 2 + 3, (COLS) / 8);
        curs_set(TRUE);
        echo();
        char email[150];
        scanw("%s", email);
        curs_set(FALSE);
        noecho();

        mvprintw (LINES / 2 + 7, (COLS - 15) / 2, "press e to exit");
        mvprintw (LINES / 2 + 8, (COLS - 28) / 2, "press s to submit & continue");
        
        
        while (TRUE) {
            char ch = getch ();
            if (ch == 'e') {
                return;
            }
            else if (ch == 's') {
                int usercheck = 0, i = 1;
                FILE *f;
                f = fopen ("info.txt", "r");
                char str[100];
                while (fgets(str, 100, f)) {
                    int len = strlen(str);
                    str[len - 1] = '\0';
                    if (i == 1) {
                        if (strcmp(str, username) == 0) {
                            usercheck++; 
                            break;
                        }
                    }
                    i++;
                    i = i % 4;
                } 
                fclose(f);
                for (int i = 1; i < COLS; i++) {
                    mvprintw(1, i, " ");
                }

                border_make();
                int uper = 0, lower = 0, num = 0;
                for (int i = 0; i < strlen(password); i++) {
                    if (password[i] >= '0' && password[i] <= '9')
                        num++;
                    else if (password[i] >= 'a' && password[i] <= 'z')
                        lower++;
                    else if (password[i] >= 'A' && password[i] <= 'Z')
                        uper++;
                }
                if (usercheck != 0) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "this username already exists!");
                    attroff (COLOR_PAIR(error_color));
                    int lenuser = strlen (username);
                    for (int i = 0; i < lenuser; i++) {
                        username[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 4, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 4, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", username);
                    curs_set(FALSE);
                    noecho();
                }
                else if (strlen(password) < 7) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "password must contain at least 7 characters!");
                    attroff (COLOR_PAIR(error_color));
                    int lenpass = strlen (password);
                    for (int i = 0; i < lenpass; i++) {
                        password[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 1, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 1, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", password);
                    curs_set(FALSE);
                    noecho();
                }
                else if (num == 0) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "password must contain at least one number!");
                    attroff (COLOR_PAIR(error_color));
                    int lenpass = strlen (password);
                    for (int i = 0; i < lenpass; i++) {
                        password[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 1, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 1, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", password);
                    curs_set(FALSE);
                    noecho();
                }
                else if (uper == 0) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "password must contain at least one uppercase letter!");
                    attroff (COLOR_PAIR(error_color));
                    int lenpass = strlen (password);
                    for (int i = 0; i < lenpass; i++) {
                        password[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 1, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 1, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", password);
                    curs_set(FALSE);
                    noecho();
                }
                else if (lower == 0) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "password must contain at least one lowercase letter!");
                    attroff (COLOR_PAIR(error_color));
                    int lenpass = strlen (password);
                    for (int i = 0; i < lenpass; i++) {
                        password[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 1, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 1, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", password);
                    curs_set(FALSE);
                    noecho();
                }
                else if (email_validation(email, strlen(email)) == 0) {
                    attron (COLOR_PAIR(error_color));
                    mvprintw (1, 1, "invalid email address!");
                    attroff (COLOR_PAIR(error_color));
                    int lenemail = strlen (email);
                    for (int i = 0; i < lenemail; i++) {
                        email[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 + 3, i, " ");
                    }
                    border_make();
                    move (LINES / 2 + 3, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    // char email[150];
                    scanw("%s", email);
                    curs_set(FALSE);
                    noecho();
                } 
                else {
                    attron (COLOR_PAIR(green_color));
                    mvprintw (1, 1, "submitted successfully!");
                    attroff (COLOR_PAIR(green_color));
                    NUMBER_OF_USERS++;
                    FILE *fptr;
                    fptr = fopen ("info.txt", "a");
                    fprintf (fptr, "%s\n%d\n%s\n%s\n", username, NUMBER_OF_USERS, password, email);
                    fclose(fptr);
                    player.id = NUMBER_OF_USERS;
                    FILE* file;
                    file = fopen ("number_of_users.txt", "w");
                    fprintf (file, "%d", NUMBER_OF_USERS);
                    fclose (file);

                    make_user_file (NUMBER_OF_USERS, username);

                    pre_game ();
                    break;
                    
                }
            }
        } 
    }
}

void make_user_file (int user_id, char* username) {
    // char file_name[10] = "x.txt";
    // file_name[0] = '0' + user_id;
    FILE* file;
    // file = fopen (file_name, "w");
    if (user_id == 1) {
        file = fopen ("1.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 2) {
        file = fopen ("2.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 3) {
        file = fopen ("3.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 4) {
        file = fopen ("4.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 5) {
        file = fopen ("5.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 6) {
        file = fopen ("6.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 7) {
        file = fopen ("7.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 8) {
        file = fopen ("8.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 9) {
        file = fopen ("9.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 10) {
        file = fopen ("10.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 11) {
        file = fopen ("11.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    else if (user_id == 12) {
        file = fopen ("12.txt", "w");
        fprintf (file, "%s\n%d\n%d\n%d\n", username, 0, 0, 0);
    }
    fclose (file);
}

int email_validation(char *email, int length) {
    if (length < 5) 
        return 0;

    const char *at = strchr(email, '@');
    const char *dot;
    
    if (at == NULL || at == email || at == email + length - 1)
        return 0;

    dot = strchr(at + 1, '.');
    if (dot == NULL || dot == at + 1 || dot == email + length - 1)
        return 0;

    for (const char *p = email; p < at; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-') {
            return 0;
        }
    }
    for (const char *p = at + 1; p < email + length; p++) {
        if (!isalnum(*p) && *p != '.') {
            return 0;
        }
    }
    return 1;
}

void random_password (char *str) {
    srand(time(NULL));
    const char lower[] = "abcdefghijklmnopqrstuvwxyz";
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digit[] = "0123456789";
    for (int i = 0 ; i < 3; i++) 
        str[i] = upper[((rand() % 26) + 26) % 26];
    for (int i = 3 ; i < 6; i++) 
        str[i] = lower[((rand() % 26) + 26) % 26];
    for (int i = 6 ; i < 9; i++) 
        str[i] = digit[((rand() % 10) + 10) % 10];
    for (int i = 8; i > 0; i--) {
        int j = rand () % (i + 1);
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
    str[9] = '\0';
}


void signin_page () {
    while (TRUE) {
        clear ();
        border_make ();
    
        curs_set(FALSE);
        noecho();
        attron (A_BOLD | A_STANDOUT);
        mvprintw (2, (COLS - 7) / 2, "Sign in");
        attroff (A_BOLD | A_STANDOUT);

        mvprintw (LINES / 2 - 5, (COLS - 9) / 2, "username:");
        move (LINES / 2 - 4, (COLS) / 8);
        curs_set(TRUE);
        echo();
        char username[100];
        scanw ("%s", username);
        curs_set(FALSE);
        noecho();

        int usercheck = 0, line_index = 0, id = 0;
        while (usercheck == 0) {
            FILE *f;
            f = fopen ("info.txt", "r");
            char str[100], id_str[10];
            line_index = 0;
            while (fgets(str, 100, f)) {
                int len = strlen(str);
                str[len - 1] = '\0';
                if ((line_index + 1) % 3 == 1) {
                    if (strcmp(str, username) == 0) {
                        usercheck++; 
                        fgets (id_str, 10, f);
                        id = id_str[0] - '0';
                        break;
                    }
                }
                line_index++;
            } 
            fclose(f);
            if (usercheck == 0) {
                attron (COLOR_PAIR(error_color));
                mvprintw (1, 1, "this username doesn't exist!");
                mvprintw (2, 1, "press e to exit.");
                mvprintw (3, 1, "press t to try again.");
                attroff (COLOR_PAIR(error_color));
                char c = getch();
                if (c == 'e') {
                    return;
                }
                else if (c == 't'){
                    int lenuser = strlen (username);
                    for (int i = 0; i < lenuser; i++) {
                        username[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 4, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 4, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", username);
                    curs_set(FALSE);
                    noecho();
                }
            }
        }
        if (usercheck > 0) {
            mvprintw (1, 1, "                            ");
            mvprintw (2, 1, "                ");
            mvprintw (3, 1, "                     ");
        }

        mvprintw (LINES / 2 - 2, (COLS - 9) / 2, "password:");
        move (LINES / 2 - 1, (COLS) / 8);
        curs_set(TRUE);
        echo();
        char password[100];
        scanw ("%s", password);
        curs_set(FALSE);
        noecho();


        int passcheck = 0;
        while (passcheck == 0) {
            FILE *f;
            f = fopen("info.txt", "r");
            char str[100];
            for (int i = 0; i < line_index + 3; i++) {
                fgets(str, 100, f);
            }
            int len = strlen(str);
            str[len - 1] = '\0';
            if (strcmp (str, password) == 0) {passcheck++;}
            fclose(f);

            if (passcheck == 0) {
                attron (COLOR_PAIR(error_color));
                mvprintw (1, 1, "the password is incorrect!");
                mvprintw (2, 1, "press e to exit.");
                mvprintw (3, 1, "press t to try again.");
                // mvprintw (LINES / 2, (COLS - 16) / 2, "forgot password?");
                // mvprintw (LINES / 2 + 1, (COLS - 28) / 2, "press f to choose a new one!");
                attroff (COLOR_PAIR(error_color));
                char c = getch();
                if (c == 'e') {
                    return;
                }
                // else if (c == 'f') {
                //     new_pass ();
                // }
                else if (c == 't') {
                    int lenpass = strlen (password);
                    for (int i = 0; i < lenpass; i++) {
                        password[i] = '\0';
                    }
                    for (int i = 1; i < COLS; i++) {
                        mvprintw(LINES / 2 - 1, i, " ");
                    }
                    border_make();
                    move (LINES / 2 - 1, (COLS) / 8);
                    curs_set(TRUE);
                    echo();
                    scanw ("%s", password);
                    curs_set(FALSE);
                    noecho();
                }
            }
        }
        if (passcheck > 0) {
            player.id = id;
            pre_game ();
        }
    }
}

void guest_mode () {
    player.id = 0;
    game ();
}

void pre_game () {
    while (TRUE) {
        clear ();
        border_make ();
        noecho();
        curs_set(FALSE);
        const char wlc[20] = "Pre-game menu";
        attron (A_STANDOUT | A_BOLD);
        mvprintw (LINES / 2 - 9, (COLS - strlen(wlc)) / 2, "Pre-game menu");
        attroff (A_STANDOUT | A_BOLD);
        const char cmd[10][100] = {"1.New game", "", "2.Continue the previous game", "in case you have an unfinished game", "3.Scoreboard", "compare yourself with other players","4.Settings", "color, music, etc.", "5.Exit", "go to previous page"};
        for (int i = 0; i < 10; i++) {
            if (i % 2 == 0) {
                attron (A_BOLD);
                mvprintw (LINES / 2 + ((3 * i) / 2) - 6, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
                attroff (A_BOLD);
            }
            else {
                mvprintw (LINES / 2 + ((3 * (i - 1)) / 2) - 5, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
            }
        }
        mvprintw (LINES / 2 + 9, (COLS - 31) / 2, "enter the corresponding number.");

        int c = getch ();
        
        switch (c) {
            case '1':
                game();
                break;
            case '2':
                game();
                break;
            case '3':
                scoreboard ();
                break;
            case '4':
                settings ();
                break;
            case '5':
                return;
                break;
        }
    }
    refresh();
    endwin ();
}

void scoreboard () {
    
    while (TRUE) {
        clear();
        border_make();
        mvprintw(3, (COLS - 10) / 2, "scoreboard");
        mvprintw(30, (COLS - 16) / 2, "press e to exit.");
        char username[NUMBER_OF_USERS + 1];
        int sum_gold[NUMBER_OF_USERS + 1];
        int experience[NUMBER_OF_USERS + 1];


        for (int j = 0; j <= NUMBER_OF_USERS + 1; j++) {
            for (int i = COLS / 8; i <= 7 * COLS / 8; i++) 
                mvprintw (7 + 2 * j, i, "_");
        }
        for (int j = 0; j <= NUMBER_OF_USERS; j++) {
            mvprintw (8 + 2 * j, COLS / 8 + COLS / 15 + 3, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 2 * COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 3 * COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 4 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, COLS / 8 + COLS / 15 + 3, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 2 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 3 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 4 * COLS / 15, "|");
            
            mvprintw (10, COLS / 8 + COLS / 15 - 2, "\U0001f947");
            mvprintw (12, COLS / 8 + COLS / 15 - 2, "\U0001f948");
            mvprintw (14, COLS / 8 + COLS / 15 - 2, "\U0001f949");
            mvprintw (10, COLS / 8 + COLS / 15 - 10, "GOATest");
            mvprintw (12, COLS / 8 + COLS / 15 - 10, "GOATer");
            mvprintw (14, COLS / 8 + COLS / 15 - 10, "GOAT");

            mvprintw (8, COLS / 8 + COLS / 15 - 2, "RANK");
            mvprintw (8, COLS / 8 + COLS / 15 + 5, "USERNAME");
            mvprintw (8, 7 * COLS / 8 - COLS / 15 + 1, "EXPERIENCE");
            mvprintw (8, 7 * COLS / 8 - 2 * COLS / 15 + 3, "GAMES");
            mvprintw (8, 7 * COLS / 8 - 3 * COLS / 15 + 3, "COINS");
            mvprintw (8, 7 * COLS / 8 - 4 * COLS / 15 + 3, "SCORE");

            border_make();
        }

        USER user[NUMBER_OF_USERS + 1];
        char files[20][20] = {"0.txt" ,"1.txt", "2.txt", "3.txt", "4.txt", "5.txt", "6.txt", "7.txt", "8.txt", "9.txt", "10.txt", "11.txt", "12.txt"};
        for (int i = 1; i <= NUMBER_OF_USERS; i++) {
            FILE* f;
            f = fopen(files[i], "r");
            fgets (user[i].username, 100, f);
            char gol[10], exp[10], num[10];
            fgets (gol, 10, f);
            fgets (num, 10, f);
            fgets (exp, 10, f);
            user[i].gold = atol(gol);
            user[i].experience = atol(exp);
            user[i].number_of_games = atol(num);
            user[i].score = user[i].gold + user[i].experience;
            user[i].rank = i;
            fclose (f);
        }
        for (int i = 1; i <= NUMBER_OF_USERS; i++) {
            for (int j = 1; j < i; j++) {
                if (user[i].score > user[j].score && user[i].rank > user[j].rank) {
                    int temp = user[j + 1].rank;
                    user[j + 1].rank = user[j].rank;
                    user[j].rank = temp;
                }
            }
        }
        int i = 1;
        while (i <= NUMBER_OF_USERS) {
            for (int j = 1; j <= NUMBER_OF_USERS; j++) {
                if (user[j].rank == i) {
                    if (j == player.id)
                        attron (A_BOLD | COLOR_PAIR(green_color));
            
                    if (i >= 1 && i <= 3)
                        attron (COLOR_PAIR(top_users_color));
                    mvprintw (8 + 2 * i, COLS / 8 + COLS / 15 + 5, "%s", user[j].username);
                    if (i >= 1 && i <= 3)
                        attroff (COLOR_PAIR(top_users_color));

                    mvprintw (8 + 2 * i, COLS / 8 + COLS / 15, "%d", i);
                    mvprintw (8 + 2 * i, 7 * COLS / 8 - 4 * COLS / 15 + 2, "%d", user[j].score);
                    mvprintw (8 + 2 * i, 7 * COLS / 8 - 3 * COLS / 15 + 2, "%d", user[j].gold);
                    mvprintw (8 + 2 * i, 7 * COLS / 8 - 2 * COLS / 15 + 2, "%d", user[j].number_of_games);
                    mvprintw (8 + 2 * i, 7 * COLS / 8 - COLS / 15 + 2, "%d", user[j].experience);

                    if (j == player.id)
                        attroff (A_BOLD | COLOR_PAIR(green_color));
                    
                }
            }
            border_make();
            i++;
        }


        for (int j = 0; j <= NUMBER_OF_USERS; j++) {
            mvprintw (8 + 2 * j, COLS / 8 + COLS / 15 + 3, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 2 * COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 3 * COLS / 15, "|");
            mvprintw (8 + 2 * j, 7 * COLS / 8 - 4 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, COLS / 8 + COLS / 15 + 3, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 2 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 3 * COLS / 15, "|");
            mvprintw (8 + 2 * j + 1, 7 * COLS / 8 - 4 * COLS / 15, "|");
            
            mvprintw (10, COLS / 8 + COLS / 15 - 2, "\U0001f947");
            mvprintw (12, COLS / 8 + COLS / 15 - 2, "\U0001f948");
            mvprintw (14, COLS / 8 + COLS / 15 - 2, "\U0001f949");
            mvprintw (10, COLS / 8 + COLS / 15 - 10, "GOATest");
            mvprintw (12, COLS / 8 + COLS / 15 - 10, "GOATer");
            mvprintw (14, COLS / 8 + COLS / 15 - 10, "GOAT");

            mvprintw (8, COLS / 8 + COLS / 15 - 2, "RANK");
            mvprintw (8, COLS / 8 + COLS / 15 + 5, "USERNAME");
            mvprintw (8, 7 * COLS / 8 - COLS / 15 + 1, "EXPERIENCE");
            mvprintw (8, 7 * COLS / 8 - 2 * COLS / 15 + 3, "GAMES");
            mvprintw (8, 7 * COLS / 8 - 3 * COLS / 15 + 3, "COINS");
            mvprintw (8, 7 * COLS / 8 - 4 * COLS / 15 + 3, "SCORE");

            border_make();
        }

        int c = getch();
        if (c == 'e')
            break;
    }
    //endwin ();
    return;
}

void settings () {

    while (TRUE) {
        clear ();
        border_make ();
        noecho();
        curs_set(FALSE);
        const char wlc[20] = "Settings";
        attron (A_STANDOUT | A_BOLD);
        mvprintw (LINES / 2 - 9, (COLS - strlen(wlc)) / 2, "Settings");
        attroff (A_STANDOUT | A_BOLD);
        const char cmd[10][200] = {"difficulty: ", "easy   1   2   3   hard", "color of the hero: ", "1.green  2.blue  3.yellow", "music: ", "0.OFF   1.Andy   2.Sandy   3.Tavalod   4.Mohsen Lorestani   5.Mokhtar"};
        for (int i = 0; i < 6; i++) {
            if (i % 2 == 0) {
                attron (A_BOLD);
                mvprintw (LINES / 2 + ((3 * i) / 2) - 6, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
                attroff (A_BOLD);
            }
            else {
                mvprintw (LINES / 2 + ((3 * (i - 1)) / 2) - 5, (COLS - strlen(cmd[i])) / 2, "%s", cmd[i]);
            }
        }

        int dif = 0;
        char diff[10];
        while (dif == 0) {
            mvprintw (LINES / 2 - 6, (COLS + strlen(cmd[0])) / 2, "                  ");
            for (int i = 0; i < 10; i++)
                diff[i] = '\0';
            echo();
            curs_set(TRUE);
            mvscanw (LINES / 2 - 6, (COLS + strlen(cmd[0])) / 2, "%s", diff);
            diff[1] = '\0';
            if (strcmp(diff, "1") == 0) {
                mvprintw (1, 1, "               ");
                // difficulty = 1;
                player.health = 5;
                player.fullness = 10;
                player.magic_food = 1;
                dif++;
            }
            else if (strcmp(diff, "2") == 0) {
                mvprintw (1, 1, "               ");
                // difficulty = 2;
                player.health = 4;
                player.fullness = 8;
                player.magic_food = 1;
                dif++;
            }
            else if (strcmp(diff, "3") == 0) {
                mvprintw (1, 1, "               ");
                // difficulty = 3;
                player.health = 3;
                player.fullness = 6;
                player.magic_food = 0;
                dif++;
            }
            else {
                noecho();
                curs_set(FALSE);
                attron (COLOR_PAIR(error_color));
                mvprintw (1, 1, "invalid number!");
                attroff (COLOR_PAIR(error_color));
            }
        }

        int col = 0;
        char colo[10];
        while (col == 0) {
            mvprintw (LINES / 2 - 3, (COLS + strlen(cmd[2])) / 2, "                  ");
            echo();
            curs_set(TRUE);
            mvscanw (LINES / 2 - 3, (COLS + strlen(cmd[2])) / 2, "%s", colo);
            colo[1] = '\0';
            if (strcmp(colo, "1") == 0) {
                mvprintw (1, 1, "               ");
                player.color = green;
                col++;
            }
            else if (strcmp(colo, "2") == 0) {
                mvprintw (1, 1, "               ");
                player.color = blue;
                col++;
            }
            else if (strcmp(colo, "3") == 0) {
                mvprintw (1, 1, "               ");
                player.color = yellow;
                col++;
            }
            else {
                noecho();
                curs_set(FALSE);
                attron (COLOR_PAIR(error_color));
                mvprintw (1, 1, "invalid number!");
                attroff (COLOR_PAIR(error_color));

            }
        }

        int mus = 0;
        char musi[10];
        while (mus == 0) {
            mvprintw (LINES / 2, (COLS + strlen(cmd[4])) / 2, "                  ");
            echo();
            curs_set(TRUE);
            mvscanw (LINES / 2, (COLS + strlen(cmd[4])) / 2, "%s", musi);
            musi[1] = '\0';

            if (strcmp(musi, "0") == 0) {
                mvprintw (1, 1, "               ");
                Mix_CloseAudio ();
                SDL_Quit ();
                mus++;
            }

            else if (strcmp(musi, "1") == 0) {
                mvprintw (1, 1, "               ");
                play_music (andy);
                mus++;
            }
            else if (strcmp(musi, "2") == 0) {
                mvprintw (1, 1, "               ");
                play_music (sandy);
                mus++;
            }
            else if (strcmp(musi, "3") == 0) {
                mvprintw (1, 1, "               ");
                play_music (birthday);
                mus++;
            }
            else if (strcmp(musi, "4") == 0) {
                mvprintw (1, 1, "               ");
                play_music (lorestani);
                mus++;
            }
            else if (strcmp(musi, "5") == 0) {
                mvprintw (1, 1, "               ");
                play_music (mokhtar);
                mus++;
            }
            else {
                noecho();
                curs_set(FALSE);
                attron (COLOR_PAIR(error_color));
                mvprintw (1, 1, "invalid number!");
                attroff (COLOR_PAIR(error_color));

            }
        }

        noecho();
        curs_set(0);
        attron (COLOR_PAIR (green_color));
        mvprintw (1, 1, "changes saved successfully!");
        attroff (COLOR_PAIR (green_color));
        mvprintw (LINES / 2 + 5, (COLS - 16) / 2, "press e to exit.");
        while (TRUE) {
            char cc = getch ();
            if (cc == 'e') {
                return;
            }
        }
        getch ();
        
    }
    refresh();
    endwin ();
}

//GAME STARTS //////////////////////////////////////////////////////////////////////////////////////////////////////////


void game () {
    srand (time(NULL));
    clear ();
    noecho ();
    curs_set (0);
    
    for (int i = 0; i < 24; i++) 
        room[i].shown = 0;
    
    player.level = 1;
    mapfloor1();

    right_box ();

    int ch;
    while ((ch = getch()) != 'p' && END == 0 && player.health > 0) {
        input_handle (ch);
        right_box ();
        hunger_health_handle ();
    }
    if (ch == 'p') {
        END = 2;
    }
    the_end ();
    END = 0;
    return;
}


//box:

void right_box () {
    draw_box ();
    info_box ();
    weapon_box ();
    spell_box ();
    food_box ();
    fullness_box ();
    // jangoolak ();

}

void draw_box () {
    attron (COLOR_PAIR (regular_room_color));
    for (int i = 0; i <= LINES; i++) 
        mvprintw (i, 3 * COLS / 4 - 1, "|");

    for (int i = 3 * COLS / 4; i <= COLS; i++)
        mvprintw (LINES / 2, i, "_");

    for (int i = 3 * COLS / 4; i <= COLS; i++)
        mvprintw (2, i, "_");

    for (int i = 3 * COLS / 4; i <= COLS; i++)
        mvprintw (5, i, "_");

    for (int i = 6; i <= LINES / 2 - 1; i++) 
        mvprintw (i, 3 * COLS / 4 + 12, "|");

    for (int i = 6; i <= LINES / 2 - 1; i++) 
        mvprintw (i, 3 * COLS / 4 + 26, "|");
    mvprintw (12, 3 * COLS / 4 + 27, "___________");
    attroff (COLOR_PAIR (regular_room_color));
}

void message_box(char* message) {
    for (int i = 3 * COLS / 4; i <= COLS; i++) {
        mvprintw (1, i, " ");
    }
    mvprintw (1, 3 * COLS / 4, "%s", message);
}

void info_box () {
    mvprintw (4, 3 * COLS / 4 + 2, "level: %d     health: %d     gold:%d", player.level, player.health, player.gold + 5 * player.black_gold);
}

void weapon_box () {
    if (weapon_blink == 1)
        {attron(A_BLINK);}
    attron (COLOR_PAIR (weapon_color));
    mvprintw (7, 3 * COLS / 4 + 2, "WEAPONS");
    attroff (COLOR_PAIR (weapon_color));
    if (weapon_blink == 1)
        {attroff(A_BLINK);}

    if (player.current_weapon == mace)
        attron (A_BOLD);
    mvprintw (10, 3 * COLS / 4, "1.mace: %d ", player.mace_weapon);
    if (player.current_weapon == mace)
        attroff (A_BOLD);

    if (player.current_weapon == knife)
        attron (A_BOLD);
    mvprintw (11, 3 * COLS / 4, "2.knife: %d ", player.knife_weapon);
    if (player.current_weapon == knife)
        attroff (A_BOLD);

    if (player.current_weapon == wand)
        attron (A_BOLD);
    mvprintw (12, 3 * COLS / 4, "3.wand: %d ", player.wand_weapon);
    if (player.current_weapon == wand)
        attroff (A_BOLD);

    if (player.current_weapon == arrow)
        attron (A_BOLD);
    mvprintw (13, 3 * COLS / 4, "4.arrow: %d ", player.arrow_weapon);
    if (player.current_weapon == arrow)
        attroff (A_BOLD);

    if (player.current_weapon == blade)
        attron (A_BOLD);
    mvprintw (14, 3 * COLS / 4, "5.blade: %d ", player.blade_weapon);
    if (player.current_weapon == blade)
        attroff (A_BOLD);
}
void spell_box () {
    if (spell_blink == 1)
        {attron(A_BLINK);}
    attron (COLOR_PAIR (spell_color));
    mvprintw (7, 3 * COLS / 4 + 16, "SPELLS");
    attroff (COLOR_PAIR (spell_color));
    if (spell_blink == 1)
        {attroff(A_BLINK);}
    mvprintw (10, 3 * COLS / 4 + 13, "6.health: %d", player.health_spell);
    mvprintw (11, 3 * COLS / 4 + 13, "7.speed: %d", player.speed_spell);
    mvprintw (12, 3 * COLS / 4 + 13, "8.damage: %d", player.damage_spell);
}
void food_box () {
    if (food_blink == 1)
        attron (A_BLINK);
    attron (COLOR_PAIR (food_color));
    mvprintw (7, 3 * COLS / 4 + 30, "FOOD");
    attroff (COLOR_PAIR (food_color));
    if (food_blink == 1)
        attroff (A_BLINK);
    mvprintw (10, 3 * COLS / 4 + 27, "9.food: %d", player.food);
    mvprintw (11, 3 * COLS / 4 + 27, "0.magic\u2728 %d", player.magic_food);
}
void fullness_box () {
    attron (COLOR_PAIR (food_color));
    mvprintw (14, 3 * COLS / 4 + 29, "fullness");
    attroff (COLOR_PAIR (food_color));
    fullness_bar (player.fullness);
}
void fullness_bar (int f) {
    attron (A_BOLD);
    if (f == 0)
        mvprintw (16, 3 * COLS / 4 + 28, "          ");
    else if (f == 1)
        mvprintw (16, 3 * COLS / 4 + 28, "*         ");
    else if (f == 2)
        mvprintw (16, 3 * COLS / 4 + 28, "**        ");
    else if (f == 3)
        mvprintw (16, 3 * COLS / 4 + 28, "***       ");
    else if (f == 4)
        mvprintw (16, 3 * COLS / 4 + 28, "****      ");
    else if (f == 5)
        mvprintw (16, 3 * COLS / 4 + 28, "*****     ");
    else if (f == 6)
        mvprintw (16, 3 * COLS / 4 + 28, "******    ");
    else if (f == 7)
        mvprintw (16, 3 * COLS / 4 + 28, "*******   ");
    else if (f == 8)
        mvprintw (16, 3 * COLS / 4 + 28, "********  ");
    else if (f == 9)
        mvprintw (16, 3 * COLS / 4 + 28, "********* ");
    else if (f == 10)
        mvprintw (16, 3 * COLS / 4 + 28, "**********");
    attroff (A_BOLD);
}
void hunger_health_handle () {
    if (player.fullness == 10 && player.health < 5) 
        player.health++;
    if (player.fullness == 0 && player.health > 0)
        player.health--;
}

// void jangoolak () {
//     mvprintw (LINES / 2 + 4, 3 * COLS / 4, "RRRRR    RRRRRRR RR   RR RRRRRRR RRRRRR");
//     mvprintw (LINES / 2 + 5, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR      RR    ");
//     mvprintw (LINES / 2 + 6, 3 * COLS / 4, "RR   RR  RR   RR RR   RR RR      RR    ");
//     mvprintw (LINES / 2 + 7, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR      RR    ");
//     mvprintw (LINES / 2 + 8, 3 * COLS / 4, "RR RR    RR   RR RR   RR RR      RR    ");
//     mvprintw (LINES / 2 + 9, 3 * COLS / 4, "RRR      RR   RR RR   RR RR RRRR RRRRRR");
//    mvprintw (LINES / 2 + 10, 3 * COLS / 4, "RR RR    RR   RR RR   RR RR   RR RR    ");
//    mvprintw (LINES / 2 + 11, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR   RR RR    ");
//    mvprintw (LINES / 2 + 12, 3 * COLS / 4, "RR   RR  RR   RR RR   RR RR   RR RR    ");
//    mvprintw (LINES / 2 + 13, 3 * COLS / 4, "RR    RR RRRRRRR RRRRRRR RRRRRRR RRRRRR");    
// }

//map:

void mapfloor1() {
    
    featuresfloor1 ();

    location loc0;
    loc0.x = COLS / 13;
    loc0.y = 7 * LINES / 12;

    player.position.x = loc0.x + 1;
    player.position.y = loc0.y + 1;

    room_maker (loc0, 1, 2, 0);
    room[0].shown = 2;
    print_room (room[0]);
    attron (COLOR_PAIR (player.color));
    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
    attroff (COLOR_PAIR (player.color));

    room_maker (random_location_maker(1), 1, 2, 1);
    room_maker (random_location_maker(2), 2, 0, 2);
    room_maker (random_location_maker(3), 3, 1, 3);
    room_maker (random_location_maker(4), 0, 3, 4);
    room_maker (random_location_maker(5), 1, 0, 5);

    corrider_maker (0);
    corrider_maker (1);
    corrider_maker (2);
    corrider_maker (3);
    corrider_maker (4);
}

void featuresfloor1 () {
    for (int i = 0; i < 6; i++) 
        room[i].theme = regular;

    room[0].pillar = 1;
    room[1].gold = 1;
    room[2].speed_spell = 1; 
    room[3].food = 1; 
    room[3].knife_weapon = 1;
    room[3].trap = 1;
    room[4].food = 1; 
    room[5].wand_weapon = 1;
    room[5].gold = 1;
    room[5].staircase = 1;
    room[5].monster = Deamon;
}

void mapfloor2() {
    clear();
    message_box (" second floor!");
    featuresfloor2 ();
    
    copy_room (6, 5);
    room[6].health_spell_loc.y = room[6].corner.y + 2 + (rand() % (room[6].height - 3));
    room[6].health_spell_loc.x = room[6].corner.x + 2 + (rand() % (room[6].width - 3));
    room[6].damage_spell_loc.y = room[6].corner.y + 2 + (rand() % (room[6].height - 3));
    room[6].damage_spell_loc.x = room[6].corner.x + 2 + (rand() % (room[6].width - 3));

    room_maker (random_location_maker(1), 1, 2, 10);
    room_maker (random_location_maker(2), 2, 0, 9);
    room_maker (random_location_maker(3), 3, 1, 8);
    room_maker (random_location_maker(4), 0, 3, 7);
    room_maker (random_location_maker(0), 1, 0, 11);

    room[6].shown = 2;
    print_room (room[6]);

    attron (COLOR_PAIR (player.color));
    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
    attroff (COLOR_PAIR (player.color));

    corrider_maker (6);
    corrider_maker (7);
    corrider_maker (8);
    corrider_maker (9);
    corrider_maker (10);
}

void featuresfloor2 () {
    room[6].theme = enchant;
    room[7].theme = regular;
    room[8].theme = regular;
    room[9].theme = regular;
    room[10].theme = regular;
    room[11].theme = regular;

    room[6].damage_spell = 1;
    room[6].health_spell = 1;
    room[7].food = 1;
    room[8].trap = 1;
    room[8].gold = 1;
    room[9].blade_weapon = 1;
    room[9].food = 1;
    room[10].food = 1;
    room[11].staircase = 1;
    room[11].black_gold = 1;
    room[11].gold = 1;
    room[11].monster = Fire;
}

void mapfloor3 () {
    clear ();
    message_box (" third floor!");
    featuresfloor3 ();
    copy_room (12, 11);

    room[12].health_spell_loc.y = room[12].corner.y + 2 + (rand() % (room[12].height - 3));
    room[12].health_spell_loc.x = room[12].corner.x + 2 + (rand() % (room[12].width - 3));
    room[12].damage_spell_loc.y = room[12].corner.y + 2 + (rand() % (room[12].height - 3));
    room[12].damage_spell_loc.x = room[12].corner.x + 2 + (rand() % (room[12].width - 3));

    room_maker (random_location_maker(1), 1, 2, 13);
    room_maker (random_location_maker(2), 2, 0, 14);
    room_maker (random_location_maker(3), 3, 1, 15);
    room_maker (random_location_maker(4), 0, 3, 16);
    room_maker (random_location_maker(5), 1, 0, 17);

    room[12].shown = 2;
    print_room (room[12]);

    attron (COLOR_PAIR (player.color));
    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
    attroff (COLOR_PAIR (player.color));

    corrider_maker (12);
    corrider_maker (13);
    corrider_maker (14);
    corrider_maker (15);
    corrider_maker (16);
}

void featuresfloor3 () {
    room[12].theme = enchant;
    room[13].theme = regular;
    room[14].theme = regular;
    room[15].theme = regular;
    room[16].theme = regular;
    room[17].theme = regular;

    room[12].speed_spell = 1;
    room[12].health_spell = 1;
    room[12].trap = 1;
    room[13].food = 1;
    room[14].trap = 1;
    room[14].food = 1;
    room[15].arrow_weapon = 1;
    room[16].food = 1;
    room[16].trap = 1;
    room[17].staircase = 1;
    room[17].gold = 1;
    room[17].food = 1;
    room[17].monster = Giant;
}

void mapfloor4 () {
    clear ();
    message_box (" fourth floor!");
    featuresfloor4 ();
    copy_room (18, 17);

    room[18].health_spell_loc.y = room[18].corner.y + 2 + (rand() % (room[18].height - 3));
    room[18].health_spell_loc.x = room[18].corner.x + 2 + (rand() % (room[18].width - 3));
    room[18].damage_spell_loc.y = room[18].corner.y + 2 + (rand() % (room[18].height - 3));
    room[18].damage_spell_loc.x = room[18].corner.x + 2 + (rand() % (room[18].width - 3));

    room_maker (random_location_maker(1), 1, 2, 22);
    room_maker (random_location_maker(2), 2, 0, 21);
    room_maker (random_location_maker(3), 3, 1, 20);
    room_maker (random_location_maker(4), 0, 3, 19);
    room_maker (random_location_maker(0), 1, 0, 23);

    room[18].shown = 2;
    print_room (room[18]);

    attron (COLOR_PAIR (player.color));
    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
    attroff (COLOR_PAIR (player.color));

    corrider_maker (18);
    corrider_maker (19);
    corrider_maker (20);
    corrider_maker (21);
    corrider_maker (22);
}

void featuresfloor4 () {
    room[18].theme = enchant;
    room[19].theme = regular;
    room[20].theme = treasure;
    room[21].theme = treasure;
    room[22].theme = regular;
    room[23].theme = treasure;

    room[18].speed_spell = 1;
    room[18].trap = 1;
    room[18].damage_spell = 1;
    room[19].food = 1;
    room[19].knife_weapon = 1;
    room[19].trap = 1;
    room[20].trap = 1;
    room[20].gold = 1;
    room[20].black_gold = 1;
    room[21].gold = 1;
    room[21].black_gold = 1;
    room[21].trap = 1;
    room[22].food = 1;
    room[23].gold = 1;
    room[23].black_gold = 1;
    room[23].trap = 1;
    room[23].staircase = 1;
    room[23].monster = Snake;
}

void mapfloor5 () {
    featuresfloor5 ();
    room[24].corner.x = 3 * COLS / 4 + 1;
    room[24].corner.y = LINES / 2 + 2;
    room[24].height = LINES / 2 - 4;
    room[24].width = COLS / 4 - 3;
    room_maker (room[24].corner, 0, 0, 24);
    room[24].shown = 2;
    print_room (room[24]);
    
    player.position.y = room[24].corner.y + 1;
    player.position.x = room[24].corner.x + 1;

    attron (COLOR_PAIR (player.color));
    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
    attroff (COLOR_PAIR (player.color));
}
void featuresfloor5 () {
    room[24].theme = treasure;
    room[24].trap = 1;
    room[24].gold = 1;
    room[24].black_gold = 1;
    room[24].monster = Undeed;
    room[24].end = 1;
}

//inputs:

void input_handle (int input) {
    location new_position;
    new_position.x = player.position.x;
    new_position.y = player.position.y;
    if (fast_speed == 0) {
        if (input == KEY_UP) {
            new_position.y = player.position.y - 1;
            player_move (new_position);
        }
        else if (input == KEY_DOWN) {
            new_position.y = player.position.y + 1;
            player_move (new_position);
        }
        else if (input == KEY_RIGHT) {
            new_position.x = player.position.x + 1;
            player_move (new_position);
        }
        else if (input == KEY_LEFT) {
            new_position.x = player.position.x - 1;
            player_move (new_position);
        }
        else if (input == 'n' || input == 'N') {
            new_position.x = player.position.x + 1;
            new_position.y = player.position.y + 1;
            player_move (new_position);
        }
        else if (input == 'u' || input == 'U') {
            new_position.x = player.position.x + 1;
            new_position.y = player.position.y - 1;
            player_move (new_position);
        }
        else if (input == 'y' || input == 'Y') {
            new_position.x = player.position.x - 1;
            new_position.y = player.position.y - 1;
            player_move (new_position);
        }
        else if (input == 'b' || input == 'B') {
            new_position.x = player.position.x - 1;
            new_position.y = player.position.y + 1;
            player_move (new_position);
        }
        else if (input == 'm' || input == 'M') {
            show_map (player.level);
        }
        else if (input == 'e' || input == 'E') {
            food_use ();
        }
        else if (input == 'w' || input == 'W') {
            weapon_choose ();
        }
        else if (input == ' ') {
            weapon_use ();
        }
        else if (input == 's' || input == 'S') {
            spell_use ();
        }
    }

    else if (fast_speed > 0) {
        if (input == KEY_UP) {
            new_position.y = player.position.y - 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == KEY_DOWN) {
            new_position.y = player.position.y + 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == KEY_RIGHT) {
            new_position.x = player.position.x + 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == KEY_LEFT) {
            new_position.x = player.position.x - 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == 'n' || input == 'N') {
            new_position.x = player.position.x + 2;
            new_position.y = player.position.y + 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == 'u' || input == 'U') {
            new_position.x = player.position.x + 2;
            new_position.y = player.position.y - 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == 'y' || input == 'Y') {
            new_position.x = player.position.x - 2;
            new_position.y = player.position.y - 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == 'b' || input == 'B') {
            new_position.x = player.position.x - 2;
            new_position.y = player.position.y + 2;
            player_move (new_position);
            fast_speed--;
        }
        else if (input == 'm' || input == 'M') {
            show_map (player.level);
        }
        else if (input == 'e' || input == 'E') {
            food_use ();
        }
        else if (input == 'w' || input == 'W') {
            weapon_choose ();
        }
        else if (input == ' ') {
            weapon_use ();
        }
        else if (input == 's' || input == 'S') {
            spell_use ();
        }
    }

}

void player_move (location new_position) {
    char ch = mvinch (new_position.y, new_position.x) & A_CHARTEXT;
    if (ch == '.') {
        if (is_inside()) {
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));
        }
        else if (is_on_door()) {
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, "+");
            attroff (COLOR_PAIR (room[current_room()].theme));
        }

        if (is_trap (player.position)) {
            attron (COLOR_PAIR (error_color));
            mvprintw (player.position.y, player.position.x, "!");
            attroff (COLOR_PAIR (error_color));
        }

        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
        if (is_trap (player.position)) {
            player.health--;
            message_box ("you just stepped on a trap!");
        }
        if (is_in_room (5)) {
            if (deamon.health > 0)
                deamon_move ();
            else if (room[5].monster != 0){
                room[5].monster = 0;
                attron (COLOR_PAIR (room[5].theme));
                mvprintw (deamon.position.y, deamon.position.x, ".");
                attroff (COLOR_PAIR (room[5].theme));
                message_box ("You defeated the Deamon!");
            }
        }
        else if (is_in_room (11)) {
            if (fire.health > 0)
                fire_move ();
            else if (room[11].monster != 0){
                room[11].monster = 0;
                attron (COLOR_PAIR (room[11].theme));
                mvprintw (fire.position.y, fire.position.x, ".");
                attroff (COLOR_PAIR (room[11].theme));
                message_box ("You defeated the Fire Monster!");
            }
        }
        else if (is_in_room (17)) {
            if (giant.health > 0)
                giant_move ();
            else if (room[17].monster != 0){
                room[17].monster = 0;
                attron (COLOR_PAIR (room[17].theme));
                mvprintw (giant.position.y, giant.position.x, ".");
                attroff (COLOR_PAIR (room[17].theme));
                message_box ("You defeated the Giant!");
            }
        }
        else if (is_in_room (23)) {
            if (snake.health > 0)
                snake_move ();
            else if (room[23].monster != 0){
                room[23].monster = 0;
                attron (COLOR_PAIR (room[23].theme));
                mvprintw (snake.position.y, snake.position.x, ".");
                attroff (COLOR_PAIR (room[23].theme));
                message_box ("You defeated the Snake!");
            }
        }
        else if (is_in_room (24)) {
            if (undeed.health > 0)
                undeed_move ();
            else if (room[24].monster != 0){
                room[24].monster = 0;
                attron (COLOR_PAIR (room[24].theme));
                mvprintw (undeed.position.y, undeed.position.x, ".");
                attroff (COLOR_PAIR (room[24].theme));
                message_box ("You defeated the Undead!");
            }
        }
    }
    else if (ch == '+') {
        if (is_inside()) {
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            if (current_room() != 5 && current_room() != 11 && current_room() != 17 && current_room() != 23) {
                corrider[current_room()].shown[0] = 1;
                corrider[current_room()].shown[1] = 1;
            }
            print_corrider(current_room());

            attroff (COLOR_PAIR (room[current_room()].theme));
        }
        else if (is_inside() == 0 && is_on_door() == 0) {
            attron (COLOR_PAIR (regular_room_color));
            mvprintw (player.position.y, player.position.x, "#");
            attroff (COLOR_PAIR (regular_room_color));
        }
        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
        
        room[current_room()].shown = 1;
    }
    else if (ch == '#') {
        if (is_on_door()) {
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, "+");
            attroff (COLOR_PAIR (room[current_room()].theme));
        }
        else if (is_inside() == 0 && is_on_door() == 0) {
            attron (COLOR_PAIR (regular_room_color));
            mvprintw (player.position.y, player.position.x, "#");
            attroff (COLOR_PAIR (regular_room_color));
        }
        if (is_going_to() != 0 && is_going_to() != 6 && is_going_to() != 12 && is_going_to() != 18) {
            for (int j = 0; j < corrider[is_going_to() - 1].n - 1; j++) {
                if (new_position.y == corrider[is_going_to() - 1].piece[j].y && new_position.x == corrider[is_going_to() - 1].piece[j].x) {
                    corrider[is_going_to() - 1].shown[j + 1] = 1;
                    corrider[is_going_to() - 1].shown[j + 2] = 1;
            
                    // attron (COLOR_PAIR(regular_room_color));
                    print_corrider (is_going_to() - 1);
                    // attroff (COLOR_PAIR(regular_room_color));

                    break;
                }
            }
        }
        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
    }

    else if (ch == ' ') {
        for (int i = 0; i < 24; i++) {
            if (room[i].shown == 0) {
                if (room[i].door.x == new_position.x && room[i].door.y == new_position.y) {
                    room[i].shown = 2;
                    print_room (room[i]);

                    attron (COLOR_PAIR (regular_room_color));
                    mvprintw (player.position.y, player.position.x, "#");
                    attroff (COLOR_PAIR (regular_room_color));

                    player.position.y = new_position.y;
                    player.position.x = new_position.x;

                    attron (COLOR_PAIR (player.color));
                    mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                    attroff (COLOR_PAIR (player.color));

                    break;
                }
            }
        }
    }

    else if (ch == 'c') {
        player.gold++;
        room[current_room()].gold --;
        attron (COLOR_PAIR (room[current_room()].theme));
        mvprintw (player.position.y, player.position.x, ".");
        attroff (COLOR_PAIR (room[current_room()].theme));

        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
        message_box (" gold coin ++ !");
    }

    else if (ch == 'o') {
        player.black_gold++;
        room[current_room()].black_gold --;
        attron (COLOR_PAIR (room[current_room()].theme));
        mvprintw (player.position.y, player.position.x, ".");
        attroff (COLOR_PAIR (room[current_room()].theme));

        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
        message_box (" black gold coin ++ !");
    }

    else if (ch == '>') {
        message_box ("if you wanna go to next floor, press x");
        int c = getch ();
        if (c == 'x'){
            player.level++;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));

            if (player.level == 2) {
                mapfloor2();
            }
            else if (player.level == 3){
                mapfloor3();
            }
            else if (player.level == 4){
                mapfloor4();
            }
            else if (player.level == 5) {
                mapfloor5 ();
            }

        }
    }

    else if (ch == 'f') {
        message_box ("if you wanna grab food, press x");
        int c = getch ();
        if (c == 'x') {
            player.food ++;
            room[current_room()].food --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" food ++ !");
        }
    }
    else if (ch == 'h') {
        message_box ("if you wanna grab health spell, press x");
        int c = getch ();
        if (c == 'x') {
            player.health_spell ++;
            room[current_room()].health_spell --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" health spell ++ !");
            player.fullness--;
        }
    }
    else if (ch == 's') {
        message_box ("if you wanna grab speed spell, press x");
        int c = getch ();
        if (c == 'x') {
            player.speed_spell ++;
            room[current_room()].speed_spell --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" speed spell ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'd') {
        message_box ("if you wanna grab damage spell, press x");
        int c = getch ();
        if (c == 'x'){
            player.damage_spell ++;
            room[current_room()].damage_spell --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" damage spell ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'm') {
        message_box ("if you wanna grab the mace, press x");
        int c = getch ();
        if (c == 'x') {
            player.mace_weapon ++;
            room[current_room()].mace_weapon --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" mace ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'k') {
        message_box ("if you wanna grab the knife, press x");
        int c = getch ();
        if (c == 'x') {
            if (room[current_room()].knife_weapon != 0){
                player.knife_weapon += 10;
                room[current_room()].knife_weapon --;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" knife += 10 !");
                player.fullness--;
            }
            else if (room[current_room()].knife_weapon == 0) {
                player.knife_weapon ++;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" knife ++ !");
            }
        }
    }
    else if (ch == 'w') {
        message_box ("if you wanna grab the wand, press x");
        int c = getch ();
        if (c == 'x') {
            if (room[current_room()].wand_weapon != 0){
                player.wand_weapon += 8;
                room[current_room()].wand_weapon --;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" wand += 8 !");
                player.fullness--;
            }
            else if (room[current_room()].wand_weapon == 0) {
                player.wand_weapon += 8;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" wand ++ !");
            }
        }
    }
    else if (ch == 'a') {
        message_box ("if you wanna grab the arrow, press x");
        int c = getch ();
        if (c == 'x') {
            if (room[current_room()].arrow_weapon != 0) {
                player.arrow_weapon += 20;
                room[current_room()].arrow_weapon --;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" arrow += 20 !");
                player.fullness--;
            }
            else if (room[current_room()].arrow_weapon == 0) {
                player.arrow_weapon ++;
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (player.position.y, player.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));

                player.position.y = new_position.y;
                player.position.x = new_position.x;

                attron (COLOR_PAIR (player.color));
                mvprintw (player.position.y, player.position.x, "\U0001fbc5");
                attroff (COLOR_PAIR (player.color));
                message_box (" arrow ++ !");
            }
            
        }
    }
    else if (ch == 'b') {
        message_box ("if you wanna grab the blade, press x");
        int c = getch ();
        if (c == 'x') {
            player.blade_weapon ++;
            room[current_room()].blade_weapon --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" blade ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'E') {
        message_box ("wanna finish the game? press x");
        int c = getch ();
        if (c == 'x') {
            END = 1;
        }
    }
}


//player_move functions:

int is_in_room (int i) {
    if (i >= 0 && i <=5 && player.level == 1) {
        if (player.position.x > room[i].corner.x && player.position.x < room[i].corner.x + room[i].width && player.position.y > room[i].corner.y && player.position.y < room[i].corner.y + room[i].height) {
            return 1;
        }
        else if (player.position.x == room[i].door.x && player.position.y == room[i].door.y) {
            return 2;
        }
    }
    else if (i >= 6 && i <= 11 && player.level == 2) {
        if (player.position.x > room[i].corner.x && player.position.x < room[i].corner.x + room[i].width && player.position.y > room[i].corner.y && player.position.y < room[i].corner.y + room[i].height) {
            return 1;
        }
        else if (player.position.x == room[i].door.x && player.position.y == room[i].door.y) {
            return 2;
        }
    }
    else if (i >= 12 && i <= 17 && player.level == 3) {
        if (player.position.x > room[i].corner.x && player.position.x < room[i].corner.x + room[i].width && player.position.y > room[i].corner.y && player.position.y < room[i].corner.y + room[i].height) {
            return 1;
        }
        else if (player.position.x == room[i].door.x && player.position.y == room[i].door.y) {
            return 2;
        }
    }
    else if (i >= 18 && i <= 23 && player.level == 4) {
        if (player.position.x > room[i].corner.x && player.position.x < room[i].corner.x + room[i].width && player.position.y > room[i].corner.y && player.position.y < room[i].corner.y + room[i].height) {
            return 1;
        }
        else if (player.position.x == room[i].door.x && player.position.y == room[i].door.y) {
            return 2;
        }
    }
    else if (i == 24 && player.level == 5) {
        if (player.position.x > room[i].corner.x && player.position.x < room[i].corner.x + room[i].width && player.position.y > room[i].corner.y && player.position.y < room[i].corner.y + room[i].height) {
            return 1;
        }
    }

    return 0;
}

int current_section () {   /*unnecessary*/
    if (player.position.x >= 0 && player.position.x <= COLS / 3 && player.position.y >= 0 && player.position.y <= LINES / 2) 
        return 1;
    else if (player.position.x >= COLS / 3 && player.position.x <= 2 * COLS / 3 && player.position.y >= 0 && player.position.y <= LINES / 2)
        return 2;
    else if (player.position.x >= 2 * COLS / 3 && player.position.x <= COLS && player.position.y >= 0 && player.position.y <= LINES / 2)
        return 3;
    if (player.position.x >= 0 && player.position.x <= COLS / 3 && player.position.y >= LINES / 2 && player.position.y <= LINES) 
        return 0;
    else if (player.position.x >= COLS / 3 && player.position.x <= 2 * COLS / 3 && player.position.y >= LINES / 2 && player.position.y <= LINES)
        return 5;
    else if (player.position.x >= 2 * COLS / 3 && player.position.x <= COLS && player.position.y >= LINES / 2 && player.position.y <= LINES)
        return 4;   
}

int current_room () {
    for (int i = 0; i < 25; i++) {
        if (is_in_room(i) >= 1)
            return i;
    }
    return -1;
}

int is_going_to() {
    if (is_inside() == 0 /*&& is_on_door() == 0*/) {
        for (int i = 1; i < 24; i++) {
            if (room[i].shown == 0)
                return i;
        }
    }
    return -1;
}

int is_inside() {
    for (int i = 0; i <= 24; i++) {
        if (is_in_room(i) == 1)
            return 1;
    }

    return 0;
}
int is_on_door() {
    for (int i = 0; i < 24; i++) {
        if (is_in_room(i) == 2)
            return 1;
    }

    return 0;
}

int is_trap (location loc) {
    if (room[current_room ()].trap == 1) {
        if (room[current_room()].trap_loc.x == loc.x && room[current_room()].trap_loc.y == loc.y)
            return 1;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void show_map (int level) {
    WINDOW* window;
    window = newwin (LINES, COLS, 0, 0);
    
    if (level == 1) {
        for (int i = 0; i <= 5; i++) 
            print_room_in_window (room[i], window);
        for (int i = 0; i <= 4; i++) 
            print_corrider_in_window (i, window);
    }
    else if (level == 2) {
        for (int i = 6; i <= 11; i++) 
            print_room_in_window (room[i], window);
        for (int i = 6; i <= 10; i++)
            print_corrider_in_window (i, window);
    }
    else if (level == 3) {
        for (int i = 12; i <= 17; i++) 
            print_room_in_window (room[i], window);
        for (int i = 12; i <= 16; i++)
            print_corrider_in_window (i, window);
    }
    else if (level == 4) {
        for (int i = 18; i <= 23; i++) 
            print_room_in_window (room[i], window);
        for (int i = 18; i <= 22; i++)
            print_corrider_in_window (i, window);
    }
    wrefresh(window);
    while (1) {
        int c = getch();
        if (c == 'M' || c == 'm') {
            END = 2;
            // return;
            message_box (" press any key to exit.");
            break;
        }
    }
    wclear (window);
    delwin (window);
}

void print_room_in_window (ROOM room, WINDOW* window) {
    if (room.theme == regular)
        attron (COLOR_PAIR (regular_room_color));
    else if (room.theme == enchant)
        attron (COLOR_PAIR (enchant_room_color));
    else if (room.theme == treasure)
        attron (COLOR_PAIR (treasure_room_color));

    for (int i = room.corner.x; i <= room.corner.x + room.width; i++){
        mvwprintw (window, room.corner.y, i, "-");
        mvwprintw (window, room.corner.y + room.height, i, "-");
    }
    for (int i = room.corner.y + 1; i < room.corner.y + room.height; i++) {
        mvwprintw (window, i, room.corner.x, "|");
        mvwprintw (window, i, room.corner.x + room.width, "|");
    }
    mvwprintw (window, room.door.y, room.door.x, "+");
    mvwprintw (window, room.window.y, room.window.x, "=");

    for (int i = room.corner.x + 1; i < room.corner.x + room.width; i++) {
        for (int j = room.corner.y + 1; j < room.corner.y + room.height; j++) {
            mvwprintw (window, j, i, ".");
        }
    }
    if (room.pillar == 1) 
        mvwprintw (window, room.pillar_loc.y, room.pillar_loc.x, "O");

    if (room.trap == 1) {
        attron (COLOR_PAIR (error_color));
        mvprintw (room.trap_loc.y, room.trap_loc.x, "!");
        attroff (COLOR_PAIR (error_color));
    }
    
    if (room.theme == regular)
        attroff (COLOR_PAIR (regular_room_color));
    else if (room.theme == enchant)
        attroff (COLOR_PAIR (enchant_room_color));
    else if (room.theme == treasure)
        attroff (COLOR_PAIR (treasure_room_color));

}

void print_corrider_in_window (int i, WINDOW* window) {
    for (int j = 0; j < corrider[i].n; j++) {
        mvwprintw (window, corrider[i].piece[j].y, corrider[i].piece[j].x, "#");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


location random_location_maker (int section) {
    location result;
    if (section == 1) {
        result.x = 1 + (rand() % (COLS / 12 - 1));
        result.y = 1 + (rand() % (LINES / 8 - 1));
    }
    else if (section == 2) {
        result.x = COLS / 4 + 1 + (rand() % (COLS / 12 - 1));
        result.y = 1 + (rand() % (LINES / 8 - 1));
    }
    else if (section == 3) {
        result.x = COLS / 2 + 1 + (rand() % (COLS / 12 - 1));
        result.y = 1 + (rand() % (LINES / 8 - 1));
    }
    else if (section == 0) {
        result.x = 1 + (rand() % (COLS / 12 - 1));
        result.y = LINES / 2 + 1 + (rand() % (LINES / 8 - 1));
    }
    else if (section == 5) {
        result.x = COLS / 4 + 1 + (rand() % (COLS / 12 - 1));
        result.y = LINES / 2 + 1 + (rand() % (LINES / 8 - 1));
    }
    else if (section == 4) {
        result.x = COLS / 2 + 1 + (rand() % (COLS / 12 - 1));
        result.y = LINES / 2 + 1 + (rand() % (LINES / 8 - 1));
    }
    return result;
}

void copy_room (int i_copy, int i) {
    room[i_copy].corner.x = room[i].corner.x;
    room[i_copy].corner.y = room[i].corner.y;
    room[i_copy].height = room[i].height;
    room[i_copy].width = room[i].width;
    room[i_copy].staircase = room[i].staircase;
    room[i_copy].door.x = room[i].door.x; 
    room[i_copy].door.y = room[i].door.y; 
    room[i_copy].window.x = room[i].window.x;
    room[i_copy].window.y = room[i].window.y;
}

void room_maker (location loc, int doorside, int windowside, int i) {
    room[i].corner.x = loc.x;
    room[i].corner.y = loc.y;
    if (i < 24) {
        room[i].height = (rand() % (LINES / 2 - (loc.y % (LINES / 2)) - 10)) + 8;
        room[i].width = (rand() % (COLS / 4 - (loc.x % (COLS / 4)) - 9)) + 7;
    }
    int height = room[i].height, width = room[i].width;
    //doorside
    if (doorside == 0) {
        room[i].door.x = loc.x + 1 + (rand() % (width - 2));
        room[i].door.y = loc.y;
    }
    else if (doorside == 2) {
        room[i].door.x = loc.x + 1 + (rand() % (width - 2));
        room[i].door.y = loc.y + height;
    }
    else if (doorside == 1) {
        room[i].door.x = loc.x + width;
        room[i].door.y = loc.y + 1 + (rand() % (height - 2));
    }
    else if (doorside == 3) {
        room[i].door.x = loc.x;
        room[i].door.y = loc.y + 1 + (rand() % (height - 2));
    }
    //windowside
    if (windowside == 0) {
        room[i].window.x = loc.x + 1 + (rand() % (width - 2));
        room[i].window.y = loc.y;
    }
    else if (windowside == 2) {
        room[i].window.x = loc.x + 1 + (rand() % (width - 2));
        room[i].window.y = loc.y + height;
    }
    else if (windowside == 1) {
        room[i].window.x = loc.x + width;
        room[i].window.y = loc.y + 1 + (rand() % (height - 2));
    }
    else if (windowside == 3) {
        room[i].window.x = loc.x;
        room[i].window.y = loc.y + 1 + (rand() % (height - 2));
    }
    //items
    if (room[i].pillar == 1) {
        room[i].pillar_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].pillar_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].gold == 1) {
        room[i].gold_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].gold_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].black_gold == 1) {
        room[i].black_gold_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].black_gold_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].speed_spell == 1) {
        room[i].speed_spell_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].speed_spell_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].health_spell == 1) {
        room[i].health_spell_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].health_spell_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].damage_spell == 1) {
        room[i].damage_spell_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].damage_spell_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].food == 1) {
        room[i].food_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].food_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].mace_weapon == 1) {
        room[i].mace_weapon_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].mace_weapon_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].knife_weapon == 1) {
        room[i].knife_weapon_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].knife_weapon_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].wand_weapon == 1) {
        room[i].wand_weapon_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].wand_weapon_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].arrow_weapon == 1) {
        room[i].arrow_weapon_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].arrow_weapon_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].blade_weapon == 1) {
        room[i].blade_weapon_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].blade_weapon_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }
    if (room[i].staircase == 1) {
        room[i].staircase_loc.y = room[i].corner.y + 1;
        room[i].staircase_loc.x = room[i].corner.x + 1;
    }
    if (room[i].end == 1) {
        room[i].end_loc.y = room[i].corner.y + room[i].height - 1;
        room[i].end_loc.x = room[i].corner.x + room[i].width - 1;
    }
    if (room[i].trap == 1) {
        room[i].trap_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].trap_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
    }

    if (room[i].monster == Deamon) {
        deamon.position.y = room[i].corner.y + room[i].height - 1;
        deamon.position.x = room[i].corner.x + 1;
    }
    if (room[i].monster == Fire) {
        fire.position.y = room[i].corner.y + room[i].height - 1;
        fire.position.x = room[i].corner.x + 1;
    }
    if (room[i].monster == Giant) {
        giant.position.y = room[i].corner.y + room[i].height - 1;
        giant.position.x = room[i].corner.x + 1;
    }
    if (room[i].monster == Snake) {
        snake.position.y = room[i].corner.y + room[i].height - 1;
        snake.position.x = room[i].corner.x + 1;
    }
    if (room[i].monster == Undeed) {
        undeed.position.y = room[i].corner.y + room[i].height - 1;
        undeed.position.x = room[i].corner.x + 1;
    }
}

void print_room (ROOM room) {
    if (room.theme == regular)
        attron (COLOR_PAIR (regular_room_color));
    else if (room.theme == enchant)
        attron (COLOR_PAIR (enchant_room_color));
    else if (room.theme == treasure)
        attron (COLOR_PAIR (treasure_room_color));

    for (int i = room.corner.x; i <= room.corner.x + room.width; i++){
        mvprintw (room.corner.y, i, "-");
        mvprintw (room.corner.y + room.height, i, "-");
    }
    for (int i = room.corner.y + 1; i < room.corner.y + room.height; i++) {
        mvprintw (i, room.corner.x, "|");
        mvprintw (i, room.corner.x + room.width, "|");
    }
    if (room.has_door != 0) {
        mvprintw (room.door.y, room.door.x, "+");
        mvprintw (room.window.y, room.window.x, "=");
    }
    

    for (int i = room.corner.x + 1; i < room.corner.x + room.width; i++) {
        for (int j = room.corner.y + 1; j < room.corner.y + room.height; j++) {
            mvprintw (j, i, ".");
        }
    }
    if (room.pillar == 1) 
        mvprintw (room.pillar_loc.y, room.pillar_loc.x, "O");
    if (room.gold == 1) {
        // wchar_t gold_emoji = L'\u0c77';
        attron (COLOR_PAIR (gold_color));
        mvprintw (room.gold_loc.y, room.gold_loc.x, /*"\u0c77"*/ "c");
        // mvadd_wch (room.gold_loc.y, room.gold_loc.x, &gold_emoji);
        attroff (COLOR_PAIR (gold_color));
    }
    if (room.black_gold == 1) {
        attron (COLOR_PAIR (gold_color));
        mvprintw (room.black_gold_loc.y, room.black_gold_loc.x, /*"\u0c77"*/ "o");
        attroff (COLOR_PAIR (gold_color));
    }
    if (room.speed_spell == 1) {
        attron (COLOR_PAIR (spell_color));
        mvprintw (room.speed_spell_loc.y, room.speed_spell_loc.x, "s");
        attroff (COLOR_PAIR (spell_color));
    }
    if (room.health_spell == 1) {
        attron (COLOR_PAIR (spell_color));
        mvprintw (room.health_spell_loc.y, room.health_spell_loc.x, "h");
        attroff (COLOR_PAIR (spell_color));
    }
    if (room.damage_spell == 1) {
        attron (COLOR_PAIR (spell_color));
        mvprintw (room.damage_spell_loc.y, room.damage_spell_loc.x, "d");
        attroff (COLOR_PAIR (spell_color));
    }
    if (room.food == 1) {
        attron (COLOR_PAIR (food_color));
        mvprintw (room.food_loc.y, room.food_loc.x, "f");
        attroff (COLOR_PAIR (food_color));
    }
    
    if (room.mace_weapon == 1) {
        attron (COLOR_PAIR (weapon_color));
        mvprintw (room.mace_weapon_loc.y, room.mace_weapon_loc.x, "m");
        attroff (COLOR_PAIR (weapon_color));
    }
    if (room.knife_weapon == 1) {
        attron (COLOR_PAIR (weapon_color));
        mvprintw (room.knife_weapon_loc.y, room.knife_weapon_loc.x, "k");
        attroff (COLOR_PAIR (weapon_color));
    }
    if (room.wand_weapon == 1) {
        attron (COLOR_PAIR (weapon_color));
        mvprintw (room.wand_weapon_loc.y, room.wand_weapon_loc.x, "w");
        attroff (COLOR_PAIR (weapon_color));
    }
    if (room.arrow_weapon == 1) {
        attron (COLOR_PAIR (weapon_color));
        mvprintw (room.arrow_weapon_loc.y, room.arrow_weapon_loc.x, "a");
        attroff (COLOR_PAIR (weapon_color));
    }
    if (room.blade_weapon == 1) {
        attron (COLOR_PAIR (weapon_color));
        mvprintw (room.blade_weapon_loc.y, room.blade_weapon_loc.x, "b");
        attroff (COLOR_PAIR (weapon_color));
    }

    if (room.staircase == 1) {
        attron (COLOR_PAIR (regular_room_color));
        mvprintw (room.staircase_loc.y, room.staircase_loc.x, /*"\U0001328D"*/ ">"); 
        attroff (COLOR_PAIR (regular_room_color));
    }

    if (room.end == 1) {
        attron (COLOR_PAIR (regular_room_color));
        mvprintw (room.end_loc.y, room.end_loc.x, /*"\U0001328D"*/ "E"); 
        attroff (COLOR_PAIR (regular_room_color));
    }

    attron (COLOR_PAIR(enemy_color));
    if (room.monster == Deamon) {
        mvprintw (room.corner.y + room.height - 1, room.corner.x + 1, "D");
    }
    if (room.monster == Fire) {
        mvprintw (room.corner.y + room.height - 1, room.corner.x + 1, "F");
    }
    if (room.monster == Giant) {
        mvprintw (room.corner.y + room.height - 1, room.corner.x + 1, "G");
    }
    if (room.monster == Snake) {
        mvprintw (room.corner.y + room.height - 1, room.corner.x + 1, "S");
    }
    if (room.monster == Undeed) {
        mvprintw (room.corner.y + room.height - 1, room.corner.x + 1, "U");
    }
    attroff(COLOR_PAIR(enemy_color));
    

    if (room.theme == regular)
        attroff (COLOR_PAIR (regular_room_color));
    else if (room.theme == enchant)
        attroff (COLOR_PAIR (enchant_room_color));
    else if (room.theme == treasure)
        attroff (COLOR_PAIR (treasure_room_color));

}

void corrider_maker (int i) {
    if (i == 0) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y - 1; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 1) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y + 1; j--) {
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 2) {
        corrider[i].n = 0;
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x + 1; j <= COLS / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 3) {
        corrider[i].n = 0;
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 4) {
        corrider[i].n = 0;
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 6) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 7) {
        corrider[i].n = 0;
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 8) {
        corrider[i].n = 0;
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2 - 2; j >= room[i + 1].door.x; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2 ; j >= room[i + 1].door.y + 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 9) {
        corrider[i].n = 0;
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 4 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2 ; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 10) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= room[i + 1].door.y; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }

    if (i == 12) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y - 1; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 13) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y + 1; j--) {
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 14) {
        corrider[i].n = 0;
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x + 1; j <= COLS / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 15) {
        corrider[i].n = 0;
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 16) {
        corrider[i].n = 0;
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 18) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2; j <= room[i + 1].door.y - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 19) {
        corrider[i].n = 0;
        for (int j = room[i].door.y - 1; j >= LINES / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2; j <= room[i + 1].door.x - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 20) {
        corrider[i].n = 0;
        for (int j = room[i].door.x - 1; j >= COLS / 2 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 2 - 2; j >= room[i + 1].door.x; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2 ; j >= room[i + 1].door.y + 1; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i + 1].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 21) {
        corrider[i].n = 0;
        for (int j = room[i].door.y + 1; j <= LINES / 2 - 1; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = room[i].door.x;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.x - 1; j >= COLS / 4 - 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = LINES / 2 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = LINES / 2 - 2; j >= room[i + 1].door.y; j--) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2 ; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
    else if (i == 22) {
        corrider[i].n = 0;
        for (int j = room[i].door.x + 1; j <= COLS / 4 - 1; j++) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = room[i].door.y + 1; j <= room[i + 1].door.y; j++) {
            corrider[i].piece[corrider[i].n].y = j;
            corrider[i].piece[corrider[i].n].x = COLS / 4 - 1;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
        for (int j = COLS / 4 - 2; j >= room[i + 1].door.x + 1; j--) {
            corrider[i].piece[corrider[i].n].x = j;
            corrider[i].piece[corrider[i].n].y = room[i + 1].door.y;
            corrider[i].shown[corrider[i].n] = 0;
            corrider[i].n++;
        }
    }
}

void print_corrider (int i) {
    attron (COLOR_PAIR(regular_room_color));
    for (int j = 0; j < corrider[i].n; j++) {
        if (corrider[i].shown[j] == 1) {
            mvprintw (corrider[i].piece[j].y, corrider[i].piece[j].x, "#");
        }
        else {
            return;
        }
    }
    attroff (COLOR_PAIR(regular_room_color));
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void the_end () {
    clear ();
    if (player.id == 0) {
        if (player.health > 0 && END == 2) {
            border_make();
            attron(COLOR_PAIR(pause_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 12) / 2, "GAME PAUSED!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(pause_color));

            attron(COLOR_PAIR(regular_room_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(regular_room_color));

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }
            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            END = 0;
            return;
        }
        else if (player.health > 0) {
            border_make();
            attron(COLOR_PAIR(win_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 8) / 2, "YOU WON!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(win_color));

            attron(COLOR_PAIR(green_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(green_color));

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }
            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            END = 0;
            return;
        }
        else if (player.health == 0) {
            border_make();
            attron(COLOR_PAIR(lose_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 10) / 2, "GAME OVER!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(lose_color));

            attron(COLOR_PAIR(error_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(error_color));

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }
            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            END = 0;
            return;
        }
    }
    else {
        if (player.health > 0 && END == 2) {
            border_make();
            attron(COLOR_PAIR(pause_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 12) / 2, "GAME PAUSED!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(pause_color));

            attron(COLOR_PAIR(regular_room_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(regular_room_color));

            change_info (player.id, player.gold + 5 * player.black_gold, 1, 0);

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }
            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            END = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            return;
        }
        else if (player.health > 0) {
            border_make();
            attron(COLOR_PAIR(win_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 8) / 2, "YOU WON!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(win_color));

            attron(COLOR_PAIR(green_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(green_color));

            change_info (player.id, player.gold + 5 * player.black_gold, 1, 1);

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }
            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            END = 0;
            return;
        }
        else if (player.health == 0) {
            border_make();
            attron(COLOR_PAIR(lose_color));
            attron(A_BOLD);
            mvprintw (LINES / 2 - 4, (COLS - 10) / 2, "GAME OVER!");
            attroff(A_BOLD);
            mvprintw (LINES / 2 + 1, (COLS - 22) / 2, "you achieved %d coins.", player.gold + 5 * player.black_gold);
            attroff(COLOR_PAIR(lose_color));

            attron(COLOR_PAIR(error_color));
            mvprintw (7 * LINES / 8, (COLS - 16) / 2, "press e to exit.");
            attroff(COLOR_PAIR(error_color));

            change_info (player.id, player.gold + 5 * player.black_gold, 1, 0);

            border_make();
            int c;
            while ((c = getch()) != 'e') {
                border_make();
            }

            save_game();
            player.gold = 0;
            player.level = 0;
            player.health = 5;
            player.gold = 0;
            player.black_gold = 0;

            player.speed_spell = 0;
            player.health_spell = 0;
            player.damage_spell = 0;

            player.mace_weapon = 10;
            player.knife_weapon = 0;
            player.wand_weapon = 0;
            player.arrow_weapon = 0;
            player.blade_weapon = 0;

            player.food = 0;
            player.fullness = 10;

            deamon.health = 5;
            fire.health = 10;
            giant.health = 15;
            snake.health = 20;
            undeed.health = 30;
            player.current_weapon = mace;
            weapon_blink = 0;
            spell_blink = 0;
            deamon.move = 1;
            fire.move = 1;
            giant.move = 1;
            snake.move = 1;
            undeed.move = 1;
            fast_speed = 0;
            deamon.number_of_moves = 15;
            fire.number_of_moves = 15;
            for (int i = 0; i < 24; i++) {
                room[i].has_door = 1;
            }
            room[24].has_door = 0;
            END = 0;
            return;
        }
    }

    
}


void change_info (int id, int gold, int number_of_games, int number_of_wins) {
    FILE* f;
    if (id == 1) 
        f = fopen("1.txt", "r");
    else if (id == 2) 
        f = fopen("2.txt", "r");
    else if (id == 3) 
        f = fopen("3.txt", "r");
    else if (id == 4) 
        f = fopen("4.txt", "r");
    else if (id == 5) 
        f = fopen("5.txt", "r");
    else if (id == 6) 
        f = fopen("6.txt", "r");
    else if (id == 7) 
        f = fopen("7.txt", "r");
    else if (id == 8) 
        f = fopen("8.txt", "r");
    else if (id == 9) 
        f = fopen("9.txt", "r");
    else if (id == 10) 
        f = fopen("10.txt", "r");
    else if (id == 11) 
        f = fopen("11.txt", "r");
    else if (id == 12) 
        f = fopen("12.txt", "r");

    char user[100], gol[10], num[10], exp[10];
    fgets (user, 100, f);
    fgets (gol, 10, f);
    fgets (num, 10, f);
    fgets (exp, 10, f);
    int new_gol, new_num, new_exp;
    new_gol = atol(gol) + gold;
    new_num = atol(num) + number_of_games;
    new_exp = atol(exp) + number_of_wins;
    fclose (f);

    if (id == 1) 
        f = fopen("1.txt", "w");
    else if (id == 2) 
        f = fopen("2.txt", "w");
    else if (id == 3) 
        f = fopen("3.txt", "w");
    else if (id == 4) 
        f = fopen("4.txt", "w");
    else if (id == 5) 
        f = fopen("5.txt", "w");
    else if (id == 6) 
        f = fopen("6.txt", "w");
    else if (id == 7) 
        f = fopen("7.txt", "w");
    else if (id == 8) 
        f = fopen("8.txt", "w");
    else if (id == 9) 
        f = fopen("9.txt", "w");
    else if (id == 10) 
        f = fopen("10.txt", "w");
    else if (id == 11) 
        f = fopen("11.txt", "w");
    else if (id == 12) 
        f = fopen("12.txt", "w");

    fprintf (f, "%s%d\n%d\n%d\n", user, new_gol, new_num, new_exp);
    fclose (f);

}

void save_game () {
    FILE* f;
    f = fopen ("save.txt", "w");
    fprintf (f, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", player.id, player.position.y, player.position.x, player.color, player.level, player.health, player.gold, player.black_gold, player.speed_spell, player.health_spell, player.damage_spell, player.mace_weapon, player.knife_weapon, player.wand_weapon, player.arrow_weapon, player.blade_weapon, player.food, player.fullness);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void deamon_move () {
    if (deamon.number_of_moves > 0) {
        if (abs(player.position.x - deamon.position.x) + abs(player.position.y - deamon.position.y) <= 2) {
            if (player.fullness > 0)
                player.fullness--;
            message_box (" Deamon hit you!");
        }
        if (abs(player.position.y - deamon.position.y) > 0) {
            if (player.position.y > deamon.position.y) {
                if (is_point (deamon.position.y + 1, deamon.position.x)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (deamon.position.y, deamon.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    deamon.position.y++;
                    deamon.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (deamon.position.y, deamon.position.x, "D");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
            else if (player.position.y < deamon.position.y) {
                if (is_point (deamon.position.y - 1, deamon.position.x)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (deamon.position.y, deamon.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    deamon.position.y--;
                    deamon.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (deamon.position.y, deamon.position.x, "D");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
        }
        else if (abs(player.position.x - deamon.position.x) > 0) {
            if (player.position.x > deamon.position.x) {
                if (is_point (deamon.position.y, deamon.position.x + 1)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (deamon.position.y, deamon.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    deamon.position.x++;
                    deamon.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (deamon.position.y, deamon.position.x, "D");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
            else if (player.position.x < deamon.position.x) {
                if (is_point (deamon.position.y, deamon.position.x - 1)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (deamon.position.y, deamon.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    deamon.position.x--;
                    deamon.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (deamon.position.y, deamon.position.x, "D");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
        }
    }
}
void fire_move () {
    if (fire.number_of_moves > 0) {
        if (abs(player.position.x - fire.position.x) + abs(player.position.y - fire.position.y) <= 2) {
            if (player.fullness > 0)
                player.fullness--;
            message_box (" Fire breathing monster hit you!");
        }
        int check = 0;
        if (abs(player.position.y - fire.position.y) > 0) {
            if (player.position.y > fire.position.y) {
                if (is_point (fire.position.y + 1, fire.position.x)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (fire.position.y, fire.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    fire.position.y++;
                    fire.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (fire.position.y, fire.position.x, "F");
                    attroff (COLOR_PAIR (enemy_color));
                    check++;
                }
            }
            else if (player.position.y < fire.position.y) {
                if (is_point (fire.position.y - 1, fire.position.x)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (fire.position.y, fire.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    fire.position.y--;
                    fire.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (fire.position.y, fire.position.x, "F");
                    attroff (COLOR_PAIR (enemy_color));
                    check++;
                }
            }
        }
        if (abs(player.position.x - fire.position.x) > 0 && check == 0) {
            if (player.position.x > fire.position.x) {
                if (is_point (fire.position.y, fire.position.x + 1)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (fire.position.y, fire.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    fire.position.x++;
                    fire.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (fire.position.y, fire.position.x, "F");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
            else if (player.position.x < fire.position.x) {
                if (is_point (fire.position.y, fire.position.x - 1)) {
                    attron (COLOR_PAIR (room[current_room()].theme));
                    mvprintw (fire.position.y, fire.position.x, ".");
                    attroff (COLOR_PAIR (room[current_room()].theme));
                    fire.position.x--;
                    fire.number_of_moves--;
                    attron (COLOR_PAIR (enemy_color));
                    mvprintw (fire.position.y, fire.position.x, "F");
                    attroff (COLOR_PAIR (enemy_color));
                }
            }
        }
    }
}
void giant_move () {
    if (abs(player.position.x - giant.position.x) + abs(player.position.y - giant.position.y) <= 2) {
        if (player.fullness > 0)
            player.fullness--;
        message_box (" Giant hit you!");
    }
    int check = 0;
    if (abs(player.position.y - giant.position.y) > 0) {
        if (player.position.y > giant.position.y) {
            if (is_point (giant.position.y + 1, giant.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (giant.position.y, giant.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                giant.position.y++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (giant.position.y, giant.position.x, "G");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
        else if (player.position.y < giant.position.y) {
            if (is_point (giant.position.y - 1, giant.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (giant.position.y, giant.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                giant.position.y--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (giant.position.y, giant.position.x, "G");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
    }
    if (abs(player.position.x - giant.position.x) > 0 && check == 0) {
        if (player.position.x > giant.position.x) {
            if (is_point (giant.position.y, giant.position.x + 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (giant.position.y, giant.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                giant.position.x++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (giant.position.y, giant.position.x, "G");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
        else if (player.position.x < giant.position.x) {
            if (is_point (giant.position.y, giant.position.x - 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (giant.position.y, giant.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                giant.position.x--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (giant.position.y, giant.position.x, "G");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
    }
}
void snake_move () {
    if (abs(player.position.x - snake.position.x) + abs(player.position.y - snake.position.y) <= 2) {
        if (player.fullness > 0)
            player.fullness--;
        message_box (" Snake hit you!");
    }
    int check = 0;
    if (abs(player.position.y - snake.position.y) > 0) {
        if (player.position.y > snake.position.y) {
            if (is_point (snake.position.y + 1, snake.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (snake.position.y, snake.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                snake.position.y++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (snake.position.y, snake.position.x, "S");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
        else if (player.position.y < snake.position.y) {
            if (is_point (snake.position.y - 1, snake.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (snake.position.y, snake.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                snake.position.y--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (snake.position.y, snake.position.x, "S");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
    }
    if (abs(player.position.x - snake.position.x) > 0 && check == 0) {
        if (player.position.x > snake.position.x) {
            if (is_point (snake.position.y, snake.position.x + 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (snake.position.y, snake.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                snake.position.x++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (snake.position.y, snake.position.x, "S");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
        else if (player.position.x < snake.position.x) {
            if (is_point (snake.position.y, snake.position.x - 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (snake.position.y, snake.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                snake.position.x--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (snake.position.y, snake.position.x, "S");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
    }
}
void undeed_move () {
    if (abs(player.position.x - undeed.position.x) + abs(player.position.y - undeed.position.y) <= 2) {
        if (player.fullness > 0)
            player.fullness--;
        message_box (" Undead hit you!");
    }
    int check = 0;
    if (abs(player.position.x - undeed.position.x) > 0) {
        if (player.position.x > undeed.position.x) {
            if (is_point (undeed.position.y, undeed.position.x + 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (undeed.position.y, undeed.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                undeed.position.x++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (undeed.position.y, undeed.position.x, "U");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
        else if (player.position.x < undeed.position.x) {
            if (is_point (undeed.position.y, undeed.position.x - 1)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (undeed.position.y, undeed.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                undeed.position.x--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (undeed.position.y, undeed.position.x, "U");
                attroff (COLOR_PAIR (enemy_color));
            }
        }
    }
    if (abs(player.position.y - undeed.position.y) > 0 && check == 0) {
        if (player.position.y > undeed.position.y) {
            if (is_point (undeed.position.y + 1, undeed.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (undeed.position.y, undeed.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                undeed.position.y++;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (undeed.position.y, undeed.position.x, "U");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
        else if (player.position.y < undeed.position.y) {
            if (is_point (undeed.position.y - 1, undeed.position.x)) {
                attron (COLOR_PAIR (room[current_room()].theme));
                mvprintw (undeed.position.y, undeed.position.x, ".");
                attroff (COLOR_PAIR (room[current_room()].theme));
                undeed.position.y--;
                attron (COLOR_PAIR (enemy_color));
                mvprintw (undeed.position.y, undeed.position.x, "U");
                attroff (COLOR_PAIR (enemy_color));
                check++;
            }
        }
    }
}
int is_point (int y, int x) {
    char ch = mvinch (y, x) & A_CHARTEXT;
    if (ch == '.') {
        return 1;
    }
    return 0;
}


void weapon_choose () {
    message_box ("choose a weapon! (press w to get back.)");
    weapon_blink = 1;
    while (1) {
        weapon_box ();
        int c = getch ();
        if (c == '1') {
            if (player.mace_weapon > 0) {
                message_box ("Your default weapon is mace. press w.");
                player.current_weapon = mace;
            }
            else 
                message_box ("You have no mace!");
        }
        else if (c == '2') {
            if (player.knife_weapon > 0) {
                message_box ("Your default weapon is knife. Press w.");
                player.current_weapon = knife;
            }
            else 
                message_box ("You have no knife!");
        }
        else if (c == '3') {
            if (player.wand_weapon > 0) {
                message_box ("Your default weapon is wand. Press w.");
                player.current_weapon = wand;
            }
            else 
                message_box ("You have no wand!");
        }
        else if (c == '4') {
            if (player.arrow_weapon > 0) {
                message_box ("Your default weapon is arrow. Press w.");
                player.current_weapon = arrow;
            }
            else 
                message_box ("You have no arrow!");
        }
        else if (c == '5') {
            if (player.blade_weapon > 0) {
                message_box ("Your default weapon is blade. Press w.");
                player.current_weapon = blade;
            }
            else 
                message_box ("You have no blade!");
        }
        else if (c == 'w' || c == 'W') {
            weapon_blink = 0;
            return;
        }
    }
}

void weapon_use () {
    if (player.current_weapon == mace) {
        if (player.mace_weapon == 0) {
            message_box ("You have no mace!");
        }
        else if (player.mace_weapon > 0) {
            // player.mace_weapon -= 1;
            if (current_room() == 5) {
                if (deamon.position.x <= player.position.x + 1 && deamon.position.x >= player.position.x - 1 && deamon.position.y <= player.position.y + 1 && deamon.position.y >= player.position.y - 1) {
                    if (deamon.health >= 5)
                        deamon.health -= 5;
                    else if (deamon.health < 5)
                        deamon.health = 0;
                    message_box ("You hit the Deamon!");
                }
                else {
                    message_box ("No enemy around here!");
                }
            }
            else if (current_room() == 11) {
                if (fire.position.x <= player.position.x + 1 && fire.position.x >= player.position.x - 1 && fire.position.y <= player.position.y + 1 && fire.position.y >= player.position.y - 1) {
                    if (fire.health >= 5)
                        fire.health -= 5;
                    else if (fire.health < 5)
                        fire.health = 0;
                    message_box ("You hit the Fire Monster!");
                }
                else {
                    message_box ("No enemy around here!");
                }
            }
            else if (current_room() == 17) {
                if (giant.position.x <= player.position.x + 1 && giant.position.x >= player.position.x - 1 && giant.position.y <= player.position.y + 1 && giant.position.y >= player.position.y - 1) {
                    if (giant.health >= 5)
                        giant.health -= 5;
                    else if (giant.health < 5)
                        giant.health = 0;
                    message_box ("You hit the Giant!");
                }
                else {
                    message_box ("No enemy around here!");
                }
            }
            else if (current_room() == 23) {
                if (snake.position.x <= player.position.x + 1 && snake.position.x >= player.position.x - 1 && snake.position.y <= player.position.y + 1 && snake.position.y >= player.position.y - 1) {
                    if (snake.health >= 5)
                        snake.health -= 5;
                    else if (snake.health < 5)
                        snake.health = 0;
                    message_box ("You hit the Snake!");
                }
                else {
                    message_box ("No enemy around here!");
                }
            }
            else if (current_room() == 24) {
                if (undeed.position.x <= player.position.x + 1 && undeed.position.x >= player.position.x - 1 && undeed.position.y <= player.position.y + 1 && undeed.position.y <= player.position.y - 1) {
                    if (undeed.health >= 5)
                        undeed.health -= 5;
                    else if (undeed.health < 5)
                        undeed.health = 0;
                    message_box ("You hit the Undead!");
                }
                else {
                    message_box ("No enemy around here!");
                }
            }
            else {
                message_box ("No enemy around here!");
                // attron (COLOR_PAIR (weapon_color));
                // if (is_point (player.position.y, player.position.x + 1)) 
                //     mvprintw (player.position.y, player.position.x + 1, "m");
                // else if (is_point (player.position.y, player.position.x - 1))
                //     mvprintw (player.position.y, player.position.x - 1, "m");
                // else if (is_point (player.position.y + 1, player.position.x))
                //     mvprintw (player.position.y + 1, player.position.x, "m");
                // else if (is_point (player.position.y - 1, player.position.x))
                //     mvprintw (player.position.y - 1, player.position.x, "m");
                // attroff (COLOR_PAIR (weapon_color));
            }
        }
    }

    else if (player.current_weapon == blade) {
        if (player.blade_weapon == 0) {
            message_box ("You have no blade!");
        }
        else if (player.blade_weapon > 0) {
            if (current_room() == 5) {
                if (deamon.position.x <= player.position.x + 1 && deamon.position.x >= player.position.x - 1 && deamon.position.y <= player.position.y + 1 && deamon.position.y >= player.position.y - 1) {
                    if (deamon.health >= 10)
                        deamon.health -= 10;
                    else if (deamon.health < 10)
                        deamon.health = 0;
                    message_box ("You hit the Deamon!");
                }
            }
            else if (current_room() == 11) {
                if (fire.position.x <= player.position.x + 1 && fire.position.x >= player.position.x - 1 && fire.position.y <= player.position.y + 1 && fire.position.y >= player.position.y - 1) {
                    if (fire.health >= 10)
                        fire.health -= 10;
                    else if (fire.health < 10)
                        fire.health = 0;
                    message_box ("You hit the Fire Monster!");
                }
            }
            else if (current_room() == 17) {
                if (giant.position.x <= player.position.x + 1 && giant.position.x >= player.position.x - 1 && giant.position.y <= player.position.y + 1 && giant.position.y >= player.position.y - 1) {
                    if (giant.health >= 10)
                        giant.health -= 10;
                    else if (giant.health < 10)
                        giant.health = 0;
                    message_box ("You hit the Giant!");
                }
            }
            else if (current_room() == 23) {
                if (snake.position.x <= player.position.x + 1 && snake.position.x >= player.position.x - 1 && snake.position.y <= player.position.y + 1 && snake.position.y >= player.position.y - 1) {
                    if (snake.health >= 10)
                        snake.health -= 10;
                    else if (snake.health < 10)
                        snake.health = 0;
                    message_box ("You hit the Snake!");
                }
            }
            else if (current_room() == 24) {
                if (undeed.position.x <= player.position.x + 1 && undeed.position.x >= player.position.x - 1 && undeed.position.y <= player.position.y + 1 && undeed.position.y <= player.position.y - 1) {
                    if (undeed.health >= 10)
                        undeed.health -= 10;
                    else if (undeed.health < 10)
                        undeed.health = 0;
                    message_box ("You hit the Undead!");
                }
            }
        }
    }

    else if (player.current_weapon == knife) {
        if (player.knife_weapon == 0) {
            message_box ("You have no knife!");
        }
        else if (player.knife_weapon > 0) {
            player.knife_weapon -= 1;
            int c = getch();
            if (current_room () == 5) {
                if (c == KEY_UP) {
                    if (player.position.x == deamon.position.x && (player.position.y - 5 == deamon.position.y || player.position.y - 4 == deamon.position.y || player.position.y - 3 == deamon.position.y || player.position.y - 2 == deamon.position.y || player.position.y - 1 == deamon.position.y)) {
                        if (deamon.health >= 12)
                            deamon.health -= 12;
                        else if (deamon.health < 12)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == deamon.position.x && (player.position.y + 5 == deamon.position.y || player.position.y + 4 == deamon.position.y || player.position.y + 3 == deamon.position.y || player.position.y + 2 == deamon.position.y || player.position.y + 1 == deamon.position.y)) {
                        if (deamon.health >= 12)
                            deamon.health -= 12;
                        else if (deamon.health < 12)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == deamon.position.y && (player.position.x + 5 == deamon.position.x || player.position.x + 4 == deamon.position.x || player.position.x + 3 == deamon.position.x || player.position.x + 2 == deamon.position.x || player.position.x + 1 == deamon.position.x)) {
                        if (deamon.health >= 12)
                            deamon.health -= 12;
                        else if (deamon.health < 12)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == deamon.position.y && (player.position.x - 5 == deamon.position.x || player.position.x - 4 == deamon.position.x || player.position.x - 3 == deamon.position.x || player.position.x - 2 == deamon.position.x || player.position.x - 1 == deamon.position.x)) {
                        if (deamon.health >= 12)
                            deamon.health -= 12;
                        else if (deamon.health < 12)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 11) {
                if (c == KEY_UP) {
                    if (player.position.x == fire.position.x && (player.position.y - 5 == fire.position.y || player.position.y - 4 == fire.position.y || player.position.y - 3 == fire.position.y || player.position.y - 2 == fire.position.y || player.position.y - 1 == fire.position.y)) {
                        if (fire.health >= 12)
                            fire.health -= 12;
                        else if (fire.health < 12)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == fire.position.x && (player.position.y + 5 == fire.position.y || player.position.y + 4 == fire.position.y || player.position.y + 3 == fire.position.y || player.position.y + 2 == fire.position.y || player.position.y + 1 == fire.position.y)) {
                        if (fire.health >= 12)
                            fire.health -= 12;
                        else if (fire.health < 12)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == fire.position.y && (player.position.x + 5 == fire.position.x || player.position.x + 4 == fire.position.x || player.position.x + 3 == fire.position.x || player.position.x + 2 == fire.position.x || player.position.x + 1 == fire.position.x)) {
                        if (fire.health >= 12)
                            fire.health -= 12;
                        else if (fire.health < 12)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == fire.position.y && (player.position.x - 5 == fire.position.x || player.position.x - 4 == fire.position.x || player.position.x - 3 == fire.position.x || player.position.x - 2 == fire.position.x || player.position.x - 1 == fire.position.x)) {
                        if (fire.health >= 12)
                            fire.health -= 12;
                        else if (fire.health < 12)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 17) {
                if (c == KEY_UP) {
                    if (player.position.x == giant.position.x && (player.position.y - 5 == giant.position.y || player.position.y - 4 == giant.position.y || player.position.y - 3 == giant.position.y || player.position.y - 2 == giant.position.y || player.position.y - 1 == giant.position.y)) {
                        if (giant.health >= 12)
                            giant.health -= 12;
                        else if (giant.health < 12)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == giant.position.x && (player.position.y + 5 == giant.position.y || player.position.y + 4 == giant.position.y || player.position.y + 3 == giant.position.y || player.position.y + 2 == giant.position.y || player.position.y + 1 == giant.position.y)) {
                        if (giant.health >= 12)
                            giant.health -= 12;
                        else if (giant.health < 12)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == giant.position.y && (player.position.x + 5 == giant.position.x || player.position.x + 4 == giant.position.x || player.position.x + 3 == giant.position.x || player.position.x + 2 == giant.position.x || player.position.x + 1 == giant.position.x)) {
                        if (giant.health >= 12)
                            giant.health -= 12;
                        else if (giant.health < 12)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == giant.position.y && (player.position.x - 5 == giant.position.x || player.position.x - 4 == giant.position.x || player.position.x - 3 == giant.position.x || player.position.x - 2 == giant.position.x || player.position.x - 1 == giant.position.x)) {
                        if (giant.health >= 12)
                            giant.health -= 12;
                        else if (giant.health < 12)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 23) {
                if (c == KEY_UP) {
                    if (player.position.x == snake.position.x && (player.position.y - 5 == snake.position.y || player.position.y - 4 == snake.position.y || player.position.y - 3 == snake.position.y || player.position.y - 2 == snake.position.y || player.position.y - 1 == snake.position.y)) {
                        if (snake.health >= 12)
                            snake.health -= 12;
                        else if (snake.health < 12)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == snake.position.x && (player.position.y + 5 == snake.position.y || player.position.y + 4 == snake.position.y || player.position.y + 3 == snake.position.y || player.position.y + 2 == snake.position.y || player.position.y + 1 == snake.position.y)) {
                        if (snake.health >= 12)
                            snake.health -= 12;
                        else if (snake.health < 12)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == snake.position.y && (player.position.x + 5 == snake.position.x || player.position.x + 4 == snake.position.x || player.position.x + 3 == snake.position.x || player.position.x + 2 == snake.position.x || player.position.x + 1 == snake.position.x)) {
                        if (snake.health >= 12)
                            snake.health -= 12;
                        else if (snake.health < 12)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == snake.position.y && (player.position.x - 5 == snake.position.x || player.position.x - 4 == snake.position.x || player.position.x - 3 == snake.position.x || player.position.x - 2 == snake.position.x || player.position.x - 1 == snake.position.x)) {
                        if (snake.health >= 12)
                            snake.health -= 12;
                        else if (snake.health < 12)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 24) {
                if (c == KEY_UP) {
                    if (player.position.x == undeed.position.x && (player.position.y - 5 == undeed.position.y || player.position.y - 4 == undeed.position.y || player.position.y - 3 == undeed.position.y || player.position.y - 2 == undeed.position.y || player.position.y - 1 == undeed.position.y)) {
                        if (undeed.health >= 12)
                            undeed.health -= 12;
                        else if (undeed.health < 12)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == undeed.position.x && (player.position.y + 5 == undeed.position.y || player.position.y + 4 == undeed.position.y || player.position.y + 3 == undeed.position.y || player.position.y + 2 == undeed.position.y || player.position.y + 1 == undeed.position.y)) {
                        if (undeed.health >= 12)
                            undeed.health -= 12;
                        else if (undeed.health < 12)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == undeed.position.y && (player.position.x + 5 == undeed.position.x || player.position.x + 4 == undeed.position.x || player.position.x + 3 == undeed.position.x || player.position.x + 2 == undeed.position.x || player.position.x + 1 == undeed.position.x)) {
                        if (undeed.health >= 12)
                            undeed.health -= 12;
                        else if (undeed.health < 12)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == undeed.position.y && (player.position.x - 5 == undeed.position.x || player.position.x - 4 == undeed.position.x || player.position.x - 3 == undeed.position.x || player.position.x - 2 == undeed.position.x || player.position.x - 1 == undeed.position.x)) {
                        if (undeed.health >= 12)
                            undeed.health -= 12;
                        else if (undeed.health < 12)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a knife!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a knife!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else {
                if (c == KEY_UP) {
                    if (is_point (player.position.y - 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a knife!");
                }
                else if (c == KEY_DOWN) {
                    if (is_point (player.position.y + 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a knife!");
                }
                else if (c == KEY_RIGHT) {
                    if (is_point (player.position.y, player.position.x + 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a knife!");
                }
                else if (c == KEY_LEFT) {
                    if (is_point (player.position.y, player.position.x - 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a knife!");
                }
                else {
                    message_box (" invalid key!");
                }
            }
        }
        
    }

    else if (player.current_weapon == arrow) {
        if (player.arrow_weapon == 0) {
            message_box ("You have no arrow!");
        }
        else if (player.arrow_weapon > 0) {
            player.arrow_weapon -= 1;
            int c = getch();
            if (current_room () == 5) {
                if (c == KEY_UP) {
                    if (player.position.x == deamon.position.x && (player.position.y - 5 == deamon.position.y || player.position.y - 4 == deamon.position.y || player.position.y - 3 == deamon.position.y || player.position.y - 2 == deamon.position.y || player.position.y - 1 == deamon.position.y)) {
                        if (deamon.health >= 5)
                            deamon.health -= 5;
                        else if (deamon.health < 5)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == deamon.position.x && (player.position.y + 5 == deamon.position.y || player.position.y + 4 == deamon.position.y || player.position.y + 3 == deamon.position.y || player.position.y + 2 == deamon.position.y || player.position.y + 1 == deamon.position.y)) {
                        if (deamon.health >= 5)
                            deamon.health -= 5;
                        else if (deamon.health < 5)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == deamon.position.y && (player.position.x + 5 == deamon.position.x || player.position.x + 4 == deamon.position.x || player.position.x + 3 == deamon.position.x || player.position.x + 2 == deamon.position.x || player.position.x + 1 == deamon.position.x)) {
                        if (deamon.health >= 5)
                            deamon.health -= 5;
                        else if (deamon.health < 5)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == deamon.position.y && (player.position.x - 5 == deamon.position.x || player.position.x - 4 == deamon.position.x || player.position.x - 3 == deamon.position.x || player.position.x - 2 == deamon.position.x || player.position.x - 1 == deamon.position.x)) {
                        if (deamon.health >= 5)
                            deamon.health -= 5;
                        else if (deamon.health < 5)
                            deamon.health = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 11) {
                if (c == KEY_UP) {
                    if (player.position.x == fire.position.x && (player.position.y - 5 == fire.position.y || player.position.y - 4 == fire.position.y || player.position.y - 3 == fire.position.y || player.position.y - 2 == fire.position.y || player.position.y - 1 == fire.position.y)) {
                        if (fire.health >= 5)
                            fire.health -= 5;
                        else if (fire.health < 5)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == fire.position.x && (player.position.y + 5 == fire.position.y || player.position.y + 4 == fire.position.y || player.position.y + 3 == fire.position.y || player.position.y + 2 == fire.position.y || player.position.y + 1 == fire.position.y)) {
                        if (fire.health >= 5)
                            fire.health -= 5;
                        else if (fire.health < 5)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == fire.position.y && (player.position.x + 5 == fire.position.x || player.position.x + 4 == fire.position.x || player.position.x + 3 == fire.position.x || player.position.x + 2 == fire.position.x || player.position.x + 1 == fire.position.x)) {
                        if (fire.health >= 5)
                            fire.health -= 5;
                        else if (fire.health < 5)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == fire.position.y && (player.position.x - 5 == fire.position.x || player.position.x - 4 == fire.position.x || player.position.x - 3 == fire.position.x || player.position.x - 2 == fire.position.x || player.position.x - 1 == fire.position.x)) {
                        if (fire.health >= 5)
                            fire.health -= 5;
                        else if (fire.health < 5)
                            fire.health = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 17) {
                if (c == KEY_UP) {
                    if (player.position.x == giant.position.x && (player.position.y - 5 == giant.position.y || player.position.y - 4 == giant.position.y || player.position.y - 3 == giant.position.y || player.position.y - 2 == giant.position.y || player.position.y - 1 == giant.position.y)) {
                        if (giant.health >= 5)
                            giant.health -= 5;
                        else if (giant.health < 5)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == giant.position.x && (player.position.y + 5 == giant.position.y || player.position.y + 4 == giant.position.y || player.position.y + 3 == giant.position.y || player.position.y + 2 == giant.position.y || player.position.y + 1 == giant.position.y)) {
                        if (giant.health >= 5)
                            giant.health -= 5;
                        else if (giant.health < 5)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == giant.position.y && (player.position.x + 5 == giant.position.x || player.position.x + 4 == giant.position.x || player.position.x + 3 == giant.position.x || player.position.x + 2 == giant.position.x || player.position.x + 1 == giant.position.x)) {
                        if (giant.health >= 5)
                            giant.health -= 5;
                        else if (giant.health < 5)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == giant.position.y && (player.position.x - 5 == giant.position.x || player.position.x - 4 == giant.position.x || player.position.x - 3 == giant.position.x || player.position.x - 2 == giant.position.x || player.position.x - 1 == giant.position.x)) {
                        if (giant.health >= 5)
                            giant.health -= 5;
                        else if (giant.health < 5)
                            giant.health = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 23) {
                if (c == KEY_UP) {
                    if (player.position.x == snake.position.x && (player.position.y - 5 == snake.position.y || player.position.y - 4 == snake.position.y || player.position.y - 3 == snake.position.y || player.position.y - 2 == snake.position.y || player.position.y - 1 == snake.position.y)) {
                        if (snake.health >= 5)
                            snake.health -= 5;
                        else if (snake.health < 5)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == snake.position.x && (player.position.y + 5 == snake.position.y || player.position.y + 4 == snake.position.y || player.position.y + 3 == snake.position.y || player.position.y + 2 == snake.position.y || player.position.y + 1 == snake.position.y)) {
                        if (snake.health >= 5)
                            snake.health -= 5;
                        else if (snake.health < 5)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == snake.position.y && (player.position.x + 5 == snake.position.x || player.position.x + 4 == snake.position.x || player.position.x + 3 == snake.position.x || player.position.x + 2 == snake.position.x || player.position.x + 1 == snake.position.x)) {
                        if (snake.health >= 5)
                            snake.health -= 5;
                        else if (snake.health < 5)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == snake.position.y && (player.position.x - 5 == snake.position.x || player.position.x - 4 == snake.position.x || player.position.x - 3 == snake.position.x || player.position.x - 2 == snake.position.x || player.position.x - 1 == snake.position.x)) {
                        if (snake.health >= 5)
                            snake.health -= 5;
                        else if (snake.health < 5)
                            snake.health = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 24) {
                if (c == KEY_UP) {
                    if (player.position.x == undeed.position.x && (player.position.y - 5 == undeed.position.y || player.position.y - 4 == undeed.position.y || player.position.y - 3 == undeed.position.y || player.position.y - 2 == undeed.position.y || player.position.y - 1 == undeed.position.y)) {
                        if (undeed.health >= 5)
                            undeed.health -= 5;
                        else if (undeed.health < 5)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == undeed.position.x && (player.position.y + 5 == undeed.position.y || player.position.y + 4 == undeed.position.y || player.position.y + 3 == undeed.position.y || player.position.y + 2 == undeed.position.y || player.position.y + 1 == undeed.position.y)) {
                        if (undeed.health >= 5)
                            undeed.health -= 5;
                        else if (undeed.health < 5)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == undeed.position.y && (player.position.x + 5 == undeed.position.x || player.position.x + 4 == undeed.position.x || player.position.x + 3 == undeed.position.x || player.position.x + 2 == undeed.position.x || player.position.x + 1 == undeed.position.x)) {
                        if (undeed.health >= 5)
                            undeed.health -= 5;
                        else if (undeed.health < 5)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == undeed.position.y && (player.position.x - 5 == undeed.position.x || player.position.x - 4 == undeed.position.x || player.position.x - 3 == undeed.position.x || player.position.x - 2 == undeed.position.x || player.position.x - 1 == undeed.position.x)) {
                        if (undeed.health >= 5)
                            undeed.health -= 5;
                        else if (undeed.health < 5)
                            undeed.health = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted an arrow!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted an arrow!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else {
                if (c == KEY_UP) {
                    if (is_point (player.position.y - 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted an arrow!");
                }
                else if (c == KEY_DOWN) {
                    if (is_point (player.position.y + 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted an arrow!");
                }
                else if (c == KEY_RIGHT) {
                    if (is_point (player.position.y, player.position.x + 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted an arrow!");
                }
                else if (c == KEY_LEFT) {
                    if (is_point (player.position.y, player.position.x - 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "k");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted an arrow!");
                }
                else {
                    message_box (" invalid key!");
                }
            }
        }
    }

    else if (player.current_weapon == wand) {
        if (player.wand_weapon == 0) {
            message_box ("You have no wand!");
        }
        else if (player.wand_weapon > 0) {
            player.wand_weapon -= 1;
            int c = getch();
            if (current_room () == 5) {
                if (c == KEY_UP) {
                    if (player.position.x == deamon.position.x && (player.position.y - 5 == deamon.position.y || player.position.y - 4 == deamon.position.y || player.position.y - 3 == deamon.position.y || player.position.y - 2 == deamon.position.y || player.position.y - 1 == deamon.position.y)) {
                        if (deamon.health >= 15)
                            deamon.health -= 15;
                        else if (deamon.health < 15)
                            deamon.health = 0;
                        deamon.move = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == deamon.position.x && (player.position.y + 5 == deamon.position.y || player.position.y + 4 == deamon.position.y || player.position.y + 3 == deamon.position.y || player.position.y + 2 == deamon.position.y || player.position.y + 1 == deamon.position.y)) {
                        if (deamon.health >= 15)
                            deamon.health -= 15;
                        else if (deamon.health < 15)
                            deamon.health = 0;
                        deamon.move = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == deamon.position.y && (player.position.x + 5 == deamon.position.x || player.position.x + 4 == deamon.position.x || player.position.x + 3 == deamon.position.x || player.position.x + 2 == deamon.position.x || player.position.x + 1 == deamon.position.x)) {
                        if (deamon.health >= 15)
                            deamon.health -= 15;
                        else if (deamon.health < 15)
                            deamon.health = 0;
                        deamon.move = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == deamon.position.y && (player.position.x - 5 == deamon.position.x || player.position.x - 4 == deamon.position.x || player.position.x - 3 == deamon.position.x || player.position.x - 2 == deamon.position.x || player.position.x - 1 == deamon.position.x)) {
                        if (deamon.health >= 15)
                            deamon.health -= 15;
                        else if (deamon.health < 15)
                            deamon.health = 0;
                        deamon.move = 0;
                        message_box ("You hit the Deamon!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 11) {
                if (c == KEY_UP) {
                    if (player.position.x == fire.position.x && (player.position.y - 5 == fire.position.y || player.position.y - 4 == fire.position.y || player.position.y - 3 == fire.position.y || player.position.y - 2 == fire.position.y || player.position.y - 1 == fire.position.y)) {
                        if (fire.health >= 15)
                            fire.health -= 15;
                        else if (fire.health < 15)
                            fire.health = 0;
                        fire.move = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == fire.position.x && (player.position.y + 5 == fire.position.y || player.position.y + 4 == fire.position.y || player.position.y + 3 == fire.position.y || player.position.y + 2 == fire.position.y || player.position.y + 1 == fire.position.y)) {
                        if (fire.health >= 15)
                            fire.health -= 15;
                        else if (fire.health < 15)
                            fire.health = 0;
                        fire.move = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == fire.position.y && (player.position.x + 5 == fire.position.x || player.position.x + 4 == fire.position.x || player.position.x + 3 == fire.position.x || player.position.x + 2 == fire.position.x || player.position.x + 1 == fire.position.x)) {
                        if (fire.health >= 15)
                            fire.health -= 15;
                        else if (fire.health < 15)
                            fire.health = 0;
                        fire.move = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == fire.position.y && (player.position.x - 5 == fire.position.x || player.position.x - 4 == fire.position.x || player.position.x - 3 == fire.position.x || player.position.x - 2 == fire.position.x || player.position.x - 1 == fire.position.x)) {
                        if (fire.health >= 15)
                            fire.health -= 15;
                        else if (fire.health < 15)
                            fire.health = 0;
                        fire.move = 0;
                        message_box ("You hit the Fire Monster!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 17) {
                if (c == KEY_UP) {
                    if (player.position.x == giant.position.x && (player.position.y - 5 == giant.position.y || player.position.y - 4 == giant.position.y || player.position.y - 3 == giant.position.y || player.position.y - 2 == giant.position.y || player.position.y - 1 == giant.position.y)) {
                        if (giant.health >= 15)
                            giant.health -= 15;
                        else if (giant.health < 15)
                            giant.health = 0;
                        giant.move = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == giant.position.x && (player.position.y + 5 == giant.position.y || player.position.y + 4 == giant.position.y || player.position.y + 3 == giant.position.y || player.position.y + 2 == giant.position.y || player.position.y + 1 == giant.position.y)) {
                        if (giant.health >= 15)
                            giant.health -= 15;
                        else if (giant.health < 15)
                            giant.health = 0;
                        giant.move = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == giant.position.y && (player.position.x + 5 == giant.position.x || player.position.x + 4 == giant.position.x || player.position.x + 3 == giant.position.x || player.position.x + 2 == giant.position.x || player.position.x + 1 == giant.position.x)) {
                        if (giant.health >= 15)
                            giant.health -= 15;
                        else if (giant.health < 15)
                            giant.health = 0;
                        giant.move = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == giant.position.y && (player.position.x - 5 == giant.position.x || player.position.x - 4 == giant.position.x || player.position.x - 3 == giant.position.x || player.position.x - 2 == giant.position.x || player.position.x - 1 == giant.position.x)) {
                        if (giant.health >= 15)
                            giant.health -= 15;
                        else if (giant.health < 15)
                            giant.health = 0;
                        giant.move = 0;
                        message_box ("You hit the Giant!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 23) {
                if (c == KEY_UP) {
                    if (player.position.x == snake.position.x && (player.position.y - 5 == snake.position.y || player.position.y - 4 == snake.position.y || player.position.y - 3 == snake.position.y || player.position.y - 2 == snake.position.y || player.position.y - 1 == snake.position.y)) {
                        if (snake.health >= 15)
                            snake.health -= 15;
                        else if (snake.health < 15)
                            snake.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == snake.position.x && (player.position.y + 5 == snake.position.y || player.position.y + 4 == snake.position.y || player.position.y + 3 == snake.position.y || player.position.y + 2 == snake.position.y || player.position.y + 1 == snake.position.y)) {
                        if (snake.health >= 15)
                            snake.health -= 15;
                        else if (snake.health < 15)
                            snake.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == snake.position.y && (player.position.x + 5 == snake.position.x || player.position.x + 4 == snake.position.x || player.position.x + 3 == snake.position.x || player.position.x + 2 == snake.position.x || player.position.x + 1 == snake.position.x)) {
                        if (snake.health >= 15)
                            snake.health -= 15;
                        else if (snake.health < 15)
                            snake.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == snake.position.y && (player.position.x - 5 == snake.position.x || player.position.x - 4 == snake.position.x || player.position.x - 3 == snake.position.x || player.position.x - 2 == snake.position.x || player.position.x - 1 == snake.position.x)) {
                        if (snake.health >= 15)
                            snake.health -= 15;
                        else if (snake.health < 15)
                            snake.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Snake!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else if (current_room() == 24) {
                if (c == KEY_UP) {
                    if (player.position.x == undeed.position.x && (player.position.y - 5 == undeed.position.y || player.position.y - 4 == undeed.position.y || player.position.y - 3 == undeed.position.y || player.position.y - 2 == undeed.position.y || player.position.y - 1 == undeed.position.y)) {
                        if (undeed.health >= 15)
                            undeed.health -= 15;
                        else if (undeed.health < 15)
                            undeed.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y - 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_DOWN) {
                    if (player.position.x == undeed.position.x && (player.position.y + 5 == undeed.position.y || player.position.y + 4 == undeed.position.y || player.position.y + 3 == undeed.position.y || player.position.y + 2 == undeed.position.y || player.position.y + 1 == undeed.position.y)) {
                        if (undeed.health >= 15)
                            undeed.health -= 15;
                        else if (undeed.health < 15)
                            undeed.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y + 5, player.position.x)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (player.position.y == undeed.position.y && (player.position.x + 5 == undeed.position.x || player.position.x + 4 == undeed.position.x || player.position.x + 3 == undeed.position.x || player.position.x + 2 == undeed.position.x || player.position.x + 1 == undeed.position.x)) {
                        if (undeed.health >= 15)
                            undeed.health -= 15;
                        else if (undeed.health < 15)
                            undeed.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x + 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else if (c == KEY_LEFT) {
                    if (player.position.y == undeed.position.y && (player.position.x - 5 == undeed.position.x || player.position.x - 4 == undeed.position.x || player.position.x - 3 == undeed.position.x || player.position.x - 2 == undeed.position.x || player.position.x - 1 == undeed.position.x)) {
                        if (undeed.health >= 15)
                            undeed.health -= 15;
                        else if (undeed.health < 15)
                            undeed.health = 0;
                        snake.move = 0;
                        message_box ("You hit the Undead!");
                    }
                    else if (is_point (player.position.y, player.position.x - 5)) {
                        message_box ("You wasted a wand!");
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    else {
                        message_box ("You wasted a wand!");
                    }
                }
                else {
                    message_box (" invalid key!");
                }
            }
            else {
                if (c == KEY_UP) {
                    if (is_point (player.position.y - 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y - 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a wand!");
                }
                else if (c == KEY_DOWN) {
                    if (is_point (player.position.y + 5, player.position.x)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y + 5, player.position.x, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a wand!");
                }
                else if (c == KEY_RIGHT) {
                    if (is_point (player.position.y, player.position.x + 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x + 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a wand!");
                }
                else if (c == KEY_LEFT) {
                    if (is_point (player.position.y, player.position.x - 5)) {
                        attron (COLOR_PAIR (weapon_color));
                        mvprintw (player.position.y, player.position.x - 5, "w");
                        attroff (COLOR_PAIR (weapon_color));
                    }
                    message_box ("You wasted a wand!");
                }
                else {
                    message_box (" invalid key!");
                }
            }
        }
    }
}

void spell_use () {
    message_box ("choose a spell! (press s to get back.)");
    spell_blink = 1;
    while (1) {
        spell_box ();
        int c = getch ();
        if (c == '6') {
            if (player.health_spell > 0) {
                message_box ("Your are using health spell.");
            }
            else 
                message_box ("You have no health spell!");
        }
        else if (c == '7') {
            if (player.speed_spell > 0) {
                message_box ("Your are using speed spell.");
                fast_speed = 15;
            }
            else 
                message_box ("You have no speed spell!");
        }
        else if (c == '8') {
            if (player.damage_spell > 0) {
                message_box ("Your are using damage spell.");
            }
            else 
                message_box ("You have no damage spell!");
        }
        else if (c == 's' || c == 'S') {
            spell_blink = 0;
            return;
        }
    }
}

void food_use () {
    message_box ("choose your food! (press e to get back)");
    food_blink = 1;
    while (1) {
        food_box ();
        int c = getch ();
        if (c == '9') {
            if (player.food > 0) {
                if (player.fullness < 10) {
                    player.food--;
                    player.fullness++;
                    message_box (" eaten successfully!");
                }
                else {
                    message_box (" You're already full! (press e)");
                }
            }
            else {
                message_box (" You have no food!");
            }
        }
        else if (c == '0') {
            if (player.magic_food > 0) {
                if (player.fullness < 10) {
                    player.magic_food--;
                    player.fullness++;
                    fast_speed = 15;
                    message_box (" eaten successfully!");
                }
                else {
                    message_box (" You're already full!");
                }
            }
            else {
                message_box (" You have no magic food!");
            }
        }
        else if (c == 'e' || c == 'E') {
            food_blink = 0;
            return;
        }
    }
}


