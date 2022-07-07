/**
 * @file gpio_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __GPIO_EXE_HPP
#define __GPIO_EXE_HPP

    extern char        gpio_buffer[];
    extern const char  TCA_ID_PILOTO[9];
    extern const char  TCA_ID_COPILOTO[9];
    extern const char  TCA_ID_TRIPULANTE[9];
    extern const char  TCA_ID_LECTOR_HUE[9];
    extern const char  TCA_ID_BLUETOOTH[9];
    
    extern const char  IO_ENGINE_LOCK[26];
    extern const char  IO_ENGINE_UNLOCK[26];

    // FUNCTIONS
    void process_Inputs();
    void process_inputRainSensor();
    
#endif // __GPIO_EXE_HPP