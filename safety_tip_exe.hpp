/**
 * @file safety_tip_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2021-01-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __SAFETY_TIP_EXE_HPP
#define __SAFETY_TIP_EXE_HPP

    // Variables
    extern const int   safety_tip_min;
    extern const int   safety_tip_max;
    extern const char  TCA_ID_SAFETY_TIP[10];
    extern const char  TCA_ID_FRI_ICS5[10];
    extern const char  TCA_ID_OFF_ICS5[10];
    extern const char  TCA_ID_GPIO_UPDATE[9];
    extern int         safety_tip_id;
    extern char        safety_tip_buffer[];
    

    // Funciones
    void play_SafetyTip();
    void exe_SafetyTip();
    void send_fri_ics();
    void tx_fri_ics();
    
#endif // __SAFETY_TIP_EXE_HPP