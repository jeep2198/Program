/**
 * @file gpio_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "mbed.h"
#include "inputDebounce.hpp"
#include "BufferedSerial.h"
#include "New_GT521Fx.hpp"
#include "jq8400_voice.hpp"
#include "voice_cn_pa.hpp"
#include "teltonika_fmu130.hpp"
#include "gpio_exe.hpp"
#include "fireup_exe.hpp"
#include "safety_tip_exe.hpp"
#include "USBSerial.h"


//Objetos Entradas Digitales / Seriales
/**
 * @brief 
 * 
 */
extern BufferedSerial   avl_uart;
extern BufferedSerial   voice_uart;
extern BufferedSerial   fingerprint;
extern DigitalIn        in1_ignition;
extern DigitalIn        in2_pilot;
extern DigitalIn        in3_copilot;
extern DigitalIn        in4_crew;
extern DigitalIn        in5_rain_sensor;
extern DigitalIn        bluetooth_state;
extern USBSerial        myPC_debug;
extern Ticker           tick_fingerprint;
extern Ticker           tick_safety_tip;
extern Ticker           tick_fri_ics;


// Variables Externas
/**
 * @brief 
 * 
 */
extern bool    ignition;
extern bool    ignition_prev;
extern bool    ignition_samples[4];
extern int     ignition_counter;

extern bool    pilot_seatbelt;
extern bool    pilot_seatbelt_prev;
extern bool    pilot_seatbelt_samples[4];
extern int     pilot_seatbelt_c;

extern bool    copilot_seatbelt;
extern bool    copilot_seatbelt_prev;
extern bool    copilot_seatbelt_samples[4];
extern int     copilot_seatbelt_c;

extern bool    crew_seatbelt;
extern bool    crew_seatbelt_prev;
extern bool    crew_seatbelt_samples[4];
extern int     crew_seatbelt_c;

extern bool    ble_connection_state;
extern bool    ble_connection_state_prev;
extern bool    ble_connection_state_samples[4];
extern int     ble_connection_state_samples_c;

extern bool    rg9_raining;
extern bool    rg9_raining_prev;
extern bool    rg9_raining_samples[4];
extern int     rg9_raining_samples_c;

extern int     flag_fingerprint_turOn;
extern int     flag_fingerprint_Sleep;
extern int     flag_ignition;
extern int     flag_pilot_seatbelt;
extern int     flag_copilot_seatbelt;
extern int     flag_crew_pilot_seatbelt;
extern int     flag_fingerprint_query;
extern int     flag_ble_connection_state; 

extern bool    functionality_fingerprint_reader;
extern bool    fingerprint_remotly_disable;
extern bool    flag_driver_loggin;
extern bool    finterprint_flag_working;
extern bool    fingerprint_flag_poweroff;
extern bool    functionality_force_driver_buclke; 
extern bool    functionality_seatbelt_reading;
extern bool    functionality_safety_tip;
extern bool    flag_read_inputs;
extern bool    flag_query_rain_sensor;
extern bool    functionality_rain_sensor;
extern bool    functionality_rainSensor_silentMode;
extern int     flag_rainsensorDigInput;

extern char     wet_Speed_CMD[30];
extern char     dry_Speed_CMD[30];
extern int      pilot_buckleUp;
extern int      pilot_unfasten;
extern int      copilot_buckleUp;
extern int      copilot_unfasten;
extern int      crew_buckleUp;
extern int      crew_unfasten;

// Variables locales
/**
 * @brief 
 * 
 */
char        gpio_buffer[64];
bool        estado_actual_cinturon_piloto;
bool        estado_actual_cinturon_copiloto;
bool        estado_actual_cinturon_tripulante;
bool        estado_actual_sensor_lluvia;
extern bool estado_actual_ignicion;
const char  TCA_ID_PILOTO[]     = "TCA|101|";
const char  TCA_ID_COPILOTO[]   = "TCA|102|";
const char  TCA_ID_TRIPULANTE[] = "TCA|103|";
const char  TCA_ID_LECTOR_HUE[] = "TCA|111|";
const char  TCA_ID_BLUETOOTH[]  = "TCA|124|";
const char  TCA_ID_WEATHER[]    = "TCA|120|";
const char  IO_ENGINE_LOCK[]    = "setdigout 0?? ? ? ? ? ? ?";
const char  IO_ENGINE_UNLOCK[]  = "setdigout 1?? ? ? ? ? ? ?";

#define DEBUG_GPIO_CPP
#define IO_FP_READER_GT521     1

/**
 * @brief 
 * 
 */
void process_Inputs() {
    flag_read_inputs = false;
    memset(gpio_buffer, '\0', sizeof(gpio_buffer));
    

    /**
     * @brief 
     * 
     */
    flag_ignition = exeIgnition(&in1_ignition, ignition_samples, &ignition, &ignition_prev, &ignition_counter);

    /**
     * @brief Construct a new if object
     * 
     */
    if (flag_ignition == 1) {
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("\r\nGPIO - Ign ON\r\n");
        #endif
        estado_actual_ignicion = true;
        tick_fri_ics.detach();
        tick_fri_ics.attach(&send_fri_ics, 300.0);
        process_Engine_FireUp();
        if (rg9_raining) {
            if (!(functionality_rainSensor_silentMode)) {
                jq8400_addQueue(VOICE_TRACK_CONDITION_WET);
            }
        }

    } else if (flag_ignition == -1) {
        jq8400_addQueue(VOICE_DOOR_SAFETY_REMINDER);
        tick_fri_ics.detach();
        tick_fri_ics.attach(&send_fri_ics, 1800.0);
        if (functionality_fingerprint_reader) {
            #if IO_FP_READER_GT521 == 1
                FP_Close();
            #else
            #endif
            wait_us(1000000);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Pendiente apagar FP GT521Fx...\r\n");
            #endif
            fingerprint_flag_poweroff = true;
        }
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("GPIO - Ign OFF\r\n");
        #endif
        estado_actual_ignicion = false;
        flag_driver_loggin = false;
        if (functionality_force_driver_buclke || functionality_fingerprint_reader) {
            tx_fmu130_command(IO_ENGINE_LOCK, &avl_uart);        // No se debe enviar a desactivar la entrada.
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", IO_ENGINE_LOCK);
            #endif
        }
        flag_fingerprint_turOn = false;
        tick_fingerprint.detach();
        if (functionality_safety_tip) {
            tick_safety_tip.detach();
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Ticker de Tip de Seguridad Detenido\r\n");
            #endif
        }
        sprintf(gpio_buffer, "%s3", TCA_ID_LECTOR_HUE);
        tx_fmu130_message(gpio_buffer, &avl_uart);
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("%s\r\n", gpio_buffer);
        #endif
        // Se deja activado el lector para la siguiente interacción
        if (!(fingerprint_remotly_disable)) {
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - FP Activado automaticamente\r\n");
                myPC_debug.printf("%s\r\n", IO_ENGINE_LOCK);
            #endif
            functionality_fingerprint_reader = true;
            tx_fmu130_command(IO_ENGINE_LOCK, &avl_uart);        // No se debe enviar a desactivar la entrada.
        } else {
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - FP Desactivado remotamente\r\n");
            #endif
        }
    } else {
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("GPIO - Ign sin cambio:%d\r\n", ignition);
        #endif
    }


     /**
     * @brief 
     * 
     */
    flag_ble_connection_state = exeDigInput(&bluetooth_state, ble_connection_state_samples, &ble_connection_state, &ble_connection_state_prev, &ble_connection_state_samples_c);
    if (flag_ble_connection_state == 1) {
        //jq8400_addQueue(VOICE_BLE_FMU130_CONNECTED);
        flag_ble_connection_state = 0;
        sprintf(gpio_buffer, "%s1", TCA_ID_BLUETOOTH);
        tx_fmu130_message(gpio_buffer, &avl_uart);
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("GPIO - Bluetooth Conectado\r\n");
            myPC_debug.printf("%s\r\n", gpio_buffer);
        #endif
    } else if  (flag_ble_connection_state == -1) {
        //jq8400_addQueue(VOICE_BLE_FMU130_DISCONNECTED);
        flag_ble_connection_state = 0;
        sprintf(gpio_buffer, "%s0", TCA_ID_BLUETOOTH);
        tx_fmu130_message(gpio_buffer, &avl_uart);
        #ifdef DEBUG_GPIO_CPP
            myPC_debug.printf("GPIO - Bluetooth Desconectado\r\n");
            myPC_debug.printf("%s\r\n", gpio_buffer);
        #endif
    }

    if (functionality_seatbelt_reading) {
        /**
         * @brief 
         * CAMIONES: INTER, KENWORK, FREIGHT
         * Normalmente Cerrado
         *      Abrochado = -1      (buckleUp)
         *      Desabrochado = 1    (unfasten)
         * 
         * Livianos y cinturones Chinos:
         * Normalmente Abierto
         *      Abrochado = 1
         *      Desabrochado = -1
         */


        /**
         * @brief 
         * Lectura del Cinturón del piloto
         * 
         */
        flag_pilot_seatbelt = exeDigInput(&in2_pilot, pilot_seatbelt_samples, &pilot_seatbelt, &pilot_seatbelt_prev, &pilot_seatbelt_c);
        if (flag_pilot_seatbelt == pilot_buckleUp) {
            estado_actual_cinturon_piloto = true;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Piloto Abrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_DRIVER_SEATBELT_FASTENED);
            }
            flag_pilot_seatbelt = 0;
            process_SeatBelt_FireUp();
            sprintf(gpio_buffer, "%s1", TCA_ID_PILOTO);
            tx_fmu130_message(gpio_buffer, &avl_uart);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            
        } else if  (flag_pilot_seatbelt == pilot_unfasten) {
            estado_actual_cinturon_piloto = false;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Piloto Desabrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_DRIVER_SEATBELT_UNFASTENED);           // Evaluar si se corta el audio.
            }
            flag_pilot_seatbelt = 0;
            sprintf(gpio_buffer, "%s0", TCA_ID_PILOTO);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            tx_fmu130_message(gpio_buffer, &avl_uart);
            if (functionality_fingerprint_reader && functionality_force_driver_buclke) { ///// PENDIENTE
                #if IO_FP_READER_GT521 == 1
                    FP_Close();
                #else
                #endif
                wait_us(1000000);
                #ifdef DEBUG_GPIO_CPP
                    myPC_debug.printf("GPIO - Pendiente apagar FP GT521Fx...\r\n");
                #endif
                fingerprint_flag_poweroff = true;
            }
        }
        

        /**
         * @brief 
         * 
         */
        flag_copilot_seatbelt = exeDigInput(&in3_copilot, copilot_seatbelt_samples, &copilot_seatbelt, &copilot_seatbelt_prev, &copilot_seatbelt_c);
        if (flag_copilot_seatbelt == copilot_buckleUp) {
            estado_actual_cinturon_copiloto = true;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Copiloto Abrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_COPILOT_SEATBELT_FASTENED);
            }
            flag_copilot_seatbelt = 0;
            sprintf(gpio_buffer, "%s1", TCA_ID_COPILOTO);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            tx_fmu130_message(gpio_buffer, &avl_uart);
        } else if  (flag_copilot_seatbelt == copilot_unfasten) {
            estado_actual_cinturon_copiloto = false;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - copiloto Desabrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_COPILOT_SEATBELT_UNFASTENED);
            }
            flag_copilot_seatbelt = 0;
            sprintf(gpio_buffer, "%s0", TCA_ID_COPILOTO);
            tx_fmu130_message(gpio_buffer, &avl_uart);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
        }


        /**
         * @brief 
         * 
         */
        flag_crew_pilot_seatbelt = exeDigInput(&in4_crew, crew_seatbelt_samples, &crew_seatbelt, &crew_seatbelt_prev, &crew_seatbelt_c);
        if (flag_crew_pilot_seatbelt == crew_buckleUp) {
            estado_actual_cinturon_tripulante = true;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Tripulante Abrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_CREW_SEATBELT_FASTENED);
            }
            flag_crew_pilot_seatbelt = 0;
            sprintf(gpio_buffer, "%s1", TCA_ID_TRIPULANTE);
            tx_fmu130_message(gpio_buffer, &avl_uart);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
        } else if  (flag_crew_pilot_seatbelt == crew_unfasten) {
            estado_actual_cinturon_tripulante = false;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - Tripulante Desabrochado\r\n");
            #endif
            if (estado_actual_ignicion) {
                jq8400_addQueue(VOICE_CREW_SEATBELT_UNFASTENED);
            }
            flag_crew_pilot_seatbelt = 0;
            sprintf(gpio_buffer, "%s0", TCA_ID_TRIPULANTE);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            tx_fmu130_message(gpio_buffer, &avl_uart);
        }
    }
}


void process_inputRainSensor() {
    flag_query_rain_sensor = false;
    if (functionality_rain_sensor) {
        memset(gpio_buffer, '\0', sizeof(gpio_buffer));
        flag_rainsensorDigInput = exeDigRain(&in5_rain_sensor, rg9_raining_samples, &rg9_raining, &rg9_raining_prev, &rg9_raining_samples_c);
        if (flag_rainsensorDigInput == 1) {
            estado_actual_sensor_lluvia = true;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - RG9 Lloviendo \r\n");
            #endif
            flag_rainsensorDigInput = 0;
            sprintf(gpio_buffer, "%s%d", TCA_ID_WEATHER, 1);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            tx_fmu130_message(gpio_buffer, &avl_uart);   
            if (functionality_rainSensor_silentMode) { 
                #ifdef DEBUG_GPIO_CPP
                    myPC_debug.printf("GPIO - RG9 Modo silencioso no se notifica voz ni cambia velocidad\r\n");
                #endif
            } else {    
                if (estado_actual_ignicion) {   
                    #ifdef DEBUG_GPIO_CPP
                        myPC_debug.printf("GPIO - RG9 - Condicion de pista mojada\r\n");
                    #endif   
                    jq8400_addQueue(VOICE_TRACK_CONDITION_WET);
                }
                tx_fmu130_var_command(wet_Speed_CMD, &avl_uart);
            }
        } else if  (flag_rainsensorDigInput == -1) {
            estado_actual_sensor_lluvia = false;
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("GPIO - RG9 Seco\r\n");
            #endif
            flag_rainsensorDigInput = 0;
            sprintf(gpio_buffer, "%s%d", TCA_ID_WEATHER, 0);
            tx_fmu130_message(gpio_buffer, &avl_uart);
            #ifdef DEBUG_GPIO_CPP
                myPC_debug.printf("%s\r\n", gpio_buffer);
            #endif
            if (functionality_rainSensor_silentMode) {
                #ifdef DEBUG_GPIO_CPP
                    myPC_debug.printf("GPIO - RG9 Modo silencioso no se notifica voz ni cambia velocidad\r\n");
                #endif
            } else {
                if (estado_actual_ignicion) {
                    #ifdef DEBUG_GPIO_CPP
                        myPC_debug.printf("GPIO - RG9 - Condicion de pista seca\r\n");
                    #endif
                    jq8400_addQueue(VOICE_TRACK_CONDITION_DRY);
                }
                tx_fmu130_var_command(dry_Speed_CMD, &avl_uart);
            }
        }
    }
}