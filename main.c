#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

int main(){
    Evento *novo = NULL, *raiz = NULL;
    printf("----------------------AGENDA---------------------");
    while(1){
        int opcao = 0, codigo=0, dia=0, mes=0, ano=0, hora=0, minuto=0;
        float duracao=0.0;
        char descricao[100];

        printf("\n\nMENU:\n1- Incluir evento\n2- Excluir evento\n3- Consultar\n4- Alterar evento\n5- Listar todos\n6- Sair\n");
        scanf("%d", &opcao);
        
        if(opcao == 1){ //INCLUIR EVENTO ok
            printf("Insira os dados do evento\n");
            printf("Codigo: ");
            scanf("%d", &codigo);
            printf("Data (DD/MM/AAAA): ");
            scanf("%d/%d/%d", &dia, &mes, &ano);
            printf("Horario (HH:MM): ");
            scanf("%d:%d", &hora, &minuto);
            printf("Duracao (em horas): ");
            scanf("%f", &duracao);
            printf("Descricao: ");
            scanf("%s", descricao);

            Data dataNovo = {dia, mes, ano, hora, minuto};
            novo = criarEvento(codigo, dataNovo, duracao, descricao);
            raiz = inserirEvento(raiz, novo);
        }
        else if(opcao == 2){//EXCLUIR EVENTO
            printf("\nInsira a data e a hora do evento que deseja excluir (DD/MM/AAAA HH:MM): ");
            scanf("%d/%d/%d %d:%d", &dia, &mes, &ano, &hora, &minuto);
            Data data = {dia, mes, ano, hora, minuto};

            raiz = excluirEvento(raiz, data);
        }
        else if(opcao == 3){//CONSULTAR EVENTOS POR DATA - ok
            printf("Insira a data desejada (DD/MM/AAAA): ");
            scanf("%d/%d/%d", &dia, &mes, &ano);
            Data data ={dia, mes, ano, 0, 0};
            if(raiz == NULL){
                printf("Agenda vazia!");
            }
            else{
                printf("\nEventos agendados para %d/%d/%d: \n", dia, mes, ano);
                consultarEventoData(raiz, data);
            }
        }
        else if(opcao == 4){//ALTERAR EVENTO - ok
            printf("\nInsira a data e a hora do evento que deseja alterar (DD/MM/AAAA HH:MM): ");
            scanf("%d/%d/%d %d:%d", &dia, &mes, &ano, &hora, &minuto);
            Data data = {dia, mes, ano, hora, minuto};
        
            raiz = alterarEvento(raiz, data);
        }
        else if(opcao == 5){//LISTAR TODOS - ok
            if(raiz == NULL){
                printf("Agenda vazia!");
            }
            else{
                printf("\nEventos cadastrados: \n");
                consultarEventosIn_order(raiz);
            }
        }
        else{
            printf("\nSistema encerrado.\n");
            break;
        }
    }
}

// Função para criar um novo evento
Evento* criarEvento(int codigo, Data dataEvento, float duracao, const char* descricao) {
    Evento* novoEvento = (Evento*)malloc(sizeof(Evento));
    if (novoEvento == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o evento.\n");
        exit(EXIT_FAILURE);
    }
    novoEvento->codigo = codigo;
    novoEvento->dataEvento = dataEvento;
    novoEvento->duracao = duracao;
    strcpy(novoEvento->descricao, descricao);
    novoEvento->direita = NULL;
    novoEvento->esquerda = NULL;
    return novoEvento;
}

int compararDatas(Data d1, Data d2) {
    if (d1.ano != d2.ano) {
        return d1.ano - d2.ano;
    }
    if (d1.mes != d2.mes) {
        return d1.mes - d2.mes;
    }
    if (d1.dia != d2.dia) {
        return d1.dia - d2.dia;
    }
    if (d1.hora != d2.hora) {
        return d1.hora - d2.hora;
    }
    return d1.minuto - d2.minuto;
}

// Função para calcular o horário de término do evento - isso foi necessário para não sobrepor um evento ao outro
Data calcularFimEvento(Data inicio, float duracao) {
    Data fim = inicio;
    int duracaoMinutos = duracao;
    fim.minuto += (int)(60 * duracaoMinutos);

    // Ajustando os minutos e horas
    while (fim.minuto >= 60) {
        fim.minuto -= 60;
        fim.hora++;
    }
    while (fim.hora >= 24) {
        fim.hora -= 24;
        fim.dia++;
    }
    while(fim.dia > 30){
        fim.dia -= 30;
        fim.mes++;
    }
    while(fim.mes > 12){
        fim.mes -= 12;
        fim.ano++;
    }
    return fim;
}

int eventosSobrepostos(Evento* raiz, Data novoInicio, float novaDuracao) {
    if (raiz == NULL) {
        return 0; // Não há eventos na árvore
    }

    // Calcular o horário de término do novo evento
    Data fimNovo = calcularFimEvento(novoInicio, novaDuracao);

    // Verifica se o novo evento se sobrepõe ao evento atual
    Data fimExistente = calcularFimEvento(raiz->dataEvento, raiz->duracao);
    
    // Verifica se o novo evento começa antes do existente terminar e se o existente começa antes do novo terminar
    if (compararDatas(novoInicio, fimExistente) < 0 && compararDatas(raiz->dataEvento, fimNovo) < 0) {
        return 1; // Há sobreposição
    }

    // Verifica nas subárvores
    return eventosSobrepostos(raiz->esquerda, novoInicio, novaDuracao) || 
           eventosSobrepostos(raiz->direita, novoInicio, novaDuracao);
}

Evento* inserirEvento(Evento* raiz, Evento* novoEvento) {
    if (raiz == NULL) {
        printf("\nEvento incluido com sucesso!");
        return novoEvento; // A árvore está vazia, retorna o novo evento como raiz
    }

    if (eventosSobrepostos(raiz, novoEvento->dataEvento, novoEvento->duracao)) { // Verificando se o novo evento se sobrepõe ao evento existente
        printf("Erro: O evento com codigo %d se sobrepoe ao evento existente com codigo %d. Assim, nao e possivel cadastrar um novo evento para essa data e horario.\n", novoEvento->codigo, raiz->codigo);
        free(novoEvento); 
        return raiz; 
    }

    int comparacao = compararDatas(novoEvento->dataEvento, raiz->dataEvento); // Comparando as datas dos eventos
    
    if (comparacao < 0) {
        raiz->esquerda = inserirEvento(raiz->esquerda, novoEvento); // Novo evento deve ir para a subárvore esquerda
    } else {
        raiz->direita = inserirEvento(raiz->direita, novoEvento); // Novo evento deve ir para a subárvore direita
    }
    printf("\nEvento incluido com sucesso!");
    return raiz; 
}

void consultarEventosIn_order(Evento* raiz) {
    if(raiz != NULL) {
        consultarEventosIn_order(raiz->esquerda);
        printf("\nCodigo: %d", raiz->codigo);
        printf("\nData: %d/%d/%d\nHorario: %d:%d", raiz->dataEvento.dia, raiz->dataEvento.mes, raiz->dataEvento.ano, raiz->dataEvento.hora, raiz->dataEvento.minuto);
        printf("\nDuracao: %.1f\nDescricao: %s\n", raiz->duracao, raiz->descricao);
        consultarEventosIn_order(raiz->direita);
    }
}

Evento* excluirEvento(Evento* raiz, Data data) {
    if (raiz == NULL) {
        printf("Evento nao encontrado.\n");
        return raiz; // O evento não foi encontrado
    }

    // Comparar data e hora
    int cmp = compararDatas(data, raiz->dataEvento);
    
    if (cmp < 0) {
        raiz->esquerda = excluirEvento(raiz->esquerda, data);
    } 
    else if (cmp > 0) {
        raiz->direita = excluirEvento(raiz->direita, data);
    } 
    else { // Evento encontrado
        // Caso 1: O evento é uma folha
        if (raiz->esquerda == NULL) {
            Evento* temp = raiz->direita;
            free(raiz);
            printf("Evento excluido com sucesso!\n");
            return temp;
        } else if (raiz->direita == NULL) {
            Evento* temp = raiz->esquerda;
            free(raiz);
            printf("Evento excluido com sucesso!\n");
            return temp;
        }

        // Caso 2: O evento tem dois filhos
        Evento* temp = raiz->direita; // Encontrar o menor valor na subárvore direita
        while (temp && temp->esquerda != NULL) {
            temp = temp->esquerda;
        }

        // Substituir o valor do evento a ser excluído pelo menor valor da subárvore direita
        raiz->codigo = temp->codigo;
        raiz->dataEvento = temp->dataEvento;
        raiz->duracao = temp->duracao;
        strcpy(raiz->descricao, temp->descricao);

        // Excluir o evento duplicado
        raiz->direita = excluirEvento(raiz->direita, temp->dataEvento);
    }
    return raiz;
}

int verificaDatasIguais(Data data1, Data data2){
    if(data1.ano == data2.ano && data1.mes == data2.mes && data1.dia == data2.dia){
        return 1;
    }
    else{
        return 0;
    }
}

void consultarEventoData(Evento* raiz, Data data) {
    if (raiz != NULL) {
        // Primeiro, percorre a subárvore esquerda
        consultarEventoData(raiz->esquerda, data);
        
        // Verifica se a data do evento atual é igual à data procurada
        if (verificaDatasIguais(raiz->dataEvento, data)) { 
            printf("\nCodigo: %d\nData: %d/%d/%d\nHorario: %d:%d\nDuracao: %.1f\nDescricao: %s\n", 
                   raiz->codigo, 
                   raiz->dataEvento.dia, 
                   raiz->dataEvento.mes, 
                   raiz->dataEvento.ano, 
                   raiz->dataEvento.hora, 
                   raiz->dataEvento.minuto, 
                   raiz->duracao, 
                   raiz->descricao);
        }
        
        // Depois, percorre a subárvore direita
        consultarEventoData(raiz->direita, data);
    }
}

void percorrerPosOrdem(Evento* raiz) {
    if (raiz != NULL) {
        percorrerPosOrdem(raiz->esquerda); // Primeiro, percorre a subárvore esquerda
        percorrerPosOrdem(raiz->direita); // Depois, percorre a subárvore direita
        percorrerPosOrdem(raiz); // Por último, visita o nó raiz
    }
}

Evento* alterarEvento(Evento* raiz, Data data) { //pressumindo que o evento exista e que, caso a duracao seja alterada, ela nao ira sobrepor outro evento
    Evento *aux = (Evento*)malloc(sizeof(Evento));
    char descricao[100];
    float duracao;
    aux = raiz;
    int option = 0;
    if(aux != NULL) {
        percorrerPosOrdem(aux->esquerda);
        if(verificaDatasIguais(aux->dataEvento, data) == 1 && aux->dataEvento.hora == data.hora && aux->dataEvento.minuto == data.minuto){
            printf("\nSelecione o campo que deseja alterar:\n1-Descricao\n2-Duracao\n3-Descricao e duracao\n4- Cancelar\n");
            scanf("%d", &option);
            if(option == 1){
                printf("\nInsira a nova descricao: ");
                scanf("%s", descricao);
                duracao = aux->duracao;
            }
            else if(option == 2){
                printf("\nInsira a nova duracao (hora): ");
                scanf("%f", &duracao);
                strcpy(descricao, aux->descricao);
            }
            else if(option == 3){
                printf("\nInsira a nova descricao: ");
                scanf("%s", descricao);
                printf("Insira a nova duracao (hora): ");
                scanf("%f", &duracao);
            }
            else{
                printf("\nOperacao cancelada!");
                return raiz;
            }
            strcpy(aux->descricao, descricao);
            aux->duracao = duracao;
            printf("\nEvento alterado com sucesso!");
            return raiz;
        }
        percorrerPosOrdem(aux->direita);
        percorrerPosOrdem(aux);
    }
    else{
        printf("A agenda está vazia.");
        return raiz;
    }
}