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
#include "Botao_I.h"
#include "Sensor_A.h"
#include "Sensor_B.h"
#include "Sensor_C.h"
#include "Sensor_D.h"
#include "Evs.h"
#include "Eve.h"
#include "Alerta.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

void main(void)
{
  /* Write your local variable definition here */
  
  unsigned char evs_estado = 0; /* Variável para manter a histerese da válvula de saída */
  unsigned char eve_estado = 0;
  unsigned char erro_logico = 0;
    
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  
  /* Garante todas as saidas desligadas */
  Evs_PutVal(1);
  Eve_PutVal(1);
  Alerta_PutVal(1);

  /* CICLO INFINITO DE EXECUÇÃO */
  for(;;) {
         /*
         * Leitura das entradas e conversão para lógica positiva (1 = Ativo/Líquido presente)
         * Sensor A (Push button): Ativa em Nível Baixo (0)
         * Sensores B, C, D e Botão I (Switches): Ativam em Nível Alto (1)
         */
         unsigned char i_ligado = (Botao_I_GetVal() != 0);
         unsigned char nivel_A  = (Sensor_A_GetVal() == 0); /* Mantém igual, pois ativa em nível baixo */
         unsigned char nivel_B  = (Sensor_B_GetVal() != 0);
         unsigned char nivel_C  = (Sensor_C_GetVal() != 0);
         unsigned char nivel_D  = (Sensor_D_GetVal() != 0);

        /* 
         * Lógica de Erro (Mapa de Karnaugh)            
         * Acusa erro se: A tem líquido e B não, ou B tem e C não, ou C tem e D não.
         */
        erro_logico = (nivel_A && !nivel_B) || 
                      (nivel_B && !nivel_C) || 
                      (nivel_C && !nivel_D);

        /* Lógica de Controle do Sistema */
        if (i_ligado) {
            /* 
             * Eletroválvula de Entrada (Eve)
             * Liga se o sistema estiver acionado e o nível for D, C ou B. No A, desliga.
             */
            if (!nivel_A && (nivel_B || nivel_C || nivel_D)) {
                eve_estado = 1;
            } else {
                eve_estado = 0;
            }

            /* 
             * Eletroválvula de Saída (Evs) - Lógica com Histerese
             * Liga se estiver em A ou B. Desliga em D (ausência de C) até atingir B.
             */
            if (nivel_B || nivel_A) {
                evs_estado = 1; /* Liga a saída */
            } else if (!nivel_C) {
                evs_estado = 0; /* Desliga ao descer abaixo de C (chegando em D) */
            }
            
        } else {
            /* Sistema Desligado pelo botão I */
            eve_estado = 0; /* Interrompe a entrada imediatamente */
            
            /* Escoa todo o líquido até que o sensor D acuse vazio */
            if (nivel_D) {
                evs_estado = 1;
            } else {
                evs_estado = 0;
            }
        }

        /* 
         * Atualização dos pinos
         * (Lógica invertida na saída física: 0 = Liga/Atua, 1 = Desliga)
         */
         
        if (evs_estado) {
            Evs_PutVal(0); /* Liga Evs */
        } else {
            Evs_PutVal(1); /* Desliga Evs */
        }

        if (eve_estado) {
            Eve_PutVal(0); /* Liga Eve */
        } else {
            Eve_PutVal(1); /* Desliga Eve */
        }

        if (erro_logico) {
            Alerta_PutVal(0); /* Liga Alerta */
        } else {
            Alerta_PutVal(1); /* Desliga Alerta */
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
