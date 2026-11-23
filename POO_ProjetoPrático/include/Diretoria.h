#ifndef DIRETORIA_H
#define DIRETORIA_H

#include "Base.h"

class Diretoria : public Base {
    public:
        Diretoria(string n);
        virtual ~Diretoria();

        void addEntrada(Base* e);
        vector<Base*>& getEntradas(); // guarda os filhos num vetor
        bool removerEntrada(Base* e);

        int getTamanho() const;
        bool isDir() const;


        int ContaDiretoriasRec() const;
        int ContaFicheirosRec() const;
        int MemoriaRec() const;

    private:
        vector<Base*> entradas;
};

#endif // DIRETORIA_H
