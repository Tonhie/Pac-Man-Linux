// Writer: C.Xavier
// Sponsors: One autumn leaf & The cat staring at the sky
// Product: Pac-Man (on the terminal)
// Version: 1.0.0_base
// Source Code Link: https://github.com/CloudXavier/Pac-Man-Linux.git

#include "base_data.h"
#include "functions.h"

int add_score;
inline void Status_bar(bool add) {
    SetPos(Height + 1, 1);
    std::cout << "Lifes: ";
    printf("\033[33m\033[1m");
    for (int i = 1; i <= life; i++) putchar('>'), putchar(' ');
    printf("\033[31m");
    for (int i = 1; i <= 3 - life; i++) putchar('>'), putchar(' ');
    printf("\033[37m\033[0m");
    std::cout << "Score: " << Score << "  Level: " << level << ' ';
    if (add)
        std::cout << '+' << add_score << '!';
    else
        std::cout << "      ";
    // Load the life and the Score
}

inline void Drawer(void) {
    printf("\033[33m\033[1m");
    if ((PacMan.locked & 1) == 1) printf("\033[5m");
    SetPos(PacMan.X, PacMan.Y), putchar(Pic[PacMan.Direction]);
    // Load the Pac-Man

    for (int i = 1; i < 5; i++) {
        printf("\033[%dm", Ghost[i].Color);
        if (Ghost[i].locked < 0) printf("\033[34m");
        if (Strong_Time && Strong_Time < 6 && (Strong_Time & 1) == 1) printf("\033[37m");
        SetPos(Ghost[i].X, Ghost[i].Y), putchar('M');
    }
    // Load the Ghost

    printf("\033[37m\033[0m");
    // Reset the color
}  // Color drawer

void Framework(void) {
    SetPos(1, 1);
    for (int i = 1; i <= Height; i++)
        for (int j = 1; j <= Width; j++) {
            if (Save_Map[i][j] == '@') printf("\033[5m");
            putchar(Save_Map[i][j]);
            if (Save_Map[i][j] == '@') printf("\033[0m");
            if (i ^ Height || j ^ Width) putchar(j ^ Width ? ' ' : '\n');
        }
    // Load the map

    Status_bar(false);
}

inline void Direction_Controller(char temp) {
    int record = PacMan.Direction;

    if (temp == 'p') {
        printf("\033]0;Pac-Man (Pausing)\007");
        while (getchar() ^ 'c')
            ;
        printf("\033]0;Pac-Man\007");
    }
    // Check the pause order

    if (temp == 'w') PacMan.Direction = 1;
    if (temp == 's') PacMan.Direction = 2;
    if (temp == 'a') PacMan.Direction = 3;
    if (temp == 'd') PacMan.Direction = 4;
    // Check what the direction going to change

    if (Map[PacMan.X + Move[PacMan.Direction][0]][PacMan.Y + Move[PacMan.Direction][1]] == '#')
        PacMan.Direction = record;
    // If the next pos can't walk, then not change the direction
}

inline void Move_PacMan(void) {
    if (PacMan.locked) {
        PacMan.locked--;
        return;
    }

    COORD temp;
    temp.X = PacMan.X + Move[PacMan.Direction][0];
    temp.Y = PacMan.Y + Move[PacMan.Direction][1];
    // Get the new coord

    if (Map[temp.X][temp.Y] == '.') beans--, Score += 10;
    // Load the life and the Score

    Status_bar(false);

    if (Map[temp.X][temp.Y] == '@') {
        Strong_Time = 25;
        // see the name

        for (int i = 1; i < 5; i++) Ghost[i].locked = -1;
        // Lock the ghosts
    }  // Check whether ate the strong pill

    if (Map[temp.X][temp.Y] == '#') return;
    // Check whether PacMan can go, if not, then not go

    SetPos(PacMan.X, PacMan.Y), putchar(' ');
    Map[PacMan.X][PacMan.Y] = ' ';
    // Clean the passed place and the record

    if (temp.X == 14) {
        if (!temp.Y) temp.Y = Width;
        if (temp.Y == Width + 1) temp.Y = 1;
    }  // Check special coordinate

    PacMan.X = temp.X, PacMan.Y = temp.Y;
    // Give the new coord to PacMan
}

inline bool Check_Obstacle(int x, int y, int Direction) {
    while (x ^ PacMan.X || y ^ PacMan.Y) {
        x += Move[Direction][0], y += Move[Direction][1];
        if (x == 14) {
            if (!y) y = Width;
            if (y == Width + 1) y = 1;
        }
        if (Map[x][y] == '#') return false;
    }
    return true;
}

inline int Change_Direction(int x, int y, int d, int a) {
    int temp = d;
    if (x == PacMan.X) temp = PacMan.Y - y > 0 ? (Ghost[a].locked ^ -1 ? 4 : 3) : (Ghost[a].locked ^ -1 ? 3 : 4);
    if (y == PacMan.Y) temp = PacMan.X - x > 0 ? (Ghost[a].locked ^ -1 ? 2 : 1) : (Ghost[a].locked ^ -1 ? 1 : 2);
    if (temp ^ d && Check_Obstacle(x, y, temp)) return temp;
    // The algorithm of grab (or escape form) the Pac-Man

    if (!Turnings[x][y]) return d;  // If it's not a turning, don't change the direction

    d = d ^ 1 ? (d ^ 2 ? (d ^ 3 ? 3 : 4) : 1) : 2;
    // Reverse process

    temp = (rand() & 3) + 1;
    while (Map[x + Move[temp][0]][y + Move[temp][1]] == '#' || temp == d) temp = (rand() & 3) + 1;
    // Find a reasonable value of "temp"

    return temp;
}

inline void Move_Ghost(int a) {
    if (Ghost[a].Y == 11) {
        if (Ghost[a].X == 11) Map[12][11] = '#';
        if (Ghost[a].X == 13 && !Ghost[a].locked) Map[12][11] = ' ';
    }

    if (Ghost[a].locked > 0) {
        Ghost[a].locked--;
        if (Ghost[a].X == 13) Ghost[a].Direction = 2;
        if (Ghost[a].X == 14) Ghost[a].Direction = 1;
    }  // If the Ghost has die and didn't revival, don't move
    else
        Ghost[a].Direction = Change_Direction(Ghost[a].X, Ghost[a].Y, Ghost[a].Direction, a);
    // Change this ghost's direction

    SetPos(Ghost[a].X, Ghost[a].Y), putchar(Map[Ghost[a].X][Ghost[a].Y]);
    if (Ghost[a].Y == 11) {
        if (Ghost[a].X == 11) SetPos(12, 11), putchar('-');
        if (Ghost[a].X == 13 && !Ghost[a].locked) SetPos(12, 11), putchar(' ');
    }
    // Clean the passed place

    if (Ghost[a].X > 11 && Ghost[a].X < 15 && Ghost[a].Y == 11 && !Ghost[a].locked) Ghost[a].Direction = 1;
    // Help the ghost out of the gate

    COORD temp;
    temp.X = Ghost[a].X + Move[Ghost[a].Direction][0];
    temp.Y = Ghost[a].Y + Move[Ghost[a].Direction][1];

    if (temp.X == 14) {
        if (!temp.Y) temp.Y = Width;
        if (temp.Y == Width + 1) temp.Y = 1;
    }
    // Check whether the special coordinate

    Ghost[a].X = temp.X;
    Ghost[a].Y = temp.Y;
}
inline void Clear_Character(void) {
    for (int i = 1; i < 5; i++) SetPos(Ghost[i].X, Ghost[i].Y), putchar(Map[Ghost[i].X][Ghost[i].Y]);
    SetPos(PacMan.X, PacMan.Y), putchar(' ');
}

inline bool check_alive(int x, int y) {
    if (!Strong_Time) add_score = 0;

    for (int i = 1; i < 5; i++) {
        if (!beans || life < 0) return false;

        if (x ^ Ghost[i].X || y ^ Ghost[i].Y) continue;

        if (Ghost[i].locked > -1) {
            usleep(75000);

            life--;  // See the name

            Status_bar(false);  // See the name

            if (life) Clear_Character();

            Data_Processing(false);
            // Reset Ghost and the PacMan

            continue;
        }  // If was eatten

        printf("\033[40;33m");
        if ((PacMan.locked & 1) == 1) printf("\033[5m");
        SetPos(PacMan.X, PacMan.Y), putchar(Pic[PacMan.Direction]);
        printf("\033[40;37m\033[0m");

        usleep(75000);

        Clear_Character();
        Ghost[i] = Save_Ghost[i];
        Ghost[i].locked = 15;
        // Reset this Ghost

        add_score += 200;
        Score += add_score;
        Status_bar(true);  // See the name
    }
    return true;
}  // Check whether be catched by ghosts

void Game(void) {
    //system("clear");
    SetPos(1, 1);

    Data_Processing(true);

    Framework();  // Load the map

    Status_bar(false);

    Drawer();  // Load the Ghost and the PacMan

    SetPos(16, 10);
    printf("READY");
    usleep(1500000);
    SetPos(16, 10), std::cout << "     ";

    int timer = 0;  // see the name

    while (check_alive(PacMan.X, PacMan.Y)) {           // Continue when alive
        if (_kbhit()) Direction_Controller(getchar());  // Change the PacMan.Direction
        // Move_PacMan(); //See the name

        if (PacMan.Timer == 50000) Move_PacMan(), PacMan.Timer = 0;

        if (!check_alive(PacMan.X, PacMan.Y)) continue;  // Continue when alive

        Drawer();  // Draw the Ghost and the PacMan

        bool temp = false;

        for (int i = 1; i < 5; i++) {
            int temp = Ghost[i].locked < 0 ? 100000 : 50000;
            temp *= (Ghost[i].X == 14 && (Ghost[i].Y <= 5 || Ghost[i].Y >= 17)) ? 2 : 1;

            if (Ghost[i].Timer == temp) {
                Move_Ghost(i);  // Move the Ghost

                Ghost[i].Timer = 0;  // Reset the timer of this ghost

                temp = true;  // Temporary mark

                if (!Strong_Time && Ghost[i].locked == -1) Ghost[i].locked = 0;
                // When the efficacy is over, restore
            }

            Ghost[i].Timer += 10;  // see the name
        }                          // About the Ghost

        Strong_Time -= Strong_Time && temp ? 1 : 0;  // see the name

        Drawer();  // Draw the Ghost and the PacMan

        PacMan.Timer += 10, timer++;  // Waiting sum

        usleep(1);  // Step interval
    }

    if (!beans) level++;

    Status_bar(false);
}

int main(int argc, char const *argv[]) {
    printf("\033]0;Pac-Man\007");

    srand((unsigned)(time(NULL)));  // Reset the Random seed

    system("resize -s 28 42");

    Termanal_Prepare();
    // Set the terminal

    while (true) {
        Cursor_Status(false);  // Hide the Cursor

        ReadOne_Mode(true);

        Load_Data();  // Process the data

        life--;

        while (life > -1) Game();

        SetPos(16, 9), std::cout << "Game Over";

        SetPos(Height + 1, 1), std::cout << "Wether have another game? (Y / n): ";

        Cursor_Status(true);

        char temp = getchar();

        if (temp == 'N' || temp == 'n') break;

    }
    // Reset the terminal

    ReadOne_Mode(false);

    system("resize -s 24 80");
}