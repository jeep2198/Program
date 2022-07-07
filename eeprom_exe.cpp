/**
 * @file flash_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2021-02-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "mbed.h"
#include "eeprom_exe.hpp"
#include "eeprom.h"
#include "USBSerial.h"
#include <ctype.h>

/** * INTERFACES * **/
    extern USBSerial    myPC_debug;
    extern DigitalOut   flashLED;
    extern EEPROM       ep;

/** * VARIABLES * **/
    // Manejo de EEPROM FLASH
    uint32_t  eprom_AddressArray[] = {
                EE_Address_FINGERPRINT,
                EE_Address_OVERRIDE_FP,
                EE_Address_OVERRIDE_LIMIT,
                EE_Address_IDLE_SHUTDOWN, 
                EE_Address_IDLE_REMINDER, 
                EE_Address_IDLE_S_TIME,   
                EE_Address_IDLE_R_TIME,   
                EE_Address_SEATBELT_READ, 
                EE_Address_ENFORCE_SB,    
                EE_Address_PILOT_BTYPE,   
                EE_Address_COPILOT_BTYPE, 
                EE_Address_CREW_BTYPE,    
                EE_Address_RAINSENSOR,    
                EE_Address_RS_SILENTMODE, 
                EE_Address_RS_SAMPLE_T,   
                EE_Address_WET_LIMIT,     
                EE_Address_WET_WARNING,   
                EE_Address_DRY_LIMIT,   
                EE_Address_DRY_WARNING, 
                EE_Address_SAFETY_TIP,  
                EE_Address_SAFETY_TIP_T,
                EE_Address_GEOZONE,     
                EE_Address_VOLUME,     
                EE_Address_BLE_RESET,  
                EE_Address_BLE_RESET_T,
                EE_Address_FINGERPRINT_RE
            };

    // Lector de Huellas
    extern bool     functionality_fingerprint_reader;
    extern bool     fingerprint_remotly_disable;
    extern bool     fingerprint_override;
    extern int      fp_override_limit;              // 4 intentos

    // Ralentí
    extern bool     functionality_idle_shutdown;
    extern bool     functionality_idle_reminder;
    extern float    wait_idle_shutdown;             //600.0 segundos
    extern float    wait_idle_reminder;             // 180.0 segundos

    // Cinturones
    extern bool     functionality_seatbelt_reading;
    extern bool     functionality_force_driver_buclke;
    extern bool 	pilot_buckle_type;
    extern int      pilot_buckleUp;
    extern int      pilot_unfasten;
    extern bool 	copilot_buckle_type;
    extern int      copilot_buckleUp;
    extern int      copilot_unfasten;
    extern bool 	crew_buckle_type;
    extern int      crew_buckleUp;
    extern int      crew_unfasten;

    // Sensor de Lluvia
    extern bool     functionality_rain_sensor;
    extern bool     functionality_rainSensor_silentMode;
    extern float    time_sample_rain;               // 6.0 segundos

    // Limites de Velocidad en Mojado y seco
    extern int     wet_Speed_Warning;               // 50 Km/h
    extern int     wet_Speed_Limit;                 // 55 Km/h
    extern int     dry_Speed_Warning;               // 70 Km/h
    extern int     dry_Speed_Limit;                 // 80 Km/h

    // Tip de Seguridad y Geocercas
    extern bool     functionality_geo_warning;
    extern bool     functionality_safety_tip;
    extern float    time_safety_tip;                // 900.0 segundos

    // Volumen de Parlante
    extern int     temp_JQ8400_Volume;              // 4 HIGH

    // Autoreset de BLE
    extern bool     functionality_ble_autoreset;
    extern float    time_ble_autoreset;

    #define DEBUG_FLASH_CPP

/**
 * @brief 
 * 
 */
void load_eepromVar() {
    //myPC_debug.printf("\r\n FLASH - Cargando datos en Variables...");
    int16_t tempData; 

    // Lector de Huellas    
    ep.read(eprom_AddressArray[EE_Address_FINGERPRINT], tempData);
    functionality_fingerprint_reader = (bool) tempData;

    ep.read(eprom_AddressArray[EE_Address_FINGERPRINT_RE], tempData);
    fingerprint_remotly_disable = (bool) tempData;
    
    ep.read(eprom_AddressArray[EE_Address_OVERRIDE_FP], tempData);
    fingerprint_override = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_OVERRIDE_LIMIT], tempData);
    fp_override_limit = (int) tempData;  

    // Ralentí
    ep.read(eprom_AddressArray[EE_Address_IDLE_SHUTDOWN], tempData);
    functionality_idle_shutdown = (bool)  tempData;

    ep.read(eprom_AddressArray[EE_Address_IDLE_REMINDER], tempData);
    functionality_idle_reminder = (bool) tempData;

    ep.read(eprom_AddressArray[EE_Address_IDLE_S_TIME], tempData);
    wait_idle_shutdown = ((int) tempData) * 1.0;

    ep.read(eprom_AddressArray[EE_Address_IDLE_R_TIME], tempData);
    wait_idle_reminder = ((int) tempData) * 1.0;

    // Sensor de Lluvia
    ep.read(eprom_AddressArray[EE_Address_RAINSENSOR], tempData);
    functionality_rain_sensor = (bool) tempData; 

    ep.read(eprom_AddressArray[EE_Address_RS_SILENTMODE], tempData);
    functionality_rainSensor_silentMode = (bool) tempData; 

    ep.read(eprom_AddressArray[EE_Address_RS_SAMPLE_T], tempData);
    time_sample_rain = ((int) tempData) * 1.0;
    
    // Limites de Velocidad en Mojado y seco
    ep.read(eprom_AddressArray[EE_Address_WET_WARNING], tempData);
    wet_Speed_Warning = (int) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_WET_LIMIT], tempData);
    wet_Speed_Limit = (int) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_DRY_WARNING], tempData);
    dry_Speed_Warning = (int) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_DRY_LIMIT], tempData);
    dry_Speed_Limit = (int) tempData; 

    // Tip de Seguridad y Geocercas
    ep.read(eprom_AddressArray[EE_Address_SAFETY_TIP], tempData);
    functionality_safety_tip = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_SAFETY_TIP_T], tempData);                // 900.0 segundos
    time_safety_tip = ((int) tempData) * 1.0;
    
    ep.read(eprom_AddressArray[EE_Address_GEOZONE], tempData);
    functionality_geo_warning = (bool) tempData; 

    // Volumen de Parlante
    ep.read(eprom_AddressArray[EE_Address_VOLUME], tempData);
    temp_JQ8400_Volume = (int) tempData; 

    // Autoreset BLE
    ep.read(eprom_AddressArray[EE_Address_BLE_RESET], tempData);
    functionality_ble_autoreset = (bool) tempData; 

    ep.read(eprom_AddressArray[EE_Address_BLE_RESET_T], tempData);
    time_ble_autoreset = ((int) tempData) * 1.0;

    // Cinturones
    ep.read(eprom_AddressArray[EE_Address_SEATBELT_READ], tempData);
    functionality_seatbelt_reading = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_ENFORCE_SB], tempData);
    functionality_force_driver_buclke = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_PILOT_BTYPE], tempData);
    pilot_buckle_type = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_COPILOT_BTYPE], tempData);
    copilot_buckle_type = (bool) tempData; 
    
    ep.read(eprom_AddressArray[EE_Address_CREW_BTYPE], tempData);
    crew_buckle_type = (bool) tempData; 

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
    
    #ifdef DEBUG_FLASH_CPP
        myPC_debug.printf("\r\n FLASH - Completado!");
        myPC_debug.printf("\r\n FLASH - Valores cargados:");
        myPC_debug.printf("\r\n  Lector de Huellas: %s", functionality_fingerprint_reader?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Lector de Huellas Deshabilitado Remoto: %s", functionality_fingerprint_reader?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Anulacion de Lector: %s", fingerprint_override?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Cantidad para Anulacion: %d\r\n", fp_override_limit);
        myPC_debug.printf("\r\n  Apagado por Ralenti: %s", functionality_idle_shutdown?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Recordatorio de Ralenti: %s", functionality_idle_reminder?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tiempo Apagado por Ralenti: %.1f", wait_idle_shutdown);
        myPC_debug.printf("\r\n  Tiempo Recordatorio de Ralenti: %.1f\r\n", wait_idle_reminder);
        myPC_debug.printf("\r\n  Lectura de Cinturones: %s", functionality_seatbelt_reading?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Forzar Conductor Abrochado: %s\r\n", functionality_force_driver_buclke?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Piloto: %s", pilot_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Copiloto: %s", copilot_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Tipo de Hebilla Tripulante: %s", crew_buckle_type?"N. OPEN":"N. CLOSE");
        myPC_debug.printf("\r\n  Sensor de Lluvia: %s", functionality_rain_sensor?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Modo Silencioso de Lluvia: %s", functionality_rainSensor_silentMode?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Temporizador para Lluvia: %.1f\r\n", time_sample_rain);
        myPC_debug.printf("\r\n  Advertencia Mojado: %d", wet_Speed_Warning);
        myPC_debug.printf("\r\n  Limite Mojado: %d", wet_Speed_Limit);
        myPC_debug.printf("\r\n  Advertencia Seco: %d", dry_Speed_Warning);
        myPC_debug.printf("\r\n  Limite Seco: %d\r\n", dry_Speed_Limit);
        myPC_debug.printf("\r\n  Notificacion Geocerca: %s", functionality_geo_warning?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tip de Seguridad: %s", functionality_safety_tip?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  Tiempo entre Tip de Seguridad: %.1f\r\n", time_safety_tip);
        myPC_debug.printf("\r\n  Volumen Parlante: %d\r\n", temp_JQ8400_Volume);
        myPC_debug.printf("\r\n  BLE Autoreset: %s", functionality_ble_autoreset?"TRUE":"FALSE");
        myPC_debug.printf("\r\n  BLE Temporizador: %.1f\r\n\r\n", time_ble_autoreset);
    #endif
}

void eeprom_Default() {
    
    //myPC_debug.printf("\r\n FLASH - Cargando valores por defecto");
    #define SEATBELT_CONFIG 7
    
    ep.write(eprom_AddressArray[EE_Address_FINGERPRINT],     ((uint16_t) true));     // Fingerprint Enable
    ep.write(eprom_AddressArray[EE_Address_OVERRIDE_FP],     ((uint16_t) true));     // Override Fingerprint Authentication
    ep.write(eprom_AddressArray[EE_Address_OVERRIDE_LIMIT],  ((uint16_t) 4));        // Override Fingerprint Quantity
    ep.write(eprom_AddressArray[EE_Address_IDLE_SHUTDOWN],   ((uint16_t) true));     // Idle Shutdown Enable
    ep.write(eprom_AddressArray[EE_Address_IDLE_REMINDER],   ((uint16_t) true));     // Idle Shutdown Reminder
    ep.write(eprom_AddressArray[EE_Address_IDLE_S_TIME],     ((uint16_t) 600));      // Idle Shutdown Cutoff Time
    ep.write(eprom_AddressArray[EE_Address_IDLE_R_TIME],     ((uint16_t) 180));      // Idle Shutdown Reminder Time
    ep.write(eprom_AddressArray[EE_Address_SEATBELT_READ],   ((uint16_t) true));     // Seatbelt reading Enable
    ep.write(eprom_AddressArray[EE_Address_ENFORCE_SB],      ((uint16_t) true));     // Force Driver Buckleup
    #if (SEATBELT_CONFIG == 0)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 1)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 2)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 3)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 4)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 5)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 6)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) false));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #elif (SEATBELT_CONFIG == 7)
        ep.write(eprom_AddressArray[EE_Address_PILOT_BTYPE],     ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_COPILOT_BTYPE],   ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
        ep.write(eprom_AddressArray[EE_Address_CREW_BTYPE],      ((uint16_t) true));    // Seatbelt reading Type (FALSE = PA | TRUE = HN)
    #endif
    ep.write(eprom_AddressArray[EE_Address_RAINSENSOR],      ((uint16_t) true));     // Rain Sensor Enable
    ep.write(eprom_AddressArray[EE_Address_RS_SILENTMODE],   ((uint16_t) false));     // Silent mode Rain Sensor
    ep.write(eprom_AddressArray[EE_Address_RS_SAMPLE_T],     ((uint16_t) 6));        // Sample Time Rain Sensor
    ep.write(eprom_AddressArray[EE_Address_WET_LIMIT],       ((uint16_t) 55));       // Wet Speed Limit
    ep.write(eprom_AddressArray[EE_Address_WET_WARNING],     ((uint16_t) 50));       // Wet Speed Warning
    ep.write(eprom_AddressArray[EE_Address_DRY_LIMIT],       ((uint16_t) 80));       // Dry Speed Limit
    ep.write(eprom_AddressArray[EE_Address_DRY_WARNING],     ((uint16_t) 70));       // Dry Speed Warning
    ep.write(eprom_AddressArray[EE_Address_SAFETY_TIP],      ((uint16_t) true));    // Enable Safety Tip
    ep.write(eprom_AddressArray[EE_Address_SAFETY_TIP_T],    ((uint16_t) 1800));      // Time for Safety Tip
    ep.write(eprom_AddressArray[EE_Address_GEOZONE],         ((uint16_t) false));    // Geozone alert
    ep.write(eprom_AddressArray[EE_Address_VOLUME],          ((uint16_t) 5));        // Volume Level 6=Maximum
    ep.write(eprom_AddressArray[EE_Address_BLE_RESET],       ((uint16_t) true));     // BLE Autoreset
    ep.write(eprom_AddressArray[EE_Address_BLE_RESET_T],     ((uint16_t) 3600));     // BLE Autoreset Time
    ep.write(eprom_AddressArray[EE_Address_FINGERPRINT_RE],  ((uint16_t) false));     // Fingerprint Enable

    // LED
    flashLED = 1;
    wait_ms(3750);
    flashLED = 0;
    #ifdef DEBUG_FLASH_CPP
        myPC_debug.printf("\r\n FLASH - Completado!");
    #endif
}

void writeEE(int myAddress, int myValue) {
    ep.write(((uint32_t) myAddress), ((uint16_t) myValue));
}