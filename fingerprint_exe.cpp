/**
 * @file fingerprint_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "mbed.h"
#include "BufferedSerial.h"
//#include "GT521FX.hpp"
#include "New_GT521Fx.hpp"
#include "jq8400_voice.hpp"
#include "voice_cn_pa.hpp"
#include "teltonika_fmu130.hpp"
#include "jq8400_voice.hpp"
#include "playlist.hpp"
#include "safety_tip_exe.hpp"
#include "USBSerial.h"


// Objetos Entradas Digitales / Seriales
/**
 * @brief 
 * 
 */
extern BufferedSerial   avl_uart;
extern BufferedSerial   voice_uart;
extern DigitalOut       out1_fingerprint;
extern DigitalOut       out3_gt521fx;
extern DigitalOut       out4_gt521fx;
extern BufferedSerial   fingerprint;
extern USBSerial        myPC_debug;
extern Ticker           tick_fingerprint;
extern Ticker           tick_safety_tip;
extern queue            colaPlaylist;


// Variables Externas
/**
 * @brief 
 * 
 */
extern float    time_safety_tip;
extern bool     flag_fingerprint_Sleep;
extern bool     flag_driver_loggin;
extern bool     functionality_safety_tip;
extern bool     flag_fingerprint_query;
extern bool     ble_connection_state;
extern bool     fingerprint_override;
extern bool     ignition;
extern bool     finterprint_flag_working;
extern bool     fingerprint_flag_poweroff;
extern int      fingerprint_login;
extern int      flag_fingerprint_turOn;
extern int      fp_override_limit;
extern int      fp_unauthorized_count;
extern int      items_queue;


// Variables locales
/**
 * @brief 
 * 
 */
int         fp_answer = 0;
int         w;
char        id_buffer[64];
const char  TCA_ID_FP_LOGIN[]       = "TCA|004|DTK_FP;";
const char  TCA_ID_UNIDENTIFIED[]   = "TCA|110|";
const char  TCA_ID_LECTOR_HUE[]     = "TCA|111|";
const char  TCA_FP_INIT[]           = "TCA|154|";
const char  TCA_ID_OVERRIDE_ID[]    = "TCA|149|0";
const char  FP_ENGINE_LOCK[]        = "setdigout 0?? ? ? ? ? ? ?";
const char  FP_ENGINE_UNLOCK[]      = "setdigout 1?? ? ? ? ? ? ?";

#define     DEBUG_F_CPP         1
#define     FP_FP_READER_GT521     1
/**
 * @brief 
 * 
 */
void identify_fingerPrint() {
    flag_fingerprint_query = false;
    memset(id_buffer, '\0', sizeof(id_buffer));

    if (out3_gt521fx) {
        /* DESHABILITADA LA FUNCIÓN DE SLEEP EN EL LECTOR
            if (flag_fingerprint_Sleep) {
                fingerprint.WakeUp();
                flag_fingerprint_Sleep = false;
                //myPC_debug.printf("FP - GT521Fx@WakeUp\r\n");
                jq8400_addQueue(VOICE_functionality_fingerprint_reader);
            }
        */
        if (!(flag_driver_loggin) && ignition) {
            //Verificar con ignicion
            //fingerprint.CmosLed(1);
            #if FP_FP_READER_GT521 == 1
                FP_LED_open();
            #else
            #endif
            wait_us(50000);

            #if FP_FP_READER_GT521 == 1
                fp_answer = FP_IsPressFinger();
            #else
            #endif

            if (fp_answer == 0) {
                #if FP_FP_READER_GT521 == 1
                    FP_CaptureFinger(1);
                #else
                #endif

                #ifdef DEBUG_F_CPP
                    myPC_debug.printf("FP - Capturando...\r\n");
                #endif
                
                #if FP_FP_READER_GT521 == 1
                    fingerprint_login = FP_Identify();
                    FP_LED_close();
                #else
                #endif

                #ifdef DEBUG_F_CPP
                    myPC_debug.printf("FP - ID: %d\r\n", fingerprint_login);
                #endif
                
                if (fingerprint_login >= 0) {
                    #ifdef DEBUG_F_CPP
                        myPC_debug.printf("FP - Autorizado...\r\n");
                    #endif
                    tick_fingerprint.detach();
                    jq8400_addQueue(VOICE_DRIVER_AUTHORIZED);
                    jq8400_addQueue(VOICE_WELLCOME);

                    // Limpiar cola
                    if (ble_connection_state) {
                        // No hacer nada
                    } else {
                        // jq8400_addQueue(VOICE_FP_AUTH_NO_BLUETOOTH); // Solicitar asistencia remota
                    }
                    
                    #ifdef DEBUG_F_CPP
                        myPC_debug.printf("%s\r\n", FP_ENGINE_UNLOCK);
                    #endif
                    tx_fmu130_command(FP_ENGINE_UNLOCK, &avl_uart);
                    out1_fingerprint = 1;
                    wait_us(1000000);
                    out1_fingerprint = 0;
                    fp_unauthorized_count = 0;
                    flag_fingerprint_turOn = false;
                    flag_driver_loggin = true;
                    #if FP_FP_READER_GT521 == 1
                        FP_LED_close();
                        FP_Close();
                    #else
                    #endif
                    wait_us(1000000);
                    fingerprint_flag_poweroff = true;

                    if (functionality_safety_tip) {
                        #ifdef DEBUG_F_CPP
                            myPC_debug.printf("FP - Ticker de Tip de Seguridad Iniciado\r\n");
                        #endif
                        tick_safety_tip.attach(&play_SafetyTip, time_safety_tip);
                    }

                    /* DESHABILITADA LA FUNCIÓN DE SLEEP EN EL LECTOR
                            fingerprint.Sleep();
                            flag_fingerprint_Sleep = true;
                        */

                    sprintf(id_buffer, "%s%04d", TCA_ID_FP_LOGIN, fingerprint_login);
                    tx_fmu130_message(id_buffer, &avl_uart);
                    #ifdef DEBUG_F_CPP
                        myPC_debug.printf("GPIO - GT521Fx@Sleep %d\r\n", flag_fingerprint_Sleep);
                        myPC_debug.printf("%s\r\n", id_buffer);
                    #endif

                    wait_us(100000);
                    sprintf(id_buffer, "%s3", TCA_ID_LECTOR_HUE);
                    tx_fmu130_message(id_buffer, &avl_uart);
                    #ifdef DEBUG_F_CPP
                        myPC_debug.printf("%s\r\n", id_buffer);
                    #endif
                } else {
                    jq8400_addQueue(VOICE_DRIVER_UNAUTHORIZED);
                    sprintf(id_buffer, "%s%d", TCA_ID_UNIDENTIFIED, fingerprint_login);
                    #ifdef DEBUG_F_CPP
                        myPC_debug.printf("FP - No Autorizado...\r\n");
                        myPC_debug.printf("%s\r\n", id_buffer);
                    #endif
                    tx_fmu130_message(id_buffer, &avl_uart);
                    
                    #if FP_FP_READER_GT521 == 1
                        FP_LED_close();
                    #else
                    #endif

                    if (fingerprint_override) {
                        fp_unauthorized_count++;
                        #ifdef DEBUG_F_CPP
                            myPC_debug.printf("FP - %d de %d para Anular Identificacion de Huella\r\n", fp_unauthorized_count, fp_override_limit);
                        #endif
                        if (fp_unauthorized_count >= fp_override_limit)
                        {
                            #ifdef DEBUG_F_CPP
                                myPC_debug.printf("FP - Anulando identificacion de Conductor...\r\n");
                            #endif
                            /////////////////////////////
                            // Limpiar cola de mensajes de voz

                            //
                            if (ble_connection_state) {
                                jq8400_addQueue(VOICE_DRIVER_AUTHORIZED);
                            } else {
                                // jq8400_addQueue(VOICE_FP_AUTH_NO_BLUETOOTH); // Solicitar asistencia remota
                            }
                            #ifdef DEBUG_F_CPP
                                myPC_debug.printf("%s\r\n", FP_ENGINE_UNLOCK);
                            #endif
                            tx_fmu130_command(FP_ENGINE_UNLOCK, &avl_uart);
                            out1_fingerprint = 1;
                            wait_us(1000000);
                            out1_fingerprint = 0;
                            fp_unauthorized_count = 0;
                            flag_fingerprint_turOn = false;
                            flag_driver_loggin = true;
                            tick_fingerprint.detach();
                            #if FP_FP_READER_GT521 == 1
                                FP_LED_close();
                            #else
                            #endif
                            wait_us(1000000);
                            fingerprint_flag_poweroff = true;

                            if (functionality_safety_tip) {
                                #ifdef DEBUG_F_CPP
                                    myPC_debug.printf("FP - Ticker de Tip de Seguridad Iniciado\r\n");
                                #endif
                                tick_safety_tip.attach(&play_SafetyTip, time_safety_tip);
                            }
                            sprintf(id_buffer, "%s", TCA_ID_OVERRIDE_ID);
                            #ifdef DEBUG_F_CPP
                                myPC_debug.printf("%s\r\n", id_buffer);
                            #endif
                            tx_fmu130_message(id_buffer, &avl_uart);
                        }
                    }
                }
            } else {
                #if FP_FP_READER_GT521 == 1
                    FP_LED_close();
                #else
                #endif

                // Limpiar cola de mensajes de voz
                #ifdef DEBUG_F_CPP
                    myPC_debug.printf("FP - No Presionado\r\n");
                #endif
            }
        }
    }
}

/**
 * @brief 
 * 
 */
void initFingerprintReader() {
    // Variable Interna
    int answer_GT521Fx = -1;
    wait_us(100000); // Tiempo de espera para inicializar el sensor
    #ifdef DEBUG_F_CPP
        myPC_debug.printf("\r\nInicializando GT521Fx");
    #endif
    
    #if FP_FP_READER_GT521 == 1
        answer_GT521Fx = FP_Open();
    #else
    #endif
    sprintf(id_buffer, "%s%d", TCA_FP_INIT, answer_GT521Fx);
    #ifdef DEBUG_F_CPP
        myPC_debug.printf("%s\r\n", id_buffer);
        myPC_debug.printf("\r\nGT521Fx Open: OK\r\n");
    #endif
    tx_fmu130_message(id_buffer, &avl_uart);
    #if FP_FP_READER_GT521 == 1
        FP_Close();
    #else
    #endif
}
