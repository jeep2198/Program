/**
 * @file flash_ee_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2022-07-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "mbed.h"
#include "flash_ee_exe.hpp"
#include "flash_eeprom.h"
#include "USBSerial.h"
#include <ctype.h>
 
/** * INTERFACES * **/
    extern USBSerial    myPC_debug;
    extern DigitalOut   flashLED;
 
/** * VARIABLES * **/
    // Manejo de EEPROM FLASH
    extern uint16_t VirtAddVarTab[];
    extern uint16_t VarDataTab[];
 
    // Lector de Huellas
    extern bool     fingerprint_enable;
    extern bool     fingerprint_remotly_disable;
    extern bool     fingerprint_override;
    extern int      fp_override_limit;              // 4 intentos
 
    // Ralentí
    extern bool     enable_idle_shutdown;
    extern bool     enable_idle_reminder;
    extern float    wait_idle_shutdown;             //600.0 segundos
    extern float    wait_idle_reminder;             // 180.0 segundos
 
    // Cinturones
    extern bool     enable_seatbelt_reading;
    extern bool     enable_force_seatbelt_buclke;
    extern bool     pilot_buckle_type;
    extern int      pilot_buckleUp;
    extern int      pilot_unfasten;
    extern bool     copilot_buckle_type;
    extern int      copilot_buckleUp;
    extern int      copilot_unfasten;
    extern bool     crew_buckle_type;
    extern int      crew_buckleUp;
    extern int      crew_unfasten;
 
    // Sensor de Lluvia
    extern bool     enable_rain_sensor;
    extern bool     enable_rain_silent_mode;
    extern float    time_sample_rain;               // 6.0 segundos
 
    // Limites de Velocidad en Mojado y seco
    extern int     wet_Speed_Warning;               // 50 Km/h
    extern int     wet_Speed_Limit;                 // 55 Km/h
    extern int     dry_Speed_Warning;               // 70 Km/h
    extern int     dry_Speed_Limit;                 // 80 Km/h
 
    // Tip de Seguridad y Geocercas
    extern bool     enable_geo_warning;
    extern bool     enable_safety_tip;
    extern float    time_safety_tip;                // 900.0 segundos
 
    // Volumen de Parlante
    extern int     temp_JQ8400_Volume;              // 4 HIGH
 
    // Autoreset de BLE
    extern bool     enable_ble_autoreset;
    extern float    time_ble_autoreset;
 
    #define     DEBUG_FLASH_EE_CPP  1
 
/**
 * @brief 
 *          Esta función inicializa la EEPROM. Después ejecuta un lazo a lo largo 
 *          de las direcciones predefinidas como EEPROM y carga en un arreglo su
 *          valor.
 *         
 */
void readFlash_Booting() {
    #ifdef DEBUG_FLASH_EE_CPP
        myPC_debug.printf("\r\n FLASH - Leyendo datos de memoria...");
    #endif
    // Variable local
    int i;
    
    // Leer todas las variables de Flash
    for(i = 0; i < NB_OF_VAR; i++) {
        EE_ReadVariable(VirtAddVarTab[i], &VarDataTab[i]);
    }
}
 
/**
 * @brief 
 * 
 */
void load_2_Var() {
    #ifdef DEBUG_FLASH_EE_CPP
        myPC_debug.printf("\r\n FLASH - Cargando datos en Variables...");
    #endif
    // Lector de Huellas
    fingerprint_enable          = (bool) VarDataTab[AP_FINGERPRINT];
    fingerprint_remotly_disable = (bool) VarDataTab[AP_FINGERPRINT_RE];
    fingerprint_override        = (bool) VarDataTab[AP_OVERRIDE_FP];
    fp_override_limit           = (int) VarDataTab[AP_OVERRIDE_QT];
 
 
    // Ralentí
    enable_idle_shutdown    = (bool) VarDataTab[AP_IDLE_SHUTDOWN];
    enable_idle_reminder    = (bool) VarDataTab[AP_IDLE_REMINDER];
    wait_idle_shutdown      = ((int) VarDataTab[AP_IDLE_S_TIME]) * 1.0;
    wait_idle_reminder      = ((int) VarDataTab[AP_IDLE_R_TIME]) * 1.0;
 
    // Cinturones
    enable_seatbelt_reading      = (bool) VarDataTab[AP_SEATBELT_READ];
    enable_force_seatbelt_buclke = (bool) VarDataTab[AP_ENFORCE_SB];
    pilot_buckle_type            = (bool) VarDataTab[AP_PILOT_BTYPE];
    copilot_buckle_type          = (bool) VarDataTab[AP_COPILOT_BTYPE];
    crew_buckle_type             = (bool) VarDataTab[AP_CREW_BTYPE];
 
    // Pilot Seatbelt Buckle Type Definition
    if (pilot_buckle_type) { // N.O. - Honduras
        pilot_buckleUp = 1;
        pilot_unfasten = -1;
    } else { // N.C. - Panamá
        pilot_buckleUp = -1;
        pilot_unfasten = 1;
    }
 
    // Copilot Seatbelt Buckle Type Definition
    if (copilot_buckle_type) { // N.O. - Honduras
        copilot_buckleUp = 1;
        copilot_unfasten = -1;
    } else { // N.C. - Panamá
        copilot_buckleUp = -1;
        copilot_unfasten = 1;
    }
 
    // Crew Seatbelt Buckle Type Definition
    if (crew_buckle_type) { // N.O. - Honduras
        crew_buckleUp = 1;
        crew_unfasten = -1;
    } else { // N.C. - Panamá
        crew_buckleUp    = -1;
        crew_unfasten  = 1;
    }
    
    // Sensor de Lluvia
    enable_rain_sensor          = (bool) VarDataTab[AP_RAINSENSOR];
    enable_rain_silent_mode     = (bool) VarDataTab[AP_RS_SILENTMODE];
    time_sample_rain            = ((int) VarDataTab[AP_RS_SAMPLE_T]) * 1.0;;
    
    // Limites de Velocidad en Mojado y seco
    wet_Speed_Warning   = (int) VarDataTab[AP_WET_WARNING];
    wet_Speed_Limit     = (int) VarDataTab[AP_WET_LIMIT];
    dry_Speed_Warning   = (int) VarDataTab[AP_DRY_WARNING];
    dry_Speed_Limit     = (int) VarDataTab[AP_DRY_LIMIT];
 
    // Tip de Seguridad y Geocercas
    enable_safety_tip   = (bool) VarDataTab[AP_SAFETY_TIP];
    time_safety_tip     = ((int) VarDataTab[AP_SAFETY_TIP_T]) * 1.0;                // 900.0 segundos
    enable_geo_warning  = (bool) VarDataTab[AP_GEOZONE];
 
    // Volumen de Parlante
    temp_JQ8400_Volume  = (int) VarDataTab[AP_VOLUME];
 
    // Autoreset BLE
    enable_ble_autoreset = (bool) VarDataTab[AP_BLE_RESET];
    time_ble_autoreset   = ((int) VarDataTab[AB_BLE_RESET_T]) * 1.0;
 
    
    #ifdef DEBUG_FLASH_EE_CPP
        myPC_debug.printf("\r\n FLASH - Completado!");
        myPC_debug.printf("\r\n FLASH - Valores cargados:");
        myPC_debug.printf("\r\n  Lector de Huellas: %s", fingerprint_enable?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Lector de Huellas Deshabilitado Remoto: %s", fingerprint_enable?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Anulacion de Lector: %s", fingerprint_override?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Cantidad para Anulacion: %d\r\n", fp_override_limit);
        myPC_debug.printf("\r\n  Apagado por Ralenti: %s", enable_idle_shutdown?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Recordatorio de Ralenti: %s", enable_idle_reminder?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tiempo Apagado por Ralenti: %.1f", wait_idle_shutdown);
        myPC_debug.printf("\r\n  Tiempo Recordatorio de Ralenti: %.1f\r\n", wait_idle_reminder);
        myPC_debug.printf("\r\n  Lectura de Cinturones: %s", enable_seatbelt_reading?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Forzar Conductor Abrochado: %s\r\n", enable_force_seatbelt_buclke?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Piloto: %s", pilot_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Copiloto: %s", copilot_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Tripulante: %s", crew_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Sensor de Lluvia: %s", enable_rain_sensor?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Modo Silencioso de Lluvia: %s", enable_rain_silent_mode?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Temporizador para Lluvia: %.1f\r\n", time_sample_rain);
        myPC_debug.printf("\r\n  Advertencia Mojado: %d", wet_Speed_Warning);
        myPC_debug.printf("\r\n  Limite Mojado: %d", wet_Speed_Limit);
        myPC_debug.printf("\r\n  Advertencia Seco: %d", dry_Speed_Warning);
        myPC_debug.printf("\r\n  Limite Seco: %d\r\n", dry_Speed_Limit);
        myPC_debug.printf("\r\n  Notificacion Geocerca: %s", enable_geo_warning?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tip de Seguridad: %s", enable_safety_tip?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tiempo entre Tip de Seguridad: %.1f\r\n", time_safety_tip);
        myPC_debug.printf("\r\n  Volumen Parlante: %d\r\n", temp_JQ8400_Volume);
        myPC_debug.printf("\r\n  BLE Autoreset: %s", enable_ble_autoreset?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  BLE Temporizador: %.1f\r\n\r\n", time_ble_autoreset);
    #endif
}
 
void flash_Booting() {
    HAL_FLASH_Unlock();
    EE_Init();
    readFlash_Booting();
    load_2_Var();
}
 
void flash_Default() {
    // Desbloquear Flash para Escritura
    #ifdef DEBUG_FLASH_EE_CPP
        myPC_debug.printf("\r\n FLASH - Cargando valores por defecto");
    #endif
    #define SEATBELT_CONFIG 7
    EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT],     ((uint16_t) true));     // Fingerprint Enable
    EE_WriteVariable(VirtAddVarTab[AP_OVERRIDE_FP],     ((uint16_t) true));     // Override Fingerprint Authentication
    EE_WriteVariable(VirtAddVarTab[AP_OVERRIDE_QT],     ((uint16_t) 4));        // Override Fingerprint Quantity
    EE_WriteVariable(VirtAddVarTab[AP_IDLE_SHUTDOWN],   ((uint16_t) false));    // Idle Shutdown Enable
    EE_WriteVariable(VirtAddVarTab[AP_IDLE_REMINDER],   ((uint16_t) true));     // Idle Shutdown Reminder
    EE_WriteVariable(VirtAddVarTab[AP_IDLE_S_TIME],     ((uint16_t) 600));      // Idle Shutdown Cutoff Time
    EE_WriteVariable(VirtAddVarTab[AP_IDLE_R_TIME],     ((uint16_t) 180));      // Idle Shutdown Reminder Time
    EE_WriteVariable(VirtAddVarTab[AP_SEATBELT_READ],   ((uint16_t) true));     // Seatbelt reading Enable
    EE_WriteVariable(VirtAddVarTab[AP_ENFORCE_SB],      ((uint16_t) true));     // Force Driver Buckleup
    #if (SEATBELT_CONFIG == 0)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 1)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 2)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 3)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 4)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 5)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 6)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 7)
        EE_WriteVariable(VirtAddVarTab[AP_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        EE_WriteVariable(VirtAddVarTab[AP_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #endif
    EE_WriteVariable(VirtAddVarTab[AP_RAINSENSOR],      ((uint16_t) true));     // Rain Sensor Enable
    EE_WriteVariable(VirtAddVarTab[AP_RS_SILENTMODE],   ((uint16_t) false));     // Silent mode Rain Sensor
    EE_WriteVariable(VirtAddVarTab[AP_RS_SAMPLE_T],     ((uint16_t) 6));        // Sample Time Rain Sensor
    EE_WriteVariable(VirtAddVarTab[AP_WET_LIMIT],       ((uint16_t) 55));       // Wet Speed Limit
    EE_WriteVariable(VirtAddVarTab[AP_WET_WARNING],     ((uint16_t) 50));       // Wet Speed Warning
    EE_WriteVariable(VirtAddVarTab[AP_DRY_LIMIT],       ((uint16_t) 80));       // Dry Speed Limit
    EE_WriteVariable(VirtAddVarTab[AP_DRY_WARNING],     ((uint16_t) 70));       // Dry Speed Warning
    EE_WriteVariable(VirtAddVarTab[AP_SAFETY_TIP],      ((uint16_t) true));    // Enable Safety Tip
    EE_WriteVariable(VirtAddVarTab[AP_SAFETY_TIP_T],    ((uint16_t) 1800));      // Time for Safety Tip
    EE_WriteVariable(VirtAddVarTab[AP_GEOZONE],         ((uint16_t) false));    // Geozone alert
    EE_WriteVariable(VirtAddVarTab[AP_VOLUME],          ((uint16_t) 5));        // Volume Level 6=Maximum
    EE_WriteVariable(VirtAddVarTab[AP_BLE_RESET],       ((uint16_t) true));     // BLE Autoreset
    EE_WriteVariable(VirtAddVarTab[AB_BLE_RESET_T],     ((uint16_t) 3600));     // BLE Autoreset Time
    EE_WriteVariable(VirtAddVarTab[AP_FINGERPRINT_RE],  ((uint16_t) false));     // Fingerprint Enable
 
    
    // LED
    flashLED = 1;
    wait_ms(3750);
    flashLED = 0;
    #ifdef DEBUG_FLASH_EE_CPP
        myPC_debug.printf("\r\n FLASH - Completado!");
    #endif
 
}
 
void writeFlash(int myAddress, int myValue) {
    // Desbloquear Flash para Escritura
    EE_WriteVariable(VirtAddVarTab[myAddress], ((uint16_t) myValue));
}