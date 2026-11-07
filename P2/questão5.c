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
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* CICLO INFINITO DE EXECUÇÃO */
  for(;;) {
  
      /* 1. VERIFICAÇÃO DE MAU FUNCIONAMENTO (Filtro do Alarme) */
      if ((SENSOR_A_GetVal() && !SENSOR_B_GetVal()) || 
          (SENSOR_B_GetVal() && !SENSOR_C_GetVal()) || 
          (SENSOR_C_GetVal() && !SENSOR_D_GetVal())) {
          
          /* Medida de Segurança Crítica em caso de falha nos sensores: */
          VALV_EVE_SetVal();    // DESLIGADO (Injeta 1 -> Corta fluxo)
          VALV_EVS_SetVal();    // DESLIGADO (Injeta 1 -> Corta fluxo)
          LED_ALARME_ClrVal();  // LIGADO (Zera o pino -> ACENDE Alarme Active-Low)
          
          estado_atual = ESTADO_DESLIGADO; // Força retorno ao repouso seguro
      } 
      else {
          /* Operação física normal: garante sinal de alarme apagado (Nível Alto) */
          LED_ALARME_SetVal(); 

          /* 2. DETECÇÃO DE BORDA DO BOTÃO I (Filtro de Pulso Toggle) */
          if (BOTAO_I_GetVal() && I_anterior == 0) { 
              if (estado_atual == ESTADO_DESLIGADO) {
                  estado_atual = ESTADO_ENCHENDO;
              } else {
                  estado_atual = ESTADO_PARADA;
              }
          }
          I_anterior = BOTAO_I_GetVal() ? 1 : 0; // Atualiza o histórico com valor normalizado

          /* 3. MÁQUINA DE ESTADOS HIDRÁULICA DO RESERVATÓRIO */
          switch (estado_atual) {
              
              case ESTADO_DESLIGADO:
                  VALV_EVE_SetVal(); // Garante atuadores desligados via SetVal
                  VALV_EVS_SetVal(); 
                  break;

              case ESTADO_ENCHENDO:
                  VALV_EVE_ClrVal(); // LIGA/Abre Entrada (Nível Baixo)
                  VALV_EVS_SetVal(); // DESLIGA/Fecha Saída (Histerese de subida inicial)
                  
                  /* Transição: O fluido subiu e cobriu o sensor operacional B */
                  if (SENSOR_B_GetVal()) {
                      estado_atual = ESTADO_OPERANDO;
                  }
                  break;

              case ESTADO_OPERANDO:
                  /* Regra de atuação da Eletroválvula de Entrada (EVE) */
                  if (SENSOR_A_GetVal()) {
                      VALV_EVE_SetVal(); // Reservatório cheio: DESLIGA Entrada
                  } else {
                      VALV_EVE_ClrVal(); // Nível flutuou abaixo do topo: LIGA Entrada
                  }

                  /* Regra de atuação da Eletroválvula de Saída (EVS) */
                  if (SENSOR_A_GetVal() || SENSOR_B_GetVal()) {
                      VALV_EVS_ClrVal(); // Volume seguro em A ou B: LIGA Saída para escoamento
                  } 
                  
                  /* Filtro de Histerese de Descida: Se o nível cair abaixo de B e tocar em D */
                  if (SENSOR_D_GetVal() && !SENSOR_B_GetVal()) {
                      VALV_EVS_SetVal(); // DESLIGA Saída para recuperar volume interno
                      estado_atual = ESTADO_ENCHENDO; // Retorna para o ciclo de recarga pura
                  }
                  break;

              case ESTADO_PARADA:
                  VALV_EVE_SetVal(); // DESLIGA entrada imediatamente!
                  
                  /* Rotina de Proteção: Mantém saída aberta escoando enquanto houver fluido */
                  if (SENSOR_D_GetVal()) {
                      VALV_EVS_ClrVal(); // LIGA saída para limpar o tanque
                  } else {
                      VALV_EVS_SetVal(); // Tanque completamente vazio (D=0): Fecha saída
                      estado_atual = ESTADO_DESLIGADO; // Encerra processo e vai para repouso
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
