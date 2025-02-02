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
} ROOM;

ROOM room[24];

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

    int food;
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
void jangoolak ();

void show_map (int level);
void print_room_in_window (ROOM room, WINDOW* window);
void print_corrider_in_window (int i, WINDOW* window);
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
    player.mace_weapon = 10;
    player.gold = 0;
    player.color = blue;
    player.fullness = 10;

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
                    int temp = user[i].rank;
                    user[i].rank = user[j].rank;
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
                player.mace_weapon = 10;
                dif++;
            }
            else if (strcmp(diff, "2") == 0) {
                mvprintw (1, 1, "               ");
                // difficulty = 2;
                player.health = 4;
                player.mace_weapon = 8;
                dif++;
            }
            else if (strcmp(diff, "3") == 0) {
                mvprintw (1, 1, "               ");
                // difficulty = 3;
                player.health = 3;
                player.mace_weapon = 6;
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
    jangoolak ();

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
    mvprintw (11, 3 * COLS / 4 + 27, "___________");
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
    attron (COLOR_PAIR (weapon_color));
    mvprintw (7, 3 * COLS / 4 + 2, "WEAPONS");
    attroff (COLOR_PAIR (weapon_color));
    mvprintw (10, 3 * COLS / 4 + 1, "1.mace: %d", player.mace_weapon);
    mvprintw (11, 3 * COLS / 4 + 1, "2.knife: %d", player.knife_weapon);
    mvprintw (12, 3 * COLS / 4 + 1, "3.wand: %d", player.wand_weapon);
    mvprintw (13, 3 * COLS / 4 + 1, "4.arrow: %d", player.arrow_weapon);
    mvprintw (14, 3 * COLS / 4 + 1, "5.blade: %d", player.blade_weapon);
}
void spell_box () {
    attron (COLOR_PAIR (spell_color));
    mvprintw (7, 3 * COLS / 4 + 16, "SPELLS");
    attroff (COLOR_PAIR (spell_color));
    mvprintw (10, 3 * COLS / 4 + 14, "6.health: %d", player.health_spell);
    mvprintw (11, 3 * COLS / 4 + 14, "7.speed: %d", player.speed_spell);
    mvprintw (12, 3 * COLS / 4 + 14, "8.damage: %d", player.damage_spell);
}
void food_box () {
    attron (COLOR_PAIR (food_color));
    mvprintw (7, 3 * COLS / 4 + 30, "FOOD");
    attroff (COLOR_PAIR (food_color));
    mvprintw (10, 3 * COLS / 4 + 28, "9.food: %d", player.food);
}
void fullness_box () {
    attron (COLOR_PAIR (food_color));
    mvprintw (13, 3 * COLS / 4 + 29, "fullness");
    attroff (COLOR_PAIR (food_color));
    fullness_bar (player.fullness);
}
void fullness_bar (int f) {
    attron (A_BOLD);
    if (f == 0)
        mvprintw (15, 3 * COLS / 4 + 28, "          ");
    else if (f == 1)
        mvprintw (15, 3 * COLS / 4 + 28, "*         ");
    else if (f == 2)
        mvprintw (15, 3 * COLS / 4 + 28, "**        ");
    else if (f == 3)
        mvprintw (15, 3 * COLS / 4 + 28, "***       ");
    else if (f == 4)
        mvprintw (15, 3 * COLS / 4 + 28, "****      ");
    else if (f == 5)
        mvprintw (15, 3 * COLS / 4 + 28, "*****     ");
    else if (f == 6)
        mvprintw (15, 3 * COLS / 4 + 28, "******    ");
    else if (f == 7)
        mvprintw (15, 3 * COLS / 4 + 28, "*******   ");
    else if (f == 8)
        mvprintw (15, 3 * COLS / 4 + 28, "********  ");
    else if (f == 9)
        mvprintw (15, 3 * COLS / 4 + 28, "********* ");
    else if (f == 10)
        mvprintw (15, 3 * COLS / 4 + 28, "**********");
    attroff (A_BOLD);
}
void hunger_health_handle () {
    if (player.fullness == 10 && player.health < 5) 
        player.health++;
    else if (player.fullness == 0 && player.health > 0)
        player.health--;
}

void jangoolak () {
    mvprintw (LINES / 2 + 4, 3 * COLS / 4, "RRRRR    RRRRRRR RR   RR RRRRRRR RRRRRR");
    mvprintw (LINES / 2 + 5, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR      RR    ");
    mvprintw (LINES / 2 + 6, 3 * COLS / 4, "RR   RR  RR   RR RR   RR RR      RR    ");
    mvprintw (LINES / 2 + 7, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR      RR    ");
    mvprintw (LINES / 2 + 8, 3 * COLS / 4, "RR RR    RR   RR RR   RR RR      RR    ");
    mvprintw (LINES / 2 + 9, 3 * COLS / 4, "RRR      RR   RR RR   RR RR RRRR RRRRRR");
   mvprintw (LINES / 2 + 10, 3 * COLS / 4, "RR RR    RR   RR RR   RR RR   RR RR    ");
   mvprintw (LINES / 2 + 11, 3 * COLS / 4, "RR  RR   RR   RR RR   RR RR   RR RR    ");
   mvprintw (LINES / 2 + 12, 3 * COLS / 4, "RR   RR  RR   RR RR   RR RR   RR RR    ");
   mvprintw (LINES / 2 + 13, 3 * COLS / 4, "RR    RR RRRRRRR RRRRRRR RRRRRRR RRRRRR");    
}

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
}

void mapfloor2() {
    clear();
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
}

void mapfloor3 () {
    clear ();
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
    room[13].food = 1;
    room[14].trap = 1;
    room[15].arrow_weapon = 1;
    room[16].food = 1;
    room[16].trap = 1;
    room[17].staircase = 1;
    room[17].gold = 1;
    room[17].food = 1;
}

void mapfloor4 () {
    clear ();
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
    room[18].damage_spell = 1;
    room[19].food = 1;
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
    room[23].end = 1;
}

//inputs:

void input_handle (int input) {
    location new_position;
    new_position.x = player.position.x;
    new_position.y = player.position.y;
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
        if (player.food > 0) {
            if (player.fullness < 10) {
                player.food--;
                player.fullness++;
                message_box (" eaten successfully!");
            }
            else {
                message_box (" you're already full!");
            }
        }
        else {
            message_box (" you have no food!");
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
        player.position.y = new_position.y;
        player.position.x = new_position.x;

        attron (COLOR_PAIR (player.color));
        mvprintw (player.position.y, player.position.x, "\U0001fbc5");
        attroff (COLOR_PAIR (player.color));
        if (is_trap (player.position)) {
            player.health--;
            message_box ("you just stepped on a trap!");
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
                message_box (" second floor!");
                mapfloor2();
            }
            else if (player.level == 3){
                message_box (" third floor!");
                mapfloor3();
            }
            else if (player.level == 4){
                message_box (" fourth floor!");
                mapfloor4();
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
            player.knife_weapon ++;
            room[current_room()].knife_weapon --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" knife ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'w') {
        message_box ("if you wanna grab the wand, press x");
        int c = getch ();
        if (c == 'x') {
            player.wand_weapon ++;
            room[current_room()].wand_weapon --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" wand ++ !");
            player.fullness--;
        }
    }
    else if (ch == 'a') {
        message_box ("if you wanna grab the arrow, press x");
        int c = getch ();
        if (c == 'x') {
            player.arrow_weapon ++;
            room[current_room()].arrow_weapon --;
            attron (COLOR_PAIR (room[current_room()].theme));
            mvprintw (player.position.y, player.position.x, ".");
            attroff (COLOR_PAIR (room[current_room()].theme));

            player.position.y = new_position.y;
            player.position.x = new_position.x;

            attron (COLOR_PAIR (player.color));
            mvprintw (player.position.y, player.position.x, "\U0001fbc5");
            attroff (COLOR_PAIR (player.color));
            message_box (" arrow ++ !");
            player.fullness--;
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
    for (int i = 0; i < 24; i++) {
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
    for (int i = 0; i < 24; i++) {
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
    room[i].height = (rand() % (LINES / 2 - (loc.y % (LINES / 2)) - 10)) + 8;
    room[i].width = (rand() % (COLS / 4 - (loc.x % (COLS / 4)) - 9)) + 7;
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
        room[i].end_loc.y = room[i].corner.y + 1;
        room[i].end_loc.x = room[i].corner.x + 1;
    }
    if (room[i].trap == 1) {
        room[i].trap_loc.y = room[i].corner.y + 2 + (rand() % (room[i].height - 3));
        room[i].trap_loc.x = room[i].corner.x + 2 + (rand() % (room[i].width - 3));
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
    mvprintw (room.door.y, room.door.x, "+");
    mvprintw (room.window.y, room.window.x, "=");

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
            return;
        }
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

    END = 0;
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