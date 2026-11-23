#include "SistemaFicheiros.h"
#include "Base.h"
#include "Diretoria.h"

using namespace std;
namespace fs = std::filesystem;
using namespace std::filesystem;
using namespace std::chrono;

// Construtor e Destrutor
SistemaFicheiros::SistemaFicheiros() {
    raiz = new Diretoria("raiz"); // cria uma diretoria raiz chamada "raiz".
}

SistemaFicheiros::~SistemaFicheiros() {
    if (raiz) {
        DeleteDiretoriaRec(raiz); // limpa toda a memória alocada para a diretoria raiz e as suas subpastas/ficheiros
        raiz = nullptr; // evita ponteiros para a memória já apagada
    }
}

void SistemaFicheiros::DeleteDiretoriaRec(Diretoria* dir) {
    if (!dir) return;

    for (auto e : dir->getEntradas()) { // percorre tds os e(elementos) da diretoria
        if (e->isDir()) // se o e for pasta chama a função para apagar
            DeleteDiretoriaRec(static_cast<Diretoria*>(e));
        delete e; // apaga o objeto da memória
    }

    dir->getEntradas().clear(); // limpa o vetor/array de entradas da pasta
}

void SistemaFicheiros::setRaiz(Diretoria* d) { // substitui a raiz por uma nova diretoria
    if (raiz) DeleteDiretoriaRec(raiz);
    raiz = d;
}

// 1. Load: carregar a diretoria e conteúdos para memória
bool SistemaFicheiros::Load(const string &path) {
    fs::path p = fs::u8path(path); // UTF-8

    if (!exists(p) || !is_directory(p)) // verifica se o caminho e a pasta existem
        return false;

    DeleteDiretoriaRec(raiz); // se já existe algo carregado, apaga primeiro para evitar "vazamento" de memória

    raiz = LoadDiretoria(p.string()); // chama a função que cria a estrutura em memória de forma recursiva
    return true; // Sucesso
}

Diretoria* SistemaFicheiros::LoadDiretoria(const string &path) {
    fs::path p = fs::u8path(path); // UTF-8(consegue ler caminhos com caracteres especiais)

    Diretoria *dir = new Diretoria(p.filename().string()); // Cria um objeto Diretoria com o nome da pasta atual

    for (const auto &entry : fs::directory_iterator(p)) // Percorre todos os ficheiros e pastas dentro do diretório path
    {
        if (entry.is_directory()) // se for pasta
        {
            Diretoria *sub = LoadDiretoria(entry.path().string()); // carregar a subpasta
            dir->addEntrada(sub); // adiciona a subpasta ao diretório atual
        }
        else if (entry.is_regular_file()) // se for ficheiro
        {
            string nome = entry.path().filename().string(); // obtem nome

            int tam = file_size(entry.path()); // tamanho

            auto ftime = last_write_time(entry.path()); // data real - data da última modificação do ficheiro

            auto sctp = time_point_cast<system_clock::duration>( // converte o tempo de file_time para system_clock
                ftime - file_time_type::clock::now() + system_clock::now());

            time_t tt = system_clock::to_time_t(sctp);     // converte o time point para time_t
            tm local {};
            localtime_s(&local, &tt); // transforma o tempo para hora local

            stringstream ss;
            ss << put_time(&local, "%Y|%m|%d"); // string formatada para dar a data
            string data = ss.str(); // data real

            Ficheiro *f = new Ficheiro(nome, tam, data);
            dir->addEntrada(f); // adiciona o ficheiro á pasta atual
        }
    }

    return dir; // Retorna o diretório com todo o novo conteúdo carregado
}

// 2. Contar ficheiros totais
int SistemaFicheiros::ContarFicheiros() {
    if (!raiz) return 0;
    return raiz->ContaFicheirosRec();
}

// 3. Contar directorias totais
int SistemaFicheiros::ContarDirectorias() {
    if (!raiz) return 0;
    return raiz->ContaDiretoriasRec();
}

// 4. Memoria: soma de tamanhos de todos os ficheiros
int SistemaFicheiros::Memoria() {
    if (!raiz) return 0;
    return raiz->MemoriaRec();
}

// 5. Diretoria com mais elementos diretos
void SistemaFicheiros::procurarDiretoriasMais(Diretoria* d) {
    size_t count = d->getEntradas().size(); // Conta o número de entradas diretas da diretoria

    if (count > maxElementos) { // Verifica se esta diretoria tem mais elementos que a atual
        maxElementos = count;
        dMaisElementos = d;
    }
    for (Base* e : d->getEntradas()) { // Percorre tds os elementos da dir atual
        if (e->isDir())
            procurarDiretoriasMais(static_cast<Diretoria*>(e));
    }
}

string* SistemaFicheiros::DirectoriaMaisElementos() {
    if (!raiz) return nullptr;

    dMaisElementos = nullptr;
    maxElementos = 0;

    for (Base* e : raiz->getEntradas()) { // percorre apenas as diretorias dentro da raiz
        if (e->isDir())
            procurarDiretoriasMais(static_cast<Diretoria*>(e));
    }

    if (dMaisElementos)
        return new string(dMaisElementos->getNome());

    return nullptr; // caso não haja subdiretorias
}

// 6. Diretoria com menos elementos diretos
void SistemaFicheiros::procurarDiretoriasMenos(Diretoria* d) {
    size_t count = d->getEntradas().size(); // conta o número de entradas diretas de uma diretoria

    if (count < minElementos) {
        minElementos = count;
        dMenosElementos = d;
    }
    for (Base* e : d->getEntradas()) {
        if (e->isDir()) procurarDiretoriasMenos(static_cast<Diretoria*>(e)); // percorre as subpastas
    }
}

string* SistemaFicheiros::DirectoriaMenosElementos() {
    if (!raiz) return nullptr;

    dMenosElementos = raiz;
    minElementos = raiz->getEntradas().size();
    procurarDiretoriasMenos(raiz);

    return new string(dMenosElementos->getNome()); // retorna o nome da diretoria com menos elementos
}

// 7. Ficheiro maior
void SistemaFicheiros::procuraFicheiroMaior(Diretoria* d, const string& path) {
    for (Base* e : d->getEntradas()) {
        if (e->isDir())
            procuraFicheiroMaior(static_cast<Diretoria*>(e), path + "/" + e->getNome()); // percorre todas as entradas
        else {
            Ficheiro* f = static_cast<Ficheiro*>(e);
            if (!maiorFicheiro || f->getTamanho() > maiorFicheiro->getTamanho()) { // atualiza a referência
                maiorFicheiro = f;
                caminhoMaiorFicheiro = path + "/" + f->getNome(); // guarda o caminho do ficheiro
            }
        }
    }
}

string* SistemaFicheiros::FicheiroMaior() {
    if (!raiz) return nullptr;

    maiorFicheiro = nullptr;
    caminhoMaiorFicheiro = "";
    procuraFicheiroMaior(raiz, raiz->getNome());

    return maiorFicheiro ? new string(caminhoMaiorFicheiro) : nullptr; // retorna o caminho do maior ficheiro
}

// 8. Diretoria com mais espaço total
int SistemaFicheiros::somaEspaco(Diretoria* d) {
    int total = 0;

    for (Base* e : d->getEntradas()) { // Percorre todos os elementos dentro da diretoria

        if (e->isDir()) // Se for diretoria, soma recursivamente o espaço dentro dela
            total += somaEspaco(static_cast<Diretoria*>(e));

        else // Se for ficheiro, soma apenas o seu tamanho
            total += e->getTamanho();
    }

    if (d != raiz && total > maxEspaco) { // A diretoria raiz NÃO pode ser candidata
        maxEspaco = total;   // guarda o maior espaço encontrado
        dMaisEspaco = d;     // guarda ponteiro para a diretoria campeã
    }

    return total; // devolve o espaço total desta diretoria
}

string* SistemaFicheiros::DirectoriaMaisEspaco() {
    if (!raiz) return nullptr;

    maxEspaco = 0;
    dMaisEspaco = nullptr; // não assumimos que a raiz é a maior

    somaEspaco(raiz);

    if (!dMaisEspaco) return nullptr; // caso não haja subpastas

    return new string(dMaisEspaco->getNome());
}

// 9. Search ficheiro/diretoria
bool SistemaFicheiros::Buscarec(Diretoria* d, const string& nome, int tipo, const string& path) {
    for (Base* e : d->getEntradas()) {
        string full = path + "/" + e->getNome(); // Cria o caminho completo até essa entrada (raiz/pasta/subpasta/ficheiro.txt)
        if ((tipo == 0 && !e->isDir() && e->getNome() == nome) || (tipo == 1 && e->isDir() && e->getNome() == nome)) { // tipo = 0 ficheiro / tipo = 1 diretoria
            resultadoSearch = full;
            return true;
        }
        if (e->isDir() && Buscarec(static_cast<Diretoria*>(e), nome, tipo, full)) // se ainda n encontrou e for dir continua a busca
            return true;
    }
    return false; // não encontrou nada
}

string* SistemaFicheiros::Search(const string &s, int Tipo) {
    if (!raiz) return nullptr;

    resultadoSearch = ""; // Limpa resultados anteriores
    Buscarec(raiz, s, Tipo, raiz->getNome()); // nome / ficheiro ou diretoria / caminho inicial (nome da raiz)
    return resultadoSearch.empty() ? nullptr : new string(resultadoSearch);
}

// 10. Remover ficheiros ou diretorias
void SistemaFicheiros::Limparec(Diretoria* d, bool removerDirs) {
    auto &v = d->getEntradas();

    for (auto it = v.begin(); it != v.end();) { // iterador para percorrer todos os elementos (ficheiros e pastas) dentro da diretoria.
        Base* e = *it;

        if (e->isDir()) {
            Diretoria* sub = static_cast<Diretoria*>(e);

            if (removerDirs) { // limpa recursivamente antes de apagar
                DeleteDiretoriaRec(sub);
                delete sub;
                it = v.erase(it);
                continue;
            } else { // apenas entra para remover ficheiros
                Limparec(sub, removerDirs);
            }
        } else {
            if (!removerDirs) {
                delete e;
                it = v.erase(it); // remove o ponteiro do vetor
                continue;
            }
        }
        ++it;
    }
}

bool SistemaFicheiros::RemoverAll(const string &s, const string &tipo) {
    if (!raiz) return false;

    string t = tipo; // Converter para maiúsculas
    for (auto &c : t) c = toupper(c); // Transforma tudo em maiúsculas

    if (t != "DIR" && t != "FILE") {
        cerr << "Erro: Caracteres inválidos.\n";
        return false;
    }

    bool removerDirs = (t == "DIR");

    if (removerDirs) {
        Limparec(raiz, removerDirs);
        delete raiz; // apaga a raiz em si
        raiz = nullptr;
    } else {
        Limparec(raiz, removerDirs);
    }

    return true;
}

// 11. Gravar em XML todo o Sistema de Ficheiros
void SistemaFicheiros::Escrever_XML(const string &s) {
    ofstream f(s);

    if (!f.is_open()) { // verificar o ficheiro
        cout << "Erro ao criar o ficheiro XML!" << endl;
        return;
    }

    f << "<?xml version=\"1.0\"?>\n";
    f << "<SistemaFicheiros>\n";

    // Escreve a raiz recursivamente
    escreverXML_Diretoria(f, raiz, 1);

    f << "</SistemaFicheiros>\n";
    f.close();

    cout << "XML guardado com sucesso em: " << s << endl;
}
// Para Diretorias
void SistemaFicheiros::escreverXML_Diretoria(ofstream &f, Diretoria *d, int nivel) {
    string tab(nivel * 2, ' ');

    f << tab << "<Diretoria nome=\"" << d->getNome() << "\">\n";

    // percorre as entradas (ficheiros ou diretorias)
    for (Base* b : d->getEntradas())
    {
        if (b->isDir()) {
            // b é uma diretoria
            escreverXML_Diretoria(f, (Diretoria*)b, nivel + 1);
        }
        else {
            // b é ficheiro
            escreverXML_Ficheiro(f, (Ficheiro*)b, nivel + 1);
        }
    }

    f << tab << "</Diretoria>\n";
}
// Para Ficheiros
void SistemaFicheiros::escreverXML_Ficheiro(ofstream &f, Ficheiro *fi, int nivel) {
    string tab(nivel * 2, ' ');

    f << tab << "<Ficheiro nome=\"" << fi->getNome()
      << "\" tamanho=\"" << fi->getTamanho()
      << "\" data=\"" << fi->getData()
      << "\"/>\n";
}

// 12. Ler de um ficheiro em formato XML todo o Sistema de Ficheiros
bool SistemaFicheiros::Ler_XML(const string &s) {
    if (!raiz) return false;

    ifstream f(s);
    if (!f.is_open()) { // Abre o ficheiro XML
        cout << "Erro ao abrir o ficheiro XML: " << s << endl;
        return false;
    }

    vector<string> lines;
    string line;
    while (getline(f, line)) { // Le cada linha do ficheiro e guarda num vetor de strings
        lines.push_back(line);
    }
    f.close(); // Fecha o ficheiro após leitura

    int idx = 0;
    Diretoria* d = parseDiretoriaXML(lines, idx); // Cria a raiz a partir do XML
    if (!d) return false;

    raiz = d; // Define a nova raiz

    return true; // Concluiu a leitura com sucesso
}

Diretoria* SistemaFicheiros::parseDiretoriaXML(const vector<string>& lines, int& i) {

    while (i < lines.size() && lines[i].find("<Diretoria") == string::npos) i++; // Procura a próxima tag <Diretoria
    if (i >= lines.size()) return nullptr; // Nenhuma diretoria encontrada

    size_t n1 = lines[i].find("nome=\"") + 6; // Extrai o atributo nome da diretoria
    size_t n2 = lines[i].find("\"", n1);
    string nome = lines[i].substr(n1, n2 - n1);

    Diretoria* dir = new Diretoria(nome); // Cria a diretoria
    i++; // Passa para a próxima linha

    while (i < lines.size()) { // Ler os filhos da diretoria (outras diretoria ou ficheiros)
        const string &l = lines[i];

        if (l.find("</Diretoria>") != string::npos) {
            i++; // Fecha a diretoria atual e sai da recursão
            break;
        }
        else if (l.find("<Diretoria") != string::npos) { // Chamada recursiva para subdiretoria
            Diretoria* sub = parseDiretoriaXML(lines, i);
            if (sub) dir->addEntrada(sub); // Adiciona a subdiretoria à diretoria atual
        }
        else if (l.find("<Ficheiro") != string::npos) { // Extrair atributos do ficheiro
            size_t n1 = l.find("nome=\"") + 6;
            size_t n2 = l.find("\"", n1);
            string fNome = l.substr(n1, n2 - n1);

            size_t t1 = l.find("tamanho=\"") + 9;
            size_t t2 = l.find("\"", t1);
            int tamanho = stoi(l.substr(t1, t2 - t1));

            size_t d1 = l.find("data=\"") + 6;
            size_t d2 = l.find("\"", d1);
            string data = l.substr(d1, d2 - d1);

            Ficheiro* fObj = new Ficheiro(fNome, tamanho, data); // Cria um ficheiro
            dir->addEntrada(fObj); // Adiciona à diretoria atual
            i++;
        }
        else i++; // Ignora as linhas sem tags relevantes
    }
    return dir; // Retorna a diretoria criada
}

// 13. Mover um Ficheiro para outra diretoria
bool SistemaFicheiros::MoveFicheiro(const string &Fich, const string &DirNova){
    if (!raiz) return false;

    Diretoria* dirOrigem = nullptr;
    Ficheiro* ficheiro = nullptr;

    if (!encontrarFicheiro(raiz, Fich, dirOrigem, ficheiro)) // Encontrar ficheiro e a diretoria onde está
        return false; // ficheiro não existe

    Diretoria* dirDestino = encontrarDiretoria(raiz, DirNova); // Encontrar diretoria destino
    if (!dirDestino)
        return false; // diretoria não encontrada

    if (dirOrigem == dirDestino) // Verificar se já está na diretoria destino
        return false;

    for (Base* e : dirDestino->getEntradas()) // Verificar se já existe um ficheiro com o mesmo nome
        if (!e->isDir() && e->getNome() == Fich)
            return false;

    auto& vec = dirOrigem->getEntradas(); // Remover ficheiro da diretoria origem
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        if (*it == ficheiro) // it é um iterador (ponteiro inteligente)
        {
            vec.erase(it);
            break;
        }
    }
    dirDestino->addEntrada(ficheiro); // Adicionar ficheiro à nova diretoria

    return true;
}

bool SistemaFicheiros::encontrarFicheiro(Diretoria* atual, const string& nomeFich, Diretoria*& dirOrigem, Ficheiro*& ficheiroEncontrado) {

    for (Base* e : atual->getEntradas()) { // encontrou ficheiro
        if (!e->isDir() && e->getNome() == nomeFich) {
            dirOrigem = atual;
            ficheiroEncontrado = static_cast<Ficheiro*>(e);
            return true;
        }
        if (e->isDir()) { // continuar recursão
            if (encontrarFicheiro(static_cast<Diretoria*>(e), nomeFich, dirOrigem, ficheiroEncontrado))
                return true;
        }
    }
    return false;
}

Diretoria* SistemaFicheiros::encontrarDiretoria(Diretoria* atual, const string& nomeDir) {
     if (!atual) return nullptr;

     if (atual->getNome() == nomeDir) return atual; // comparar apenas o nome da diretoria, não o caminho completo

     for (Base* e : atual->getEntradas())
        if (e->isDir()) {
            Diretoria* res = encontrarDiretoria(static_cast<Diretoria*>(e), nomeDir);
            if (res) return res;
        }
     return nullptr;
}

// 14. Mover uma diretoria (e tudo o que lhe está associado!) para outra diretoria
Diretoria* SistemaFicheiros::buscarDiretoriaComPai(Diretoria* atual, const string &nome, Diretoria*& pai) {
    if (!raiz) return nullptr;

    for (Base* e : atual->getEntradas()) {
        Diretoria* d = dynamic_cast<Diretoria*>(e);
        if (!d) continue;

        if (d->getNome() == nome) {
            pai = atual;
            return d;
        }

        Diretoria* sub = buscarDiretoriaComPai(d, nome, pai);
        if (sub) return sub;
    }

    return nullptr;
}

bool SistemaFicheiros::Subdiretoria(Diretoria* atual, Diretoria* sub) {
    if (!atual) return false;
    if (atual == sub) return true;

    for (Base* e : atual->getEntradas()) {
        Diretoria* d = dynamic_cast<Diretoria*>(e);
        if (d && Subdiretoria(d, sub)) return true;
    }

    return false;
}

bool SistemaFicheiros::MoverDirectoria(const string &DirOld, const string &DirNew) {
    if (!raiz) return false;

    Diretoria* paiOld = nullptr;
    Diretoria* dirOld = buscarDiretoriaComPai(raiz, DirOld, paiOld);

    if (!dirOld || !paiOld) return false; // Não encontrou ou é raiz

    Diretoria* dirNew = nullptr;
    Diretoria* paiDir = nullptr;
    dirNew = buscarDiretoriaComPai(raiz, DirNew, paiDir);

    if (!dirNew) return false; // diretoria destino não encontrada

    if (Subdiretoria(dirOld, dirNew)) return false; // DirNew é subdiretoria de DirOld

    // Move a diretoria
    paiOld->removerEntrada(dirOld);
    dirNew->addEntrada(dirOld);

    return true;
}

//15. Determine a data de um ficheiro
string* SistemaFicheiros::ProcurarData(Diretoria* dir, const string &ficheiro, const string &caminhoAtual) {
    if (!raiz) return nullptr;

    for (Base* b : dir->getEntradas()) { // percorre as entradas da diretoria
        if (!b) continue;

        string novoCaminho = caminhoAtual + "/" + b->getNome();

        if (b->isDir()) { // recursão para subdiretorias
            Diretoria* sub = static_cast<Diretoria*>(b);
            string* resultado = ProcurarData(sub, ficheiro, novoCaminho);

            if (resultado) return resultado;
        } else { // é ficheiro - comparar nome
            Ficheiro* f = static_cast<Ficheiro*>(b);

            if (f->getNome() == ficheiro) { // devolve "caminho|data"
                return new string(novoCaminho + "|" + f->getData());
            }
        }
    }

    return nullptr; // não encontrado
}

string* SistemaFicheiros::DataFicheiro(const string &ficheiro) {
    if (!raiz) return nullptr;
    return ProcurarData(raiz, ficheiro, raiz->getNome()); // começa no caminho "raiz"
}

// 16. Implemente o método Tree
void SistemaFicheiros::printfTree(Diretoria* dir, ostream &out, string prefixo) {
    // Imprime a diretoria atual
    out << prefixo << "[DIR] " << path(dir->getNome()).filename().string() << "\n";

    auto e = dir->getEntradas(); // vector<Base*>

    for (size_t i = 0; i < e.size(); i++) {
        bool isLast = (i == e.size() - 1);
        string novoPrefixo = prefixo + "    "; // 4 espaços para cada nível

        if (e[i]->isDir()) { // Diretoria: imprime [DIR] e chama recursivamente
            Diretoria* subDir = dynamic_cast<Diretoria*>(e[i]);
            if (subDir)
                printfTree(subDir, out, novoPrefixo);
        } else { // Ficheiro: imprime [FILE] com o prefixo adequado
            out << novoPrefixo << "[FILE] " << path(e[i]->getNome()).filename().string() << "\n";
        }
    }
}

void SistemaFicheiros::Tree(const string *fich) {
    if (!raiz) return;

    if (fich == nullptr) { // Sem ficheiro envia para consola
        printfTree(raiz, cout);
        return;
    }
    ofstream file(*fich);
    if (!file.is_open()) { // Com ficheiro grava no ficheiro
        cout << "Erro ao abrir o ficheiro: " << *fich << endl;
        return;
    }

    printfTree(raiz, file);

    file.close();
    cout << "Arvore gravada em " << *fich << endl;
}

// 17. Pesquisar diretorias e colocar em lres
void SistemaFicheiros::PesquisarAllDirectorias(list<string> &lres, const string &dir) {
    lres.clear(); // Limpa a lista de resultados
    if (!raiz) return;

    PesquisarDirrec(raiz, dir, lres, ""); // Começamos a pesquisa pela raiz, caminho inicial é vazio
}

void SistemaFicheiros::PesquisarDirrec( Diretoria* d, const string &nomeBusca, list<string> &lres, const string &pathAtual) {
    if (d == nullptr) return;

    string novoCaminho; // Monta o caminho completo até esta diretoria

    if (pathAtual.empty())
        novoCaminho = d->getNome(); // Caso especial: estamos na raiz
    else
        novoCaminho = pathAtual + "/" + d->getNome();  // Caminho + nome atual

    if (d->getNome() == nomeBusca) { // Verifica se esta diretoria tem o nome buscado
        lres.push_back(novoCaminho); // Guarda o caminho completo na lista
    }

    vector<Base*> entradas = d->getEntradas(); // Percorre todas as entradas dentro desta diretoria

    for (Base* item : entradas) {

        if (item->isDir()) { // Verificamos se o item é uma diretoria

            Diretoria* subDir = static_cast<Diretoria*>(item); // Cast seguro Base* -> Diretoria*

            PesquisarDirrec(subDir, nomeBusca, lres, novoCaminho); // Chamada recursiva para pesquisar dentro da subdiretoria
        }
    }
}

// 18. Pesquisar ficheiros e colocar em lres
void SistemaFicheiros::PesquisarAllFicheiros(list<string> &lres, const string &file) {
    lres.clear(); // Limpa a lista de resultados antes de começar a pesquisa
    if (!raiz) return;

    PesquisarFichrec(raiz, file, lres, ""); // Começa na raiz e acaba no caminho atual
}

// Função recursiva que percorre toda a árvore de diretórios
void SistemaFicheiros::PesquisarFichrec(Diretoria* d, const string &nomeBusca, list<string> &lres, const string &pathAtual) {
    if (d == nullptr) return; // Caso a diretoria seja nula

    string novoCaminho;

    if (pathAtual.empty()) // Atualiza o caminho atual
        novoCaminho = d->getNome(); // O caminho é apenas o nome da raiz
    else
        novoCaminho = pathAtual + "/" + d->getNome(); // Adiciona esta pasta ao caminho atual

    vector<Base*> entradas = d->getEntradas(); // Todas as entradas (ficheiros e subdiretórios) desta diretoria

    for (size_t i = 0; i < entradas.size(); i++) { // Percorre todas as entradas
        Base* item = entradas[i];

        if (item->isDir()) { // Pesquisa recursivamente dentro da diretoria
            Diretoria* subDir = (Diretoria*)item;
            PesquisarFichrec(subDir, nomeBusca, lres, novoCaminho);
        }
        else { // Verificamos se o nome do fich bate com o que estamos a procurar
            if (item->getNome() == nomeBusca) {
                lres.push_back(novoCaminho + "/" + item->getNome()); // Adiciona o caminho completo do ficheiro à lista de resultados
            }
        }
    }
}

//19. Renomear ficheiros
void SistemaFicheiros::RenomearFicheiros(const string &fich_old, const string &fich_new) {
    if (!raiz) return;

    Diretoria* dirOrigem = nullptr; //cria o pondeiro de uma diretoria vazia
    Ficheiro* ficheiro = nullptr; //cria o ponteiro de um ficheiro vazio


    if (!encontrarFicheiro(raiz, fich_old, dirOrigem, ficheiro)) {
        cout << "Erro: Ficheiro \"" << fich_old << "\" não encontrado.\n"; //Procura o ficheiro e a diretoria onde está
        return;
    }


    for (Base* e : dirOrigem->getEntradas()) {
        if (!e->isDir() && e->getNome() == fich_new) { //Verifica se há duplicado na mesma diretoria
            cout << "Erro: Já existe um ficheiro com o nome \"" << fich_new << "\".\n";
            return;
        }
    }


    auto &vec = dirOrigem->getEntradas();
    for (auto it = vec.begin(); it != vec.end(); ++it) { //Encontrar posição no vector da diretoria
        if (*it == ficheiro) {

            int tamanho = ficheiro->getTamanho();
            string data = static_cast<Ficheiro*>(ficheiro)->getData(); // Copiar atributos essenciais do ficheiro antigo


            Ficheiro* novo = new Ficheiro(fich_new, tamanho, data); // Criar novo ficheiro com o novo nome


            *it = novo; // Substituir no vector e apagar o antigo
            delete ficheiro;

            cout << "Ficheiro renomeado com sucesso: " << fich_old << " -> " << fich_new << endl;
            return;
        }
    }
}

//20. Verificar se existe Ficheiro Duplicado
void SistemaFicheiros::RecolherFicheiros(Diretoria* d, vector<Ficheiro*>& lista) {
    if (!d) return;

    for (Base* e : d->getEntradas()) {
        if (e->isDir()) {
            RecolherFicheiros(static_cast<Diretoria*>(e), lista); // recursão para subpastas
        } else {
            lista.push_back(static_cast<Ficheiro*>(e)); // adiciona ficheiro à lista
        }
    }
}

// Função principal para verificar ficheiros duplicados
bool SistemaFicheiros::FicheiroDuplicados() {
    if (!raiz) return false;

    vector<Ficheiro*> todosFicheiros;
    RecolherFicheiros(raiz, todosFicheiros); // preenche o vetor com todos os ficheiros

    bool encontrouDuplicados = false;

    for (size_t i = 0; i < todosFicheiros.size(); i++) { // Percorre todos os ficheiros comparando com os restantes
        for (size_t j = i + 1; j < todosFicheiros.size(); j++) {

            // Consideramos duplicado se tiver o mesmo nome e tamanho
            if (todosFicheiros[i]->getNome() == todosFicheiros[j]->getNome() && todosFicheiros[i]->getTamanho() == todosFicheiros[j]->getTamanho()) {

                cout << "Duplicado encontrado: "
                     << todosFicheiros[i]->getNome()
                     << " (tamanho: " << todosFicheiros[i]->getTamanho() << " bytes)\n";

                encontrouDuplicados = true;
            }
        }
    }

    if (!encontrouDuplicados)
        cout << "Nenhum ficheiro duplicado encontrado.\n";

    return encontrouDuplicados;
}

// 21. Copiar todos os ficheiros de uma directoria (e da respetiva estrutura de sub-directorias)
void SistemaFicheiros::CopyBatchRecursiva(Diretoria* origem,const string &padrao,Diretoria* destino) {
    if (!origem) return;

    for (Base* e : origem->getEntradas()) { // Percorre todas as entradas da pasta atual

        if (e->isDir()) { // Se for diretoria recursiva
            CopyBatchRecursiva(static_cast<Diretoria*>(e), padrao, destino);
        }
        else { // Se for ficheiro e contiver o padrão no nome
            if (e->getNome().find(padrao) != string::npos) {

                string nomeFinal = e->getNome();
                int contador = 1;
                bool existeNome;

                do {
                    existeNome = false;
                    for (Base* b : destino->getEntradas()) { // Verifica se já existe algúm ficheiro com 'nomeFinal' no destino
                        if (!b->isDir() && b->getNome() == nomeFinal) {
                            existeNome = true;
                            break;
                        }
                    }

                    if (existeNome) { // Se existe, cria um novo nome (ex: FicheiroABC001.txt) e repete o teste
                        size_t ponto = e->getNome().find_last_of(".");
                        string base = (ponto == string::npos) ? e->getNome() : e->getNome().substr(0, ponto);
                        string ext = (ponto == string::npos) ? "" : e->getNome().substr(ponto);

                        char buffer[20]; // Usa sprintf para meter os zeros (001, 002...)
                        sprintf(buffer, "%03d", contador++);
                        nomeFinal = base + buffer + ext;
                    }
                } while (existeNome);

                // Cria a cópia com o nome final garantidamente único
                Ficheiro* fOrigem = static_cast<Ficheiro*>(e);
                Ficheiro* fCopia = new Ficheiro(nomeFinal, fOrigem->getTamanho(), fOrigem->getData());

                destino->addEntrada(fCopia);
            }
        }
    }
}

bool SistemaFicheiros::CopyBatch(const string &padrao,const string &DirOrigem,const string &DirDestino) {
    if (!raiz) return false;

    Diretoria* origem = encontrarDiretoria(raiz, DirOrigem);
    Diretoria* destino = encontrarDiretoria(raiz, DirDestino);

    if (!origem || !destino) {
        cout << "Erro: Uma das diretorias nao foi encontrada.\n";
        return false;
    }
    CopyBatchRecursiva(origem, padrao, destino);

    return true;
}
