/**
 * @file fmu130_exe.hpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __FMU130_EXE_HPP
#define __FMU130_EXE_HPP

    #define             BLE_RESET_PULSE_WIDTH   100

    extern char         fmu_buffer[];
    extern const char   TCA_ID_FP_WORKING_STATUS[9];
    extern const char   TCA_ID_FP_SERVER_ADD[9];
    extern const char   TCA_ID_FP_ENROLL_CNT[9];
    extern const char   TCA_ID_FP_SERVER_DEL[9];
    extern const char   TCA_ID_IDLE_SHUTDOWN[9];
    extern const char   TCA_ID_FORCE_SEATBELT[9];
    extern const char   TCA_ID_FORCE_IDLE_SHUTDOWN[9];
    extern const char   TCA_ID_RAIN_SENSOR_ENA[9];
    extern const char   TCA_ID_SEATBELT_READ_ENA[9];
    extern const char   TCA_ID_VOLUME_SET[9];
    extern const char   TCA_ID_QUERY_SPEED_LIMIT[9];
    extern const char   TCA_ID_QUERY_IDLE_TIMER[9];
    extern const char   TCA_ID_IDLE_REMINDER[10];
    extern const char   TCA_ID_IDLE_REMINDER_ENA[10];
    extern const char   TCA_ID_IDLE_REMINDER_DIS[10];
    extern const char   TCA_ID_SAFETY_TIP_REMINDER[10];
    extern const char   TCA_ID_SAFETY_TIP_ENABLE[10];
    extern const char   TCA_ID_SAFETY_TIP_DISABLE[10];
    extern const char   TCA_ID_QUERY_SAFETY_TIP[9];
    extern const char   TCA_ID_Q_FINGERPRINT_STATUS[9];
    extern const char   TCA_ID_Q_RAINSENSOR_STATUS[9];
    extern const char   TCA_ID_Q_IDLE_SHUTDOWN_ST[9];
    extern const char   TCA_ID_Q_FORCE_SEATBELT_ST[9];
    extern const char   TCA_ID_Q_VOLUME_LEVEL[9];
    extern const char   TCA_ID_CHECK_ENROLLED_ID[9];
    extern const char   TCA_ID_HARDWARE_VERSION[9];
    extern const char   TCA_ID_FIRMWARE_VERSION[9];
    extern const char   TCA_ID_DIN_CONFIGURATION[9];
    extern const char   TCA_ID_Q_RAIN_SAMPLE_TIMER[9];
    extern const char   TCA_ID_FP_OVERRIDE_ENABLE[10];
    extern const char   TCA_ID_FP_OVERRIDE_DISABLE[10];
    extern const char   TCA_ID_Q_FP_OVERRIDE[9];
    extern const char   TCA_ID_GEO_WARNING_ENABLE[10];
    extern const char   TCA_ID_GEO_WARNING_DISABLE[10];
    extern const char   TCA_ID_GEO_WARNING_QUERY_OFF[10];
    extern const char   TCA_ID_GEO_WARNING_QUERY_ON[10];
    extern const char   TCA_ID_GEO_WARNING_GETIN[11];
    extern const char   TCA_ID_GEO_WARNING_EVENT[11];
    extern const char   TCA_ID_GEO_WARNING_GETOUT[11];
    extern const char   TCA_ID_RAIN_SILENT_MODE_ENA[10];
    extern const char   TCA_ID_RAIN_SILENT_MODE_DIS[10];
    extern const char   TCA_ID_RAIN_SILENT_MODE_Q[9];
    extern const char   TCA_ID_BLE_AUTORESET_DIS[10];
    extern const char   TCA_ID_BLE_AUTORESET_ENA[10];
    extern const char   TCA_ID_BLE_AUTORESET_qENA[11];
    extern const char   TCA_ID_BLE_AUTORESET_qDIS[11];
    extern const char   TCA_ID_PILOT_BUCKLE_NO[10];
    extern const char   TCA_ID_PILOT_BUCKLE_NC[10];
    extern const char   TCA_ID_COPILOT_BUCKLE_NO[10];
    extern const char   TCA_ID_COPILOT_BUCKLE_NC[10];
    extern const char   TCA_ID_CREW_BUCKLE_NO[10];
    extern const char   TCA_ID_CREW_BUCKLE_NC[10];
    extern const char  TCA_ID_IDLE_REMINDER_TIME[9];

    extern const char  FMU_ENGINE_LOCK[26];
    extern const char  FMU_ENGINE_UNLOCK[26];
    extern bool        trama_con_huella;

    void activate_idle_shutdown();
    void activate_idle_reminder();
    void queryRG9();
    void process_FMU130();
    void exe_idle_shutdown();
    void exe_idle_reminder();
    void boot_message();
    void BLE_reset();
    
#endif // __FMU130_EXE_HPP
