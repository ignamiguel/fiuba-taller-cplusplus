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
const double PLAYER_SPEED = 1/1000 * MILLI_SEC_PER_FRAME;
/*

1     s -- 1000 ms
0.1   s --  100 ms
0.01  s --   10 ms
0.001 s --    1 ms
*/
const double INCREMENTO = 0.01;
const double TIME_DIFF = 0.001;

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

    virtual void mover() {};
    ~Entity() = default;
};

class Player : Entity {
public:
    double dist = 0;
    void mover() {
        this->dist += PLAYER_SPEED;
    };
};

class Enemy : Entity {
public:
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

    return new Entity(id, pos_x, type, level);
};

list<Entity *> getEntities() {
    const std::string ENTITY_FILE_PATH = "entidades.txt";
    list<Entity *> entidades;

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

        entidades.push_back(e);
    }

    myFile.close();
    return entidades;
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

bool areEquals(double a, double b) {
    return abs(a - b) < TIME_DIFF;
}

int main() {

    std::cout << "#1 Cargo entidades" << std::endl;
    list<Entity *> entities = getEntities();

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
        std::cout << "TIME: " << time << std::endl;
        
        // processInput();
        // update();
        // render();

        for(auto c: commands) {
            if (areEquals(c->time, time)) {
                std::cout << "ejectuar comando: " << c->id << std::endl;
            }
        }

        for(auto e: entities) {
            e->mover();
        }


        //std::cout << "TICK" << std::endl;
        sleep(MILLI_SEC_PER_FRAME);
        time += INCREMENTO;
    }

    // #4 Procesar resultado
    std::cout << "#4 Procesar resultado - Pendiente" << std::endl;
}