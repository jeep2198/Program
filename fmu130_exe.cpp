/**
 * @file fmu130_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "mbed.h"
#include "fmu130_exe.hpp"   //////
#include "BufferedSerial.h"
//#include "GT521FX.hpp"
#include "New_GT521Fx.hpp"
#include "jq8400_voice.hpp"
#include "voice_cn_pa.hpp"
#include "teltonika_fmu130.hpp"
#include "fingerprint_frame.hpp"
#include "avl_fmu130.hpp"
#include "USBSerial.h"
#include "eeprom.h"
#include "flash_eeprom.h"
#include "eeprom_exe.hpp"
#include "flash_ee_exe.hpp"
#include "custom_def.hpp"


/**
 * @brief 
 *              Objetos Entradas Digitales / Seriales
 */
extern BufferedSerial   avl_uart;
extern BufferedSerial   voice_uart;
extern DigitalOut       out1_fingerprint;
extern DigitalOut       out2_ble_reset;
extern DigitalOut       out3_gt521fx;
extern DigitalOut       out4_gt521fx;
extern BufferedSerial   fingerprint;
extern USBSerial        myPC_debug;
extern Ticker           tick_idle_shutdown;
extern Ticker           tick_idle_reminder;
extern Ticker           tick_safety_tip;
extern Ticker           tick_rain_sensor;
extern Ticker           tick_autoreset_ble;
extern EEPROM           ep;

#define DEBUG_FMU130_CPP    1
#define FMU_FP_READER_GT521     1

/**
 * @brief 
 *              Variables Externas de Recepción
 */
extern bool    rg9_raining;
extern bool    ignition;
extern bool    estado_actual_ignicion;
extern char    fmu130_payload[1024];
extern int     fmu130_payload_type;
extern int     incoming_bytes;
extern int     temp_JQ8400_Volume;

/**
 * @brief 
 *              Variables externas de comandos AVL
 */
extern const char   hardware_version[];
extern const char   firmware_version[];
extern char         avl_fmu130_header[2];
extern char         avl_fmu130_imei[16];
extern int          avl_fmu130_id;
extern int          avl_fmu130_status;

/**
 * @brief 
 *              Variables externas con comandos del lector de huellas y 
 *              funciones de habilitación
 */
extern int      bluetooth_cmd_id;
extern int      fingerprint_id;
extern char     fingerprint_cmd[4];
extern char     fingerprint_hex[997];
extern char     fingerprint_asc[499];
extern bool     flag_idle_force_shutdown;
extern bool     flag_idle_reminder;
extern bool     flag_fingerprint_Sleep;
extern bool     functionality_fingerprint_reader;
extern bool     fingerprint_remotly_disable;
extern bool     functionality_force_driver_buclke;
extern bool     functionality_idle_shutdown;
extern bool     functionality_idle_reminder;
extern bool     functionality_rain_sensor;
extern bool     functionality_seatbelt_reading;
extern int      wet_Speed_Limit;
extern int      wet_Speed_Warning;
extern int      dry_Speed_Limit;
extern int      dry_Speed_Warning;
extern char     wet_Speed_CMD[30];
extern char     dry_Speed_CMD[30];
extern bool     flag_query_rain_sensor;
extern bool     flag_safety_tip_enable;
extern bool     functionality_safety_tip;
extern float    time_safety_tip;
extern float    time_sample_rain;
extern bool     fingerprint_override;
extern int      fp_override_limit;
extern bool     functionality_geo_warning;
extern bool     functionality_rainSensor_silentMode;
extern bool     functionality_ble_autoreset;
extern float    time_ble_autoreset;
extern bool     finterprint_flag_working;
extern bool     fingerprint_flag_poweroff;
// Cinturones
extern bool 	pilot_buckle_type;
extern int      pilot_buckleUp;
extern int      pilot_unfasten;
extern bool 	copilot_buckle_type;
extern int      copilot_buckleUp;
extern int      copilot_unfasten;
extern bool 	crew_buckle_type;
extern int      crew_buckleUp;
extern int      crew_unfasten;


// Variables de flash
extern uint16_t VirtAddVarTab[];
extern uint16_t VarDataTab[];
extern uint32_t eprom_AddressArray[];
extern const bool  external_eeprom;

/**
 * @brief 
 *          Variables locales
 */
const char  TCA_ID_FP_WORKING_STATUS[]      = "TCA|111|";
const char  TCA_ID_FP_SERVER_ADD[]          = "TCA|112|";
const char  TCA_ID_FP_ENROLL_CNT[]          = "TCA|114|";
const char  TCA_ID_FP_SERVER_DEL[]          = "TCA|115|";
const char  TCA_ID_IDLE_SHUTDOWN[]          = "TCA|123|";
const char  TCA_ID_FORCE_SEATBELT[]         = "TCA|126|";
const char  TCA_ID_FORCE_IDLE_SHUTDOWN[]    = "TCA|127|";
const char  TCA_ID_RAIN_SENSOR_ENA[]        = "TCA|128|";
const char  TCA_ID_SEATBELT_READ_ENA[]      = "TCA|129|";
const char  TCA_ID_VOLUME_SET[]             = "TCA|130|";
const char  TCA_ID_QUERY_SPEED_LIMIT[]      = "TCA|131|";
const char  TCA_ID_QUERY_IDLE_TIMER[]       = "TCA|132|";
const char  TCA_ID_IDLE_REMINDER[]          = "TCA|133|2";
const char  TCA_ID_IDLE_REMINDER_ENA[]      = "TCA|133|1";
const char  TCA_ID_IDLE_REMINDER_DIS[]      = "TCA|133|0";
const char  TCA_ID_SAFETY_TIP_REMINDER[]    = "TCA|134|2";
const char  TCA_ID_SAFETY_TIP_ENABLE[]      = "TCA|134|1";
const char  TCA_ID_SAFETY_TIP_DISABLE[]     = "TCA|134|0";
const char  TCA_ID_QUERY_SAFETY_TIP[]       = "TCA|136|";
const char  TCA_ID_Q_FINGERPRINT_STATUS[]   = "TCA|137|";
const char  TCA_ID_Q_RAINSENSOR_STATUS[]    = "TCA|138|";
const char  TCA_ID_Q_IDLE_SHUTDOWN_ST[]     = "TCA|139|";
const char  TCA_ID_Q_FORCE_SEATBELT_ST[]    = "TCA|140|";
const char  TCA_ID_Q_VOLUME_LEVEL[]         = "TCA|141|";
const char  TCA_ID_CHECK_ENROLLED_ID[]      = "TCA|142|";
const char  TCA_ID_HARDWARE_VERSION[]       = "TCA|143|";
const char  TCA_ID_FIRMWARE_VERSION[]       = "TCA|144|";
const char  TCA_ID_DIN_CONFIGURATION[]      = "TCA|145|";
const char  TCA_ID_Q_RAIN_SAMPLE_TIMER[]    = "TCA|146|";
const char  TCA_ID_FP_OVERRIDE_ENABLE[]     = "TCA|147|1";
const char  TCA_ID_FP_OVERRIDE_DISABLE[]    = "TCA|147|0";
const char  TCA_ID_Q_FP_OVERRIDE[]          = "TCA|148|";
const char  TCA_ID_GEO_WARNING_DISABLE[]    = "TCA|135|0";
const char  TCA_ID_GEO_WARNING_ENABLE[]     = "TCA|135|1";
const char  TCA_ID_GEO_WARNING_QUERY_OFF[]  = "TCA|135|2";
const char  TCA_ID_GEO_WARNING_QUERY_ON[]   = "TCA|135|3";
const char  TCA_ID_GEO_WARNING_GETIN[]      = "TCA|151|0;";
const char  TCA_ID_GEO_WARNING_EVENT[]      = "TCA|151|1;";
const char  TCA_ID_GEO_WARNING_GETOUT[]     = "TCA|151|2;";
const char  TCA_ID_RAIN_SILENT_MODE_ENA[]   = "TCA|152|1";
const char  TCA_ID_RAIN_SILENT_MODE_DIS[]   = "TCA|152|0";
const char  TCA_ID_RAIN_SILENT_MODE_Q[]     = "TCA|153|";
const char  TCA_ID_BLE_AUTORESET_DIS[]      = "TCA|155|0";
const char  TCA_ID_BLE_AUTORESET_ENA[]      = "TCA|155|1";
const char  TCA_ID_BLE_AUTORESET_qDIS[]     = "TCA|156|0,";
const char  TCA_ID_BLE_AUTORESET_qENA[]     = "TCA|156|1,";
const char  TCA_ID_IDLE_REMINDER_TIME[]     = "TCA|158|";
// cinturones individuales
const char  TCA_ID_PILOT_BUCKLE_NO[]        = "TCA|145|2";
const char  TCA_ID_PILOT_BUCKLE_NC[]        = "TCA|145|3";
const char  TCA_ID_COPILOT_BUCKLE_NO[]      = "TCA|145|4";
const char  TCA_ID_COPILOT_BUCKLE_NC[]      = "TCA|145|5";
const char  TCA_ID_CREW_BUCKLE_NO[]         = "TCA|145|6";
const char  TCA_ID_CREW_BUCKLE_NC[]         = "TCA|145|7";
// Comandos
const char  FMU_ENGINE_LOCK[]               = "setdigout 0?? ? ? ? ? ? ?";
const char  FMU_ENGINE_UNLOCK[]             = "setdigout 1?? ? ? ? ? ? ?";
char        fmu_buffer[64];
// Otras
long        longitud_huella                 = 498;
bool        trama_con_huella                = false;
float       wait_idle_shutdown;
float       wait_idle_reminder;
int         tipo_trama_entrante             = 0;
int         resultado                       = 0;
int         consultar_ID_FP                 = 0;
int         enrolado                        = -1;
int         id_geozone                      = 0;

// Variables externas
extern const char  hardware_version[];
extern const char  firmware_version[];
extern const char  release_date[];



/**
 * @brief 
 * 
 */
void activate_idle_shutdown() {
    flag_idle_force_shutdown = true;
}

void activate_idle_reminder() {
    flag_idle_reminder = true;
}

void queryRG9(){
    flag_query_rain_sensor = true;
}

void BLE_reset() { // Aplicar reset a módulo BLE
    out2_ble_reset = true;
    #ifdef DEBUG_FMU130_CPP 
        myPC_debug.printf("\r\nBLE - Reset\r\n");
    #endif
    wait_ms(BLE_RESET_PULSE_WIDTH);
    out2_ble_reset = false;
}


/**
 * @brief 
 * 
 */
void exe_idle_shutdown() {
    flag_idle_force_shutdown = false;
    tick_idle_shutdown.detach();
    if (functionality_idle_shutdown) {
        if (ignition) {
            tx_fmu130_command(FMU_ENGINE_LOCK, &avl_uart);
            jq8400_addQueue(VOICE_IDLE_ENGINE_SHUTDOWN);
            sprintf(fmu_buffer, "%s1", TCA_ID_IDLE_SHUTDOWN);
            tx_fmu130_message(fmu_buffer, &avl_uart);
            #ifdef DEBUG_FMU130_CPP 
                myPC_debug.printf("IDLE - Apagado por exceso de Ralenti\r\n");
                myPC_debug.printf("%s\r\n", fmu_buffer);
            #endif
        } else {
            #ifdef DEBUG_FMU130_CPP 
                myPC_debug.printf("IDLE - Ignicion OFF - No se envia apagado.\r\n");
            #endif
        }
    }
}

void exe_idle_reminder() {
    flag_idle_reminder = false;
    jq8400_addQueue(VOICE_IDLE_REMINDER);
    sprintf(fmu_buffer, "%s", TCA_ID_IDLE_REMINDER);
    tx_fmu130_message(fmu_buffer, &avl_uart);
    #ifdef DEBUG_FMU130_CPP
        myPC_debug.printf("IDLE - Recordatorio por exceso de Ralenti\r\n");
        myPC_debug.printf("%s\r\n", fmu_buffer);
    #endif
}

/**
 * @brief 
 * 
 */
void boot_message() {
    sprintf(fmu_buffer, "TCA|100|%s", firmware_version);
    tx_fmu130_message(fmu_buffer, &avl_uart);
    #ifdef DEBUG_FMU130_CPP
        myPC_debug.printf("\r\n\r\nDETEKTOR I+D. TEAM CAPTURA\r\n");
        myPC_debug.printf("Proyecto ABInBev PA\r\n");
        myPC_debug.printf("Version %s - %s\r\n\r\n", firmware_version, release_date);
        myPC_debug.printf("%s\r\n", fmu_buffer);
    #endif
}


/**
 * @brief 
 * 
 */
void process_FMU130() {
    memset(fmu_buffer, '\0', sizeof(fmu_buffer));
    memset(fmu130_payload, '\0', sizeof(fmu130_payload));
    
    /**
     * @brief 
     * 
     */
    incoming_bytes = read_avl_uart(&avl_uart, fmu130_payload);

    if (incoming_bytes > 0) {
        #ifdef DEBUG_FMU130_CPP
            myPC_debug.printf("\r\nBLE - Bytes entrantes: %d\r\n", incoming_bytes);
        #endif
        tipo_trama_entrante = identify_fmu130_payload(fmu130_payload);

        /**
         * @brief Construct a new switch object
         * 
         */
        switch (tipo_trama_entrante) {
        case TRAMA_AVL_FMU130 :
            #ifdef DEBUG_FMU130_CPP
                myPC_debug.printf("BLE - Evento AVL\r\n");
            #endif
            if (verify_fmu130_payload(fmu130_payload)) {
                parse_fmu130_payload(fmu130_payload, avl_fmu130_header, avl_fmu130_imei, &avl_fmu130_id, &avl_fmu130_status);
                #ifdef DEBUG_FMU130_CPP
                    myPC_debug.printf("BLE - FMU_ID:%d Status:%d\r\n", avl_fmu130_id, avl_fmu130_status);
                #endif
                switch (avl_fmu130_id) {
                case AVLID_OVERSPEED:   // Exceso de velocidad
                    if (rg9_raining) {
                        if (functionality_rainSensor_silentMode) {
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Modo Silencioso Activado - No se notifica\r\n");
                            #endif
                            //
                        } else {
                            jq8400_addQueue(VOICE_OVERSPEED_ALERT_WET);
                        }
                    } else {
                        jq8400_addQueue(VOICE_OVERSPEED_ALERT);
                    }
                    break;
                
                case AVLID_TOWING:   // Movimiento ilegal      
                    // No se notifica
                    #ifdef DEBUG_FMU130_CPP
                        myPC_debug.printf("BLE - Vehiculo en Grua - No se notifica\r\n");
                    #endif
                    break;
                
                case AVLID_JAMMING:   // Jamming
                    #ifdef DEBUG_FMU130_CPP
                        myPC_debug.printf("BLE - Deteccion de Jamming - No se notifica\r\n");
                    #endif
                    break;

                case AVLID_IDLE:   // Ralentí
                    if (avl_fmu130_status == 1) {
                        if (estado_actual_ignicion) {
                            jq8400_addQueue(VOICE_IDLE_TIME_WARNING);
                            if (functionality_idle_shutdown) {
                                tick_idle_shutdown.attach(&activate_idle_shutdown, (wait_idle_shutdown + 6.0)); // 6.0 = Duración de audio de ralentí
                                #ifdef DEBUG_FMU130_CPP
                                    myPC_debug.printf("Apagar por ralenti en %.1f seg.\r\n", wait_idle_shutdown);
                                #endif
                            }
                            if (functionality_idle_reminder) {
                                tick_idle_reminder.attach(&activate_idle_reminder,(wait_idle_reminder + 0.0));
                                #ifdef DEBUG_FMU130_CPP
                                    myPC_debug.printf("Recordatorio por ralenti en %.1f seg.\r\n", wait_idle_reminder);
                                #endif
                            }
                        } else {
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Ignicion OFF no se notifica\r\n");
                            #endif
                        }
                    } else if (avl_fmu130_status == 0) {
                        tick_idle_shutdown.detach();
                        tick_idle_reminder.detach();
                    }
                    break;

                case AVLID_HARSH_BEHAVIOUR:   // Hábitos de manejo
                    if (avl_fmu130_status == 1){
                        jq8400_addQueue(VOICE_HARSH_ACCELERATION);
                    } else if (avl_fmu130_status == 2) {
                        jq8400_addQueue(VOICE_HARSH_BREAKING);
                    } else if (avl_fmu130_status == 3) {
                        jq8400_addQueue(VOICE_SHARP_CORNERING);
                    }
                    break;

                case AVLID_WARNING_SPEED:    // Pre alerta de Exceso de velocidad
                     if (rg9_raining) {
                         if (functionality_rainSensor_silentMode) {
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Modo Silencioso Activado - No se notifica\r\n");
                            #endif
                        } else {
                            jq8400_addQueue(VOICE_WARNING_OVERSPEED_WET);
                        }
                    } else {
                        jq8400_addQueue(VOICE_WARNING_OVERSPEED);
                    }
                    break;

                case AVLID_CRASH:   // Colisión
                    jq8400_addQueue(VOICE_CRASH_DETECTION);
                    break;

                default:
                    // ID no identificado
                    break;
                }

            } else {
                // Trama invalida
            }
            break;
        

        case TRAMA_SERVIDOR:
            #ifdef DEBUG_FMU130_CPP
                myPC_debug.printf("BLE - Comando Servidor\r\n");
                myPC_debug.printf("RAW DATA:\r\n%s\r\n\r\n", fmu130_payload);
            #endif
            if (verify_finger_payload(fmu130_payload)) {
                /* DESHABILITADA LA FUNCIÓN DE SLEEP EN EL LECTOR
                    if (flag_fingerprint_Sleep) {
                        fingerprint.WakeUp();
                        flag_fingerprint_Sleep = false;
                        //myPC_debug.printf("BLE - GT521Fx@WakeUp\r\n");
                    }
                */
                trama_con_huella = parse_finger_payload(fmu130_payload, fingerprint_cmd, &fingerprint_id, fingerprint_hex);
                if (trama_con_huella) {
                    fingerprint_HEX2CHAR(fingerprint_hex, fingerprint_asc);
                    #ifdef DEBUG_FMU130_CPP
                        myPC_debug.printf("BLE - Huella HEX:\r\n%s\r\n", fingerprint_hex);
                    #endif
                }
                bluetooth_cmd_id = identify_server_cmd(fingerprint_cmd);
                
                switch (bluetooth_cmd_id) {
                /** ********************************
                *   COMANDOS DE NOTIFICACION DE GEOCERCAS
                *   ******************************** */  
                    case CMD_GEO_WARNING_GETIN:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Notificacion de Entrada a Geocerca Controlada\r\n");
                        #endif
                        //
                        if (functionality_geo_warning) {
                            id_geozone = fingerprint_id;
                            jq8400_addQueue(id_geozone);
                            sprintf(fmu_buffer, "%s%d", TCA_ID_GEO_WARNING_GETIN, id_geozone);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Funcion deshabilitada. No se notifica Entrada de Geocerca\r\n");
                            #endif
                        }
                        break;

                    case CMD_GEO_WARNING_GETOUT:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Notificacion de Salida de Geocerca Controlada\r\n");
                        #endif
                        if (functionality_geo_warning) {
                            id_geozone = fingerprint_id;
                            jq8400_addQueue(id_geozone);
                            sprintf(fmu_buffer, "%s%d", TCA_ID_GEO_WARNING_GETOUT, id_geozone);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Funcion deshabilitada. No se notifica Salida de Geocerca\r\n");
                            #endif
                        }
                        break;

                    case CMD_GEO_WARNING_EVENT:
                        #ifdef DEBUG_FMU130_CPP

                        #endif
                        //myPC_debug.printf("BLE - Notificacion de Evento en Geocerca Controlada\r\n");
                        if (functionality_geo_warning) {
                            id_geozone = fingerprint_id;
                            jq8400_addQueue(id_geozone);
                            sprintf(fmu_buffer, "%s%d", TCA_ID_GEO_WARNING_EVENT, id_geozone);
                            #ifdef DEBUG_FMU130_CPP

                            #endif
                            //myPC_debug.printf("%s\r\n", fmu_buffer);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            
                        } else {
                            #ifdef DEBUG_FMU130_CPP

                            #endif
                            //myPC_debug.printf("BLE - Funcion deshabilitada. No se notifica Evento de Geocerca\r\n");
                        }
                        break;



                /** ********************************
                *   COMANDOS DE LECTOR DE HUELLA
                *   ******************************** */
                    case CMD_FINGER_ADD:
                        out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;
                        wait_us(500000);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - OUT3:TRUE\r\n");
                        #endif
                        fingerprint_flag_poweroff = false;
                        #if FMU_FP_READER_GT521 == 1
                            FP_Open();
                            resultado = FP_SetTemplate(fingerprint_id, fingerprint_asc);
                            FP_Close();
                        #else
                        #endif
                        wait_us(1000000);
                        if (resultado == 0) {
                            sprintf(fmu_buffer, "%s%d", TCA_ID_FP_SERVER_ADD, fingerprint_id);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Huella agregada exitosamente\r\n");
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s%d", TCA_ID_FP_SERVER_ADD, resultado);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Error al agregar huella\r\n");
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        }                       
                        fingerprint_flag_poweroff = true;
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Pendiente apagar FP GT521Fx...\r\n");
                        #endif
                        break;
                    
                    case CMD_FINGER_DELETE:
                        out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;
                        wait_us(500000);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - OUT3:TRUE\r\n");
                        #endif
                        fingerprint_flag_poweroff = false;
                        #if FMU_FP_READER_GT521 == 1
                            FP_Open();
                        #else
                        #endif
                        if (fingerprint_id == 3000) {
                            #if FMU_FP_READER_GT521 == 1
                                resultado = FP_DeleteAll();
                            #else
                            #endif
                        } else {
                            #if FMU_FP_READER_GT521 == 1
                                resultado = FP_DeleteID(fingerprint_id);
                            #else
                            #endif
                        }
                        if (resultado >= 0) {
                            sprintf(fmu_buffer, "%s%d", TCA_ID_FP_SERVER_DEL, fingerprint_id);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Huella eliminada exitosamente\r\n");
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s%d", TCA_ID_FP_SERVER_DEL, resultado);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("BLE - Error al eliminar huella\r\n");
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        }
                        #if FMU_FP_READER_GT521 == 1
                            FP_Close();
                        #else
                        #endif
                        wait_us(1000000);
                        fingerprint_flag_poweroff = true;
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Pendiente apagar FP GT521Fx...\r\n");
                        #endif
                        break;

                    case CMD_FINGER_ENABLE:
                        jq8400_addQueue(VOICE_functionality_fingerprint_reader);
                        functionality_fingerprint_reader = true;
                        fingerprint_remotly_disable = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_FINGERPRINT], ((uint16_t) true));
                            ep.write(eprom_AddressArray[EE_Address_FINGERPRINT_RE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT], ((uint16_t) true));
                            EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT_RE], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s1", TCA_ID_FP_WORKING_STATUS);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #if FMU_FP_READER_GT521 == 1
                            FP_Flush();
                        #else
                        #endif
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Lector Huella Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_FINGER_DISABLE:
                        jq8400_addQueue(VOICE_FINGERPRINT_DISABLE);
                        tx_fmu130_command(FMU_ENGINE_UNLOCK, &avl_uart);
                        out1_fingerprint = 1;
                        wait_us(1000000);
                        out1_fingerprint = 0;
                        functionality_fingerprint_reader = false;
                        fingerprint_remotly_disable = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_FINGERPRINT], ((uint16_t) false));
                            ep.write(eprom_AddressArray[EE_Address_FINGERPRINT_RE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT], ((uint16_t) false));
                            EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT_RE], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s0", TCA_ID_FP_WORKING_STATUS);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Lector Huella Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_FINGERPRINT_QUERY:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_Q_FINGERPRINT_STATUS, functionality_fingerprint_reader);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estatus del Lector Biometrico\r\n");
                            myPC_debug.printf("BLE - Lector Biometrico: %d\r\n", functionality_fingerprint_reader);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
            
                    case CMD_FINGER_ENROLL_CNT:
                        out3_gt521fx = out4_gt521fx = finterprint_flag_working = true;
                        wait_us(500000);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - OUT3:TRUE\r\n");
                        #endif
                        fingerprint_flag_poweroff = false;
                        #if FMU_FP_READER_GT521 == 1
                            FP_Open();
                            resultado = FP_GetEnrollCount();
                            FP_Close();
                        #else
                        #endif
                        wait_us(1000000);
                        sprintf(fmu_buffer, "%s%d", TCA_ID_FP_ENROLL_CNT, resultado);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        fingerprint_flag_poweroff = true;
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                            myPC_debug.printf("BLE - Pendiente apagar FP GT521Fx...\r\n");
                        #endif
                        break;

                    case CMD_CHECK_ENROLLED_ID:
                        out3_gt521fx = out4_gt521fx = finterprint_flag_working= true;
                        fingerprint_flag_poweroff = false;
                        consultar_ID_FP = fingerprint_id;
                        wait_us(500000);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - OUT3:TRUE\r\n");
                            myPC_debug.printf("BLE - Consulta de Posicion de Memoria - Lector de Huellas\r\n");
                        #endif
                        #if FMU_FP_READER_GT521 == 1
                            FP_Open();
                            enrolado = FP_CheckEnrolled(consultar_ID_FP);
                            FP_Close();
                        #else
                        #endif
                        wait_us(1000000);
                        sprintf(fmu_buffer, "%s%d", TCA_ID_CHECK_ENROLLED_ID, enrolado);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        fingerprint_flag_poweroff = true;
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Posicion de huella %d: %d\r\n", consultar_ID_FP, enrolado);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                            myPC_debug.printf("BLE - Pendiente apagar FP GT521Fx...\r\n");
                        #endif
                        break;

                    case CMD_FP_OVERRIDE_ENABLE:
                        fingerprint_override = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_OVERRIDE_FP], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_OVERRIDE_FP], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_FP_OVERRIDE_ENABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Habilitar Anulacion de Lector de Huella\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_FP_OVERRIDE_DISABLE:
                        fingerprint_override = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_OVERRIDE_FP], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_OVERRIDE_FP], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_FP_OVERRIDE_DISABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Deshabilitar Anulacion de Lector de Huella\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_FP_OVERRIDE_COUNT:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_OVERRIDE_LIMIT], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_OVERRIDE_QT], ((uint16_t) fingerprint_id));
                        }
                        fp_override_limit = fingerprint_id;
                        sprintf(fmu_buffer, "%s%d,%d", TCA_ID_Q_FP_OVERRIDE, fingerprint_override, fp_override_limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Muestras para Anulacion de Lector de Huella\n");
                            myPC_debug.printf("BLE - Nuevo valor de Huellas no Autorizadas: %d seg\r\n", fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_FP_OVERRIDE_QUERY:
                        sprintf(fmu_buffer, "%s%d,%d", TCA_ID_Q_FP_OVERRIDE, fingerprint_override, fp_override_limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Anulacion de Lector de Huella\r\n");
                            myPC_debug.printf("BLE - Anulacion: %s\r\n", fingerprint_override?"TRUE":"FALSE");
                            myPC_debug.printf("BLE - Cantidad de No Autorizados: %d\r\n", fp_override_limit);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;


                    /** ********************************
                    *   COMANDOS DE PARLANTE
                    *   ******************************** */
                    case CMD_SET_VOLUME:
                        temp_JQ8400_Volume = fingerprint_id;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_VOLUME], ((uint16_t) temp_JQ8400_Volume));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_VOLUME], ((uint16_t) temp_JQ8400_Volume));
                        }
                        jq8400_setVol(&temp_JQ8400_Volume, &voice_uart);
                        sprintf(fmu_buffer, "%s%d", TCA_ID_VOLUME_SET, fingerprint_id);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Volumen %d\r\n",fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_VOLUME_LEVEL:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_Q_VOLUME_LEVEL, temp_JQ8400_Volume);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por el Nivel de Volumen del Parlante\r\n");
                            myPC_debug.printf("BLE - Nivel de Volumen del Parlante: %d\r\n", temp_JQ8400_Volume);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    /** ********************************
                    *   COMANDOS DE SENSOR DE LLUVIA
                    *   ******************************** */
                    case CMD_RAIN_ENABLE:
                        functionality_rain_sensor = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_RAINSENSOR], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_RAINSENSOR], ((uint16_t) true));
                        }
                        tick_rain_sensor.detach();
                        sprintf(fmu_buffer, "%s%d", TCA_ID_RAIN_SENSOR_ENA, 1);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        tick_rain_sensor.attach(&queryRG9, time_sample_rain);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Sensor de Lluvia Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_RAIN_DISABLE:
                        functionality_rain_sensor = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_RAINSENSOR], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_RAINSENSOR], ((uint16_t) false));
                        }
                        tick_rain_sensor.detach();
                        sprintf(fmu_buffer, "%s%d", TCA_ID_RAIN_SENSOR_ENA, 0);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        tx_fmu130_var_command(dry_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Sensor de Lluvia Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_RAIN_QUERY:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_Q_RAINSENSOR_STATUS, functionality_rain_sensor);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estatus del Sensor de Lluvia\r\n");
                            myPC_debug.printf("BLE - Sensor de Lluvia: %d\r\n", functionality_rain_sensor);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_RAIN_TIMER:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_RS_SAMPLE_T], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_RS_SAMPLE_T], ((uint16_t) fingerprint_id));
                        }
                        time_sample_rain = fingerprint_id * 1.0;
                        if (functionality_rain_sensor) {
                            tick_rain_sensor.detach();
                            tick_rain_sensor.attach(&queryRG9, time_sample_rain);
                        }
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_Q_RAIN_SAMPLE_TIMER, time_sample_rain);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Temp. de Muestras para Sensor de Lluvia\n");
                            myPC_debug.printf("BLE - Nuevo Temp. para muestreo: %d seg\r\n", fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_RAIN_TIMER:
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_Q_RAIN_SAMPLE_TIMER, time_sample_rain);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Temp. de Muestras para Sensor de Lluvia\r\n");
                            myPC_debug.printf("BLE - Muestras en:  %.1f seg\r\n", time_sample_rain);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_RAIN_SILENT_MODE_ENA:
                        functionality_rainSensor_silentMode = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_RS_SILENTMODE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_RS_SILENTMODE], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_RAIN_SILENT_MODE_ENA);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        tx_fmu130_var_command(dry_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Habilitar Modo silencioso de Sensor de Lluvia\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;


                    case CMD_RAIN_SILENT_MODE_DIS:
                        functionality_rainSensor_silentMode = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_RS_SILENTMODE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_RS_SILENTMODE], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_RAIN_SILENT_MODE_DIS);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Deshabilitar Modo silencioso de Sensor de Lluvia\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_RAIN_SILENT_MODE_Q:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_RAIN_SILENT_MODE_Q, functionality_rainSensor_silentMode);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Modo silencioso de Sensor de Lluvia\r\n");
                            myPC_debug.printf("BLE - Modo silencioso: %s\r\n", functionality_rainSensor_silentMode?"TRUE":"FALSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_WSL:
                        wet_Speed_Limit = fingerprint_id;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_WET_LIMIT], ((uint16_t) wet_Speed_Limit));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_WET_LIMIT], ((uint16_t) wet_Speed_Limit));
                        }
                        sprintf(wet_Speed_CMD, "setparam 50092:%d", wet_Speed_Limit);
                        tx_fmu130_var_command(wet_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Limite velocidad Mojado\r\n");
                            myPC_debug.printf("BLE - Nuevo Limite: %d\r\n", wet_Speed_Limit);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        sprintf(fmu_buffer, "%s%d,%d,%d,%d", TCA_ID_QUERY_SPEED_LIMIT, 
                                wet_Speed_Warning, wet_Speed_Limit, dry_Speed_Warning, dry_Speed_Limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_WSW:
                        wet_Speed_Warning = fingerprint_id;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_WET_WARNING], ((uint16_t) wet_Speed_Warning));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_WET_WARNING], ((uint16_t) wet_Speed_Warning));
                        }
                        sprintf(wet_Speed_CMD, "setparam 11104:%d", wet_Speed_Warning);
                        tx_fmu130_var_command(wet_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Limite advertencia Mojado\r\n");
                            myPC_debug.printf("BLE - Nueva advertencia: %d\r\n", wet_Speed_Warning);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        sprintf(fmu_buffer, "%s%d,%d,%d,%d", TCA_ID_QUERY_SPEED_LIMIT, 
                                wet_Speed_Warning, wet_Speed_Limit, dry_Speed_Warning, dry_Speed_Limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_CHANGE_DSL:
                        dry_Speed_Limit = fingerprint_id;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_DRY_LIMIT], ((uint16_t) dry_Speed_Limit));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_DRY_LIMIT], ((uint16_t) dry_Speed_Limit));
                        }
                        sprintf(dry_Speed_CMD, "setparam 50092:%d", dry_Speed_Limit);
                        tx_fmu130_var_command(dry_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Limite velocidad Seco\r\n");
                            myPC_debug.printf("BLE - Nuevo Limite: %d\r\n", dry_Speed_Limit);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        sprintf(fmu_buffer, "%s%d,%d,%d,%d", TCA_ID_QUERY_SPEED_LIMIT, 
                                wet_Speed_Warning, wet_Speed_Limit, dry_Speed_Warning, dry_Speed_Limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_DSW:
                        dry_Speed_Warning = fingerprint_id;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_DRY_WARNING], ((uint16_t) dry_Speed_Warning));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_DRY_WARNING], ((uint16_t) dry_Speed_Warning));
                        }
                        sprintf(dry_Speed_CMD, "setparam 11104:%d", dry_Speed_Warning);
                        tx_fmu130_var_command(dry_Speed_CMD, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Limite advertencia Seco\r\n");
                            myPC_debug.printf("BLE - Nueva advertencia: %d\r\n", dry_Speed_Warning);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        sprintf(fmu_buffer, "%s%d,%d,%d,%d", TCA_ID_QUERY_SPEED_LIMIT, 
                                wet_Speed_Warning, wet_Speed_Limit, dry_Speed_Warning, dry_Speed_Limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_SPEED_LIMIT:
                        sprintf(fmu_buffer, "%s%d,%d,%d,%d", TCA_ID_QUERY_SPEED_LIMIT, 
                                wet_Speed_Warning, wet_Speed_Limit, dry_Speed_Warning, dry_Speed_Limit);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Limites Velocidad\r\n");
                            myPC_debug.printf("BLE - Mojado Advertencia: %d, Limite: %d\r\n", wet_Speed_Warning, wet_Speed_Limit);
                            myPC_debug.printf("BLE - Seco   Advertencia: %d, Limite: %d\r\n", dry_Speed_Warning, dry_Speed_Limit);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;



                    /** ********************************
                    *   COMANDOS DE CINTURONES
                    *   ******************************** */
                    case CMD_SEATBELT_ENABLE:
                        functionality_seatbelt_reading = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SEATBELT_READ], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SEATBELT_READ], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s%d", TCA_ID_SEATBELT_READ_ENA, 1);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Lectura Cinturones Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_SEATBELT_DISABLE:
                        functionality_seatbelt_reading = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SEATBELT_READ], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SEATBELT_READ], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s%d", TCA_ID_SEATBELT_READ_ENA, 0);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Lectura Cinturones Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_SEATBELT_QUERY:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_SEATBELT_READ_ENA, functionality_force_driver_buclke);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estado de Lectura de Cinturones\r\n");
                            myPC_debug.printf("BLE - Lectura de Cinturones: %s\r\n", functionality_force_driver_buclke?"TRUE":"FALSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
    
                    case CMD_FORCE_BUCLKE_ENABLE:
                        functionality_force_driver_buclke = true;
                        functionality_seatbelt_reading = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SEATBELT_READ], ((uint16_t) true));
                            ep.write(eprom_AddressArray[EE_Address_ENFORCE_SB], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SEATBELT_READ], ((uint16_t) true));
                            EE_WriteVariable(VirtAddVarTab[AP_ENFORCE_SB], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s%d", TCA_ID_FORCE_SEATBELT, 1);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Forzar Cinturon de Piloto Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_FORCE_BUCLKE_DISABLE:
                        tx_fmu130_command(FMU_ENGINE_UNLOCK, &avl_uart);
                        out1_fingerprint = 1;
                        wait_us(1000000);
                        out1_fingerprint = 0;
                        functionality_force_driver_buclke = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_ENFORCE_SB], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_ENFORCE_SB], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s%d", TCA_ID_FORCE_SEATBELT, 0);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Forzar Cinturon de Piloto Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_FORCE_SEATBELT:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_Q_FORCE_SEATBELT_ST, functionality_force_driver_buclke);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estatus para Forzar Cinturon de Piloto\r\n");
                            myPC_debug.printf("BLE - Forzar Cinturon de Piloto: %d\r\n", functionality_force_driver_buclke);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_SET_INPUT_NORMAL_O: // CONFIGURACIÓN DE LIVIANO y Cinturón Chino
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Entradas como Normalmente Abierta\r\n");
                        #endif
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE], ((uint16_t) true));
                            ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE], ((uint16_t) true));
                            ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE], ((uint16_t) true));
                            EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE], ((uint16_t) true));
                            EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE], ((uint16_t) true));
                        }
                        pilot_buckle_type = copilot_buckle_type = crew_buckle_type = true;   // HONDURAS
                        pilot_buckleUp = copilot_buckleUp = crew_buckleUp =  1;
                        pilot_unfasten = copilot_unfasten = crew_unfasten = -1;
                        sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        break;

                    case CMD_SET_INPUT_NORMAL_C: // CONFIGURACIÓN DE CAMIÓN
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Entradas como Normalmente Cerradas\r\n");
                        #endif
                        pilot_buckle_type = copilot_buckle_type = crew_buckle_type = false;   // PANAMA 
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE], ((uint16_t) false));
                            ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE], ((uint16_t) false));
                            ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE], ((uint16_t) false));
                            EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE], ((uint16_t) false));
                            EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE], ((uint16_t) false));
                        }
                        pilot_buckleUp = copilot_buckleUp = crew_buckleUp = -1;
                        pilot_unfasten = copilot_unfasten = crew_unfasten =  1;
                        sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        break;

                    case CMD_QUERY_DIN_CONFIG:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por DIN Config.\r\n");
                        #endif

                        if (pilot_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NO);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Piloto: NO");
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NC);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Piloto: NC");
                            #endif
                        }

                        if (copilot_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NO);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Copiloto: NO");
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NC);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Copiloto: NC");
                            #endif
                        }

                        if (crew_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NO);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Tripulante: NO");
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NC);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", "BLE - Cinturon de Tripulante: NC");
                            #endif
                        }
                        break;

                    case CMD_PILOT_BUCKLE_NO: // CONFIGURACIÓN DE LIVIANO y Cinturón Chino
                        pilot_buckle_type = true;   // HONDURAS
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE], ((uint16_t) true));
                        }
                        pilot_buckleUp =  1;
                        pilot_unfasten = -1;
                        sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Piloto: NO\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_PILOT_BUCKLE_NC: // CONFIGURACIÓN DE CAMIÓN
                        pilot_buckle_type = false;   // PANAMA 
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE], ((uint16_t) false));
                        }
                        pilot_buckleUp = -1;
                        pilot_unfasten =  1;
                        sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Piloto: NC\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_PILOT_BT:
                        if (pilot_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NO);
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_PILOT_BUCKLE_NC);
                        }
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Config. Cinturon de Piloto\r\n");
                            myPC_debug.printf("BLE - Cinturon Piloto Tipo: %s\r\n", pilot_buckle_type?"N. OPEN":"N. CLOSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;
                    
                    case CMD_COPILOT_BUCKLE_NO: // CONFIGURACIÓN DE LIVIANO y Cinturón Chino
                        copilot_buckle_type = true;   // HONDURAS
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE], ((uint16_t) true));
                        }
                        copilot_buckleUp =  1;
                        copilot_unfasten = -1;
                        sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Copiloto: NO\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_COPILOT_BUCKLE_NC: // CONFIGURACIÓN DE CAMIÓN
                        copilot_buckle_type = false;   // PANAMA 
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE], ((uint16_t) false));
                        }
                        copilot_buckleUp = -1;
                        copilot_unfasten =  1;
                        sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Copiloto: NC\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_COPILOT_BT:
                        if (copilot_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NO);
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_COPILOT_BUCKLE_NC);
                        }
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Config. Cinturon de Copiloto\r\n");
                            myPC_debug.printf("BLE - Cinturon Copiloto Tipo: %s\r\n", copilot_buckle_type?"N. OPEN":"N. CLOSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CREW_BUCKLE_NO: // CONFIGURACIÓN DE LIVIANO y Cinturón Chino
                        crew_buckle_type = true;   // HONDURAS
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE], ((uint16_t) true));
                        }
                        crew_buckleUp =  1;
                        crew_unfasten = -1;
                        sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NO);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Tripulante: NO\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CREW_BUCKLE_NC: // CONFIGURACIÓN DE CAMIÓN
                        crew_buckle_type = false;   // PANAMA 
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE], ((uint16_t) false));
                        }
                        crew_buckleUp = -1;
                        crew_unfasten =  1;
                        sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NC);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Config. Cinturon de Tripulante: NC\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_CREW_BT:
                        if (crew_buckle_type) {
                            sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NO);
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_CREW_BUCKLE_NC);
                        }
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Config. Cinturon de Tripulante\r\n");
                            myPC_debug.printf("BLE - Cinturon Tripulante Tipo: %s\r\n", crew_buckle_type?"N. OPEN":"N. CLOSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;



                    /** ********************************
                    *   COMANDOS DE APAGADO POR RALENTI
                    *   ******************************** */
                    case CMD_IDLE_SHUTDOWN_ENABLE:
                        functionality_idle_shutdown = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_SHUTDOWN], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_SHUTDOWN], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s%d", TCA_ID_FORCE_IDLE_SHUTDOWN, 1);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Apagado por Ralenti Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_IDLE_SHUTDOWN_DISABLE:
                        functionality_idle_shutdown = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_SHUTDOWN], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_SHUTDOWN], ((uint16_t) false));
                        }
                        tick_idle_shutdown.detach();
                        sprintf(fmu_buffer, "%s%d", TCA_ID_FORCE_IDLE_SHUTDOWN, 0);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Apagado por Ralenti Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_IDLE_SHUTDOWN_QUERY:
                        sprintf(fmu_buffer, "%s%d", TCA_ID_Q_IDLE_SHUTDOWN_ST, functionality_idle_shutdown);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estatus de Apagado por Ralenti\r\n");
                            myPC_debug.printf("BLE - Apagado por Ralenti: %s\r\n", functionality_idle_shutdown?"TRUE":"FALSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_IDLE_SHUTDOWN_T:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_S_TIME], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_S_TIME], ((uint16_t) fingerprint_id));
                        }
                        wait_idle_shutdown = fingerprint_id * 1.0;
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_QUERY_IDLE_TIMER, wait_idle_shutdown);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Temp. de Apagado Ralenti\r\n");
                            myPC_debug.printf("BLE - Nuevo Temp. Ralendi: %d seg\r\n", fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_IDLE_SHUTDOWN_T:
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_QUERY_IDLE_TIMER, wait_idle_shutdown);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Temp. de Apagado Ralenti\r\n");
                            myPC_debug.printf("BLE - Apagado Ralenti en %.1f seg\r\n", wait_idle_shutdown);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    
                    /** ********************************
                    *   COMANDOS DE RECORDATORIO DE RALENTI
                    *   ******************************** */
                    case CMD_IDLE_REMINDER_ENABLE:
                        functionality_idle_reminder = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_REMINDER], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_REMINDER], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_IDLE_REMINDER_ENA);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Recordatorio por Ralenti Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_IDLE_REMINDER_DISABLE:
                        functionality_idle_reminder = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_REMINDER], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_REMINDER], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_IDLE_REMINDER_DIS);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        tick_idle_reminder.detach();
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Recordatorio por Ralenti Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_IDLE_REMINDER_QUERY:
                        if (functionality_idle_reminder) {
                            sprintf(fmu_buffer, "%s", TCA_ID_IDLE_REMINDER_ENA);
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_IDLE_REMINDER_DIS);
                        }
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Estatus de Recordatorio por Ralenti\r\n");
                            myPC_debug.printf("BLE - Recordatorio por Ralenti: %s\r\n", functionality_idle_reminder?"TRUE":"FALSE");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_CHANGE_IDLE_REMINDER_T:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_IDLE_R_TIME], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_IDLE_R_TIME], ((uint16_t) fingerprint_id));
                        }
                        wait_idle_reminder = fingerprint_id * 1.0;
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_IDLE_REMINDER_TIME, wait_idle_reminder);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Temp. de Recordatorio Ralenti\r\n");
                            myPC_debug.printf("BLE - Nuevo Recordatorio Ralendi: %d seg\r\n", fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_IDLE_REMINDER_T:
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_IDLE_REMINDER_TIME, wait_idle_reminder);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Temp. de Recordatorio Ralenti\r\n");
                            myPC_debug.printf("BLE - Recordatorio Ralenti en %.1f seg\r\n", wait_idle_reminder);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;


                    /** ********************************
                    *   COMANDOS DE TIP DE SEGURIDAD
                    *   ******************************** */
                    case CMD_QUERY_SAFETY_TIP_TIME:
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_QUERY_SAFETY_TIP, time_safety_tip);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta Temp. de Tip de Seguridad\r\n");
                            myPC_debug.printf("BLE - Tip de Seguridad en %.1f seg\r\n", time_safety_tip);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CHANGE_SAFETY_TIP_TIME:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SAFETY_TIP_T], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SAFETY_TIP_T], ((uint16_t) fingerprint_id));
                        }
                        time_safety_tip = fingerprint_id * 1.0;
                        sprintf(fmu_buffer, "%s%.1f", TCA_ID_QUERY_SAFETY_TIP, time_safety_tip);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Temp. de Tip de Seguridad\r\n");
                            myPC_debug.printf("BLE - Nuevo Temp. para Tip de Seguridad: %d seg\r\n", fingerprint_id);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_SAFETY_TIP_ENABLE:
                        functionality_safety_tip = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SAFETY_TIP], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SAFETY_TIP], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_SAFETY_TIP_ENABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Tip de Seguridad Habilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_SAFETY_TIP_DISABLE:
                        functionality_safety_tip = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_SAFETY_TIP], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_SAFETY_TIP], ((uint16_t) false));
                        }
                        tick_safety_tip.detach();
                        sprintf(fmu_buffer, "%s", TCA_ID_SAFETY_TIP_DISABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Tip de Seguridad Deshabilitado\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    /** ********************************
                    *   COMANDOS DE INFORMACIÓN ICS5
                    *   ******************************** */
                    case CMD_QUERY_HARDWARE_VER:
                        sprintf(fmu_buffer, "%s%s", TCA_ID_HARDWARE_VERSION, hardware_version);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Version de HW\r\n");
                            myPC_debug.printf("BLE - HW: %s\r\n", hardware_version);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_QUERY_FIRMWARE_VER:
                        sprintf(fmu_buffer, "%s%s", TCA_ID_FIRMWARE_VERSION, firmware_version);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Version de Firmware\r\n");
                            myPC_debug.printf("BLE - FW: %s\r\n", firmware_version);
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    /** ********************************
                    *   COMANDOS DE GEOCERCAS
                    *   ******************************** */
                    case CMD_GEO_WARNING_ENABLE:
                        functionality_geo_warning = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_GEOZONE], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_GEOZONE], ((uint16_t) true));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_GEO_WARNING_ENABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Habilitar Notificacion de Geocercas Controladas\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_GEO_WARNING_DISABLE:
                        functionality_geo_warning = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_GEOZONE], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_GEOZONE], ((uint16_t) false));
                        }
                        sprintf(fmu_buffer, "%s", TCA_ID_GEO_WARNING_DISABLE);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Deshabilitar Notificacion de Geocercas Controladas\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_GEO_WARNING_QUERY:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Notificacion de Geocercas Controladas\r\n");
                            myPC_debug.printf("BLE - Notificacion de Geocercas: %s\r\n", functionality_geo_warning?"TRUE":"FALSE");
                        #endif
                        if (functionality_geo_warning) {
                            sprintf(fmu_buffer, "%s", TCA_ID_GEO_WARNING_QUERY_ON);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s", TCA_ID_GEO_WARNING_QUERY_OFF);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        }
                        break;

                    
                    /** ********************************
                    *   COMANDOS DE AUTORESET BLUETOOTH
                    *   ******************************** */
                    case CMD_BLE_AUTORESET_ENABLE:
                        functionality_ble_autoreset = true;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_BLE_RESET], ((uint16_t) true));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_BLE_RESET], ((uint16_t) true));
                        }
                        tick_autoreset_ble.detach();
                        tick_autoreset_ble.attach(&BLE_reset, time_ble_autoreset);
                        sprintf(fmu_buffer, "%s", TCA_ID_BLE_AUTORESET_ENA);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Habilitar Autoreset de Bluetooth\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_BLE_AUTORESET_DISABLE:
                        functionality_ble_autoreset = false;
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_BLE_RESET], ((uint16_t) false));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AP_BLE_RESET], ((uint16_t) false));
                        }
                        tick_autoreset_ble.detach();
                        sprintf(fmu_buffer, "%s", TCA_ID_BLE_AUTORESET_DIS);
                        tx_fmu130_message(fmu_buffer, &avl_uart);
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Deshabilitar Autoreset de Bluetooth\r\n");
                            myPC_debug.printf("%s\r\n", fmu_buffer);
                        #endif
                        break;

                    case CMD_BLE_AUTORESET_TIME:
                        if (external_eeprom) {
                            ep.write(eprom_AddressArray[EE_Address_BLE_RESET_T], ((uint16_t) fingerprint_id));
                        } else {
                            EE_WriteVariable(VirtAddVarTab[AB_BLE_RESET_T], ((uint16_t) fingerprint_id));
                        }
                        time_ble_autoreset = fingerprint_id * 1.0;
                        if (functionality_ble_autoreset) {
                            tick_autoreset_ble.detach();
                            tick_autoreset_ble.attach(&BLE_reset, time_ble_autoreset);
                        }
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Cambio Temp. Autoreset de Bluetooth\n");
                            myPC_debug.printf("BLE - Nuevo Temp. para muestreo: %d seg\r\n", fingerprint_id);
                        #endif
                        break;

                    case CMD_BLE_AUTORESET_QUERY:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Consulta por Autoreset de Bluetooth\r\n");
                            myPC_debug.printf("BLE - Autoreset de Bluetooth: %s\r\n", functionality_ble_autoreset?"TRUE":"FALSE");
                            myPC_debug.printf("BLE - Tiempo Autoreset de Bluetooth: %.1f\r\n", time_ble_autoreset);

                        #endif
                        if (functionality_ble_autoreset) {
                            sprintf(fmu_buffer, "%s%.0f", TCA_ID_BLE_AUTORESET_qENA, time_ble_autoreset);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        } else {
                            sprintf(fmu_buffer, "%s%.0f", TCA_ID_BLE_AUTORESET_qDIS, time_ble_autoreset);
                            tx_fmu130_message(fmu_buffer, &avl_uart);
                            #ifdef DEBUG_FMU130_CPP
                                myPC_debug.printf("%s\r\n", fmu_buffer);
                            #endif
                        }
                        break;

                    default:
                        #ifdef DEBUG_FMU130_CPP
                            myPC_debug.printf("BLE - Comando no identificado\r\n");
                        #endif
                        break;
                }
  
            } else {
                // Error trama incompleta.
                #ifdef DEBUG_FMU130_CPP
                    myPC_debug.printf("BLE - Trama incompleta\r\n");
                #endif
            }
            /* DESHABILITADA LA FUNCIÓN DE SLEEP EN EL LECTOR
                fingerprint.Sleep();
                flag_fingerprint_Sleep = true;
                myPC_debug.printf("BLE - GT521Fx@Sleep\r\n");
            */
            break;

        default:
            #ifdef DEBUG_FMU130_CPP
                myPC_debug.printf("BLE - Trama no identificada\r\n");
                myPC_debug.printf("BLE PAYLOAD:\r\n%s\r\n", fmu130_payload);
            #endif
            break;
        }
    } else {
        #ifdef DEBUG_FMU130_CPP
            myPC_debug.printf("BLE - Sin bytes recibidos\r\n");
        #endif
    }
}