/**
 * @file playlist_exe.cpp
 * @author Felícito Manzano (felicito.manzano@detektor.com.sv)
 * @brief 
 * @version 0.1
 * @date 2020-11-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "mbed.h"
#include "BufferedSerial.h"
#include "jq8400_voice.hpp"
#include "voice_cn_pa.hpp"
#include "playlist.hpp"

extern  bool             flag_playingSound;
extern  queue            colaPlaylist;
extern  BufferedSerial   voice_uart;
extern  Ticker           tick_playingAudio;

// DEFINICIÓN DE CERVECERÍA
//#if (CODIGO_PAIS == 507) // PANAMA
    float   DURATION_AUDIO_FILE[] = {8.6,   // 01 - WELCOME
                                    4.6,   // 02 - DOOR SAFETY REMINDER
                                    1.6,   // 03 - DRIVER AUTHORIZED
                                    1.6,   // 04 - DRIVER UNAUTHORIZED
                                    2.6,   // 05 - DRIVER SEATBELT FASTENED
                                    2.6,   // 06 - DRIVER SEATBELT UNFASTENED
                                    2.6,   // 07 - COPILOT SEATBELT FASTENED
                                    7.6,   // 08 - COPILOT SEATBELT UNFASTENED
                                    2.6,   // 09 - CREW SEATBELT FASTENED
                                    8.6,   // 10 - CREW SEATBELT UNFASTENED 
                                    2.6,   // 11 - WARNING OVERSPEED
                                    8.6,   // 12 - OVERSPEED ALERT
                                    8.6,   // 13 - WARNING OVERSPEED WET
                                    6.6,   // 14 - OVERSPEED WET ALERT
                                    4.6,   // 15 - HARSH ACCELERATION
                                    4.6,   // 16 - HARSH BREAKING
                                    7.6,   // 17 - SHARP CORNERING
                                    8.6,   // 18 - CRASH DETECTION
                                    6.6,   // 19 - IDLE TIME WARNING
                                    2.6,   // 20 - IDLE ENGINE SHUTDOWN
                                    3.6,   // 21 - TRACK WET
                                    5.6,   // 22 - TRACK DRY
                                    1.6,   // 23 - FINGERPRINT ACTIVE
                                    2.6,   // 24 - FINGERPRINT DISABLE
                                    5.2,   // 25 - IDLE REMINDER
                                    4.6,   // 26 - IDLE OVERRIDE (BRAKE PEDAL)
                                    8.6,   // 27 - SAFETY TIP01
                                    11.6,  // 28 - SAFETY TIP02
                                    9.6,   // 29 - SAFETY TIP03
                                    9.6,   // 30 - SAFETY TIP04
                                    7.6,   // 31 - SAFETY TIP05
                                    5.6,   // 32 - SAFETY TIP06
                                    6.6,   // 33 - SAFETY TIP07
                                    6.6,   // 34 - SAFETY TIP08
                                    6.6,   // 35 - SAFETY TIP09
                                    7.6,   // 36 - SAFETY TIP10
                                    8.6,   // 37 - SAFETY TIP11
                                    4.6,   // 38 - SAFETY TIP12
                                    6.6,   // 39 - SAFETY TIP13
                                    6.6,   // 40 - SAFETY TIP14
                                    12.6,  // 41 - FINGERPRINT AUTH - NO BLUETOOTH GPS COMMUNICATION
                                    2.6,   // 42 - FINGERPRINT ENABLE
                                    4.6, // INSIDE GEOZONE EXAMPLE
                                    7.6, // OUTSIDE GEOZONE EXAMPLE
                                    4.6, // EVENT GEOZONE EXAMPLE
                                    0.1};
/*#elif (CODIGO_PAIS == 504) // HONDURAS
    float   DURATION_AUDIO_FILE[] = {6.6,   // 01 - WELCOME
                                    6.6,   // 02 - DOOR SAFETY REMINDER
                                    2.6,   // 03 - DRIVER AUTHORIZED
                                    2.6,   // 04 - DRIVER UNAUTHORIZED
                                    2.6,   // 05 - DRIVER SEATBELT FASTENED
                                    2.6,   // 06 - DRIVER SEATBELT UNFASTENED
                                    2.6,   // 07 - COPILOT SEATBELT FASTENED
                                    2.6,   // 08 - COPILOT SEATBELT UNFASTENED
                                    2.6,   // 09 - CREW SEATBELT FASTENED
                                    2.6,   // 10 - CREW SEATBELT UNFASTENED 
                                    5.6,   // 11 - WARNING OVERSPEED
                                    5.6,   // 12 - OVERSPEED ALERT
                                    9.6,   // 13 - WARNING OVERSPEED WET
                                    8.6,   // 14 - OVERSPEED WET ALERT
                                    8.6,   // 15 - HARSH ACCELERATION
                                    8.6,   // 16 - HARSH BREAKING
                                    8.6,   // 17 - SHARP CORNERING
                                    8.6,   // 18 - CRASH DETECTION
                                    8.6,   // 19 - IDLE TIME WARNING
                                    2.6,   // 20 - IDLE ENGINE SHUTDOWN
                                    3.6,   // 21 - TRACK WET
                                    4.6,   // 22 - TRACK DRY
                                    1.6,   // 23 - FINGERPRINT ACTIVE
                                    2.6,   // 24 - FINGERPRINT UNACTIVE
                                    5.2,   // 25 - IDLE REMINDER
                                    4.6,   // 26 - IDLE OVERRIDE (BRAKE PEDAL)
                                    8.6,   // 27 - SAFETY TIP01
                                    11.6,  // 28 - SAFETY TIP02
                                    9.6,   // 29 - SAFETY TIP03
                                    9.6,   // 30 - SAFETY TIP04
                                    7.6,   // 31 - SAFETY TIP05
                                    5.6,   // 32 - SAFETY TIP06
                                    6.6,   // 33 - SAFETY TIP07
                                    6.6,   // 34 - SAFETY TIP08
                                    6.6,   // 35 - SAFETY TIP09
                                    7.6,   // 36 - SAFETY TIP10
                                    8.6,   // 37 - SAFETY TIP11
                                    4.6,   // 38 - SAFETY TIP12
                                    6.6,   // 39 - SAFETY TIP13
                                    6.6,   // 40 - SAFETY TIP14
                                    12.6,  // 41 - FINGERPRINT AUTH - NO BLUETOOTH GPS COMMUNICATION
                                    2.6,   // 42 - FINGERPRINT ENABLE
                                    4.6, // INSIDE GEOZONE EXAMPLE
                                    7.6, // OUTSIDE GEOZONE EXAMPLE
                                    4.6, // EVENT GEOZONE EXAMPLE
                                    0.1};
#endif
*/


/**
 * @brief Desactivar la bandera que se está reproduciendo audio
 * 
 */
void endPlayingAudio(){
    flag_playingSound = false;
    tick_playingAudio.detach(); 
}

/**
 * @brief 
 * 
 */
void processPlaylist() {
    if (colaPlaylist.isEmpty()) {
            //flag_playingSound = false;    
    } else {
            int tempID;
            tempID = colaPlaylist.peek();
            colaPlaylist.dequeue();
            flag_playingSound = true;
            tick_playingAudio.attach(&endPlayingAudio, DURATION_AUDIO_FILE[(tempID - 1)]);
            jq8400_playID(tempID, &voice_uart);
    }
}

