#include "Diretoria.h"

//Construtor e Destrutor
Diretoria::Diretoria(string n) {
    nome = n; // cria uma diretoria com o nome n
}
Diretoria::~Diretoria() {
    for (auto e : entradas) // apagar todas entradas
        delete e;
    entradas.clear(); // limpa o vetor
}
// Implementação do getEntradas()
vector<Base*>& Diretoria::getEntradas() {
    return entradas;
}
void Diretoria::addEntrada(Base* e) {
    entradas.push_back(e); // adiciona um elemento (ficheiro/diretoria) ao vetor entradas
}
bool Diretoria::removerEntrada(Base* e) {
    for (auto it = entradas.begin(); it != entradas.end(); ++it) {
        if (*it == e) {
            entradas.erase(it);
            return true; // removido com sucesso
        }
    }
    return false; // não encontrado
}

int Diretoria::getTamanho() const {
    return MemoriaRec(); // tamanho total da diretoria (memória dos ficheiros + subpastas)
}

bool Diretoria::isDir() const {
    return true; // diferencia diretorias de ficheiros
}

//Estas func operam apenas dentro de uma diretoria e das suas subpastas
// 2 --- Conta_todas_os_ficheiros_recursivamente -------------------------------------
int Diretoria::ContaFicheirosRec() const {
    int count = 0; // contador inicial
    for (auto e : entradas) {
        if (e->isDir())
            count += static_cast<Diretoria*>(e)->ContaFicheirosRec(); // se for diretoria chama denovo
        else
            count += 1; // se for ficheiro aumenta a contagem
    }
    return count; // retorna o número de ficheiros
}

// 3 --- Conta_todas_as_diretorias_recursivamente ----------------------------------
int Diretoria::ContaDiretoriasRec() const {
    int total = 0; // contador inicial
    for (auto e : entradas) {
        if (e->isDir()) { // adiciona as subpastas
            total += 1;
            total += static_cast<Diretoria*>(e)->ContaDiretoriasRec();
        }
    }
    return total; // retorna o número de pastas
}

// 4 --- Soma_o_tamanho_de_todos_os_ficheiros_recursivamente ----------------------------------
int Diretoria::MemoriaRec() const {
    int total = 0;
    for (auto e : entradas) {
        if (e->isDir()) {
            total += static_cast<Diretoria*>(e)->MemoriaRec(); // se for diretoria chama denovo
        } else {
            total += e->getTamanho(); // se for ficheiro adiciona o tamanho ao total
        }
    }
    return total; // retorna o tamanho total
}
