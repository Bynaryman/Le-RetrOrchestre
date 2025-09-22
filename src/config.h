#ifndef __CONFIG_H__
#define __CONFIG_H__

// Orchestra instruments description
#define NUMBER_FDDS 8  // ~500mW power consumption per FDD
#define NUMBER_SCANNERS 2
#define NUMBER_HDDS 0


// experimental constants
// /!\ TODO(binaryman): note that E5 and D#5 are bad but in the bounds
#define C0 12
#define C6 84
#define MINIMUM_FDD_NOTE C0
#define MAXIMUM_FDD_NOTE C6
#define FDD_LIMIT 158

// /!\ TODO(binaryman): new quest : find the bad notes of scanner. might depend on the material it is play on
#define MINIMUM_SCANNER_NOTE 0
#define MAXIMUM_SCANNER_NOTE 127
#define SCANNER_LIMIT 4096

// Timer
#define TIMER_RESOLUTION 10  // (usec)

// Debug
#define DEBUG false

#endif  // __CONFIG_H__
