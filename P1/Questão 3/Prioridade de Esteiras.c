/*
 Universidade Federal do Maranhão - UFMA                                                            
 Centro de Ciências Exatas e Tecnologia - CCET 
 Departamento de Engenharia Elétrica
 Professor: Dr. Denivaldo Lopes
 Alunos: Luiz Ruifeng Mei, Matheus Santos Marques e Natalia Pereira Silva
 Data: 13/10/2026
 
 Descrição: Seleciona o preenchimento de uma correia transportadora a
 * partir de quatro alimentadores (Patios A, B, C e D) conectados na Porta A. 
 * O Patio A (RA1) tem a maior prioridade (1), decrescendo ate o Patio D 
 * (RA4) com a menor prioridade (4). As saidas ativadas estao na Porta B.
 * Hardware: PIC16F84A
 */


// Configurações do Microcontrolador
#pragma config FOSC = XT  // Oscilador a Cristal (necessário para os 4MHz)
#pragma config WDTE = OFF // Desativa o Watchdog Timer (evita resets indesejados)
#pragma config PWRTE = ON // Ativa o Power-up Timer
#pragma config CP = OFF   // Desativa a proteção de código

#include <pic16f84a.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

void main(void) {
    // RA1, RA2, RA3 e RA4 como entrada (1 = entrada, 0 = saída)
    TRISA = 0b11110; 
    TRISB = 0x00;    // Toda a Porta B configurada como saída
    
    // Zera as portas na inicialização
    PORTA = 0x00;
    PORTB = 0x00;
    
    while(1) {
        // Avaliação em ordem de prioridade
        if (PORTAbits.RA1 == 1) {
            PORTB = 0b00000001; // Pátio A (Prioridade 1): Liga apenas RB0
        } else if (PORTAbits.RA2 == 1) {
            PORTB = 0b00000010; // Pátio B (Prioridade 2): Liga apenas RB1
        } else if (PORTAbits.RA3 == 1) {
            PORTB = 0b00000100; // Pátio C (Prioridade 3): Liga apenas RB2
        } else if (PORTAbits.RA4 == 1) {
            PORTB = 0b00001000; // Pátio D (Prioridade 4): Liga apenas RB3
        } else {
            PORTB = 0x00;       // Se nenhum pátio enviar sinal, apaga todas as saídas
        }
    }
}