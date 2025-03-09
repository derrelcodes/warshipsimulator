// Program: TT5L_241UC24156.RASYIDAH.KHAIRIAH.MD.RAFFAE
// Course: CCP6124 OOPDS
// Lecture Class: TC2L
// Tutorial Class: TT5L
// Trimester: 2410
// Member_1: 241UC24156 | Rasyidah Khairiah binti Md Raffae | RASYIDAH.KHAIRIAH.MD@student.mmu.edu.my | 011-15150796
// Member_2: 241UC240HF | Nur Aisyah Fitriyah binti Anuar | NUR.AISYAH.FITRIYAH@student.mmu.edu.my | 014-6924384
// Member_3: 1211107055 | Derrel Immanuel A/L Francis Xavier | 1211107055@student.mmu.edu.my | 017-2834245
// Member_4: 241UC24177 | Suhash A/L Maniam Pillai | SUHASH.AL.MANIAM@student.mmu.edu.my | 012-6119641

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

// Forward declarations
class Battlefield;
class Ship;

// ------------------------------------------------------------------------
// Ship Capabilities
class MovingShip {
public:
    virtual bool move(int dx, int dy, Battlefield &bf) = 0;
};

class ShootingShip {
public:
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) = 0;
};

class SeeingRobot {
public:
    virtual void look(int dx, int dy, Battlefield &bf, ofstream &logFile) = 0;
};

class RamShip {
public:
    virtual bool destroy(int dx, int dy, Battlefield &bf) = 0;
};

// ------------------------------------------------------------------------
class Ship {
public:
    int posX, posY;           
    string teamName;          
    string shipType;         
    string shipName;          
    char symbol;              
    int lives;                
    int killCount;            
    int reentriesLeft;        

    // Constructor (Default ShipName)
    Ship(string tName, string type, char sym)
      : teamName(tName), shipType(type), symbol(sym) {
        lives = 3;
        killCount = 0;
        reentriesLeft = 3;
        posX = posY = -1;
        shipName = type; 
    }
    
    // Constructor (Custom ShipName)
    Ship(string tName, string type, char sym, string name)
      : teamName(tName), shipType(type), symbol(sym), shipName(name) {
        lives = 3;
        killCount = 0;
        reentriesLeft = 3;
        posX = posY = -1;
    }

    // Encapsulation
    string getTeam() const { return teamName; }
    string getType() const { return shipType; }

    // Virtual Destructor
    virtual ~Ship() {}
    virtual void takeTurn(class Battlefield &bf, ofstream &logFile) = 0;
    friend ostream& operator<<(ostream &os, const Ship &s) {
        os << s.shipName << "(" << s.teamName << ")";
        return os;
    }
};

// ------------------------------------------------------------------------
// Battlefield Layout
class Battlefield {
public:
    int width, height;
    int **grid;         // 0 = Water, 1 = Island
    Ship ***occupancy;  
    
    Battlefield(int w, int h) : width(w), height(h) {
        grid = new int*[height];
        occupancy = new Ship**[height];
        for (int i = 0; i < height; i++){
            grid[i] = new int[width];
            occupancy[i] = new Ship*[width];
            for (int j = 0; j < width; j++){
                grid[i][j] = 0;
                occupancy[i][j] = NULL;
            }
        }
    }
    ~Battlefield() {
        for (int i = 0; i < height; i++){
            delete [] grid[i];
            delete [] occupancy[i];
        }
        delete [] grid;
        delete [] occupancy;
    }
    bool isValidPosition(int x, int y) {
        return (x >= 0 && x < width && y >= 0 && y < height);
    }
    bool isWater(int x, int y) {
        if (!isValidPosition(x, y)) return false;
        return grid[y][x] == 0;
    }
    bool isOccupied(int x, int y) {
        if (!isValidPosition(x, y)) return true;
        return occupancy[y][x] != NULL;
    }
    
    void updateOccupancy(int oldX, int oldY, int newX, int newY, Ship *ship) {
        if (isValidPosition(oldX, oldY))
            occupancy[oldY][oldX] = NULL;
        if (isValidPosition(newX, newY))
            occupancy[newY][newX] = ship;
    }
    
    void placeShip(int x, int y, Ship *ship) {
        if (isValidPosition(x, y))
            occupancy[y][x] = ship;
        ship->posX = x;
        ship->posY = y;
    }
    
    void removeShip(int x, int y) {
        if(isValidPosition(x, y))
            occupancy[y][x] = NULL;
    }

    // Display Battlefield
    void display(ofstream &logFile) {
        // Print column headers
        cout << "    ";
        logFile << "    ";
        for (int j = 0; j < width; j++){
            cout << j << " ";
            logFile << j << " ";
        }
        cout << "\n";
        logFile << "\n";
        
        // Print each row with a row header
        for (int i = 0; i < height; i++){
            cout << i << " | ";
            logFile << i << " | ";
            for (int j = 0; j < width; j++){
                if (occupancy[i][j] != NULL)
                    cout << occupancy[i][j]->symbol;
                else if (grid[i][j] == 1)
                    cout << "1";
                else
                    cout << "0";
                cout << " ";
                
                if (occupancy[i][j] != NULL)
                    logFile << occupancy[i][j]->symbol;
                else if (grid[i][j] == 1)
                    logFile << "1";
                else
                    logFile << "0";
                logFile << " ";
            }
            cout << "\n";
            logFile << "\n";
        }
        cout << "\n";
        logFile << "\n";
    }
};

// Randomizer
int randRange(int low, int high) {
    return low + rand() % (high - low + 1);
}

// ------------------------------------------------------------------------
bool defaultShoot(Ship *shooter, int targetX, int targetY, Battlefield &bf, bool limitRange = false) {
    if (!bf.isValidPosition(targetX, targetY))
        return false;
    if (targetX == shooter->posX && targetY == shooter->posY)
        return false;
    if (limitRange) {
        int dist = abs(targetX - shooter->posX) + abs(targetY - shooter->posY);
        if (dist > 5)
            return false;
    }
    Ship *target = bf.occupancy[targetY][targetX];
    if (target != NULL && target->teamName != shooter->teamName) {
        target->lives--;
        shooter->killCount++;
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------
struct ShipNode {
    Ship *ship;
    ShipNode *next;
};

class ShipList {
public:
    ShipNode *head;
    ShipList() : head(NULL) {}
    void addShip(Ship *s) {
        ShipNode *node = new ShipNode;
        node->ship = s;
        node->next = head;
        head = node;
    }
    void removeShip(Ship *s) {
        ShipNode *prev = NULL;
        ShipNode *curr = head;
        while(curr != NULL) {
            if(curr->ship == s) {
                if(prev == NULL)
                    head = curr->next;
                else
                    prev->next = curr->next;
                delete curr;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }
    ~ShipList() {
        ShipNode *curr = head;
        while(curr != NULL) {
            ShipNode *temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
};

// Queue for re-entry
struct QueueNode {
    Ship *ship;
    QueueNode *next;
};

class ShipQueue {
public:
    QueueNode *front;
    QueueNode *rear;
    ShipQueue() : front(NULL), rear(NULL) {}
    void enqueue(Ship *s) {
        QueueNode *node = new QueueNode;
        node->ship = s;
        node->next = NULL;
        if(rear == NULL) {
            front = rear = node;
        } else {
            rear->next = node;
            rear = node;
        }
    }
    Ship* dequeue() {
        if(front == NULL) return NULL;
        QueueNode *temp = front;
        Ship *s = temp->ship;
        front = front->next;
        if(front == NULL) rear = NULL;
        delete temp;
        return s;
    }
    bool isEmpty() {
        return front == NULL;
    }
    ~ShipQueue() {
        while(!isEmpty())
            dequeue();
    }
};

// ------------------------------------------------------------------------

class Battleship : public Ship, public MovingShip, public ShootingShip, public SeeingRobot {
public:
    Battleship(string team, char sym, string name)
      : Ship(team, "Battleship", sym, name) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << teamName << " " << shipName << " takes turn (Battleship)" << endl;
        look(0, 0, bf, logFile);    // Look at current surroundings
        int dxs[4] = {0, 0, -1, 1};
        int dys[4] = {-1, 1, 0, 0};
        int indices[4] = {0,1,2,3};
        for (int i = 3; i > 0; i--) {
            int j = randRange(0, i);
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
        bool moved = false;
        int oldX = posX, oldY = posY;
        for (int i = 0; i < 4; i++){
            int idx = indices[i];
            if (move(dxs[idx], dys[idx], bf)) {
                logFile << shipName << " at (" << oldX << "," << oldY << ") moved to (" << posX << "," << posY << ")" << endl;
                moved = true;
                break;
            }
        }
        if (!moved)
            logFile << shipName << " at (" << oldX << "," << oldY << ") failed to move" << endl;
        
        for (int shot = 0; shot < 2; shot++){   // Shoot twice at random positions
            int targetX = randRange(0, bf.width - 1);
            int targetY = randRange(0, bf.height - 1);
            if(targetX == posX && targetY == posY) { shot--; continue; }
            bool hit = shoot(targetX, targetY, bf);
            logFile << teamName << " " << shipName << " shoots at (" << targetX << "," << targetY << ")";
            if(hit)
                logFile << " and hits!" << endl;
            else
                logFile << " and misses." << endl;
        }
    }
    
    virtual bool move(int dx, int dy, Battlefield &bf) {
        int newX = posX + dx;
        int newY = posY + dy;
        if (!bf.isValidPosition(newX, newY))
            return false;

        if (bf.isOccupied(newX, newY)) {
            if (killCount < 4) 
                return false;
            Ship *target = bf.occupancy[newY][newX];    //Upgraded can ram
            if (target != NULL && target->teamName != teamName) {
                target->lives = 0; // Destroy enemy
                killCount++;
            }
        }
        // Detect water
        if (!bf.isWater(newX, newY))
            return false;
        bf.updateOccupancy(posX, posY, newX, newY, this);
        posX = newX;
        posY = newY;
        return true;
    }
    
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) {
        return defaultShoot(this, targetX, targetY, bf, true);
    }
    
    virtual void look(int dx, int dy, Battlefield &bf, ofstream &logFile) {
        int centerX = posX + dx;
        int centerY = posY + dy;
        cout << shipName << " looks around:" << endl;
        logFile << shipName << " looks around:" << endl;
        for (int i = centerY - 1; i <= centerY + 1; i++){
            for (int j = centerX - 1; j <= centerX + 1; j++){
                if (bf.isValidPosition(j, i)) {
                    if (bf.occupancy[i][j] != NULL) {
                        cout << bf.occupancy[i][j]->symbol << " ";
                        logFile << bf.occupancy[i][j]->symbol << " ";
                    } else {
                        cout << bf.grid[i][j] << " ";
                        logFile << bf.grid[i][j] << " ";
                    }
                } else {
                    cout << "X ";
                    logFile << "X ";
                }
            }
            cout << endl;
            logFile << endl;
        }
    }
};

// ------------------------------------------------------------------------
class Cruiser : public Ship, public MovingShip, public RamShip, public SeeingRobot {
public:
    Cruiser(string team, char sym, string name)
      : Ship(team, "Cruiser", sym, name) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << shipName << " takes turn (Cruiser)" << endl;
        look(0, 0, bf, logFile); // Look around
        // Enemy found adjacently, can ram
        int dxs[4] = {0, 0, -1, 1};
        int dys[4] = {-1, 1, 0, 0};
        bool acted = false;
        for (int i = 0; i < 4; i++){
            int newX = posX + dxs[i];
            int newY = posY + dys[i];
            if (bf.isValidPosition(newX, newY) && bf.isOccupied(newX, newY)) {
                Ship *target = bf.occupancy[newY][newX];
                if (target != NULL && target->teamName != teamName) {
                    int oldX = posX, oldY = posY;
                    destroy(dxs[i], dys[i], bf);
                    logFile << shipName << " at (" << oldX << "," << oldY << ") rams enemy at (" << newX << "," << newY << ")" << endl;
                    acted = true;
                    break;
                }
            }
        }
        // No enemy adjacent, try random directions
        if (!acted) {
            int dxs2[4] = {0, 0, -1, 1};
            int dys2[4] = {-1, 1, 0, 0};
            int indices[4] = {0,1,2,3};
            for (int i = 3; i > 0; i--) {
                int j = randRange(0, i);
                int temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
            bool moved = false;
            int oldX = posX, oldY = posY;
            for (int i = 0; i < 4; i++){
                int idx = indices[i];
                if (move(dxs2[idx], dys2[idx], bf)) {
                    logFile << shipName << " at (" << oldX << "," << oldY << ") moved to (" << posX << "," << posY << ")" << endl;
                    moved = true;
                    break;
                }
            }
            if (!moved)
                logFile << shipName << " at (" << oldX << "," << oldY << ") failed to move" << endl;
        }
    }
    
    virtual bool move(int dx, int dy, Battlefield &bf) {
        int newX = posX + dx;
        int newY = posY + dy;
        if (!bf.isValidPosition(newX, newY))
            return false;
        if (bf.isOccupied(newX, newY))  //Cannot enter occupied cell
            return false;
        if (!bf.isWater(newX, newY))
            return false;
        bf.updateOccupancy(posX, posY, newX, newY, this);
        posX = newX;
        posY = newY;
        return true;
    }
    
    virtual bool destroy(int dx, int dy, Battlefield &bf) {
        int targetX = posX + dx;
        int targetY = posY + dy;
        if (!bf.isValidPosition(targetX, targetY))
            return false;
        Ship *target = bf.occupancy[targetY][targetX];
        if (target != NULL && target->teamName != teamName) {
            target->lives = 0;
            killCount++;
            
            bf.updateOccupancy(posX, posY, targetX, targetY, this); // Move into the enemy’s cell
            posX = targetX;
            posY = targetY;
            return true;
        }
        return false;
    }
    
    virtual void look(int dx, int dy, Battlefield &bf, ofstream &logFile) {
        int centerX = posX + dx;
        int centerY = posY + dy;
        cout << shipName << " looks around:" << endl;
        logFile << shipName << " looks around:" << endl;
        for (int i = centerY - 1; i <= centerY + 1; i++){
            for (int j = centerX - 1; j <= centerX + 1; j++){
                if (bf.isValidPosition(j, i)) {
                    if (bf.occupancy[i][j] != NULL) {
                        cout << bf.occupancy[i][j]->symbol << " ";
                        logFile << bf.occupancy[i][j]->symbol << " ";
                    } else {
                        cout << bf.grid[i][j] << " ";
                        logFile << bf.grid[i][j] << " ";
                    }
                } else {
                    cout << "X ";
                    logFile << "X ";
                }
            }
            cout << endl;
            logFile << endl;
        }
    }
};

// ------------------------------------------------------------------------
class Destroyer : public Ship, public MovingShip, public ShootingShip, public RamShip, public SeeingRobot {
public:
    Destroyer(string team, char sym, string name)
      : Ship(team, "Destroyer", sym, name) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << shipName << " takes turn (Destroyer)" << endl;
        look(0, 0, bf, logFile);
        int dxs[4] = {0, 0, -1, 1}; // Check directions in random order
        int dys[4] = {-1, 1, 0, 0};
        int indices[4] = {0,1,2,3};
        for (int i = 3; i > 0; i--) {
            int j = randRange(0, i);
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
        bool moved = false;
        int oldX = posX, oldY = posY;
        for (int i = 0; i < 4; i++){
            int idx = indices[i];
            if (move(dxs[idx], dys[idx], bf)) {
                logFile << shipName << " at (" << oldX << "," << oldY << ") moved to (" << posX << "," << posY << ")" << endl;
                moved = true;
                break;
            }
        }
        if (!moved)
            logFile << shipName << " at (" << oldX << "," << oldY << ") failed to move" << endl;
    
        for (int shot = 0; shot < 2; shot++){  // Shoot twice at random positions
            int targetX = randRange(0, bf.width - 1);
            int targetY = randRange(0, bf.height - 1);
            if(targetX == posX && targetY == posY) { shot--; continue; }
            bool hit = shoot(targetX, targetY, bf);
            logFile << shipName << " shoots at (" << targetX << "," << targetY << ")";
            if(hit)
                logFile << " and hits!" << endl;
            else
                logFile << " and misses." << endl;
        }
    }
    
    virtual bool move(int dx, int dy, Battlefield &bf) {
        int newX = posX + dx;
        int newY = posY + dy;
        if (!bf.isValidPosition(newX, newY))
            return false;  // Allow to move into enemy cell
        if (bf.isOccupied(newX, newY)) {
            Ship *target = bf.occupancy[newY][newX];
            if (target != NULL && target->teamName != teamName) {
                target->lives = 0;
                killCount++;
            }
        }
        if (!bf.isWater(newX, newY))
            return false;
        bf.updateOccupancy(posX, posY, newX, newY, this);
        posX = newX;
        posY = newY;
        return true;
    }
    
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) {
        return defaultShoot(this, targetX, targetY, bf);
    }
    
    virtual bool destroy(int dx, int dy, Battlefield &bf) {
        int targetX = posX + dx;
        int targetY = posY + dy;
        if (!bf.isValidPosition(targetX, targetY))
            return false;
        Ship *target = bf.occupancy[targetY][targetX];
        if (target != NULL && target->teamName != teamName) {
            target->lives = 0;
            killCount++;
            bf.updateOccupancy(posX, posY, targetX, targetY, this);
            posX = targetX;
            posY = targetY;
            return true;
        }
        return false;
    }
    
    virtual void look(int dx, int dy, Battlefield &bf, ofstream &logFile) {
        int centerX = posX + dx;
        int centerY = posY + dy;
        cout << shipName << " looks around:" << endl;
        logFile << shipName << " looks around:" << endl;
        for (int i = centerY - 1; i <= centerY + 1; i++){
            for (int j = centerX - 1; j <= centerX + 1; j++){
                if (bf.isValidPosition(j, i)) {
                    if (bf.occupancy[i][j] != NULL) {
                        cout << bf.occupancy[i][j]->symbol << " ";
                        logFile << bf.occupancy[i][j]->symbol << " ";
                    } else {
                        cout << bf.grid[i][j] << " ";
                        logFile << bf.grid[i][j] << " ";
                    }
                } else {
                    cout << "X ";
                    logFile << "X ";
                }
            }
            cout << endl;
            logFile << endl;
        }
    }
};

//-----------------------------------------------------------------------------
class Frigate : public Ship, public ShootingShip {
public:
    int shootIndex; // used to cycle 8 neighbouring directions
    Frigate(string team, char sym, string name)
      : Ship(team, "Frigate", sym, name), shootIndex(0) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << shipName << " takes turn (Frigate)" << endl; // Fixed order:up, up-right, right, down-right, down, down-left, left, up-left
        int dxs[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        int dys[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
        int idx = shootIndex % 8;
        int targetX = posX + dxs[idx];
        int targetY = posY + dys[idx];
        if (!bf.isValidPosition(targetX, targetY)) {
            logFile << shipName << " attempts to shoot out-of-bounds at (" << targetX << "," << targetY << ") and fails." << endl;
        } else {
            bool hit = shoot(targetX, targetY, bf);
            logFile << teamName << " " << shipName << " shoots at (" << targetX << "," << targetY << ")";
            if(hit)
                logFile << " and hits!" << endl;
            else
                logFile << " and misses." << endl;
        }
        shootIndex++;
    }
    
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) {
        return defaultShoot(this, targetX, targetY, bf);
    }
};

// ------------------------------------------------
class Corvette : public Ship, public ShootingShip {
public:
    Corvette(string t, char sym, string name = "Corvette")
      : Ship(t, "Corvette", sym, name) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {  //Choose one of eight immediate direction randomly
        logFile << shipName << " takes turn (Corvette)" << endl;
        int dxOptions[8] = { -1,  0, 1,  1,  1,  0, -1, -1 };
        int dyOptions[8] = { -1, -1, -1,  0,  1,  1,  1,  0 };
        int idx = rand() % 8;
        int targetX = posX + dxOptions[idx];
        int targetY = posY + dyOptions[idx];
        bool hit = shoot(targetX, targetY, bf);
        logFile << shipName << " (Corvette) shoots at (" << targetX << "," << targetY << ")";
        if(hit)
            logFile << " and hits!" << endl;
        else
            logFile << " and misses." << endl;
    }
    
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) {
        return defaultShoot(this, targetX, targetY, bf);
    }
};

// --------------------------------------
class Amphibious : public Battleship {
public:
    Amphibious(string t, char sym, string name = "Amphibious") 
      : Battleship(t, sym, name) {                             // Inherit Battleship shooting behavior 
        shipType = "Amphibious";  // override type
    }
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << shipName << " takes turn (Amphibious)" << endl; 
        int dxOptions[4] = {0, 1, 0, -1};
        int dyOptions[4] = {-1, 0, 1, 0};
        int indices[4] = {0,1,2,3};
        for (int i = 3; i > 0; i--) {
            int j = randRange(0, i);
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
        bool moved = false;
        int oldX = posX, oldY = posY;
        for (int i = 0; i < 4; i++){
            int idx = indices[i];
            int nx = posX + dxOptions[idx];
            int ny = posY + dyOptions[idx];
            if (bf.isValidPosition(nx, ny) && bf.occupancy[ny][nx] == NULL) {   // For Amphibious, ignore water requirement
                bf.occupancy[ny][nx] = this;
                bf.occupancy[posY][posX] = NULL;
                posX = nx;
                posY = ny;
                logFile << shipName << " at (" << oldX << "," << oldY << ") moves to (" << nx << "," << ny << ") (island allowed)" << endl;
                moved = true;
                break;
            }
        }
        if (!moved)
            logFile << shipName << " at (" << oldX << "," << oldY << ") fails to move" << endl;
        for (int shot = 0; shot < 2; shot++){                                  // Shoot twice at random positions
            int targetX = randRange(0, bf.width - 1);
            int targetY = randRange(0, bf.height - 1);
            if(targetX == posX && targetY == posY) { shot--; continue; }
            bool hit = shoot(targetX, targetY, bf);
            logFile << shipName << " shoots at (" << targetX << "," << targetY << ")";
            if(hit)
                logFile << " and hits!" << endl;
            else
                logFile << " and misses." << endl;
        }
    }
    
};

// --------------------------------------------------------------------------------------------------------
class SuperShip : public Ship, public MovingShip, public ShootingShip, public SeeingRobot, public RamShip {
public:
    SuperShip(string t, char sym, string name = "SuperShip")   
      : Ship(t, "SuperShip", sym, name) {}
    
    virtual void takeTurn(Battlefield &bf, ofstream &logFile) {
        logFile << shipName << " takes turn (SuperShip)" << endl;   // Attempt to ram an adjacent enemy.
        int dxs[4] = {0, 1, 0, -1};
        int dys[4] = {-1, 0, 1, 0};
        bool moved = false;
        for (int i = 0; i < 4; i++){
            int nx = posX + dxs[i];
            int ny = posY + dys[i];
            if (bf.isValidPosition(nx, ny)) {
                Ship* target = bf.occupancy[ny][nx];
                if (target != NULL && target->teamName != teamName) {
                    int oldX = posX, oldY = posY;
                    target->lives = 0;
                    killCount++;
                    bf.updateOccupancy(posX, posY, nx, ny, this);
                    posX = nx;
                    posY = ny;
                    logFile << shipName << " at (" << oldX << "," << oldY << ") rams enemy at (" << nx << "," << ny << ")" << endl;
                    moved = true;
                    break;
                }
            }
        }
        if (!moved) {       // If no enemy is adjacent, try all directions in random order
            int dxs2[4] = {0, 1, 0, -1};
            int dys2[4] = {-1, 0, 1, 0};
            int indices[4] = {0,1,2,3};
            for (int i = 3; i > 0; i--) {
                int j = randRange(0, i);
                int temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
            int oldX = posX, oldY = posY;
            bool m = false;
            for (int i = 0; i < 4; i++){
                int idx = indices[i];
                if (move(dxs2[idx], dys2[idx], bf)) {
                    logFile << shipName << " at (" << oldX << "," << oldY << ") moves to (" << posX << "," << posY << ")" << endl;
                    m = true;
                    break;
                }
            }
            if (!m)
                logFile << shipName << " at (" << oldX << "," << oldY << ") fails to move" << endl;
        }
        
        for (int shot = 0; shot < 3; shot++){   // Fire three random shots anywhere on battlefield
            int tx = rand() % bf.width;
            int ty = rand() % bf.height;
            if (tx == posX && ty == posY) { shot--; continue; }
            bool hit = shoot(tx, ty, bf);
            logFile << shipName << " shoots at (" << tx << "," << ty << ")";
            if(hit)
                logFile << " and hits!" << endl;
            else
                logFile << " and misses." << endl;
        }
    }
    
    virtual bool move(int dx, int dy, Battlefield &bf) {
        int newX = posX + dx;
        int newY = posY + dy;
        if (!bf.isValidPosition(newX, newY)) return false;
        if (bf.occupancy[newY][newX] != NULL && bf.occupancy[newY][newX]->teamName != teamName) {
            bf.occupancy[newY][newX]->lives = 0; // ram the enemy
        }
        if (!bf.isWater(newX, newY)) return false;
        bf.updateOccupancy(posX, posY, newX, newY, this);
        posX = newX; posY = newY;
        return true;
    }
    
    virtual bool shoot(int targetX, int targetY, Battlefield &bf) {
        return defaultShoot(this, targetX, targetY, bf);
    }
    
    virtual void look(int dx, int dy, Battlefield &bf, ofstream &logFile) {
        int centerX = posX + dx;
        int centerY = posY + dy;
        cout << shipName << " looks around:" << endl;
        logFile << shipName << " looks around:" << endl;
        for (int i = centerY - 1; i <= centerY + 1; i++){
            for (int j = centerX - 1; j <= centerX + 1; j++){
                if (bf.isValidPosition(j, i)) {
                    if (bf.occupancy[i][j] != NULL) {
                        cout << bf.occupancy[i][j]->symbol << " ";
                        logFile << bf.occupancy[i][j]->symbol << " ";
                    } else {
                        cout << bf.grid[i][j] << " ";
                        logFile << bf.grid[i][j] << " ";
                    }
                } else {
                    cout << "X ";
                    logFile << "X ";
                }
            }
            cout << endl;
            logFile << endl;
        }
    }
    
    virtual bool destroy(int dx, int dy, Battlefield &bf) {
        int targetX = posX + dx;
        int targetY = posY + dy;
        if (!bf.isValidPosition(targetX, targetY)) return false;
        Ship* target = bf.occupancy[targetY][targetX];
        if (target != NULL && target->teamName != teamName) {
            target->lives = 0;
            bf.updateOccupancy(posX, posY, targetX, targetY, this);
            posX = targetX;
            posY = targetY;
            return true;
        }
        return false;
    }
};

//------------------------------------------------------------
bool placeShipRandomly(Battlefield &bf, Ship *ship) {        
    for (int attempt = 0; attempt < 100; attempt++){         // Try multiple attempts
        int x = randRange(0, bf.width - 1);
        int y = randRange(0, bf.height - 1);
        if (bf.isWater(x, y) && !bf.isOccupied(x, y)) {
            bf.placeShip(x, y, ship);
            return true;
        }
    }
    return false;
}

// ************************************************************************
int main() {                                                // Main simulation
    srand((unsigned)time(0));
    ifstream infile("game1.txt");
    if (!infile) {
        cout << "Error opening game.txt file." << endl;
        return 1;
    }
    ofstream logFile("simulation.log");
    
    string token;                                           // Read parameters from file
    int iterations = 0;
    int width = 0, height = 0;
    infile >> token;         // iterations
    infile >> iterations;
    infile >> token;         // width
    infile >> width;
    infile >> token;         //height
    infile >> height;
    

    Battlefield bf(width, height);                        // Create battlefield
    
    string teamNameA;                                     // Read Team A info
    int numShipTypesA;
    infile >> token;                                      
    infile >> teamNameA;
    infile >> numShipTypesA;
    
    struct ShipInfo {                                    // Structure for ship type info
        string type;
        char symbol;
        int count;
    };
    ShipInfo *teamAShips = new ShipInfo[numShipTypesA];
    for (int i = 0; i < numShipTypesA; i++){
        infile >> teamAShips[i].type >> teamAShips[i].symbol >> teamAShips[i].count;
    }
    
    string teamNameB;                                      // Read Team B info
    int numShipTypesB;
    infile >> token;                                      
    infile >> teamNameB;
    infile >> numShipTypesB;
    ShipInfo *teamBShips = new ShipInfo[numShipTypesB];
    for (int i = 0; i < numShipTypesB; i++){
        infile >> teamBShips[i].type >> teamBShips[i].symbol >> teamBShips[i].count;
    }
    
    for (int i = 0; i < height; i++){                  // Read the Battlefield Grid
        for (int j = 0; j < width; j++){
            infile >> bf.grid[i][j];
        }
    }
    infile.close();
    
    // active ship list and re-entry queue
    ShipList activeShips;
    ShipQueue reentryQueue;
    
    // ----------------------------------------
    int *countersA = new int[numShipTypesA];    //Create ship  for team A
    for (int i = 0; i < numShipTypesA; i++){
        countersA[i] = 0;
    }
    for (int i = 0; i < numShipTypesA; i++){
        for (int j = 0; j < teamAShips[i].count; j++){
            countersA[i]++;
            // Name formatting
            string name = teamNameA + "-" + teamAShips[i].type + "_" + to_string(countersA[i]);
            Ship *s = NULL;
            if (teamAShips[i].type == "Battleship")
                s = new Battleship(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "Cruiser")
                s = new Cruiser(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "Destroyer")
                s = new Destroyer(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "Frigate")
                s = new Frigate(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "Corvette")
                s = new Corvette(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "Amphibious")
                s = new Amphibious(teamNameA, teamAShips[i].symbol, name);
            else if (teamAShips[i].type == "SuperShip")
                s = new SuperShip(teamNameA, teamAShips[i].symbol, name);
            if (s != NULL) {
                if (placeShipRandomly(bf, s))
                    activeShips.addShip(s);
                else {
                    logFile << "Failed to place ship " << s->shipName << endl;
                    delete s;
                }
            }
        }
    }
    delete [] countersA;
    
    // ----------------------------------------
    int *countersB = new int[numShipTypesB];   //Create ships for team B
    for (int i = 0; i < numShipTypesB; i++){
        countersB[i] = 0;
    }
    for (int i = 0; i < numShipTypesB; i++){
        for (int j = 0; j < teamBShips[i].count; j++){
            countersB[i]++;
            string name = teamNameB + "-" + teamBShips[i].type + "_" + to_string(countersB[i]);
            Ship *s = NULL;
            if (teamBShips[i].type == "Battleship")
                s = new Battleship(teamNameB, teamBShips[i].symbol, name);
            else if (teamBShips[i].type == "Cruiser")
                s = new Cruiser(teamNameB, teamBShips[i].symbol, name);
            if (s != NULL) {       // Team B may have lesser ship types
                if (placeShipRandomly(bf, s))
                    activeShips.addShip(s);
                else {
                    logFile << "Failed to place ship " << s->shipName << endl;
                    delete s;
                }
            }
        }
    }
    delete [] countersB;
    
    delete [] teamAShips;
    delete [] teamBShips;
    
    logFile << "Initial Battlefield:" << endl;     //Initial battlefield AFTER ship is placed
    bf.display(logFile);
    
    // ************************************************************************
    // Simulation looping    
    for (int turn = 1; turn <= iterations; turn++){                                        
        logFile << "Turn " << turn << endl;                                      
        cout << "Turn " << turn << endl;
        
        // Process re-entry at the start of each turn
        for (int i = 0; i < 2; i++){
            if (!reentryQueue.isEmpty()){
                Ship *s = reentryQueue.dequeue();
                
                if (s->reentriesLeft > 0) {     // Only re-enter if there are reentries left
                    s->lives = 3;
                    activeShips.addShip(s);
                    if (placeShipRandomly(bf, s)) {
                        logFile << s->shipName << " re-enters the battlefield. (Reentries left: " << s->reentriesLeft << ")" << endl;
                    } else {
                        logFile << "Failed to place ship " << s->shipName << " on re-entry." << endl;
                    }
                } else {
                    logFile << s->shipName << " has no re-entries left and is permanently destroyed." << endl;
                    delete s;
                }
            }
        }
        
        // **********************************************
        // Upgrading the Ships
        {
            ShipNode *curr = activeShips.head;
            while (curr != NULL) {
                Ship *s = curr->ship;
                if (s->lives > 0) {
                    if (s->shipType == "Battleship" && s->killCount >= 4) {         //Battleship upgrade to Destroyer
                        string baseName = s->shipName;
                        string newName = s->teamName + "-Destroyer";
                        Ship *upgraded = new Destroyer(s->teamName, s->symbol, newName);
                        upgraded->killCount = 0;
                        upgraded->reentriesLeft = s->reentriesLeft;
                        upgraded->lives = s->lives;
                        upgraded->posX = s->posX;
                        upgraded->posY = s->posY;
                        bf.occupancy[s->posY][s->posX] = upgraded;
                        curr->ship = upgraded;
                        logFile << baseName << " has upgraded to " << newName << "!" << endl;
                        delete s;
                    } else if (s->shipType == "Cruiser" && s->killCount >= 3) {     //Cruiser upgrade to Destroyer
                        string baseName = s->shipName;
                        string newName = s->teamName + "-Destroyer";
                        Ship *upgraded = new Destroyer(s->teamName, s->symbol, newName);
                        upgraded->killCount = 0;
                        upgraded->reentriesLeft = s->reentriesLeft;
                        upgraded->lives = s->lives;
                        upgraded->posX = s->posX;
                        upgraded->posY = s->posY;
                        bf.occupancy[s->posY][s->posX] = upgraded;
                        curr->ship = upgraded;
                        logFile << baseName << " has upgraded to " << newName << "!" << endl;
                        delete s;
                    } else if (s->shipType == "Frigate" && s->killCount >= 3) {     //Frigate upgrade to Corvette
                        string baseName = s->shipName;
                        string newName = s->teamName + "-Corvette";
                        Ship *upgraded = new Corvette(s->teamName, s->symbol, newName);
                        upgraded->killCount = 0;
                        upgraded->reentriesLeft = s->reentriesLeft;
                        upgraded->lives = s->lives;
                        upgraded->posX = s->posX;
                        upgraded->posY = s->posY;
                        bf.occupancy[s->posY][s->posX] = upgraded;
                        curr->ship = upgraded;
                        logFile << baseName << " has upgraded to " << newName << "!" << endl;
                        delete s;
                    } else if (s->shipType == "Destroyer" && s->killCount >= 3) {       //Destroyer upgrade to SuperShip
                        string baseName = s->shipName;
                        string newName = s->teamName + "-SuperShip";
                        Ship *upgraded = new SuperShip(s->teamName, s->symbol, newName);
                        upgraded->killCount = 0;
                        upgraded->reentriesLeft = s->reentriesLeft;
                        upgraded->lives = s->lives;
                        upgraded->posX = s->posX;
                        upgraded->posY = s->posY;
                        bf.occupancy[s->posY][s->posX] = upgraded;
                        curr->ship = upgraded;
                        logFile << baseName << " has upgraded to " << newName << "!" << endl;
                        delete s;
                    } else if (s->shipType == "Amphibious" && s->killCount >= 4) {      //Amphibious upgrade to SuperShip
                        string baseName = s->shipName;  
                        string newName = s->teamName + "-SuperShip";
                        Ship *upgraded = new SuperShip(s->teamName, s->symbol, newName);
                        upgraded->killCount = 0;
                        upgraded->reentriesLeft = s->reentriesLeft;
                        upgraded->lives = s->lives;
                        upgraded->posX = s->posX;
                        upgraded->posY = s->posY;
                        bf.occupancy[s->posY][s->posX] = upgraded;
                        curr->ship = upgraded;
                        logFile << baseName << " has upgraded to " << newName << "!" << endl;
                        delete s;
                    }
                }
                curr = curr->next;
            }
        }
        
        // Each active ship take turn
        {
            ShipNode *curr = activeShips.head;
            while (curr != NULL) {
                Ship *s = curr->ship;
                if (s->lives > 0)
                    s->takeTurn(bf, logFile);
                curr = curr->next;
            }
        }
        
        // Mark destroyed ships and remove from active list
        {
            ShipNode *next;
            ShipNode *curr = activeShips.head;
            while (curr != NULL) {
                next = curr->next;
                if (curr->ship->lives <= 0) {
                    if (curr->ship->reentriesLeft > 0) {
                        curr->ship->reentriesLeft--; // use up one re-entry
                        logFile << curr->ship->shipName << " is destroyed and will re-enter later. (Reentries left: " 
                                << curr->ship->reentriesLeft << ")" << endl;
                        bf.removeShip(curr->ship->posX, curr->ship->posY);
                        if (curr->ship->reentriesLeft > 0) {
                            curr->ship->lives = 3; // reset for re-entry
                            reentryQueue.enqueue(curr->ship);
                        } else {
                            logFile << curr->ship->shipName << " has no re-entries left and is permanently destroyed." << endl;
                            delete curr->ship;
                        }
                        activeShips.removeShip(curr->ship);
                    } else {
                        logFile << curr->ship->shipName << " is permanently destroyed." << endl;
                        bf.removeShip(curr->ship->posX, curr->ship->posY);
                        activeShips.removeShip(curr->ship);
                        delete curr->ship;
                    }
                }
                curr = next;
            }
        }
        
        bf.display(logFile);    // Display Battlefield
        
        // Win condition
        {
            bool teamAExists = false, teamBExists = false;
            ShipNode *curr = activeShips.head;
            while (curr != NULL) {
                if (curr->ship->teamName == teamNameA)
                    teamAExists = true;
                if (curr->ship->teamName == teamNameB)
                    teamBExists = true;
                curr = curr->next;
            }
            if (!teamAExists || !teamBExists) {
                cout << "Game Over at turn " << turn << endl;
                logFile << "Game Over at turn " << turn << endl;
                break;
            }
        }
    }
    
    //Cleanup the re-entry
    while (!reentryQueue.isEmpty()) {
        Ship *s = reentryQueue.dequeue();
        delete s;
    }
    
    //End-Game Statistics
    {
        int countA = 0, countB = 0;
        ShipNode *curr = activeShips.head;
        while (curr != NULL) {
            if (curr->ship->teamName == teamNameA)
                countA++;
            if (curr->ship->teamName == teamNameB)
                countB++;
            curr = curr->next;
        }
        logFile << "\n--- Game Statistics ---" << endl;
        logFile << "Team " << teamNameA << " ships remaining: " << countA << endl;
        logFile << "Team " << teamNameB << " ships remaining: " << countB << endl;
        if (countA > countB)
            logFile << "Team " << teamNameA << " wins!" << endl;
        else if (countB > countA)
            logFile << "Team " << teamNameB << " wins!" << endl;
        else
            logFile << "The game is a draw!" << endl;
            
        cout << "\n--- Game Statistics ---" << endl;
        cout << "Team " << teamNameA << " ships remaining: " << countA << endl;
        cout << "Team " << teamNameB << " ships remaining: " << countB << endl;
        if (countA > countB)
            cout << "Team " << teamNameA << " wins!" << endl;
        else if (countB > countA)
            cout << "Team " << teamNameB << " wins!" << endl;
        else
            cout << "The game is a draw!" << endl;
    }
    
    // Clean up Active list
    {
        ShipNode *node = activeShips.head;
        while (node != NULL) {
            Ship *s = node->ship;
            delete s;
            node = node->next;
        }
    }
    
    logFile.close();
    return 0;
}
