#ifndef FICHEIROS_H
#define FICHEIROS_H

#include "Base.h"

class Ficheiro : public Base {
    public:
        Ficheiro(string n, int t, string d);
        virtual ~Ficheiro();

        int getTamanho() const;
        bool isDir() const;
        string getData() const;

    private:
        int tamanho;
        string data;
};

#endif // FICHEIROS_H
