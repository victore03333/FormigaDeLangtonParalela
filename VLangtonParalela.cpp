//
// Created by victor-manoel on 06/08/24.
//

#include "VLangtonParalela.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <unistd.h>

#define JANELA_X 500
#define JANELA_Y 500

VLangtonParalela::VLangtonParalela(int tamanhoMatriz, const std::string& regras, int &argc, char **argv){
    this->argc = argc;
    this->argv = argv;
    this->tamanhoMatriz = tamanhoMatriz;
    this->regras = regras;

    metadeMatriz = tamanhoMatriz / 2;
    metadeDaMetadeMatriz = metadeMatriz/2;

    //setando as posições das formigas
    Formiga aux{};
    aux.direcao = 'c';
    //formiga 1
    aux.x = metadeDaMetadeMatriz;
    aux.y = metadeDaMetadeMatriz;
    formigas.insert(formigas.end(), aux);

    //formiga 2
    aux.x = metadeMatriz + metadeDaMetadeMatriz;
    aux.y = metadeDaMetadeMatriz;
    formigas.insert(formigas.end(), aux);


    //formiga 3
    aux.x = metadeDaMetadeMatriz;
    aux.y = metadeMatriz + metadeDaMetadeMatriz;
    formigas.insert(formigas.end(), aux);

    //formiga 4
    aux.x = metadeMatriz + metadeDaMetadeMatriz;
    aux.y = metadeMatriz + metadeDaMetadeMatriz;
    formigas.insert(formigas.end(), aux);

    passos = 100000;
    passoAtual = 0;

}

void VLangtonParalela::exec() {
    exec_paralelo();
}

//metodos privados
void VLangtonParalela::exec_paralelo() {
    MPI_Init(&argc, &argv);

    int comecar;


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    formiga = formigas[rank]; //inicializa a posição das formigas

    subGrade.redimencionar(pow(tamanhoMatriz, 2)/4, 0); //redimenciona e inicializa cada submatriz

    if(rank == 0){
        gradeTotal.redimencionar(pow(tamanhoMatriz, 2));
    }

    MPI_Gather(subGrade.dados(), pow(tamanhoMatriz, 2)/4, MPI_INT, gradeTotal.dados(), pow(tamanhoMatriz, 2)/4, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        sf::RenderWindow window(sf::VideoMode(JANELA_X, JANELA_Y), "Formiga");

        //numeros randomicos
        std::random_device rd;   // gerador não deterministico
        std::mt19937 gen(rd());  // gera uma semente
        std::uniform_int_distribution<> dist(0,255);//define os números aleatórios

        int i = 0;
        while (i<4){//número de formigas
            std::vector<sf::Color> aux;
            for(int i=0; i<regras.length(); i++){
                sf::Color color(dist(gen), dist(gen), dist(gen), dist(gen));
                aux.insert(aux.end(), color);
            }
            corExistentes.insert(corExistentes.end(), aux);
            i++;
        }

        //divido o numero de pixel de x e y pela quantidade de quadrados que eu quero
        //para definir o tamanho do quadrado
        float tam_x = JANELA_X/tamanhoMatriz;
        float tam_y = JANELA_Y/tamanhoMatriz;

        //instaciar quadrados
        sf::RectangleShape quadrado(sf::Vector2f(tam_x, tam_y));
        std::vector<sf::RectangleShape> linha_de_pixel;

        float posicao_x;    //variavel para guardar a proxima posição onde um quadrado deve ficar
        float posicao_y = 0;
        for(int i=0; i<tamanhoMatriz; i++){
            posicao_x = 0;
            pixel.insert(pixel.end(), linha_de_pixel);
            for(int j=0; j<tamanhoMatriz; j++){
                pixel[i].insert(pixel[i].end(), quadrado);
                pixel[i][j].setPosition(posicao_x, posicao_y);//define a posição do quadrado

                if(i < metadeMatriz && j < metadeMatriz)
                    pixel[i][j].setFillColor(corExistentes[0][0]);//setar as cor do primeiro quadrante
                if(i >= metadeMatriz && j < metadeMatriz)
                    pixel[i][j].setFillColor(corExistentes[1][0]);//setar as cor do segundo quadrante
                if(i < metadeMatriz && j >= metadeMatriz)
                    pixel[i][j].setFillColor(corExistentes[2][0]);//setar as cor do terceiro quadrante
                if(i >= metadeMatriz && j >= metadeMatriz)
                    pixel[i][j].setFillColor(corExistentes[3][0]);//setar as cor do quarto quadrante

                posicao_x+=tam_x;//pŕoxima posição no eixo x
            }
            posicao_y+=tam_y;//proxima posição no eixo y
        }
        comecar = false;
        for(int i=1; i<size; i++){//avisa outras máquinas que a parte grafica está pronta para execução
            MPI_Send(&comecar, 1, MPI_INT, i, 0,MPI_COMM_WORLD);
        }

        int cont=0;
        while (window.isOpen())
        {

            window.clear();

            if(passoAtual == passos) {
                window.close();//termina o programa caso tenha dado o número de passos
                break;
            }

            formigas[0] = formiga;


            for(int i=1; i<size; i++){
                MPI_Recv(&formigas[i], sizeof(Formiga), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for(int i=0; i<size; i++) {//salva a cor da posição atual
                formigas[i].corPsicaoAtual = gradeTotal[{formigas[i].x, formigas[i].y}];
                if((formigas[i].x > 0 && formigas[i].x < tamanhoMatriz) && (formigas[i].y > 0 && formigas[i].y < tamanhoMatriz)) {
                    switch (formigas[i].direcao) {
                        case 'c':
                            formigas[i].corDoProx = gradeTotal[{formigas[i].x, formigas[i].y - 1}];
                            break;
                        case 'b':
                            formigas[i].corDoProx = gradeTotal[{formigas[i].x, formigas[i].y + 1}];
                            break;
                        case 'd':
                            formigas[i].corDoProx = gradeTotal[{formigas[i].x + 1, formigas[i].y}];
                            break;
                        case 'e':
                            formigas[i].corDoProx = gradeTotal[{formigas[i].x - 1, formigas[i].y}];
                            break;
                    }
                }
            }

            for(int i=1; i<size; i++){
                MPI_Send(&formigas[i], sizeof(Formiga), MPI_BYTE, i, 0, MPI_COMM_WORLD);
            }
            formiga = formigas[0];

            passoDaFormiga();

            formigas[0] = formiga;
            for(int i=1; i<size; i++){
                MPI_Recv(&formigas[i], sizeof(Formiga), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for(int i=0; i<size; i++)//copia a modificação para a matriz total
                gradeTotal[{formigas[i].anteriorX, formigas[i].anteriorY}] = formigas[i].corPsicaoAtual;

            for(int i=0; i<size; i++) {//copia a modificação para a matriz total
                std::cout << "Cor atual: " << formigas[i].corPsicaoAtual << std::endl;
                std::cout << "posicao anterior x: " << formigas[i].anteriorX << " posicao anterior y: "
                          << formigas[i].anteriorY << std::endl;
                std::cout << "passo: " << passoAtual << " | " << "posicao:" << i << " valorx: " << formigas[i].x
                          << " valory: " << formigas[i].y << std::endl << "Direcao: " << formigas[i].direcao
                          << std::endl << std::endl;
            }
            std::cout << "Passo: " << passoAtual << std::endl;
            std::cout << "----------------------------------------------------------" << std::endl;

            for (int i = 1; i < size; i++)
                MPI_Send(formigas.data(), sizeof(Formiga) * 4, MPI_BYTE, i, 0, MPI_COMM_WORLD);

            desenha(window);
            window.display();
            passoAtual++;
            //pause(2);

        }
    } else{
        MPI_Recv(&comecar, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Hello from rank %d of %d e %d\n", rank+1, size, comecar);
        formiga.x = formigas[rank].x;
        formiga.y = formigas[rank].y;
        //formiga.corPsicaoAtual = 0;

        for(int i=0; i<passos; i++){

            MPI_Send(&formiga, sizeof(Formiga), MPI_BYTE, 0, 0, MPI_COMM_WORLD);//mada a posição da formiga para rank 0
            MPI_Recv(&formiga, sizeof(Formiga), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            passoDaFormiga();
            //std:: cout << "passo: " << i << std::endl;
            //std::cout << "rank: " << rank << "| (x,y): " << formiga.x << " " << formiga.y << std::endl << std::endl;

            MPI_Send(&formiga, sizeof(Formiga), MPI_BYTE, 0, 0, MPI_COMM_WORLD);//mada a posição da formiga para rank 0
            MPI_Recv(formigas.data(), sizeof(Formiga)*4, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);



        }
    }


    MPI_Finalize();
}

void VLangtonParalela::desenha(sf::RenderWindow &window) {
    for(int i=0; i< size; i++)
        pixel[formigas[i].anteriorX][formigas[i].anteriorY].setFillColor(corExistentes[i][formigas[i].corPsicaoAtual]);
    for(int i=0; i<tamanhoMatriz; i++){
        for(int j=0; j<tamanhoMatriz; j++){
            window.draw(pixel[i][j]);
        }
    }
}

void VLangtonParalela::passoDaFormiga() {
    int auxRegra;
    //mudar a cor
    if(formiga.corPsicaoAtual < regras.length() - 1)//verifica as
        formiga.corPsicaoAtual++;//muda a cor atual do quadrado
    else
        formiga.corPsicaoAtual=0;

    formiga.anteriorX = formiga.x;
    formiga.anteriorY = formiga.y;

    auxRegra = formiga.corDoProx;
    switch (formiga.direcao) {
        case 'c':
            if (formiga.y > 0) {
                formiga.y--;
            }
            switch (regras[auxRegra]) {//verifica qual a regra para girar
                case 'D':
                    formiga.direcao = 'd';
                    break;
                case 'E':
                    formiga.direcao = 'e';
                    break;
                default:
                    std::cout << "erroC";
            }
            break;
        case 'b':
            if (formiga.y < tamanhoMatriz - 1)
                formiga.y++;
            switch (regras[auxRegra]) {//verifica qual a regra para girar
                case 'D':
                    formiga.direcao = 'e';
                    break;
                case 'E':
                    formiga.direcao = 'd';
                    break;
                default:
                    std::cout << "erroB";
            }
            break;
        case 'd':
            if (formiga.x < tamanhoMatriz - 1)
                formiga.x++;
            switch (regras[auxRegra]) {//verifica qual a regra para girar
                case 'D':
                    formiga.direcao = 'b';
                    break;
                case 'E':
                    formiga.direcao = 'c';
                    break;
                default:
                    std::cout << "erroD";
            }
            break;
        case 'e':
            if (formiga.x > 0)
                formiga.x--;
            switch (regras[auxRegra]) {//verifica qual a regra para girar
                case 'D':
                    formiga.direcao = 'c';
                    break;
                case 'E':
                    formiga.direcao = 'b';
                    break;
                default:
                    std::cout << "erroE";
            }
            break;
        default:
            std::cout << "ErroE" << std::endl;
    }
}

void VLangtonParalela::pause(float delay1) {
    if (delay1<0.001)
        return; // pode ser ajustado e/ou evita-se valores negativos.

    float inst1=0, inst2=0;

    inst1 = (float)clock()/(float)CLOCKS_PER_SEC;

    while (inst2-inst1<delay1)
        inst2 = (float)clock()/(float)CLOCKS_PER_SEC;

}