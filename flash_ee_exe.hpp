/**
 * @file flash_ee_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2022-07-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __FLASH_EE_EXE_HPP
#define __FLASH_EE_EXE_HPP
 
    // DIRECCION VIRUTAL
    #define ADDR_FINGERPRINT        0x5555
    #define ADDR_OVERRIDE_FP        0x5575
    #define ADDR_OVERRIDE_QT        0x5595
    #define ADDR_IDLE_SHUTDOWN      0x55B5
    #define ADDR_IDLE_REMINDER      0x55D5
    #define ADDR_IDLE_S_TIME        0x55F5
    #define ADDR_IDLE_R_TIME        0x5615
    #define ADDR_SEATBELT_READ      0x5635
    #define ADDR_ENFORCE_SB         0x5655
    // Definición indivual de cinturones
    #define ADDR_PILOT_BTYPE        0x5675
    #define ADDR_COPILOT_BTYPE      0x5695
    #define ADDR_CREW_BTYPE         0x56B5
    //
    #define ADDR_RAINSENSOR         0x56D5
    #define ADDR_RS_SILENTMODE      0x56F5
    #define ADDR_RS_SAMPLE_T        0x5715
    #define ADDR_WET_LIMIT          0x5735
    #define ADDR_WET_WARNING        0x5755
    #define ADDR_DRY_LIMIT          0x5775
    #define ADDR_DRY_WARNING        0x5795
    #define ADDR_SAFETY_TIP         0x57B5
    #define ADDR_SAFETY_TIP_T       0x57D5
    #define ADDR_GEOZONE            0x57F5
    #define ADDR_VOLUME             0x5815
    #define ADDR_BLE_RESET          0x5835
    #define ADDR_BLE_RESET_T        0x5855
 
    // ARRAY POSITION
    #define AP_FINGERPRINT      0
    #define AP_OVERRIDE_FP      1
    #define AP_OVERRIDE_QT      2
    #define AP_IDLE_SHUTDOWN    3
    #define AP_IDLE_REMINDER    4
    #define AP_IDLE_S_TIME      5
    #define AP_IDLE_R_TIME      6
    #define AP_SEATBELT_READ    7
    #define AP_ENFORCE_SB       8
    // Definición indivual de cinturones
    #define AP_PILOT_BTYPE      9
    #define AP_COPILOT_BTYPE    10
    #define AP_CREW_BTYPE       11
    //
    #define AP_RAINSENSOR       12
    #define AP_RS_SILENTMODE    13
    #define AP_RS_SAMPLE_T      14
    #define AP_WET_LIMIT        15
    #define AP_WET_WARNING      16
    #define AP_DRY_LIMIT        17
    #define AP_DRY_WARNING      18
    #define AP_SAFETY_TIP       19
    #define AP_SAFETY_TIP_T     20
    #define AP_GEOZONE          21
    #define AP_VOLUME           22
    #define AP_BLE_RESET        23
    #define AB_BLE_RESET_T      24
    #define AP_FINGERPRINT_RE   25
 
    // Lista de funciones:
    void fast_Flash();
    void readFlash_Booting();
    void load_2_Var();
    void flash_Booting();
    void flash_Default();
    void writeFlash(int myAddress, int myValue);
 
#endif // __FLASH_EE_EXE_HPP