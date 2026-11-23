#ifndef SISTEMAFICHEIROS_H
#define SISTEMAFICHEIROS_H

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>

#include "Base.h"
#include "Diretoria.h"
#include "Ficheiros.h"

class SistemaFicheiros {
    public:
        SistemaFicheiros();
        virtual ~SistemaFicheiros();

        // Gestão da raiz
        void setRaiz(Diretoria* d);

        // Funções principais

        /** \brief Carrega uma diretoria do disco para a memória
         *
         * Percorre recursivamente o caminho fornecido e cria a estrutura de dados (SistemaFicheiros ou sf)
         * composta por objetos do tipo Diretoria e Ficheiro. Limpa qualquer memória anterior.
         *
         * \param path Caminho absoluto ou relativo da diretoria a carregar (ex: "C:\POO_teste" ou "POO_teste")
         * \return bool Retorna true se a operação foi bem sucedida (caminho existe e é diretoria),
         * ou false se o caminho é inválido ou não é uma diretoria.
         */
        bool Load(const string &path);

        /** \brief Conta o número total de ficheiros em memória
         *
         * Percorre todo o sf e soma a quantidade de ficheiros encontrados
         *
         * \return int Número total de ficheiros carregados no sistema
         */
        int ContarFicheiros();

        /** \brief Conta o número total de diretorias em memória
         *
         * Percorre o sf e conta quantas pastas existem (excluindo a raiz)
         *
         * \return int Número total de diretorias
         */
        int ContarDirectorias();

        /** \brief Determina toda a memória ocupada
         *
         * Soma o tamanho (em bytes) de todos os ficheiros presentes no sf
         *
         * \return int Total de bytes ocupados
         */
        int Memoria();

        /** \brief Determina a diretoria com mais informação (elementos)
         *
         * Conta apenas os ficheiros e pastas imediatos da diretoria e compara com as restantes para concluir qual apresenta mais elementos
         *
         * \return string* Ponteiro para o nome da diretoria com mais elementos
         */
        string* DirectoriaMaisElementos();

        /** \brief Determina a diretoria com menos informação (elementos)
         *
         * Conta apenas os ficheiros e pastas imediatos da diretoria e compara com as restantes para concluir qual apresenta menos elementos
         *
         * \return string* Ponteiro para o nome da diretoria com menos elementos
         */
        string* DirectoriaMenosElementos();

        /** \brief Determina o ficheiro que ocupa mais espaço
         *
         * Percorre todo o sf para encontrar o ficheiro com maior tamanho
         *
         * \return string* Ponteiro para o caminho completo do ficheiro com maior tamanho
         */
        string* FicheiroMaior();

        /** \brief Determina qual a diretoria que ocupa mais espaço
         *
         * Soma recursivamente o tamanho de todos os ficheiros dentro de cada diretoria para determinar qual a que ocupa mais espaço
         *
         * \return string* Ponteiro para o nome da diretoria que ocupa mais espaço
         */
        string* DirectoriaMaisEspaco();

        /** \brief Pesquisa se existe um dado ficheiro ou diretoria
         *
         * Procura em todo o sf pelo nome exato da diretoria ou ficheiro
         *
         * \param s Nome do ficheiro ou diretoria a pesquisar
         * \param tipo int Define o alvo: 0 para pesquisar Ficheiro, 1 para Diretoria
         * \return string* Caminho completo até ao elemento ou NULL se não existir
         */
        string* Search(const string &s, int tipo);

        /** \brief Remove todas as diretorias ou ficheiros do sistema
         *
         * Remove todas as diretorias e ficheiros do sistema
         *
         * \param s filtro, todo o conteúdo especificado naquele filtro é removido
         * \param tipo String que define os tipos: "DIR" remove todas as diretorias, qualquer outro valor remove ficheiros
         * \return bool Retorna true se a operação foi concluída com sucesso, e false caso não seja
         */
        bool RemoverAll(const string &s, const string &tipo);

        /** \brief Grava todo o sf em formato XML
         *
         * Cria um ficheiro de texto estruturado com tags <Diretoria> e <Ficheiro> representando o sf atual
         *
         * \param s Caminho/Nome do ficheiro de saída (ex: "POO.xml")
         * \return void
         */
        void Escrever_XML(const string &s);

        /** \brief Lê um ficheiro XML e reconstrói o sf
         *
         * Apaga qualquer conteúdo existente em memória antes de ler, e analisa as tags XML
         *
         * \param s Caminho do ficheiro XML a ler
         * \return bool Retorna true se conseguir ler com sucesso e a estrutura for carregada corretamente,
         * e retorna false se der erro na leitura do ficheiro
         */
        bool Ler_XML(const string &s);

        /** \brief Move um ficheiro para outra diretoria
         *
         * Move o primeiro ficheiro encontrado com o nome 'Fich',
         * se já existir um ficheiro com esse nome no destino ou se estiver na mesma pasta, não faz nada
         *
         * \param Fich Nome do ficheiro a mover
         * \param DirNova Nome da diretoria destino
         * \return bool Retorna true se foi movido com sucesso, e false se não for movido corretamente, ou não existir a diretoria
         */
        bool MoveFicheiro(const string &Fich, const string &DirNova);

        /** \brief Move uma diretoria e o seu conteúdo para outra diretoria
         *
         * Move a primeira Diretoria encontrada com o nome 'DirOld', e não é possível mover-se uma diretoria para dentro de uma sub-diretoria
         *
         * \param DirOld Nome da Diretoria a ser movida
         * \param DirNew Nome da Diretoria de destino
         * \return bool Retorna true se foi movida com sucesso, e false se houverem diretorias não existentes ou não encontradas
         */
        bool MoverDirectoria(const string &DirOld, const string &DirNew);

        /** \brief Determina a data de um ficheiro
         *
         * Devolver a data de criação do ficheiro
         *
         * \param ficheiro Nome do ficheiro a pesquisar
         * \return string* String formatada com caminho e data (ex: "caminho|2025|11|25") ou NULL se não existir
         */
        string* DataFicheiro(const string &ficheiro); // devolve todas as datas encontradas

        /** \brief Imprime a árvore de diretorias (Tree)
         *
         * Gera uma representação visual da árvore, pode ser escrita na consola ou num ficheiro
         *
         * \param *fich Ponteiro para string com nome do ficheiro de saída. Se NULL, imprime na consola
         * \return void
         */
        void Tree(const string *fich = nullptr);

        /** \brief Pesquisa todas as diretorias com um dado nome (<dir>)
         *
         * Procura todas as ocorrências no sf e guarda os caminhos numa lista de resultados
         *
         * \param lres Lista onde serão guardados os resultados
         * \param dir Nome da diretoria a pesquisar
         * \return void
         */
        void PesquisarAllDirectorias(list<string> &lres, const string &dir);

        /** \brief Pesquisa todos os ficheiros com um dado nome (<file>)
         *
         *  Procura todas as ocorrências no sf e guarda os caminhos numa lista de resultados
         *
         * \param lres Lista onde serão guardados os resultados
         * \param file Nome do ficheiro a pesquisar
         * \return void
         */
        void PesquisarAllFicheiros(list<string> &lres, const string &file);

        /** \brief Renomeia um ficheiro existente no sistema
        *
        * Procura um ficheiro pelo nome e, caso não exista conflito de nomes
        * na mesma diretoria, atualiza-o para o novo nome mantendo os seus dados.
        *
        * \param fich_old Nome atual do ficheiro a ser renomeado
        * \param fich_new Novo nome a atribuir ao ficheiro
        * \return void
        */
        void RenomearFicheiros(const string &fich_old, const string &fich_new);

        /** \brief Verifica a existência de ficheiros duplicados em todo o sf
        *
        * Recolhe todos os ficheiros do sf e compara os seus nomes e tamanhos.
        * Imprime na consola os ficheiros que são considerados duplicados.
        *
        * \return bool Retorna true se encontrar duplicados, false caso não encontre
        */
        bool FicheiroDuplicados();

        /** \brief Copia ficheiros de uma diretoria para outra com base num padrão
        *
        * Localiza as diretorias de origem e destino pelo caminho/nome e inicia
        * a cópia recursiva de todos os ficheiros que contenham o padrão especificado.
        *
        * \param padrao Texto (substring) que o nome do ficheiro deve conter para ser copiado
        * \param DirOrigem Nome/Caminho da diretoria de origem
        * \param DirDestino Nome/Caminho da diretoria de destino
        * \return bool Retorna true se as diretorias existirem e a operação iniciar, false caso contrário
        */
        bool CopyBatch(const string &padrao, const string &DirOrigem, const string &DirDestino);

    protected:

    private:
        Diretoria* raiz;

        // Variáveis auxiliares para recursão
        Diretoria* dMaisElementos;
        size_t maxElementos;

        Diretoria* dMenosElementos;
        size_t minElementos;

        Ficheiro* maiorFicheiro;
        string caminhoMaiorFicheiro;

        Diretoria* dMaisEspaco;
        int maxEspaco;

        string resultadoSearch;

        // Funções auxiliares
        void DeleteDiretoriaRec(Diretoria* dir);
        Diretoria* LoadDiretoria(const string &path);

        void procurarDiretoriasMais(Diretoria* d);
        void procurarDiretoriasMenos(Diretoria* d);

        void procuraFicheiroMaior(Diretoria* d, const string& path);
        int somaEspaco(Diretoria* d);

        bool Buscarec(Diretoria* d, const string& nome, int tipo, const string& path);
        void Limparec(Diretoria* d, bool removerDirs);

        void escreverXML_Diretoria(ofstream &f, Diretoria *d, int nivel);
        void escreverXML_Ficheiro(ofstream &f, Ficheiro *fi, int nivel);

        Diretoria* parseDiretoriaXML(const vector<string>& lines, int& idx);

        bool encontrarFicheiro(Diretoria* atual,const string& nomeFich, Diretoria*& dirOrigem, Ficheiro*& ficheiroEncontrado);
        Diretoria* encontrarDiretoria(Diretoria* atual, const string& nomeDir);

        Diretoria* buscarDiretoriaComPai(Diretoria* atual, const string &nome, Diretoria*& pai);
        bool Subdiretoria(Diretoria* atual, Diretoria* sub);

        void printfTree(Diretoria* dir, ostream &out, string prefixo = "");

        string* ProcurarData(Diretoria* dir, const string &ficheiro, const string &caminhoAtual);

        void PesquisarDirrec( Diretoria* d, const string &nomeBusca, list<string> &lres, const string &pathAtual);
        void PesquisarFichrec(Diretoria* d, const string &nomeBusca, list<string> &lres, const string &pathAtual);

        void RecolherFicheiros(Diretoria* d, vector<Ficheiro*>& lista);
        void CopyBatchRecursiva(Diretoria* origem,const string &padrao,Diretoria* destino);
};

#endif // SISTEMAFICHEIROS_H
