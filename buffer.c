#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define verifica(test, message) if (!(test)) { printf("Teste falhou: %s\n", message); } else { printf("Teste passou: %s\n", message); }
#define verifica_igual(esperado, atual, message) verifica((esperado) == (atual), message)

typedef struct {
    uint8_t prioridade;
    uint8_t numero;
    uint8_t * nomeTarefa;
} Tarefa;

#define TAMANHO_BUFFER 10
Tarefa buffer[TAMANHO_BUFFER];
uint8_t start = 0, end = 0;

void adicionaElemento(uint8_t novaPrioridade, uint8_t novoNumero, uint8_t * novoNomeTarefa) {
    if(((end+1)%TAMANHO_BUFFER) != start) {
        buffer[end].prioridade = novaPrioridade;
        buffer[end].numero = novoNumero;
        buffer[end].nomeTarefa = novoNomeTarefa;
        end = (end+1)%TAMANHO_BUFFER;
    }
}

void removeElemento(void) {
    if(start != end) {
        buffer[start].prioridade = 0;
        buffer[start].numero = 0;
        buffer[start].nomeTarefa = 0;
        start = (start+1)%TAMANHO_BUFFER;
    }
}

void limpaBuffer() {
    for(uint8_t i = 0; i < TAMANHO_BUFFER; i++) {
        buffer[i].prioridade = 0;
        buffer[i].numero = 0;
        buffer[i].nomeTarefa = 0;
    }
}

void printarBuffer(void) {
    for(uint8_t i = 0; i < TAMANHO_BUFFER; i++) {
        if((buffer[i].nomeTarefa) == NULL) {
            continue;
        }
        printf("Tarefa: %s\n", buffer[i].nomeTarefa);
    }
}

void teste_remover_de_buffer_vazio() {
    start = 0;
    end = 0;
    
    removeElemento();
    
    verifica(start == 0 && end == 0, "End e start não devem ser alterados.");

    for (uint8_t i = 0; i < TAMANHO_BUFFER; i++) {
        verifica(buffer[i].nomeTarefa == NULL, "Buffer vazio não deve conter tarefas.");
    }
}

void teste_adicionar_buffer_cheio() {
    start = 0;
    end = 0;
    
    adicionaElemento(4, 0, "chrome");
    adicionaElemento(7, 1, "vscode");
    adicionaElemento(7, 2, "eclipse");
    adicionaElemento(7, 3, "ltspice");
    adicionaElemento(8, 4, "doom");
    adicionaElemento(3, 5, "bloco de notas");
    adicionaElemento(5, 6, "arquivos");
    adicionaElemento(9, 7, "task manager");
    adicionaElemento(8, 8, "dark souls");
    adicionaElemento(2, 9, "calculadora");
    adicionaElemento(4, 10, "firefox");
    verifica((end+1)%TAMANHO_BUFFER == start, "End e start não devem ser alterados");
    
    for (uint8_t i = 0; i < TAMANHO_BUFFER; i++) {
        if(i == end) {
            verifica(buffer[i].nomeTarefa == NULL, "O buffer deve ter uma posicao livre pelo menos.");
            continue;
        }
        verifica(buffer[i].nomeTarefa != NULL, "Buffer deve conter tarefas em todas as posicoes.");
    }
}

void teste_adicionar_buffer_vazio() {
    start = 0;
    end = 0;

    adicionaElemento(5, 1, "tarefa1");

    verifica_igual(5, buffer[0].prioridade, "Prioridade deve ser 5.");
    verifica_igual(1, buffer[0].numero, "Numero deve ser 1.");
    verifica(strcmp(buffer[0].nomeTarefa, "tarefa1") == 0, "Nome da tarefa deve ser 'tarefa1'.");
    
    verifica_igual(1, end, "End deve ser incrementado.");
}

void teste_preencher_buffer() {
    start = 0;  
    end = 0;
    
    for(uint8_t i = 0; i < TAMANHO_BUFFER - 1; i++) {
        adicionaElemento(i+1, i, "tarefa");
    }
    
    for(uint8_t i = 0; i < TAMANHO_BUFFER; i++) {
        if(i == TAMANHO_BUFFER - 1) {
            verifica(buffer[i].nomeTarefa == NULL, "Ultima posicao deve estar vazia.");
            break;
        }
        verifica(buffer[i].nomeTarefa != NULL, "O buffer deve estar cheio, exceto uma posicao.");
    }
}

void teste_remover_buffer_cheio() {
    teste_preencher_buffer();
    removeElemento();
    
    verifica_igual((end+2)%TAMANHO_BUFFER, start, "Duas posicoes vazias devem existir.");
    verifica_igual(buffer[(end+1)%TAMANHO_BUFFER].nomeTarefa, NULL, "Ultima posicao deve estar vazia.");
    verifica_igual(buffer[(end)%TAMANHO_BUFFER].nomeTarefa, NULL, "Penultima posicao deve estar vazia.");
}

int main() {
    //teste_remover_de_buffer_vazio();
    //teste_adicionar_buffer_cheio();
    //teste_adicionar_buffer_vazio();
    //teste_preencher_buffer();
    //teste_remover_buffer_cheio();
    
    return 0;
}
