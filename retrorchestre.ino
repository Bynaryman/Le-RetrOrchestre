////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  __                       __              __                       //
//                       ________  / /__________  __________/ /_  ___  _____/ /_________              //
//                      / ___/ _ \/ __/ ___/ __ \/ ___/ ___/ __ \/ _ \/ ___/ __/ ___/ _ \             //
//                     / /  /  __/ /_/ /  / /_/ / /  / /__/ / / /  __(__  ) /_/ /  /  __/             //
//                    /_/   \___/\__/_/   \____/_/   \___/_/ /_/\___/____/\__/_/   \___/              //
//                                                                                                    //
//                                                      .....                                         //
//                                                     ..:-:....                                      //
//                                                    ...:%+:::-..                                    //
//                                                  .....:::**-:...                                   //
//                                                .....::::**=-::...                                  //
//                                             .......:=#=#@##+-*:....                                //
//                                          .........:::=#######-::......                             //
//                                        .......::::::-+#######=:::........                          //
//                                     .....:-==-:::=---=*#####=--::#::::::=..                        //
//                                   ......::+*#*#-=#################-::-:-=:..                       //
//                                .........:::########################*####==..                       //
//                       ................:::--=%###########################::...                      //
//                    .................:::@##@###########################@-::...                      //
//                  ....:-::::::::::::::::-=###############################-:::.                      //
//                 ....::%-:@::==::::::::-@##################################%:.                      //
//                .....:::-+##==##+*=-::-###################################-:..                      //
//             .......::::=*##########===####################################+:.                      //
//         ........:::::-+#############@+###################################-:..                      //
//     ...........::-=-==%##################################################:...                      //
//  ....:-::::::::::=*#####################################################::...                      //
// :::-::#-:-=-=-=%+@####################################################-::....                      //
//  .....:....::::::-+#####################################################::...                      //
//      ..........::=---=%##############@###################################::..                      //
//          ........::::++*############*+###################################-:..                      //
//              ......::::**##########=-=####################################+:.                      //
//                .....:::-**#==*#=#-:::-+##################################-::.                      //
//                  ...:==:::::-=::::::::=###################################@*.                      //
//                   ...:=::...::::.::::::-*###############################-:::.                      //
//                    ..................::+#@+###########################+-::...                      //
//                         ..............::::-=############################*:...                      //
//                                 ........:::########################=%#@#+:..                       //
//                                   .......:-#*==-=##############+##-::::-=:..                       //
//                                      ....:::--::::::-=######=-::::::...::..                        //
//                                        .........::::=@#######=:::........                          //
//                                           ........:::+######%=::.....                              //
//                                              ......::=-=@##=:::....                                //
//                                                 .....:::-*=:::...                                  //
//                                                   ....::-=@=:...                                   //
//                                                    ...::%::.:..                                    //
//                                                     ..:-:...                                       //
//                                                       ...                                          //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//    .---------------------------------.                                                             //
//    |  .---------------------------.  |    Stub .ino file to eventually work with the arduino IDE.  //
//    |[]|                           |[]|    The real entrypoint is located at src/main.cpp.          //
//    |  |  _Binaryman_____________  |  |                                                             //
//    |  |  _aka_Louis_Ledoux______  |  |                                                             //
//    |  |  _beta_v1.00____________  |  |                                                             //
//    |  |                           |  |                                                             //
//    |  |                           |  |                                                             //
//    |  |                           |  |                                                             //
//    |  |                           |  |                                                             //
//    |  |                           |  |                                                             //
//    |  `---------------------------'  |                                                             //
//    |      __________________ _____   |                                                             //
//    |     |   ___            |     |  |                                                             //
//    |     |  |   |           |     |  |                                                             //
//    |     |  |   |           |     |  |                                                             //
//    |     |  |   |           |     |  |                                                             //
//    |     |  |___|           |     |  |                                                             //
//    \_____|__________________|_____|__|                                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "src/config.h"
#include "src/lut.h"
//#include <TimerOne.h>

// Instrument description part

// FDD
uint8_t m_state_notes_fdds[NUMBER_FDDS];
uint16_t m_state_halfperiods_fdds[NUMBER_FDDS] = {0};
uint8_t m_state_positions_fdds[NUMBER_FDDS] = {0};
uint8_t m_state_directions_fdds[NUMBER_FDDS] = {0};
uint8_t m_state_blinks_fdds[NUMBER_FDDS] = {0};
uint16_t m_state_current_tick_fdds[NUMBER_FDDS] = {0};

// SCANNER
uint8_t m_state_notes_scanners[NUMBER_SCANNERS];
uint16_t m_state_halfperiods_scanners[NUMBER_SCANNERS] = {0};
uint16_t m_state_positions_scanners[NUMBER_SCANNERS] = {0};
uint8_t m_state_directions_scanners[NUMBER_SCANNERS] = {0};
uint8_t m_state_blinks_scanners[NUMBER_SCANNERS] = {0};
uint16_t m_state_current_tick_scanners[NUMBER_SCANNERS] = {0};
unsigned int m_scanner_offset = NUMBER_FDDS;

// HDD
#if NUMBER_HDDS > 0
#if HDD_TRIGGER_DURATION_US < TIMER_RESOLUTION
#error "HDD_TRIGGER_DURATION_US must be greater or equal than TIMER_RESOLUTION"
#endif
const uint16_t HDD_TRIGGER_DURATION_TICKS =
    (HDD_TRIGGER_DURATION_US + TIMER_RESOLUTION - 1) / TIMER_RESOLUTION;
uint8_t m_state_notes_hdds[NUMBER_HDDS];
uint16_t m_state_trigger_countdown_hdds[NUMBER_HDDS] = {0};
#endif
unsigned int m_hdd_offset = NUMBER_FDDS+NUMBER_SCANNERS;

void receiveNoteON(byte channel, byte pitch, byte velocity) {
  if (velocity == 0) {
    receiveNoteOFF(channel, pitch, velocity);
    return;
  }
  byte instrument_id = channel-1;
	// detect what kind of instrument is played
	// note that there is no check on array sizes, the host has to be correlated with declared instruments
	if (instrument_id < m_scanner_offset) {
		if (pitch < MINIMUM_FDD_NOTE || pitch > MAXIMUM_FDD_NOTE) {

			m_state_halfperiods_fdds[instrument_id] = 0;
			m_state_notes_fdds[instrument_id] = 0xFF;
		}
		else {
			//Serial.println("good note");
			//Serial.println(pitch);
			m_state_halfperiods_fdds[instrument_id] = lut_ticks_halfperiods[pitch];
			//Serial.println(lut_ticks_halfperiods[pitch]);
			m_state_notes_fdds[instrument_id] = pitch;
		}
	}
	else if (instrument_id >= m_scanner_offset && instrument_id < m_hdd_offset) {
		if (pitch < MINIMUM_SCANNER_NOTE || pitch > MAXIMUM_SCANNER_NOTE) {
			m_state_halfperiods_scanners[instrument_id-m_scanner_offset] = 0;
			m_state_notes_scanners[instrument_id-m_scanner_offset] = 0xFF;
		}
		else {
			m_state_halfperiods_scanners[instrument_id-m_scanner_offset] = lut_ticks_halfperiods[pitch];
			m_state_notes_scanners[instrument_id-m_scanner_offset] = pitch;
		}
	}
	else if (instrument_id >= m_hdd_offset) {
#if NUMBER_HDDS > 0
		byte hdd_id = instrument_id - m_hdd_offset;
		if (hdd_id < NUMBER_HDDS) {
			if (pitch < MINIMUM_HDD_NOTE || pitch > MAXIMUM_HDD_NOTE) {
				m_state_notes_hdds[hdd_id] = 0xFF;
				m_state_trigger_countdown_hdds[hdd_id] = 0;
				digitalWriteFast(instrument_id << 1, LOW);
				digitalWriteFast((instrument_id << 1) + 1, LOW);
			}
			else {
				m_state_notes_hdds[hdd_id] = pitch;
				bool drive_positive = velocity >= 64;
				if (drive_positive) {
					digitalWriteFast(instrument_id << 1, HIGH);
					digitalWriteFast((instrument_id << 1) + 1, LOW);
				} else {
					digitalWriteFast(instrument_id << 1, LOW);
					digitalWriteFast((instrument_id << 1) + 1, HIGH);
				}
				m_state_trigger_countdown_hdds[hdd_id] = HDD_TRIGGER_DURATION_TICKS;
			}
		}
#endif
	}
}

void receiveNoteOFF(byte channel, byte pitch, byte velocity) {
	byte instrument_id = channel-1;
	if (instrument_id < m_scanner_offset) {
		m_state_halfperiods_fdds[instrument_id] = 0;
		m_state_notes_fdds[instrument_id] = 0xFF;
	}
	else if (instrument_id >= m_scanner_offset && instrument_id < m_hdd_offset) {
		m_state_halfperiods_scanners[instrument_id-m_scanner_offset] = 0;
		m_state_notes_scanners[instrument_id-m_scanner_offset] = 0xFF;
	}
	else if (instrument_id >= m_hdd_offset) {
#if NUMBER_HDDS > 0
		byte hdd_id = instrument_id - m_hdd_offset;
		if (hdd_id < NUMBER_HDDS) {
			m_state_notes_hdds[hdd_id] = 0xFF;
			m_state_trigger_countdown_hdds[hdd_id] = 0;
			digitalWriteFast(instrument_id << 1, LOW);
			digitalWriteFast((instrument_id << 1) + 1, LOW);
		}
#endif
	}

}
#pragma GCC push_options
#pragma GCC optimize("Ofast")
FASTRUN void conduct() {
//void conduct() {
	for (byte i = 0 ; i < NUMBER_FDDS ; ++i) {
		if (m_state_notes_fdds[i] != 0xFF) {
			m_state_current_tick_fdds[i]++;
			if (m_state_current_tick_fdds[i] >= m_state_halfperiods_fdds[i]) {
				m_state_blinks_fdds[i] = 1 - m_state_blinks_fdds[i];
				digitalWriteFast((i << 1), m_state_blinks_fdds[i]);
				if (m_state_positions_fdds[i] >= FDD_LIMIT) {
					m_state_directions_fdds[i] = 1;
					digitalWriteFast((i << 1) + 1, HIGH);
				}
				if (m_state_positions_fdds[i] <= 0) {
					m_state_directions_fdds[i] = 0;
					digitalWriteFast((i << 1) + 1, LOW);
				}
				if (m_state_directions_fdds[i] == 0) {
					m_state_positions_fdds[i]++;
				} else {
					m_state_positions_fdds[i]--;
				}
        m_state_current_tick_fdds[i] = 0;
			}
		} else {
			digitalWriteFast(i << 1, LOW);
		}
	}
	for (byte i = 0, j=m_scanner_offset ; i < NUMBER_SCANNERS ; ++i, ++j) {
    if (m_state_notes_scanners[i] != 0xFF) {
			m_state_current_tick_scanners[i]++;
			if (m_state_current_tick_scanners[i] >= m_state_halfperiods_scanners[i]) {
				m_state_blinks_scanners[i] = 1 - m_state_blinks_scanners[i];
				digitalWriteFast((j << 1), m_state_blinks_scanners[i]);
				if (m_state_positions_scanners[i] >= SCANNER_LIMIT) {
					m_state_directions_scanners[i] = 1;
					digitalWriteFast((j << 1) + 1, HIGH);
				}
				if (m_state_positions_scanners[i] <= 0) {
					m_state_directions_scanners[i] = 0;
					digitalWriteFast((j << 1) + 1, LOW);
				}
				if (m_state_directions_scanners[i] == 0) {
					m_state_positions_scanners[i]++;
				} else {
					m_state_positions_scanners[i]--;
				}
        m_state_current_tick_scanners[i] = 0;
			}
		} else {
			digitalWriteFast(i << 1, LOW);
		}
	}
#if NUMBER_HDDS > 0
	for (byte i = 0, k = m_hdd_offset ; i < NUMBER_HDDS ; ++i, ++k) {
		if (m_state_trigger_countdown_hdds[i] > 0) {
			m_state_trigger_countdown_hdds[i]--;
			if (m_state_trigger_countdown_hdds[i] == 0) {
				digitalWriteFast(k << 1, LOW);
				digitalWriteFast((k << 1) + 1, LOW);
			}
		} else {
			digitalWriteFast(k << 1, LOW);
			digitalWriteFast((k << 1) + 1, LOW);
		}
	}
#endif
}
#pragma GCC pop_options

#if DEBUG
void print_fdds() {
Serial.print("FDDs : ");
  for(unsigned int i = 0 ; i < 1 ; ++i) {
    Serial.print(m_state_notes_fdds [i]);
    Serial.print("; ");
    Serial.print(m_state_halfperiods_fdds [i]);
    Serial.print("; ");
    Serial.print(m_state_positions_fdds [i]);
    Serial.print("; ");
    Serial.print(m_state_directions_fdds [i]);
    Serial.print("; ");
    Serial.print(m_state_blinks_fdds [i]);
    Serial.print("; ");
    Serial.print(m_state_current_tick_fdds [i]);
    Serial.print("\n");
  }
}
void print_scanners() {
Serial.print("SCANNERs : ");
  for(unsigned int i = 0 ; i < 1 ; ++i) {
    Serial.print(m_state_notes_scanners [i]);
    Serial.print("; ");
    Serial.print(m_state_halfperiods_scanners [i]);
    Serial.print("; ");
    Serial.print(m_state_positions_scanners [i]);
    Serial.print("; ");
    Serial.print(m_state_directions_scanners [i]);
    Serial.print("; ");
    Serial.print(m_state_blinks_scanners [i]);
    Serial.print("; ");
    Serial.print(m_state_current_tick_scanners [i]);
    Serial.print("\n");
  }
}
#endif

IntervalTimer timer;

void setup() {

  #if DEBUG
    Serial.begin(9600);
  #endif

	for (unsigned int i = 0 ; i < NUMBER_FDDS + NUMBER_SCANNERS + NUMBER_HDDS ; ++i) {
    	pinMode(i << 1, OUTPUT);
    	pinMode((i << 1) + 1, OUTPUT);
		digitalWriteFast(i << 1, LOW);
		digitalWriteFast((i << 1) + 1, LOW);
	}
	for (unsigned int i = 0 ; i < NUMBER_FDDS ; ++i) {
		m_state_notes_fdds[i] = 0xFF;
	}
	for (unsigned int i = 0 ; i < NUMBER_SCANNERS ; ++i) {
		m_state_notes_scanners[i] = 0xFF;
	}
#if NUMBER_HDDS > 0
	for (unsigned int i = 0 ; i < NUMBER_HDDS ; ++i) {
		m_state_notes_hdds[i] = 0xFF;
		m_state_trigger_countdown_hdds[i] = 0;
	}
#endif
  for (byte i = 0 ; i < NUMBER_FDDS ; ++i) {
      digitalWriteFast((i << 1) + 1, HIGH);
      for (int j = 0 ; j < FDD_LIMIT ; j++) {
        digitalWriteFast(i << 1, HIGH);
        delay(5);
        digitalWriteFast(i << 1, LOW);
        delay(5);
      }
      digitalWriteFast((i << 1) + 1, LOW);
  }
  // todo similar code for scanners

	usbMIDI.setHandleNoteOn(receiveNoteON);
	usbMIDI.setHandleNoteOff(receiveNoteOFF);
	//TODO(me): add pitch bend
	//TODO(me): add control change
	//Timer1.initialize(TIMER_RESOLUTION);
	//Timer1.attachInterrupt(conduct);
  timer.begin(conduct, TIMER_RESOLUTION);
}

void loop() {
  while (1) {
    usbMIDI.read();
    #if DEBUG
      //print_fdds();
      //print_scanners();
    #endif
  }
}
