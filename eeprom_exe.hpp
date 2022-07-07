/**
 * @file flash_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2021-02-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __EEPROM_EXE_HPP
#define __EEPROM_EXE_HPP

    // Variables
    extern uint32_t  eprom_AddressArray[26];

    // EEPROM POSITION
    #define EE_Address_FINGERPRINT      0
    #define EE_Address_OVERRIDE_FP      1
    #define EE_Address_OVERRIDE_LIMIT   2
    #define EE_Address_IDLE_SHUTDOWN    3
    #define EE_Address_IDLE_REMINDER    4
    #define EE_Address_IDLE_S_TIME      5
    #define EE_Address_IDLE_R_TIME      6
    #define EE_Address_SEATBELT_READ    7
    #define EE_Address_ENFORCE_SB       8
    #define EE_Address_PILOT_BTYPE      9
    #define EE_Address_COPILOT_BTYPE    10
    #define EE_Address_CREW_BTYPE       11
    #define EE_Address_RAINSENSOR       12
    #define EE_Address_RS_SILENTMODE    13
    #define EE_Address_RS_SAMPLE_T      14
    #define EE_Address_WET_LIMIT        15
    #define EE_Address_WET_WARNING      16
    #define EE_Address_DRY_LIMIT        17
    #define EE_Address_DRY_WARNING      18
    #define EE_Address_SAFETY_TIP       19
    #define EE_Address_SAFETY_TIP_T     20
    #define EE_Address_GEOZONE          21
    #define EE_Address_VOLUME           22
    #define EE_Address_BLE_RESET        23
    #define EE_Address_BLE_RESET_T      24
    #define EE_Address_FINGERPRINT_RE   25

    // Lista de funciones:
    void load_eepromVar();
    void eeprom_Default();
    void writeEE(int myAddress, int myValue);

#endif // __EEPROM_EXE_HPP