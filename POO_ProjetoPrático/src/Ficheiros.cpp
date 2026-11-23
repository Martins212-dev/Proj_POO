#include "Ficheiros.h"

//Construtor e Destrutor
Ficheiro::Ficheiro(string n, int t, string d) {
    nome = n;
    tamanho = t;
    data = d;
}
Ficheiro::~Ficheiro() {}

int Ficheiro::getTamanho() const {return tamanho;} // confirmar o tamanho
bool Ficheiro::isDir() const {return false;} // confirmar se é diretoria
string Ficheiro::getData() const{return data;}
