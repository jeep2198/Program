/**
 * 
 * @file fingerprint_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __FINGERPRINT_EXE_HPP
#define __FINGERPRINT_EXE_HPP

    extern char         id_buffer[];
    extern const char   TCA_ID_FP_LOGIN[16];
    extern const char   TCA_ID_UNIDENTIFIED[9];
    extern const char   TCA_ID_LECTOR_HUE[9];
    extern const char   TCA_FP_INIT[9];
    extern const char   TCA_ID_OVERRIDE_ID[10];
    extern const char   FP_ENGINE_LOCK[12];
    extern const char   FP_ENGINE_UNLOCK[17];
    extern const char   QUERY_SAFETY_TIP[4];
    extern const char   CHANGE_SAFETY_TIME[4];
    extern const char   SAFETY_TIP_ENABLE[4];
    extern const char   SAFETY_TIP_DISABLE[4];

    void identify_fingerPrint();
    void initFingerprintReader();

#endif // __FINGERPRINT_EXE_HPP