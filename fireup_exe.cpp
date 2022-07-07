/**
 * @file fireup_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2021-01-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "mbed.h"
#include "BufferedSerial.h"
//#include "GT521FX.hpp"
#include "New_GT521Fx.hpp"
#include "playlist.hpp"
#include "voice_cn_pa.hpp"
#include "jq8400_voice.hpp"
#include "teltonika_fmu130.hpp"
#include "fireup_exe.hpp"
#include "safety_tip_exe.hpp"
#include "USBSerial.h"

extern BufferedSerial   avl_uart;
extern BufferedSerial   fingerprint;
//extern GT521FX        fingerprint;
extern DigitalOut       out1_fingerprint;
extern DigitalOut       out3_gt521fx;
extern DigitalOut       out4_gt521fx;
extern const char       TCA_ID_LECTOR_HUE[];
extern const char       IO_ENGINE_UNLOCK[];
extern Ticker           tick_fingerprint;
extern Ticker           tick_safety_tip;
extern USBSerial        myPC_debug;

extern bool    finterprint_flag_working;
extern bool    fingerprint_flag_poweroff;
extern bool    functionality_fingerprint_reader;
extern bool    flag_fingerprint_turOn;
extern bool    flag_driver_loggin;
extern bool    functionality_force_driver_buclke;
extern bool    estado_actual_cinturon_piloto;
extern bool    estado_actual_ignicion;
extern bool    functionality_safety_tip;
extern bool    ble_connection_state;
extern float   time_safety_tip;
extern int     flag_fingerprint_query;
extern char    gpio_buffer[];

#define DEBUG_FU_CPP        1
#define UP_FP_READER_GT521     1

/**
 * @brief 
 * 
 */
void query_fingerprint_reader() {
    flag_fingerprint_query = true;
}


/**
 * @brief 
 * 
 */
void process_Engine_FireUp() {

    if (functionality_fingerprint_reader) { // Lector habilitado
        #ifdef DEBUG_FU_CPP
            myPC_debug.printf("EnFU - Lector de huella Habilitado\r\n");
        #endif

        if (functionality_force_driver_buclke) { // Cinturón habilitado
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("EnFU - Ctrn Piloto - Debe estar abrochado\r\n");
            #endif
            if (estado_actual_cinturon_piloto && !(flag_driver_loggin)) {
                out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;  // Se activa el lector
                wait_us(500000);
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("EnFU - OUT3:TRUE");
                #endif
                finterprint_flag_working = true;
                flag_fingerprint_turOn = true;
                jq8400_addQueue(VOICE_functionality_fingerprint_reader);
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("EnFU - Activar lector de Huella\r\n");
                #endif
                tick_fingerprint.attach(&query_fingerprint_reader, 1.0);
                #if UP_FP_READER_GT521 == 1
                    FP_Open();
                #else
                #endif
                sprintf(gpio_buffer, "%s2", TCA_ID_LECTOR_HUE);
                tx_fmu130_message(gpio_buffer, &avl_uart);
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("%s\r\n", gpio_buffer);
                #endif
            }

        } else { // Cinturon deshabilitado
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("EnFU - Ctrn Piloto - No se toma en consideracion\r\n");
            #endif
            out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;
            wait_us(500000);
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("EnFU - OUT3:TRUE");
            #endif
            finterprint_flag_working = true;
            flag_fingerprint_turOn = true;
            jq8400_addQueue(VOICE_functionality_fingerprint_reader);
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("EnFU - Activar lector de Huella");
            #endif
            tick_fingerprint.attach(&query_fingerprint_reader, 1.00);
            #if UP_FP_READER_GT521 == 1
                FP_Open();
            #else
            #endif
            sprintf(gpio_buffer, "%s2", TCA_ID_LECTOR_HUE);
            tx_fmu130_message(gpio_buffer, &avl_uart);
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
        }


    } else { // Lector deshabilitado
        #ifdef DEBUG_FU_CPP
            myPC_debug.printf("\r\nEnFU - Lector de huella Deshabilitado\r\n");
        #endif
        if (functionality_force_driver_buclke) { // Cinturón habilitado
            if (!(flag_driver_loggin)) {
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("\r\nEnFU - Ctrn Piloto - Debe estar abrochado\r\n");
                #endif
                if (estado_actual_cinturon_piloto) {
                    flag_driver_loggin = true;
                    jq8400_addQueue(VOICE_WELLCOME);
                    if (ble_connection_state) {
                        // No hacer nada
                    } else {
                        //jq8400_addQueue(VOICE_FP_AUTH_NO_BLUETOOTH);
                    }
                    #ifdef DEBUG_FU_CPP
                        myPC_debug.printf("EnFU IGN=ON - Permitir encendido con lector Deshabilitado y cinturon abrochado\r\n");
                    #endif
                    tx_fmu130_command(IO_ENGINE_UNLOCK, &avl_uart);
                    out1_fingerprint = 1;
                    wait_us(1000000);
                    out1_fingerprint = 0;
                    if (functionality_safety_tip) {
                        #ifdef DEBUG_FU_CPP
                            myPC_debug.printf("FP - Ticker de Tip de Seguridad Iniciado\r\n");
                        #endif
                        tick_safety_tip.attach(&play_SafetyTip, time_safety_tip);
                    }
                }
            }

        } else { // Cinturón deshabilitado
            #ifdef DEBUG_FU_CPP
                myPC_debug.printf("\r\nEnFU - Ctrn Piloto - No se toma en consideracion\r\n");
            #endif
            jq8400_addQueue(VOICE_WELLCOME);
            if (functionality_safety_tip) {
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("FP - Ticker de Tip de Seguridad Iniciado\r\n");
                #endif
                tick_safety_tip.attach(&play_SafetyTip, time_safety_tip);
            }
        }
    }
}

/**
 * @brief 
 * 
 */
void process_SeatBelt_FireUp() {
    if (functionality_fingerprint_reader) { // Lector habilitado
        if (functionality_force_driver_buclke) { // Cinturón habilitado
            if (estado_actual_ignicion && !(flag_driver_loggin)) {
                out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;
                wait_us(500000);
                finterprint_flag_working = true;
                jq8400_addQueue(VOICE_functionality_fingerprint_reader);
                tick_fingerprint.attach(&query_fingerprint_reader, 1.00);
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("EnFU - OUT3:TRUE");
                    myPC_debug.printf("EnFU - Ignicion debe estar ON\r\n");
                    myPC_debug.printf("EnFU - Activar lector de Huella\r\n");
                #endif
                #if UP_FP_READER_GT521 == 1
                    FP_Open();
                #else
                #endif
                
                flag_fingerprint_turOn = true;
                sprintf(gpio_buffer, "%s2", TCA_ID_LECTOR_HUE);
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("%s\r\n", gpio_buffer);
                #endif
                tx_fmu130_message(gpio_buffer, &avl_uart);
            }
        } 
    } else { // Lector deshabilitado
        if (estado_actual_ignicion && functionality_force_driver_buclke) { 
            if (!(flag_driver_loggin)) {
                flag_driver_loggin = true;
                #ifdef DEBUG_FU_CPP
                    myPC_debug.printf("EnFU - Permitir encendido con lector Deshabilitado y Cinturon abrochado\r\n");
                #endif
                tx_fmu130_command(IO_ENGINE_UNLOCK, &avl_uart);
                out1_fingerprint = 1;
                wait_us(1000000);
                out1_fingerprint = 0;
                jq8400_addQueue(VOICE_WELLCOME);
                if (ble_connection_state) {
                        // No hacer nada
                    } else {
                        //jq8400_addQueue(VOICE_FP_AUTH_NO_BLUETOOTH);
                    }
                if (functionality_safety_tip) {
                    #ifdef DEBUG_FU_CPP
                        myPC_debug.printf("FP - Ticker de Tip de Seguridad Iniciado\r\n");
                    #endif
                    tick_safety_tip.attach(&play_SafetyTip, time_safety_tip);
                }
            }
        }
    }
}