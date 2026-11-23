#ifndef BASE_H
#define BASE_H

#include <vector>
#include <string>

using namespace std;

class Base {
    public:
        Base();
        virtual ~Base();

        virtual int getTamanho() const = 0;
        virtual bool isDir() const = 0;

        string getNome() const;

    protected:                  //para que as classes ficheiros e diretorias possam aceder á variável nome, caso não fosse esse o objetivo usava-se o private
        string nome;
};

#endif // BASE_H
