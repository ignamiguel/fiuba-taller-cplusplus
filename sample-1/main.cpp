#include <fstream>
#include <iostream>
#include "set"
#include "list"

using namespace std;

double STEP = 0.00001;
double PRECISION = 0.01;
double SIZE_ENEMY = 1;
char SEP = ' ';
char END_LINE = '\r';

class Entity {
    char type;
    int zPos;

public:
    Entity(string id, char type, double xPos, int zPos) {
        this->id = id;
        this->type = type;
        this->xPos = xPos;
        this->zPos = zPos;
        this->recorredX = -1;
        this->initialX = xPos;
    }
    void move() {
        xPos += STEP;
    }

    bool collition(Entity* enemy) {
        return zPos == enemy->zPos && (enemy->xPos - PRECISION < xPos && enemy->xPos + SIZE_ENEMY + PRECISION > xPos);
    }
    void setZPos(double newZPos) {
        this->zPos = newZPos;
    }
    double initialX;
    double xPos;
    double recorredX;
    string id;
};

class Command {
public:
    double time;
    string id;
    int zPos;
    bool used;
    Command(string id, double time, int zPos) {
        this->id = id;
        this->time = time;
        this->zPos = zPos;
        this->used = false;
    }
};

string getAndErase(string *linePointer) {
    string line = *linePointer;
    string value = line.substr(0, line.find(SEP));
    line.erase(0, line.find(SEP) +1);
    int hasEndLine = value.find(END_LINE);
    if (hasEndLine > 0) {
        value = value.substr(0, value.find(END_LINE));
    }
    *linePointer = line;
    return value;
}

void insertEntities(string path, set<Entity*>* players, set<Entity*>* enemies) {
    fstream myFile;
    string line;
    myFile.open(path);
    while (!myFile.eof() && myFile.is_open()) {
        getline(myFile, line);
        if (line.empty()) {
            continue;
        }
        string id = getAndErase(&line);
        double xPos = stod(getAndErase(&line));
        char type = getAndErase(&line).c_str()[0];
        int zPos = stoi(getAndErase(&line));
        if (type == 'P') {
            players->insert(new Entity(id, type, xPos, zPos));
        } else {
            enemies->insert(new Entity(id, type, xPos, zPos));
        }
    }
    myFile.close();
}

list<Command*> getCommands(string path) {
    fstream myFile;
    string line;
    list<Command*> commands;
    myFile.open(path);
    while (!myFile.eof() && myFile.is_open()) {
        getline(myFile, line);
        if (line.empty()) {
            continue;
        }
        double time = stod(getAndErase(&line));
        string id = getAndErase(&line);
        int zPos = stoi(getAndErase(&line));
        commands.push_back(new Command(id, time, zPos));
    }
    myFile.close();
    return commands;
}

double getLastEnemy(set<Entity*> enemies) {
    double maxPos = -10000;
    for (auto enemy : enemies) {
        if (enemy->xPos > maxPos) {
            maxPos = enemy->xPos;
        }
    }
    return maxPos;
}

double getFirstPlayer(set<Entity*>players) {
    double minPos = 10000;
    for (auto player : players) {
        if (player->xPos < minPos) {
            minPos = player->xPos;
        }
    }
    return minPos;
}

bool compare(Entity* first, Entity* second) {
    return first->recorredX < second->recorredX;
}

int main() {
    set<Entity*> players;
    set<Entity*> enemies;
    insertEntities("entidades.txt", &players, &enemies);
    list<Command*> commands = getCommands("comandos.txt");
    list<Entity*> playersEliminated;
    double start = clock();
    double maxEnemy = getLastEnemy(enemies);
    double minPlayer = getFirstPlayer(players);
    double time = 0;

    while (minPlayer < maxEnemy) {
        if (players.empty()) {
            break;
        }
        
        for (auto command: commands) {
            if (!command->used && command->time < time) {
                command->used = true;
                for (auto player: players) {
                    if (player->id == command->id) {
                        player->setZPos(command->zPos);
                    }
                }
            }
        }
        for (auto enemy : enemies) {
            for (auto player : players) {
                if (player->collition(enemy)) {
                    player->recorredX = player->xPos - player->initialX;
                    playersEliminated.push_back(player);
                    players.erase(player);
                }
            }
            if (players.empty()) {
                break;
            }
        }
        for (auto player: players) {
            player->move();
        }
        time += STEP;
        minPlayer += STEP;
    }

    playersEliminated.sort(compare);
    printf("ID RECORRED_X\n");
    for (auto player : playersEliminated) {
        printf("%s %.4f\n", player->id.c_str(), player->recorredX);
        delete(player);
    }
    for (auto player:players) {
        delete(player);
    }
    for (auto enemy: enemies) {
        delete(enemy);
    }
    printf("time elapsed: %f", (clock() - start) / CLOCKS_PER_SEC);
}