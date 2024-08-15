#include <stdio.h>
#include <mpi.h>

#define n 10

int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

// vetor temporario para os processadores escravos
int a2[1000];

int main(int argc, char* argv[])
{

    int pid, np,
            elementos_por_processador,
            num_elementos_recebidos;
    // np -> numero de processadores
    // pid -> identificaÃ§Ã£o do processador

    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // processador mestre
    if (pid == 0) {
        int index, i;
        elementos_por_processador = n / np;

        // verifica se ha mais de 1 processador rodando
        if (np > 1) {
            // distribui a porÃ§Ã£o do vetor para cada processador filho para calcular a soma parcial
            for (i = 1; i < np - 1; i++) {
                index = i * elementos_por_processador;

                MPI_Send(&elementos_por_processador,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
                MPI_Send(&a[index],
                         elementos_por_processador,
                         MPI_INT, i, 0,
                         MPI_COMM_WORLD);
            }

            // o ultimo processador faz a adiÃ§Ã£o dos elementos remanecentes
            index = i * elementos_por_processador;
            int elementos_remanecentes = n - index;

            MPI_Send(&elementos_remanecentes,
                     1, MPI_INT,
                     i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&a[index],
                     elementos_remanecentes,
                     MPI_INT, i, 0,
                     MPI_COMM_WORLD);
        }

        // processador mestre faz a soma do seu prÃ³prio subvetor
        int sum = 0;
        for (i = 0; i < elementos_por_processador; i++)
            sum += a[i];

        // Coleta a soma parcial dos outros processadores
        int tmp;
        for (i = 1; i < np; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;

            sum += tmp;
        }

        // imprime a soma final do vetor
        printf("Soma do vetor eh : %d\n", sum);
    }
        // processadores escravos
    else {
        MPI_Recv(&num_elementos_recebidos,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        // armazena o segmento do vetor em vetor local a2
        MPI_Recv(&a2, num_elementos_recebidos,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        // calcula a soma parcial
        int partial_sum = 0;
        for (int i = 0; i < num_elementos_recebidos; i++)
            partial_sum += a2[i];

        // envia a soma parcial para o processador raiz
        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}