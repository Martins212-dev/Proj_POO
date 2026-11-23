#include <iostream>
#include <locale>
#include <stdlib.h>

#include "Diretoria.h"
#include "Ficheiros.h"
#include "SistemaFicheiros.h"

//============================= COISAS A MELHORAR/FAZER =======================================================================
//1. Funcionam (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21) "Concluído"
//2.
//3.
//4. Organizar o Projeto
//5. etc,...
//===============================================================================================================================

void MenuMudancas(SistemaFicheiros &sf){
    int op2;

    do{
        cout << "\n#---------------- Menu Mudanças ----------------#\n";
        cout << "1 - Pesquisar ficheiro/diretoria\n";
        cout << "2 - Mover ficheiro\n";
        cout << "3 - Mover Diretoria\n";
        cout << "4 - Pesquisa <dir> para <lres>\n";
        cout << "5 - Pesquisa <file> para <lres>\n";
        cout << "6 - Renomear um ficheiro\n";
        cout << "8 - Mostrar árvore do sistema (Tree)\n";
        cout << "#-----------------------------------------------#\n";
        cout << "0 - Voltar\n";
        cout << "#-----------------------------------------------#\n";
        cout << "Escolha uma opção: ";
        cin >> op2;
        cin.ignore(); // limpar buffer do cin

        switch (op2){
            case 1: {
                string nome;
                int tipo;
                cout << "Introduza o nome a procurar: ";
                getline(cin, nome);
                cout << "Tipo (0 - ficheiro | 1 - diretoria): ";
                cin >> tipo;
                cin.ignore();

                string* res = sf.Search(nome, tipo);
                if (res) {
                    cout << "Encontrado: " << *res << endl;
                    delete res;
                } else {
                    cout << "Não encontrado.\n";
                }
                break;
            }
            case 2: {
                string nomeFich, nomeDir;
                cout << "Nome do ficheiro a mover: ";
                getline(cin, nomeFich);

                cout << "Nome da diretoria destino: ";
                getline(cin, nomeDir);

                if (sf.MoveFicheiro(nomeFich, nomeDir))
                    cout << "Ficheiro movido com sucesso!\n";
                else
                    cout << "Erro: Não foi possível mover o ficheiro.\n";
                break;
            }
            case 3: {
                string origem, destino;
                cout << "Nome da diretoria a mover: ";
                cin >> origem;
                cout << "Nome da diretoria destino: ";
                cin >> destino;

                if (sf.MoverDirectoria(origem, destino)) {
                    cout << "Diretoria movida com sucesso!\n";
                } else {
                    cout << "Erro: Não foi possível mover a diretoria.\n";
                }
                break;
            }
            case 4: {
                string nomeDir;
                list<string> resultados;

                cout << "Introduza o nome da diretoria a pesquisar: ";
                getline(cin, nomeDir); // Usa getline para o caso de ter espaços

                sf.PesquisarAllDirectorias(resultados, nomeDir);

                cout << "\nDir´s encontradas:\n";
                if (resultados.empty()) {
                    cout << "Nenhuma diretoria encontrada com o nome: " << nomeDir << endl;
                } else {

                    for (const string &caminho : resultados) {
                        cout << " - " << caminho << endl;
                    }
                }
                break;
            }
            case 5: {
                string nomeFich;
                list<string> resultados;

                cout << "Introduza o nome do ficheiro a pesquisar (Ex: Fich.txt): ";
                getline(cin, nomeFich);

                sf.PesquisarAllFicheiros(resultados, nomeFich);

                cout << "\nFile´s encontrados:\n";
                if (resultados.empty()) {
                    cout << "Nenhum ficheiro encontrado com o nome: " << nomeFich << endl;
                } else {
                    for (const string &caminho : resultados) {
                        cout << " - " << caminho << endl;
                    }
                }
                break;
            }
            case 6: {
                string oldName, newName;
                cout << "Nome do ficheiro a renomear: ";
                getline(cin, oldName);

                cout << "Novo nome do ficheiro (Ex: Fich.txt): ";
                getline(cin, newName);

                sf.RenomearFicheiros(oldName, newName);
                break;
            }
            case 7: {
                 string padrao, origem, destino;
                cout << "Introduza o nome do ficheiro (Ex: Fich.txt): ";
                getline(cin, padrao);
                cout << "Introduza o nome da diretoria de origem: ";
                getline(cin, origem);
                cout << "Introduza o nome da diretoria de destino: ";
                getline(cin, destino);

                if (sf.CopyBatch(padrao, origem, destino))
                    cout << "Operação concluída com sucesso!\n";
                else
                    cout << "Erro ao copiar ficheiros.\n";
                break;
            }
            case 8: {
                string fich;
                cout << "Nome do ficheiro para a Tree (enter = consola): ";
                getline(cin, fich);
                if (fich.empty())
                    sf.Tree();
                else
                    sf.Tree(&fich);
                break;
            }
            case 0:
                system("cls");
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }
    }while (op2 != 0);
}

void MenuTamanho(SistemaFicheiros &sf){
    int op1;

    do{
        cout << "\n#---------------- Menu Tamanho -----------------#\n";
        cout << "1 - Mostrar memória total usada\n";
        cout << "2 - Diretoria com mais elementos\n";
        cout << "3 - Diretoria com menos elementos\n";
        cout << "4 - Maior ficheiro no sistema\n";
        cout << "5 - Diretoria que ocupa mais espaço\n";
        cout << "#-----------------------------------------------#\n";
        cout << "0 - Voltar\n";
        cout << "#-----------------------------------------------#\n";
        cout << "Escolha uma opção: ";
        cin >> op1;
        cin.ignore(); // limpar buffer do cin

        switch (op1) {
            case 1:
                cout << "Memória total usada: " << sf.Memoria() << " bytes\n";
                break;
            case 2: {
                string* nome = sf.DirectoriaMaisElementos();
                if (nome) {
                    cout << "Diretoria com mais elementos: " << *nome << endl;
                    delete nome;
                } else {
                    cout << "Nenhuma diretoria carregada.\n";
                }
                break;
            }
            case 3: {
                string* nome = sf.DirectoriaMenosElementos();
                if (nome) {
                    cout << "Diretoria com menos elementos: " << *nome << endl;
                    delete nome;
                } else {
                    cout << "Nenhuma diretoria carregada.\n";
                }
                break;
            }
            case 4: {
                string* nome = sf.FicheiroMaior();
                if (nome) {
                    cout << "Ficheiro maior: " << *nome << endl;
                    delete nome;
                } else {
                    cout << "Nenhum ficheiro encontrado.\n";
                }
                break;
            }
            case 5: {
                string* nome = sf.DirectoriaMaisEspaco();
                if (nome) {
                    cout << "Diretoria com mais espaço: " << *nome << endl;
                    delete nome;
                } else {
                    cout << "Nenhuma diretoria carregada.\n";
                }
                break;
            }
            case 0:
                system("cls");
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }
    } while (op1 != 0);
}

void MenuPrincipal(SistemaFicheiros &sf) {
    int op;

    do {
        cout << "\n#--------------- Menu Principal ---------------#\n";
        cout << "1 - Carregar diretoria (Load)\n";
        cout << "2 - Contar ficheiros\n";
        cout << "3 - Contar diretorias\n";
        cout << "4 - Gravar sistema para XML\n";
        cout << "5 - Ler sistema a partir do XML\n";
        cout << "6 - Verificar Ficheiros duplicados\n";
        cout << "7 - Determinar a data de um ficheiro\n";
        cout << "8 - Remover todas diretorias/ficheiros\n";
        cout << "#---------------- Outros Menus ----------------#\n";
        cout << "9 - Menu Tamanhos\n";
        cout << "10 - Menu Mudanças\n";
        cout << "#----------------------------------------------#\n";
        cout << "0 - Sair\n";
        cout << "#----------------------------------------------#\n";
        cout << "Escolha uma opção: ";
        cin >> op;
        cin.ignore(); // limpar buffer do cin

        switch (op) {
            case 1: {
                string caminho;
                cout << "Introduza o caminho da pasta a carregar: ";
                getline(cin, caminho);
                if (sf.Load(caminho))
                    cout << "Diretoria carregada com sucesso!\n";
                else
                    cout << "Erro: Caminho inválido ou não é uma pasta.\n";
                break;
            }
            case 2:
                cout << "Número total de ficheiros: " << sf.ContarFicheiros() << endl;
                cout << "Erro: Nenhuma pasta carregada.\n";
                break;
            case 3:
                cout << "Número total de diretorias: " << sf.ContarDirectorias() << endl;
                cout << "Erro: Nenhuma pasta carregada.\n";
                break;
            case 4: {
                string caminho;
                cout << "Introduza o nome do ficheiro XML para exportar: ";
                getline(cin, caminho);
                sf.Escrever_XML(caminho);
                break;
            }
            case 5: {
                string caminho;
                cout << "Introduza o caminho do ficheiro XML: ";
                getline(cin, caminho);
                if (sf.Ler_XML(caminho))
                    cout << "Sistema de Ficheiros carregado com sucesso a partir do XML!\n";
                else
                    cout << "Erro: Não foi possível ler o XML.\n";
                break;
            }
            case 6: {
                sf.FicheiroDuplicados();
                break;
            }
            case 7: {
                string nome;
                cout << "Nome do ficheiro: ";
                getline(cin, nome);

                string* res = sf.DataFicheiro(nome);

                if (!res) {
                    cout << "Ficheiro não encontrado.\n";
                } else { // formato devolvido: caminho|data
                    size_t p = res->find("|");
                    string caminho = res->substr(0, p);
                    string data = res->substr(p + 1);

                    cout << "\nFicheiro encontrado:\n";
                    cout << "Caminho: " << caminho << endl;
                    cout << "Data: " << data << endl << endl;

                    delete res;
                }
                break;
            }
            case 8: {
                string tipo;
                cout << "Introduza o tipo a remover (DIR ou FILE): ";
                getline(cin, tipo);
                if (sf.RemoverAll("", tipo))
                    cout << tipo << " removidos com sucesso!\n";
                else
                    cout << "Erro ao remover.\n";
                break;
            }
            case 9:
                system("cls");
                MenuTamanho(sf);
                break;
            case 10:
                system("cls");
                MenuMudancas(sf);
                break;
            case 0:
                system("cls");
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }

    } while (op != 0);
}

int main() {
    setlocale(LC_ALL, "portuguese");

    SistemaFicheiros sf; // cria o sistema de ficheiros vazio
    MenuPrincipal(sf);
    return 0;
}
