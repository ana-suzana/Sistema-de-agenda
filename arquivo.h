//#ifndef arquivo .h
#define arquivo .h 

typedef struct _data {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
}Data;

typedef struct _evento {
    int codigo;
    Data dataEvento;
    float duracao;
    char descricao[100];
    struct _evento *direita;
    struct _evento *esquerda;
}Evento;

Evento* criarEvento(int codigo, Data dataEvento, float duracao, const char* descricao);
int compararDatas(Data d1, Data d2);
Data calcularFimEvento(Data inicio, float duracao);
int eventosSobrepostos(Evento* eventoExistente, Data novoInicio, float novaDuracao);
Evento* inserirEvento(Evento* raiz, Evento* novoEvento);
void consultarEventosIn_order(Evento* raiz);
int verificaDatasIguais(Data data1, Data data2);
void consultarEventoData(Evento* raiz, Data data);
void percorrerPosOrdem(Evento* raiz);
Evento* alterarEvento(Evento* raiz, Data data);
Evento* excluirEvento(Evento* raiz, Data data);

//#endif