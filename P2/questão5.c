/** ###################################################################
**     Filename  : questao5.c
**     Project   : questao5
**     Processor : MC9S12C128CFU16
**     Version   : Driver 01.14
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 05/11/2026, 18:59
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE questao5 */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "BOTAO_I.h"
#include "SENSOR_A.h"
#include "SENSOR_B.h"
#include "SENSOR_C.h"
#include "SENSOR_D.h"
#include "VALV_EVS.h"
#include "VALV_EVE.h"
#include "LED_ALARME.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"


/* Definição dos Estados Operacionais (Máquina de Estados) */
#define ESTADO_DESLIGADO  0
#define ESTADO_ENCHENDO   1
#define ESTADO_OPERANDO   2  
#define ESTADO_PARADA     3  

unsigned char estado_atual = ESTADO_DESLIGADO;
unsigned char I_anterior = 0;

void main(void)
{
  /* Variáveis lógicas normalizadas (1 = Ativo, 0 = Inativo) */
  unsigned char I, A, B, C, D;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* LAÇO INFINITO DE VARREDURA DA BANCADA REAL */
  for(;;) {
      
      /* ===================================================================
      ** 1. REORDENAÇÃO E CORREÇÃO ELÉTRICA DOS PINOS
      ** Trocamos as leituras para casar com o comportamento real da placa:
      ** Como segurar SW1 (SENSOR_A) liga a entrada, ele é o verdadeiro Botão I!
      ** O Switch de retenção (BOTAO_I) passa a atuar como o sensor de topo A.
      ** =================================================================== */
      I = !SENSOR_A_GetVal();  // SW1 pressionado (0) ativa o comando de ligar (I = 1)
      A = !BOTAO_I_GetVal();   // O Switch ligado (0) passa a simular o sensor de topo A (A = 1)
      
      /* Mantemos a leitura normalizada dos sensores de nível intermediários */
      B = SENSOR_B_GetVal() ? 1 : 0;
      C = SENSOR_C_GetVal() ? 1 : 0;
      D = SENSOR_D_GetVal() ? 1 : 0;

      /* 2. VERIFICAÇÃO DE MAU FUNCIONAMENTO (Filtro do Alarme) */
      if ((A && !B) || (B && !C) || (C && !D)) {
          VALV_EVE_SetVal();    // DESLIGADO (Nível Alto)
          VALV_EVS_SetVal();    // DESLIGADO (Nível Alto)
          LED_ALARME_ClrVal();  // LIGADO (ACENDE Alarme - Nível Baixo)
          
          estado_atual = ESTADO_DESLIGADO; // Força retorno ao repouso seguro
      } 
      else {
          LED_ALARME_SetVal();  // DESLIGADO (Apaga Alarme - Nível Alto)

          /* 3. LÓGICA DO BOTÃO OPERANDO POR PULSO (TOGGLE)
             Como o comando I agora está no botão SW1 (pulso), a detecção de borda 
             de subida garante que pressionar uma vez liga e pressionar de novo desliga */
          if (I == 1 && I_anterior == 0) {
              if (estado_atual == ESTADO_DESLIGADO || estado_atual == ESTADO_PARADA) {
                  estado_atual = ESTADO_ENCHENDO;
              } else {
                  estado_atual = ESTADO_PARADA;
              }
          }
          I_anterior = I; // Atualiza o histórico do botão

          /* 4. MÁQUINA DE ESTADOS HIDRÁULICA DO RESERVATÓRIO */
          switch (estado_atual) {
              
              case ESTADO_DESLIGADO:
                  VALV_EVE_SetVal(); // Garante válvulas fechadas
                  VALV_EVS_SetVal(); 
                  break;

              case ESTADO_ENCHENDO:
                  VALV_EVE_ClrVal(); // LIGA Entrada (Abre válvula de entrada)
                  VALV_EVS_SetVal(); // DESLIGA Saída (Fecha válvula de saída)
                  
                  /* Se o nível atingir B, passa para o regime operacional */
                  if (B) {
                      estado_atual = ESTADO_OPERANDO;
                  }
                  break;

              case ESTADO_OPERANDO:
                  /* Regra da Válvula de Entrada (EVE) */
                  if (A) {
                      VALV_EVE_SetVal(); // Se o Switch (A) for ativado (Cheio): DESLIGA Entrada
                  } else {
                      VALV_EVE_ClrVal(); // Caso contrário, LIGA Entrada
                  }

                  /* Regra da Válvula de Saída (EVS) */
                  if (A || B) {
                      VALV_EVS_ClrVal(); // Nível seguro: LIGA Saída
                  } 
                  
                  /* Filtro de Histerese de Descida: Se tocar em D e desativar B */
                  if (D && !B) {
                      VALV_EVS_SetVal(); // DESLIGA Saída
                      estado_atual = ESTADO_ENCHENDO; 
                  }
                  break;

              case ESTADO_PARADA:
                  VALV_EVE_SetVal(); // DESLIGA entrada imediatamente
                  
                  if (D) {
                      VALV_EVS_ClrVal(); // LIGA saída para escoar o resto
                  } else {
                      VALV_EVS_SetVal(); // Tanque vazio (D=0): Fecha saída
                      estado_atual = ESTADO_DESLIGADO; 
                  }
                  break;
          }
      }
  } 
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END questao5 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.05 [04.46]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
