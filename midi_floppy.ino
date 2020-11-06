// Binaryman aka Louis Ledoux

#include <avr/pgmspace.h>
//#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();
////MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

#define PIN_OFFSET 53

#define status_led_arduino_on 2
#define status_led_direction_disc_head 3
#define status_led_note_on 4

#define number_floppies 2
#define maximum_number_floppies 18 // let's use only double pins of arduino due

#define C0 12
#define C6 84
#define minimum_floppy_note C0
#define maximum_floppy_note C6

// /!\ TODO(binaryman): note that E5 and D#5 are bad but in the bounds

// comment of the 128 frequencies of midi format round to nearest integer
// uint16_t frequency[128] PROGMEM = {
// 8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, // bullshit
// 28, 29, 31, // A0 -> B0
// 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, // C1 -> B1
// 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123,// C2 -> B2
// 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,// C3 -> B3
// 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,// C4 -> B4
// 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,// C5 -> B5
// 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,// C6 -> B6
// 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,// C7 -> B7
// 4186, 4435, 4699, 4978, 5274, 5588, 5920, 5920, 6645, 7040, 7459, 7902,// C8 -> B8
// 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544// C9 -> G9
// };

// precomputed LUT of half periods
uint16_t half_periods[128] PROGMEM = {
62500, 55556, 55556, 50000, 50000, 45455, 41667, 41667, 38462, 35714, 33333, 33333, 31250, 29412, 27778, 26316, 23810, 22727, 21739, 20833,19231,
17857, 17241,16129,
15152, 14286, 13514, 12821, 12195, 11364, 10870, 10204, 9615, 9091, 8621, 8065,
7692, 7246, 6849, 6410, 6098, 5747, 5435, 5102, 4808, 4545, 4274, 4065,
3817, 3597, 3401, 3205, 3030, 2857, 2703, 2551, 2404, 2273, 2146, 2024,
1908, 1805, 1701, 1608, 1515, 1433, 1351, 1276, 1205, 1136, 1073, 1012,
956, 903, 852, 804, 759, 716, 676, 638, 602, 568, 536, 506,
478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253,
239, 226, 213, 201, 190, 179, 169, 159, 151, 142, 134, 127,
119, 113, 106, 100, 95, 89, 84, 84, 75, 71, 67, 63,
60, 56, 53, 50, 47, 45, 42, 40
};

uint16_t state_notes[number_floppies] = {0};
byte state_positions[number_floppies] = {0};
byte state_directions[number_floppies] = {0};
byte state_blinks[number_floppies] = {0};
unsigned long long state_previous_times[number_floppies] = {0};
unsigned long long t;

// variables
int limit = 158;
//unsigned long long previousTime = 0;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (pitch < minimum_floppy_note || pitch > maximum_floppy_note) {
    state_notes[channel-1] = 0xFF;
  }
  else {
    state_notes[channel-1] = pgm_read_word(&half_periods[pitch]);
  }
  //digitalWrite(status_led_note_on, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  state_notes[channel-1] = 0xFF;
  digitalWrite(status_led_note_on, LOW);
}

// void countersUpdate() {
//
//     if (dirCount >= limit) {
//       dir = 1;
//       digitalWrite(PIN_STEP_DIRECTION, HIGH);
//     }
//     if (dirCount <= 0) {
//       dir = 0;
//       digitalWrite(PIN_STEP_DIRECTION, LOW);
//     }
//     if (dir == 0) {
//       dirCount++;
//     } else {
//       dirCount--;
//     }
//
// }

void driveFloppies() {
  for (byte i = 0 ; i < number_floppies ; ++i) {

    if (state_notes[i] != 0xFF) {
      digitalWrite(status_led_note_on, HIGH);
      t = micros();
      if (t >= state_previous_times[i] + state_notes[i]) {
        state_previous_times[i]=t;
        state_blinks[i] = 1-state_blinks[i];
        digitalWrite(PIN_OFFSET-(i<<1), state_blinks[i]);
        if (state_positions[i] >= limit) {
          state_directions[i] = 1;
          digitalWrite(PIN_OFFSET-(i<<1)-1, HIGH);
        }
        if (state_positions[i] <= 0) {
          state_directions[i] = 0;
          digitalWrite(PIN_OFFSET-(i<<1)-1, LOW);
        }
        if (state_directions[i] == 0) {
          state_positions[i]++;
        } else {
          state_positions[i]--;
        }
      }
    } else {
      digitalWrite(status_led_note_on,HIGH);
      digitalWrite(PIN_OFFSET-(i<<1), LOW);
    }
  }
}

//setup: declaring iputs and outputs
void setup() {
  for (byte i = 0 ; i < number_floppies ; i++) { state_notes[i]=0xFF;}

  pinMode(status_led_arduino_on, OUTPUT);


  for (byte i = 0 ; i < number_floppies ; ++i) {
    pinMode(PIN_OFFSET-(i<<1), OUTPUT);
    pinMode(PIN_OFFSET-(i<<1)-1, OUTPUT);
  }

  pinMode(status_led_arduino_on, OUTPUT);
  pinMode(status_led_direction_disc_head, OUTPUT);
  pinMode(status_led_note_on, OUTPUT);
  Serial.begin(9600);


  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
  //MIDI.begin(1);

  for (byte i = 0 ; i < number_floppies ; ++i) {
    //Serial.println(PIN_OFFSET-(i<<1)-1);
    digitalWrite(PIN_OFFSET-(i<<1)-1, HIGH);
    for (int j = 0 ; j < limit ; j++) {
      digitalWrite(PIN_OFFSET-(i<<1), HIGH);
      delay(5);
      digitalWrite(PIN_OFFSET-(i<<1), LOW);
      delay(5);
    }
    digitalWrite(PIN_OFFSET-(i<<1)-1, LOW);
  }

  digitalWrite(status_led_arduino_on, HIGH);

}

//loop: wait for serial data
void loop () {
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
  driveFloppies();
}
