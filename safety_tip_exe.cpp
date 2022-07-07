/**
 * @file safety_tip_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2021-01-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "mbed.h"
#include "BufferedSerial.h"
#include "playlist.hpp"
#include "voice_cn_pa.hpp"
#include "jq8400_voice.hpp"
#include "teltonika_fmu130.hpp"
#include "safety_tip_exe.hpp"
#include "USBSerial.h"

#define     SAFETY_TIP_ID_BEGIN     27 // Del archivo de definición de audios
#define     SAFETY_TIP_ID_END       40 // Del archivo de definición de audios

// Variables de Tip de Seguridad
const char  TCA_ID_SAFETY_TIP[]   = "TCA|134|2";
const char  TCA_ID_FRI_ICS5[]     = "TCA|150|1";
const char  TCA_ID_OFF_ICS5[]     = "TCA|150|0";
const char  TCA_ID_GPIO_UPDATE[]  = "TCA|159|";

const int   safety_tip_min        = SAFETY_TIP_ID_BEGIN;
const int   safety_tip_max        = SAFETY_TIP_ID_END;
int         safety_tip_id         = SAFETY_TIP_ID_BEGIN;
char        safety_tip_buffer[64];

// Variables externas
extern BufferedSerial   avl_uart;
extern USBSerial        myPC_debug;
extern bool             estado_actual_ignicion;
extern bool             functionality_safety_tip;
extern bool             flag_safety_tip;
extern bool             flag_fri_ics;

// Entradas digitales externas
extern bool    pilot_seatbelt;
extern bool    copilot_seatbelt;
extern bool    crew_seatbelt;
extern bool    rg9_raining;

/**
 * @brief 
 * 
 */
void play_SafetyTip() {
    flag_safety_tip = true;
}


/**
 * @brief 
 * 
 */
void exe_SafetyTip() {
    flag_safety_tip = false;
    if (functionality_safety_tip) {
        //myPC_debug.printf("\r\nTIP - Reproducir Tip de Seguridad\r\n");
        jq8400_addQueue(safety_tip_id);
        sprintf(safety_tip_buffer, "%s", TCA_ID_SAFETY_TIP);
        //myPC_debug.printf("%s\r\n", safety_tip_buffer);
        tx_fmu130_message(safety_tip_buffer, &avl_uart);
        safety_tip_id++;
        // Mantenerse dentro de la lista de opciones.
        if (safety_tip_id >= SAFETY_TIP_ID_END) {
            safety_tip_id = SAFETY_TIP_ID_BEGIN;
        }
    }
}



/**
 * @brief 
 * 
 */
void send_fri_ics() {
    flag_fri_ics = true;
}



/**
 * @brief 
 * 
 */
extern bool     estado_actual_cinturon_piloto;
extern bool     estado_actual_cinturon_copiloto;
extern bool     estado_actual_cinturon_tripulante;
extern bool     estado_actual_sensor_lluvia;

void tx_fri_ics() {
    flag_fri_ics = false;
    // TCA|159|4,101:0,102:1,103:0,120:0
    //myPC_debug.printf("\r\nFRI - TX Actualizacion GPIO\r\n");
    sprintf(safety_tip_buffer, "%s4,101:%d,102:%d,103:%d,120:%d", 
            TCA_ID_GPIO_UPDATE, 
            estado_actual_cinturon_piloto, estado_actual_cinturon_copiloto, 
            estado_actual_cinturon_tripulante, estado_actual_sensor_lluvia);
    //myPC_debug.printf("%s\r\n", safety_tip_buffer);
    tx_fmu130_message(safety_tip_buffer, &avl_uart);
}
