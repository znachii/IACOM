/*
 Universidade Federal do Maranhão - UFMA                                                            
 Centro de Ciências Exatas e Tecnologia - CCET 
 Departamento de Engenharia Elétrica
 Professor: Dr. Denivaldo Lopes
 Alunos: Luiz Ruifeng Mei, Matheus Santos Marques e Natalia Pereira Silva
 Data: 13/10/2026

 Descrição: Verifica continuamente o sensor no pino RA1. Retorna 0x72 
 * na Porta B caso o sensor seja 1, ou 0x81 caso seja 0. Simultaneamente, 
 * gera um sinal quadrado com frequencia de 4 Hz no pino RA0.
 * Hardware: PIC16F84A
 */


// Configurações do Microcontrolador
#pragma config FOSC = XT  // Oscilador a Cristal (necessário para os 4MHz)
#pragma config WDTE = OFF // Desativa o Watchdog Timer (evita resets indesejados)
#pragma config PWRTE = OFF // Desativa o Power-up Timer
#pragma config CP = OFF   // Desativa a proteção de código

#define _XTAL_FREQ 4000000 // Clock de 4 MHz exigido
#include <pic16f84a.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

void main(void) {
    TRISA = 0b00010; // RA1 configurado como entrada; RA0 como saída
    TRISB = 0x00;    // Toda a Porta B configurada como saída
    
    // Zera as portas para evitar lixo de memória na inicialização
    PORTA = 0x00;    
    PORTB = 0x00;

    while(1) {
        // Verifica o sensor em RA1
        if (PORTAbits.RA1 == 1) {
            PORTB = 0x72; // Retorna 0x72 se o sensor for 1
        } else {
            PORTB = 0x81; // Retorna 0x81 caso contrário
        }
        
        // Gera o sinal quadrado de 4Hz em RA0
        PORTAbits.RA0 ^= 1;             // Inverte o estado do pino
        __delay_ms(125);                // Atraso de meia onda (125ms + 125ms = 250ms = 4Hz)
    }
}
