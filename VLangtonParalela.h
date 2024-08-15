//
// Created by victor-manoel on 06/08/24.
//

#ifndef TRABALHO_VLANGTONPARALELA_H
#define TRABALHO_VLANGTONPARALELA_H
#include <vector>
#include <string>
#include <mpi/mpi.h>
#include <SFML/Graphics.hpp>
#include "MatrizContigua.h"
struct Formiga{
    int x, y;
    int anteriorX, anteriorY;
    char direcao;
    int corPsicaoAtual;
    int corDoProx;
};

class VLangtonParalela {
    MatrizContigua<int> subGrade;
    MatrizContigua<int> gradeTotal;
    std::string regras;
    int tamanhoMatriz;
    int metadeMatriz;
    int metadeDaMetadeMatriz;
    std::vector<Formiga> formigas;//
    Formiga formiga;
    int passos;
    int passoAtual;

    void exec_paralelo();
    int argc;
    char **argv;

    int rank, size;

    //SFML
    std::vector<std::vector<sf::RectangleShape>> pixel;
    std::vector<std::vector<sf::Color>> corExistentes;


    void desenha(sf::RenderWindow &window);

    void passoDaFormiga();

    void pause(float delay1);

        public:

    VLangtonParalela(int tamanhoMatriz, const std::string& regras, int &argc, char **argv);
    void exec();

};


#endif //TRABALHO_VLANGTONPARALELA_H
