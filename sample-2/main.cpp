#include <iostream>
#include <fstream>
#include <string>

#include "list"

// For the clock
#include <chrono>
#include <ctime>

#include <thread>

// abs
#include <stdlib.h>

using namespace std;

const char SPACE_SEPARATOR = ' ';
const int MILLI_SEC_PER_FRAME = 10;
const double PLAYER_SPEED = 0.01;
/*

1     s -- 1000 ms
0.1   s --  100 ms
0.01  s --   10 ms
0.001 s --    1 ms
*/
const double INCREMENTO = 0.01;
const double TIME_DIFF_EPSILON = 0.001;
const double SPACE_DIFF_EPSILON = 0.01;

class Entity {
public:
    string id;
    double pos_x;
    char type;
    int level;

    Entity(string id, double pos_x, char type, int level) {
        this->id = id;
        this->pos_x = pos_x;
        this->type = type;
        this->level = level;
    };
    ~Entity() = default;

    virtual void mover() {};
    virtual double getPos() { return this->pos_x; };
};

class Player : public Entity {
public:
    Player(string id, double pos_x, char type, int level) 
    : Entity(id, pos_x, type, level) { };
    double dist = 0;
    bool killed = false;
    void mover() {
        this->dist += PLAYER_SPEED;
        this->pos_x += PLAYER_SPEED;
        std::cout << "P: " << this->id << " " << " L:" << this->level << " X: " << this->pos_x << std::endl;
    };

    void kill() {
        this->killed = true;
    }
};

class Enemy : public Entity {
public:
    Enemy(string id, double pos_x, char type, int level)
    : Entity(id, pos_x, type, level) { };
    int size = 1;
    void mover() {};
};

class Command {
public:
    double time;
    string id;
    int level;

    Command(double time, string id, int level) {
        this->time = time;
        this->id = id;
        this->level = level;
    }
    ~Command() = default;
};

Entity * getEntityFromLine(string input){
    string remanente = input;

    // P15 2.3 P 3
    string id = remanente.substr(0, remanente.find(SPACE_SEPARATOR));
    // std::cout << "id -> " << id << std::endl;

    // 2.3 P 3
    remanente = remanente.substr(remanente.find(SPACE_SEPARATOR) +1);
    double pos_x = stod(remanente.substr(0, remanente.find(SPACE_SEPARATOR)));
    // std::cout << "pos X -> " << pos_x << std::endl;
    
    // P 3
    remanente = remanente.substr(remanente.find(SPACE_SEPARATOR) +1);
    char type = remanente.substr(0, remanente.find(SPACE_SEPARATOR)).c_str()[0];
    // std::cout << "type -> " << type << std::endl;

    // 3
    remanente = remanente.substr(remanente.find(SPACE_SEPARATOR) +1);
    int level = stoi(remanente.substr(0, remanente.find(SPACE_SEPARATOR)));
    // std::cout << "level -> " << level << std::endl;

    if (type == 'P') {
       return new Player(id, pos_x, type, level);
    } else {
        return new Enemy(id, pos_x, type, level);
    }
};

void getEntities(list<Player *> *player, list<Enemy *> *enemies) {
    const std::string ENTITY_FILE_PATH = "entidades.txt";

    // Declaro un file stream "fstream"
    fstream myFile;
    string line;
    
    // Abro el file 
    myFile.open(ENTITY_FILE_PATH);

    while (!myFile.eof() && myFile.is_open()) {
        // Leo una linea
        getline(myFile, line);

        // registros vacíos?
        if (line.empty()) {
            continue;
        }
        
        Entity *e = getEntityFromLine(line);
        if (e->type == 'P') {
            player->push_back((Player *)e);
        } else {
            enemies->push_back((Enemy *)e);
        }
    }

    myFile.close();
};

Command * getCommandFromLine(string input) {
    string remanente = input;
    // std::cout << remanente << std::endl;

    // 1.1 P7 1
    double time = stod(remanente.substr(0, remanente.find(SPACE_SEPARATOR)));
    // std::cout << "TIME: " << time << std::endl;

    // P7 1
    remanente = remanente.substr(remanente.find(SPACE_SEPARATOR) +1);
    string id = remanente.substr(0, remanente.find(SPACE_SEPARATOR));
    // std::cout << "ID: " << id << std::endl;

    // 1
    remanente = remanente.substr(remanente.find(SPACE_SEPARATOR) +1);
    int level = stoi(remanente.substr(0, remanente.find(SPACE_SEPARATOR)));
    // std::cout << "LEVEL: " << level << std::endl;

    return new Command(time, id, level);
};

list<Command *> getCommands() {
    const std::string COMMAND_FILE_PATH = "comandos.txt";
    list<Command *> commands;

    fstream myFile;
    string line;

    myFile.open(COMMAND_FILE_PATH);

    while (!myFile.eof() && myFile.is_open())
    {
        getline(myFile, line);

        if(line.empty()) {
            continue;
        }

        Command *c = getCommandFromLine(line);
        commands.push_back(c);
    }

    myFile.close();
    
    return commands;
};

time_t getCurrentTime() {
    return time(0);
}

void sleep (int sleep_time) {
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

bool time_areEquals(double a, double b) {
    return abs(a - b) < TIME_DIFF_EPSILON;
}

bool space_areEqual(double a, double b) {
    return abs(a - b) < SPACE_DIFF_EPSILON;
}

void processCommand(Command *c, list<Player *> players) {
    for(auto p: players) {
        if (p->id == c->id) {
            p->level = c->level;
        }
    }
    std::cout << "Comando ejecutado" << std::endl;
}

void hit(Player *p, Enemy *e) {
    if(p->level != e->level) {
        return;
    }

    // Hit
    if(p->pos_x >= e->pos_x && p->pos_x <= (e->pos_x + 1)) {
        std::cout << "Player " << p->id << " " << p->dist << std::endl;
        p->kill();
    }
}

int main() {

    std::cout << "#1 Cargo entidades" << std::endl;
    list<Player *> players;
    list<Enemy *> enemies;
    getEntities(&players, &enemies);

    // for(auto e: entities) {
    //     // P15 2.3 P 3
    //     std::cout << "ID: " << e->id << " POS: "<< e->pos_x << " TYPE: " << e->type << " LEVEL: " << e->level << std::endl;
    // }

    std::cout << "#2 Cargo comandos" << std::endl;
    list<Command *> commands = getCommands();
    
    // for(auto c: commands) {
    //     std::cout << "TIME: " << c->time << " ID: " << c->id << " LEVEL: " << c->level << std::endl;
    // }

    std::cout << "#3 Actualizar posiciones - En progreso" << std::endl;

    // El juego se actualiza cada x tiempo en segundos
    // https://gameprogrammingpatterns.com/game-loop.html

    double time = 0;
    // Game loop
    while (true)
    {
        // std::cout << "TIME: " << time << std::endl;
        
        // processInput();
        // update();
        // render();

        for(auto c: commands) {
            if (time_areEquals(c->time, time)) {
                std::cout << "ejectuar comando sobre jugador " << c->id << std::endl;
                processCommand(c, players);
            }
        }

        for(auto p: players) {
            p->mover();

            for(auto e: enemies) {
                hit(p, e);
            }
        }

        //std::cout << "TICK" << std::endl;
        sleep(MILLI_SEC_PER_FRAME);
        time += INCREMENTO;
    }

    // #4 Procesar resultado
    std::cout << "#4 Procesar resultado - Pendiente" << std::endl;
}