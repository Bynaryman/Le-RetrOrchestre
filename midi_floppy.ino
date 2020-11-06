// A very simple MIDI synth.
// Greg Kennedy 2011

#include <avr/pgmspace.h>
//#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();
////MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

#define PIN_STEP_STEP 5
#define PIN_STEP_DIRECTION 7
#define status_led_1 41
#define status_led_2 39
#define status_led_3 6

// MIDI channel to answer to, 0x00 - 0x0F
#define myChannel 0x00


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
// variables
int dirCount = 0;
int dir = 0;
int limit = 170;
int tune = 0;
bool state_note = 0;  // 0 is note off, 1 is note on
bool state_blink = 0;
uint16_t half_period = 0;  // (1000/f) (ms)
//unsigned long long currentTime = 0;
unsigned long long previousTime = 0;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  state_note = 1;
  half_period = pgm_read_word(&half_periods[pitch]);
  digitalWrite(LED_BUILTIN, HIGH);
  switch (pitch % 3) {
    case 0:
      digitalWrite(status_led_1, HIGH);
      break;
    case 1:
      digitalWrite(status_led_2, HIGH);
      break;
//    case 2:
//      digitalWrite(status_led_3, HIGH);
//      break;
    default:
      break;
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  state_note = 0;
  half_period = 0;
  digitalWrite(LED_BUILTIN, LOW);
  switch (pitch % 3) {
    case 0:
      digitalWrite(status_led_1, LOW);
      break;
    case 1:
      digitalWrite(status_led_2, LOW);
      break;
    case 2:
      digitalWrite(status_led_3, LOW);
      break;
    default:
      break;
  }
}

void counterUpdate() {

    if (dirCount >= limit) {
      dir = 1;
      digitalWrite(PIN_STEP_DIRECTION, HIGH);
    }
    if (dirCount <= 0) {
      dir = 0;
      digitalWrite(PIN_STEP_DIRECTION, LOW);
    }
    if (dir == 0) {
      dirCount++;
    } else {
      dirCount--;
    }
  
}

void driveFloppy() {
  if (state_note == 1) {
    if (micros() >= previousTime + half_period) {
      previousTime=micros();
      state_blink = 1-state_blink;
      digitalWrite(PIN_STEP_STEP, state_blink);
      counterUpdate();
    }
  } else {
    digitalWrite(PIN_STEP_STEP, LOW);
  }
}

//setup: declaring iputs and outputs and begin serial
void setup() {

  pinMode(PIN_STEP_STEP, OUTPUT);
  pinMode(PIN_STEP_DIRECTION, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(status_led_1, OUTPUT);
  pinMode(status_led_2, OUTPUT);
  pinMode(status_led_3, OUTPUT);

  //digitalWrite(status_led_3, HIGH);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  //start serial with midi baudrate 31250
  // or 38400 for debugging (eg MIDI over serial from PC)
  //Serial.begin(31250);
  //MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
  MIDI.begin(1);

  digitalWrite(PIN_STEP_DIRECTION, HIGH);
  for (int i = 0 ; i < limit ; i++) {
    digitalWrite(PIN_STEP_STEP, HIGH);
    delay(5);
    digitalWrite(PIN_STEP_STEP, LOW);
    delay(5);

  }
  digitalWrite(PIN_STEP_DIRECTION, LOW);
}

//loop: wait for serial data
void loop () {
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read(0);
  digitalWrite(status_led_3, dir);
  driveFloppy();
}

void stepPlay(int freq_note) {
  digitalWrite(PIN_STEP_STEP, HIGH);
  delay((1000 / (freq_note)) / 2);
  digitalWrite(PIN_STEP_STEP, LOW);
  delay((1000 / (freq_note)) / 2);
}

//int getNote(){
//  if ( ((t/500)%13)==((t/500)%26) ) {
//    note = notes[(t/500)%13];
//  } else {
//    note = notes[12-((t/500)%13)];
//  }
//}

void counterIncrement() {
  if (dirCount >= limit) {
    dir = 1;
    digitalWrite(PIN_STEP_DIRECTION, HIGH);
  }
  if (dirCount <= 0) {
    dir = 0;
    digitalWrite(PIN_STEP_DIRECTION, LOW);
  }
  if (dir == 0) {
    dirCount++;
  } else {
    dirCount--;
  }
}
