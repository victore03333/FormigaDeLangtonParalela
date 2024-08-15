//
// Created by victor-manoel on 07/08/24.
//

/* Essa classe foi construida pensando na necessidade de que,
 * para que haja um maior desempenho do programa, os dados devem
 * ser armazenados de maneira contígua. A abordagem
 * usando tamplete vector<vector<int>> faz com que os dados
 * sejam armazenados de maneira não contígua na memória, necessitando
 * de mais comunicação, e por consequência, sobrecarga do barramento
 * dessa maneira eu posso aproveitar dos recursos STL sem se preocupar com
 * memória não contígua da matriz*/

#ifndef TRABALHO_MATRIZCONTIGUA_H
#define TRABALHO_MATRIZCONTIGUA_H
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

template <typename T>
class MatrizContigua {
public:
    MatrizContigua(){}

    //SOBRECARGA DE OPERADOR PARA ESCRITA DOS DADOS
    T& operator[](std::pair<size_t, size_t> indices) {
        size_t linha = indices.first;
        size_t coluna = indices.second;
        if (linha >= linhas_ || coluna >= colunas_) {
            throw std::out_of_range("Índice fora do intervalo");
        }
        return dados_[linha * colunas_ + coluna];
    }

    //SOBRECARDA DE OPERADOR PARA LEITURA
    const T& operator[](std::pair<size_t, size_t> indices) const {
        size_t linha = indices.first;
        size_t coluna = indices.second;
        if (linha >= linhas_ || coluna >= colunas_) {
            throw std::out_of_range("Índice fora do intervalo");
        }
        return dados_[linha * colunas_ + coluna];
    }

    size_t linhas() const { return linhas_; }
    size_t colunas() const { return colunas_; }
    T* dados() { return dados_.data(); }
    const T* dados() const { return dados_.data(); }
    std::vector<T> getVetor(){return this->dados_; }

    void redimencionar(size_t tamanho, T valor){//redimencionar e inicializar
        this->dados_.resize(tamanho, valor);
        this->colunas_ = this->linhas_ = sqrt(tamanho);
    }

    void redimencionar(size_t tamanho){//redimencionar
        this->dados_.resize(tamanho);
        this->colunas_ = this->linhas_ = sqrt(tamanho);
    }

    void redimencionar(size_t tamanhox, size_t tamanhoy){
        this->dados_.resize(tamanhox*tamanhoy);
        this->linhas_ = tamanhox;
        this->colunas_ = tamanhoy;
    }

private:
    size_t linhas_;
    size_t colunas_;
    std::vector<T> dados_;
};

#endif //TRABALHO_MATRIZCONTIGUA_H
