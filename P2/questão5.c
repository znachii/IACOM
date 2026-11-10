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

/* User includes (#include below this line is not maintained by Processor Expert) */

/* ===================================================================
** MAQUINA DE ESTADOS E MEMÓRIA DE CONTROLE (Escopo Global)
** =================================================================== */
#define ESTADO_DESLIGADO  0
#define ESTADO_ENCHENDO   1
#define ESTADO_OPERANDO   2  
#define ESTADO_PARADA     3  

unsigned char estado_atual = ESTADO_DESLIGADO;
unsigned char I_anterior = 0;

void main(void)
{
  unsigned char I, A, B, C, D;
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* CICLO INFINITO DE EXECUÇÃO */
  for(;;) {
  
      /* ===================================================================
      ** 1. LEITURA E NORMALIZAÇÃO DAS ENTRADAS (Tratamento de Active-Low)
      ** Como BOTAO_I (Switch) e SENSOR_A (SW1) enviam 0 quando ativos,
      ** usamos a exclamacao (!) para transformá-los em 1 dentro do software.
      ** =================================================================== */
      I = !BOTAO_I_GetVal();   // Se o Switch fechar p/ GND (0), I vira 1 (Ativo)
      A = !SENSOR_A_GetVal();  // Se segurar o botão SW1 (0), A vira 1 (Ativo)
      
      /* Se os sensores B, C e D forem sensores normais que enviam 1 com água, 
         mantenha-os assim. Se forem chaves para o GND também, adicione ! neles. */
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

          /* 3. LÓGICA DO SWITCH OPERANDO COMO INTERRUPTOR DIRETO (LIGA/DESLIGA)
             Como o Botão I é um switch (retém a posição), removemos a detecção de borda
             de pulso. O sistema liga se o switch estiver em 1 e vai para parada se estiver em 0 */
          if (I == 1) {
              if (estado_atual == ESTADO_DESLIGADO || estado_atual == ESTADO_PARADA) {
                  estado_atual = ESTADO_ENCHENDO;
              }
          } else {
              if (estado_atual == ESTADO_ENCHENDO || estado_atual == ESTADO_OPERANDO) {
                  estado_atual = ESTADO_PARADA;
              }
          }

          /* 4. MÁQUINA DE ESTADOS HIDRÁULICA DO RESERVATÓRIO */
          switch (estado_atual) {
              
              case ESTADO_DESLIGADO:
                  VALV_EVE_SetVal(); // Garante válvulas fechadas
                  VALV_EVS_SetVal(); 
                  break;

              case ESTADO_ENCHENDO:
                  VALV_EVE_ClrVal(); // LIGA Entrada (Abre válvula)
                  VALV_EVS_SetVal(); // DESLIGA Saída (Fecha válvula)
                  
                  if (B) {
                      estado_atual = ESTADO_OPERANDO;
                  }
                  break;

              case ESTADO_OPERANDO:
                  /* Regra da Válvula de Entrada (EVE) */
                  if (A) {
                      VALV_EVE_SetVal(); // Cheio (SW1 pressionado): DESLIGA Entrada
                  } else {
                      VALV_EVE_ClrVal(); // LIGA Entrada
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
