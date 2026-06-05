# Trabalho 1: MPI (Message Passing Interface)

Este repositório contém as soluções dos exercícios sobre comunicação paralela com MPI, abrangendo desde a introspecção de sistema operacional até o uso de tipos derivados e comunicação coletiva.

## Estrutura do Repositório

```text
.
├── relatorio.pdf         # Relatório técnico (3–5 páginas)
├── ex1_hello_os/         # Introspecção de SO e Comunicação em Anel
├── ex2_trap/             # Regra do Trapézio Generalizada e Escalabilidade
├── ex3_psum/             # Soma Paralela (Ponto-a-ponto)
├── ex4_coletivas/        # Hello World (Gather) e Min/Max (Reduce)
├── ex5_vecadd/           # Soma de Vetores (Gather vs Allgather)
└── ex6_derived/          # Uso de Tipos de Dados Derivados MPI

```
## Como Compilar e Executar

Cada diretório de exercício contém um `Makefile` configurado conforme as especificações do trabalho.

* **Compilar:** `make`
* **Executar (padrão p=4):** `make run`
* **Limpar arquivos compilados:** `make clean`

> **Nota:** Os códigos foram desenvolvidos para compilar com o comando `mpicc -O2 -Wall -o <prog> <prog>.c -lm`.

## Resumo dos Exercícios

* **Ex 1:** Análise de PIDs e núcleos de CPU; implementação de anel com tratamento de deadlock.
* **Ex 2:** Implementação da Regra do Trapézio para $n$ arbitrário e análise de escalabilidade forte ($T_p$, $S_p$, $E_p$).
* **Ex 3:** Soma paralela utilizando troca de mensagens simples.
* **Ex 4:** Refatoração de hello world com `MPI_Gather` e cálculo de extremos com `MPI_Reduce`.
* **Ex 5:** Soma de vetores comparando `MPI_Gather` e `MPI_Allgather`.
* **Ex 6:** Definição de `struct` como tipo derivado MPI, utilizando `offsetof` e comparação com múltiplas chamadas de `MPI_Bcast`.