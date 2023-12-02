#include <iostream>
#include "colormod.h"
#include <random>
#include <cmath>
#include <limits>
#include <termios.h>
#define STDIN_FILENO 0
using namespace std;

// Initialize colour modification
Color::Modifier RED(Color::FG_RED);
Color::Modifier GREEN(Color::FG_GREEN);
Color::Modifier BLUE(Color::FG_BLUE);
Color::Modifier YELLOW(Color::FG_YELLOW);
Color::Modifier MAGENTA(Color::FG_MAGENTA);
Color::Modifier CYAN(Color::FG_CYAN);
Color::Modifier LIGHT_GREY(Color::FG_LIGHT_GREY);
Color::Modifier DARK(Color::FG_DARK_GREY);
Color::Modifier LIGHT_RED(Color::FG_LIGHT_RED);
Color::Modifier LIGHT_GREEN(Color::FG_LIGHT_GREEN);
Color::Modifier LIGHT_YELLOW(Color::FG_LIGHT_YELLOW);
Color::Modifier LIGHT_BLUE(Color::FG_LIGHT_BLUE);
Color::Modifier LIGHT_MAGENTA(Color::FG_LIGHT_MAGENTA);
Color::Modifier LIGHT_CYAN(Color::FG_LIGHT_CYAN);
Color::Modifier WHITE(Color::FG_WHITE);
Color::Modifier DEFAULT(Color::FG_DEFAULT);
Color::Modifier BG_RED(Color::BG_RED);
Color::Modifier BG_GREEN(Color::BG_GREEN);
Color::Modifier BG_BLUE(Color::BG_BLUE);
Color::Modifier BG_DEFAULT(Color::BG_DEFAULT);
Color::Modifier RESET_ALL(Color::RESET_ALL);
Color::Modifier REST_BOLD_BRIGHT(Color::REST_BOLD_BRIGHT);
Color::Modifier RESET_DIM(Color::RESET_DIM);
Color::Modifier RESET_UNDERLINE(Color::RESET_UNDERLINE);
Color::Modifier RESET_BLINK(Color::RESET_BLINK);
Color::Modifier RESET_REVERSE(Color::RESET_REVERSE);
Color::Modifier RESET_HIDDEN(Color::RESET_HIDDEN);
Color::Modifier BOLD(Color::BOLD);
Color::Modifier DIM(Color::DIM);
Color::Modifier UNDERLINED(Color::UNDERLINED);
Color::Modifier BLINK(Color::BLINK);
Color::Modifier REVERSE(Color::REVERSE);
Color::Modifier HIDDEN(Color::HIDDEN);

// Function Prototype
void generateShip();
string getString(char x);
void next();
bool isSingleIntDigit(string line);
pair<int, int> generateRandomCoordinate();
void initGrids();
void printGrid(const string& message_above, Color::Modifier above_color, const string& message_below, Color::Modifier below_color);
void playerTurn();
void aiTurnEasy();
void rotatePoint(int pivot_x, int pivot_y, float angle, int& x, int& y);
bool rotatable(int pivot_x, int pivot_y, bool is_counter_clockwise, int x, int y);
void setBufferingOff();
void setBufferingOn();
char arrowKeyInput();
void selectPosition();

struct Grid {
    string type;
    bool hit = false;
};
Grid player_grid[8][8];
Grid ai_grid[8][8];
int player_ship_num = 4;
int ai_ship_num = 4;


void generateShip();

string getString(char x) {
    // Credit https://www.geeksforgeeks.org/how-to-convert-a-single-character-to-string-in-cpp/
    string s(1, x);
    return s;
}

void next() {
    cout << BLUE << "Press Enter to Continue....." << DEFAULT;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("clear");
}

bool isSingleIntDigit(string line) {
    if (line.length() == 1 and isdigit(line[0]))
        return true;
    return false;
}

pair<int, int> generateRandomCoordinate() {
    // Pair - datatype - stores two value in a single variable - best for coordinates
    // Reference https://stackoverflow.com/a/32538736

    // Random
    // Reference - https://stackoverflow.com/questions/45069219/how-to-succinctly-portably-and-thoroughly-seed-the-mt19937-prng
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 7);

    pair<int, int> coordinate;
    int row, col;
    do {
        row = dis(gen);
        col = dis(gen);
    } while (player_grid[row][col].hit);
    coordinate.first = row, coordinate.second = col;
    return coordinate;
    // This function returns coordinate as pair where x,y can be accessed as coordinate.first and coordinate.second
}

void initGrids() {
    player_ship_num = 4;
    ai_ship_num = 4;

    for (auto &r: player_grid)
        for (auto &c: r) {
            Grid g;
            g.hit = false;
            g.type = "~";
            c = g;
        }
    for (auto &r: ai_grid)
        for (auto &c: r) {
            Grid g;
            g.hit = false;
            g.type = "~";
            c = g;
        }

    generateShip();
}

void printGrid(const string& message_above, Color::Modifier above_color, const string& message_below, Color::Modifier below_color) {
    // Clear console
    system("clear");
    cout << above_color << message_above << DEFAULT << endl;
    // Print static information
    cout << CYAN << "                   Player's Grid                                                AI's Grid" << endl
         << DEFAULT;
    cout << YELLOW << "---------------------------------------------------" << "    |    ";
    cout << "---------------------------------------------------" << endl;
    cout << "|o|  A  |  B  |  C  |  D  |  E  |  F  |  G  |  H  |" << "    |    ";
    cout << "|o|  A  |  B  |  C  |  D  |  E  |  F  |  G  |  H  |" << endl;
    cout << "---------------------------------------------------" << "    |    ";
    cout << "---------------------------------------------------" << endl << DEFAULT;

    // Print grid
    for (int r = 0; r < 8; r++) {
        string line;  // Temporary storage for current output line

        // Convert player's grid data to line in string
        for (int c = 0; c < 8; c++) {
            // Print row num
            if (c == 0) {
                line += "|" + to_string(r + 1) + "|";
            }

            // Add type for colour coding
            line += "  " + player_grid[r][c].type;
            if (player_grid[r][c].hit)
                line += "H";
            else
                line += "N";
            line += "  |";
        }

        // Convert ai's grid data to line in string
        line += "    |    ";
        for (int c = 0; c < 8; c++) {
            if (c == 0) {
                line += "|" + to_string(r + 1) + "|";
            }

            // Add type for colour coding
            if (ai_grid[r][c].hit)
                line += "  " + ai_grid[r][c].type + "H";
            else
                line += "   ";
            line += "  |";
        }

        // Output current line
        for (int i = 0; i < line.length() - 1; i++) {
            // Check if current character is 'water' or 'ship' and hit or not
            char cur_char = line.at(i), nxt_char = line.at(i + 1);
            if (cur_char == '~' and nxt_char == 'N') {  // Water & !hit
                cout << BLUE << "~" << DEFAULT;
                i++; // Skip 'N' or 'H'
            } else if (cur_char == '~' and nxt_char == 'H') {  // Water & hit
                cout << RED << "~" << DEFAULT;
                i++; // Skip 'N' or 'H'
            } else if (cur_char == 'X' and nxt_char == 'N') {  // Ship & !hit
                cout << GREEN << "X" << DEFAULT;
                i++; // Skip 'N' or 'H'
            } else if (cur_char == 'X' and nxt_char == 'H') {  // Ship & hit
                cout << RED << "X" << DEFAULT;
                i++; // Skip 'N' or 'H'
            } else { // Static grid character
                cout << YELLOW << cur_char << DEFAULT;
            }
        }

        // Row splitter
        cout << YELLOW << "|" << endl << DEFAULT;
        cout << YELLOW << "---------------------------------------------------";
        cout << "    |    " << "---------------------------------------------------" << DEFAULT << endl;
    }
    cout << below_color << message_below << DEFAULT << endl;
}

void playerTurn() {
    string selection;
    printGrid("Player Turn", DEFAULT, "", GREEN);
    cout << "Where do you want to hit (Ex. E3): ";
    getline(cin, selection);
    // Type and value check
    if (selection.length() == 2) {
        if (ai_grid[selection.at(1) - '1'][selection.at(0) - 'A'].hit) {
            printGrid("", DEFAULT, "That position was already chosen!", RED);
            next();
            playerTurn();
            return;
        } else if ('A' <= selection.at(0) and selection.at(0) <= 'H' and '1' <= selection.at(1) and
                   selection.at(1) <= '8') {
            int col = (int) selection.at(0) - 'A';
            int row = (int) selection.at(1) - '1';
            ai_grid[row][col].hit = true;
            if (ai_grid[row][col].type == "X") {
                printGrid("Player Turn", DEFAULT, "You hit a ship!", GREEN);
                ai_ship_num--;
            } else
                printGrid("Player Turn", DEFAULT, "You missed!", BLUE);
            return;
        }
    }

    printGrid("Player Turn", DEFAULT, "That position is invalid! Please try again!", RED);
    next();
    printGrid("", DEFAULT, "", DEFAULT);
    playerTurn();

}

void aiTurnEasy() {
    pair<int, int> coor = generateRandomCoordinate();
    int row = coor.first;
    int col = coor.second;
    player_grid[row][col].hit = true;
    string message = "AI hits your " + getString(char(col + 'A')) + getString(char(row + '1')) + "!";
    printGrid("AI Turn", DEFAULT, message, DEFAULT);
    if (player_grid[row][col].type == "X") {
        player_ship_num--;
    }
    next();
}

void rotatePoint(int pivot_x, int pivot_y, float angle, int &x, int &y) {
    // https://stackoverflow.com/a/2259502
    int s = (int) sin(angle);
    int c = (int) cos(angle);

    // translate point back to origin:
    x -= pivot_x;
    y -= pivot_y;

    // rotate point
    int xnew = x * c - y * s;
    int ynew = x * s + y * c;

    // translate point back:
    x = abs(xnew + pivot_x);
    y = abs(ynew + pivot_y);
}

bool rotatable(int pivot_x, int pivot_y, bool is_counter_clockwise, int x, int y) {
    float angle = M_PI / 2;
    if (is_counter_clockwise) angle = -M_PI / 2;
    int s = (int) sin(angle);
    int c = (int) cos(angle);

    // translate point back to origin:
    x -= pivot_x;
    y -= pivot_y;

    // rotate point
    int xnew = x * c - y * s;
    int ynew = x * s + y * c;

    // translate point back:
    x = abs(xnew + pivot_x);
    y = abs(ynew + pivot_y);
    return (0 <= x and x <= 7 and 0 <= y and y <= 7);
}

void setBufferingOff() {
    // Reference https://stackoverflow.com/a/42334773
    struct termios t{}; // Console config structure
    tcgetattr(STDIN_FILENO, &t); // Get current attributes
    t.c_lflag &= ~ICANON; // Use bitwise AND operation to turn off
    tcsetattr(STDIN_FILENO, TCSANOW, &t); // Write config
}

void setBufferingOn() {
    // Reference https://stackoverflow.com/a/42334773
    struct termios t{}; // Console config structure
    tcgetattr(STDIN_FILENO, &t); // Get current attributes
    t.c_lflag |= ICANON; // Use bitwise OR to turn on
    tcsetattr(STDIN_FILENO, TCSADRAIN, &t); // Write config
}

char arrowKeyInput() {
    setBufferingOff();
    cout << RESET_ALL << GREEN << "Use arrow keys to move the ship, 'r' for rotating, and press 'n' when finish"
         << DEFAULT << HIDDEN;
    char c, d, e;
    cin >> c;
    if (c == 'r') {
        cout << endl << RESET_ALL;
        setBufferingOn();
        return 'R';
    }
    if (c == 'n') {
        cout << endl << RESET_ALL;
        setBufferingOn();
        return 'N';
    } else if (c != 27) {
        cout << RESET_ALL << RED << endl << "Invalid input! Try again." << DEFAULT << endl;
        setBufferingOn();
        return arrowKeyInput();
    }

    cin >> d;
    cin >> e;
    if ((c == 27) and (d == 91)) {
        if (e == 65) {
            cout << endl << RESET_ALL;
            setBufferingOn();
            return 'u';
        }
        if (e == 66) {
            cout << endl << RESET_ALL;
            setBufferingOn();
            return 'd';
        }
        if (e == 67) {
            cout << endl << RESET_ALL;
            setBufferingOn();
            return 'r';
        }
        if (e == 68) {
            cout << endl << RESET_ALL;
            setBufferingOn();
            return 'l';
        }
    }
    cout << endl << RESET_ALL << RED << "Invalid input! Try again" << DEFAULT << endl;
    setBufferingOn();
    return arrowKeyInput();
}

void selectPosition() {
    string message;
    bool finish = false, counter_rotate = true;
    int ship_4_x1 = 0, ship_4_x2 = 0, ship_4_x3 = 0, ship_4_x4 = 0;
    int ship_4_y1 = 0, ship_4_y2 = 1, ship_4_y3 = 2, ship_4_y4 = 3;
    player_grid[ship_4_y1][ship_4_x1].type = "X";
    player_grid[ship_4_y2][ship_4_x2].type = "X";
    player_grid[ship_4_y3][ship_4_x3].type = "X";
    player_grid[ship_4_y4][ship_4_x4].type = "X";
    while (!finish) {
        printGrid("", DEFAULT, message, RED);
        char choice = arrowKeyInput();
        switch (choice) {
            case 'R':
                if (rotatable(ship_4_x1, ship_4_y1, counter_rotate, ship_4_x2, ship_4_y2) and
                    rotatable(ship_4_x1, ship_4_y1, counter_rotate, ship_4_x3, ship_4_y3) and
                    rotatable(ship_4_x1, ship_4_y1, counter_rotate, ship_4_x4, ship_4_y4)) {
                    if (counter_rotate) {
                        counter_rotate = false;
                        player_grid[ship_4_y2][ship_4_x2].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, -M_PI / 2, ship_4_x2, ship_4_y2);
                        player_grid[ship_4_y2][ship_4_x2].type = "X";

                        player_grid[ship_4_y3][ship_4_x3].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, -M_PI / 2, ship_4_x3, ship_4_y3);
                        player_grid[ship_4_y3][ship_4_x3].type = "X";

                        player_grid[ship_4_y4][ship_4_x4].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, -M_PI / 2, ship_4_x4, ship_4_y4);
                        player_grid[ship_4_y4][ship_4_x4].type = "X";
                        message = "";
                    } else {
                        counter_rotate = true;
                        player_grid[ship_4_y2][ship_4_x2].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, M_PI / 2, ship_4_x2, ship_4_y2);
                        player_grid[ship_4_y2][ship_4_x2].type = "X";


                        player_grid[ship_4_y3][ship_4_x3].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, M_PI / 2, ship_4_x3, ship_4_y3);
                        player_grid[ship_4_y3][ship_4_x3].type = "X";

                        player_grid[ship_4_y4][ship_4_x4].type = "~";
                        rotatePoint(ship_4_x1, ship_4_y1, M_PI / 2, ship_4_x4, ship_4_y4);
                        player_grid[ship_4_y4][ship_4_x4].type = "X";
                    }
                } else {
                    message = "Cannot rotate! Move to another position and try again.";
                }
                break;
            case 'u':
                if (ship_4_y1 > 0 and ship_4_y2 > 0 and ship_4_y3 > 0 and ship_4_y4 > 0) {
                    player_grid[ship_4_y1][ship_4_x1].type = "~";
                    player_grid[ship_4_y2][ship_4_x2].type = "~";
                    player_grid[ship_4_y3][ship_4_x3].type = "~";
                    player_grid[ship_4_y4][ship_4_x4].type = "~";

                    ship_4_y1--;
                    ship_4_y2--;
                    ship_4_y3--;
                    ship_4_y4--;

                    player_grid[ship_4_y1][ship_4_x1].type = "X";
                    player_grid[ship_4_y2][ship_4_x2].type = "X";
                    player_grid[ship_4_y3][ship_4_x3].type = "X";
                    player_grid[ship_4_y4][ship_4_x4].type = "X";
                    message = "";
                } else {
                    message = "Cannot move up!";
                }
                break;
            case 'd':
                if (ship_4_y1 < 7 and ship_4_y2 < 7 and ship_4_y3 < 7 and ship_4_y4 < 7) {
                    player_grid[ship_4_y1][ship_4_x1].type = "~";
                    player_grid[ship_4_y2][ship_4_x2].type = "~";
                    player_grid[ship_4_y3][ship_4_x3].type = "~";
                    player_grid[ship_4_y4][ship_4_x4].type = "~";

                    ship_4_y1++;
                    ship_4_y2++;
                    ship_4_y3++;
                    ship_4_y4++;

                    player_grid[ship_4_y1][ship_4_x1].type = "X";
                    player_grid[ship_4_y2][ship_4_x2].type = "X";
                    player_grid[ship_4_y3][ship_4_x3].type = "X";
                    player_grid[ship_4_y4][ship_4_x4].type = "X";
                    message = "";
                } else {
                    message = "Cannot move down!";
                }
                break;
            case 'l':
                if (ship_4_x1 > 0 and ship_4_x2 > 0 and ship_4_x3 > 0 and ship_4_x4 > 0) {
                    player_grid[ship_4_y1][ship_4_x1].type = "~";
                    player_grid[ship_4_y2][ship_4_x2].type = "~";
                    player_grid[ship_4_y3][ship_4_x3].type = "~";
                    player_grid[ship_4_y4][ship_4_x4].type = "~";

                    ship_4_x1--;
                    ship_4_x2--;
                    ship_4_x3--;
                    ship_4_x4--;

                    player_grid[ship_4_y1][ship_4_x1].type = "X";
                    player_grid[ship_4_y2][ship_4_x2].type = "X";
                    player_grid[ship_4_y3][ship_4_x3].type = "X";
                    player_grid[ship_4_y4][ship_4_x4].type = "X";
                    message = "";
                } else {
                    message = "Cannot move left!";
                }
                break;
            case 'r':
                if (ship_4_x1 < 7 and ship_4_x2 < 7 and ship_4_x3 < 7 and ship_4_x4 < 7) {
                    player_grid[ship_4_y1][ship_4_x1].type = "~";
                    player_grid[ship_4_y2][ship_4_x2].type = "~";
                    player_grid[ship_4_y3][ship_4_x3].type = "~";
                    player_grid[ship_4_y4][ship_4_x4].type = "~";

                    ship_4_x1++;
                    ship_4_x2++;
                    ship_4_x3++;
                    ship_4_x4++;

                    player_grid[ship_4_y1][ship_4_x1].type = "X";
                    player_grid[ship_4_y2][ship_4_x2].type = "X";
                    player_grid[ship_4_y3][ship_4_x3].type = "X";
                    player_grid[ship_4_y4][ship_4_x4].type = "X";
                    message = "";
                } else {
                    message = "Cannot move right!";
                }
                break;
            case 'N':
                finish = true;
                break;
            default:
                break;
        }
    }


}

void generateShip() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 7);

    int r, c;
    // Randomly determine ship orientation (0 for horizontal, 1 for vertical)
    int orientation = dis(gen);

    // Randomly choose the starting position for the ship

    if (orientation == 0) {
        r = std::uniform_int_distribution<>(0, 7 - 1)(gen);
        c = std::uniform_int_distribution<>(0, 7 - 4)(gen);
    } else {
        r = std::uniform_int_distribution<>(0, 7 - 4)(gen);
        c = std::uniform_int_distribution<>(0, 7 - 1)(gen);
    }

    // Place the ship on the grid
    cout << "AI ship is at: " << endl;
    for (int i = 0; i < 4; ++i) {
        if (orientation == 0) {
            ai_grid[r][c + i].type = "X";
            cout << getString(char(c + i + 'A')) << r + 1 << endl;
        } else {
            ai_grid[r + i][c].type = "X";
            cout << getString(char(c + 'A')) << r + i + 1 << endl;
        }
    }
    next();
}

void playEasy() {
    initGrids();
    selectPosition();
    bool player_turn = true;
    while (player_ship_num > 0 and ai_ship_num > 0) {
        if (player_turn) {
            playerTurn();
            next();
            player_turn = false;
        } else {
            aiTurnEasy();
            player_turn = true;
        }
    }
    if (player_ship_num <= 0){
        printGrid("", DEFAULT, "", DEFAULT);
        cout << BLINK << BOLD << RED << "YOU LOST!" << endl << DEFAULT << RESET_ALL;
    }
    else {
        printGrid("", DEFAULT, "", DEFAULT);
        cout << BLINK << BOLD << GREEN << "YOU WON!" << endl << DEFAULT << RESET_ALL;
    }
    next();
}

int menu() {
    string choice;
    cout << MAGENTA << "Welcome to Battleship! 1v1 with AI" << endl << DEFAULT;
    cout << CYAN << "Menu:" << endl;
    cout << GREEN << "1. Rule " << endl;
    cout << "2. Play (Easy)" << endl;
    cout << "3. Exit" << endl;
    cout << YELLOW << "Choice (1~3): ";
    getline(cin, choice);
    if (!isSingleIntDigit(choice)) {
        cout << RED << "That is not a valid choice! Please try again." << endl << DEFAULT;
        next();
        return menu();
    } else {
        if (stoi(choice) > 3 or stoi(choice) < 1) {
            cout << RED << "That is not a valid choice! Please try again." << endl << DEFAULT;
            next();
            return menu();
        }
    }

    return stoi(choice);
}

void printRule() {
    system("clear");
    cout << YELLOW << "Rule:" << endl << DEFAULT;
    cout << "Welcome to 1v1 Battleship with AI!" << endl;
    cout << "Selection option 2 in the menu to play!" << endl;
    cout << "Both the player and AI will get a 4-grid-long ship which your goal is to destroy its." << endl;
    cout << "At the selection phrase, player can use the arrow key to move the ship around, press 'r' to rotate, and 'n' when finished." << endl;
    cout << "Next, player and AI will take turn hitting other's grid by selecting the column and row such E3." << endl;
    cout << "If one's ship is destroyed, game will prompt win or lose message!" << endl;
    next();
}


int main() {
    bool exit = false;
    while (!exit) {
        system("clear");
        int choice = menu();
        switch (choice) {
            case 1:
                printRule();
                break;
            case 2:
                playEasy();
                break;
            case 3:
                cout << "" << endl;
                exit = true;
                cout << MAGENTA << "Thanks for playing Battleship! Bye!" << RESET_ALL << endl;
                break;
            default:;
        }
    }


}

