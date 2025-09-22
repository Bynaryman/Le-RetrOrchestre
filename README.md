# Le RetrOrchestre

Le RetrOrchestre is a mechatronic orchestra that combines art and science by turning retro computer hardware into MIDI-driven instruments. The name echoes the repeated "tr" sounds you hear when you pronounce it (and play it). Inspired by work such as the Floppotron, this project expands on this musical floppy-drive concept, adds flatbed scanners, and is preparing hard-disk drives for the next expansion.

## Highlights
- Repurpose vintage hardware (floppy drives, scanners, and planned HDD modules) as MIDI-controlled instruments
- Teensy-based control path that enumerates as a class-compliant USB MIDI device for quick DAW (Digital Audio Workstation, e.g. Ableton, Reaper,..) integration
- Centralised configuration for instrument counts, travel limits, and timing settings in `src/config.h`
- Half-period lookup tables for every MIDI note keep the scheduler fast and timing accurate
- Reference MIDI arrangements in `live/` to exercise the system during development

## Repository Layout
```
Le-RetrOrchestre/
├── retrorchestre.ino         // Main firmware that conducts the whole orchestra
├── src/
│   ├── config.h              // Knobs for number of FDDs/scanners/HDDs and motion limits
│   └── lut.h                 // Half-period lookup tables (us and timer ticks)
├── scripts/
│   └── compute_LUTs.py       // Helper to regenerate the LUTs when timing changes
├── midi_floppy.ino           // Early floppy-only prototype (Floppotron-era reference)
├── place_scanners.ino/       // Temporary scanner positioning helper (to be replaced by panel buttons)
├── live/                     // MIDI sets and Reaper projects (drafts; live rig still in progress)
└── README.md
```

## Getting Started
### Hardware
- Teensy 3.x/4.x board (tested with 32-bit ARM Teensy devices that provide `usbMIDI` and `IntervalTimer`)
- Up to `NUMBER_FDDS` floppy disk drives wired with step and direction signals (each can draw roughly 500 mW)
- Optional flatbed scanners wired through stepper drivers (optocouplers recommended)
- Stable 5V power rails, ideally from an ATX computer supply with the PS_ON (green) pin bridged to ground (black) so it stays awake; budget ample current headroom
- CNC Shield V3 with up to four A4988 motor drivers to host the scanner steppers; ensure coil pairs are identified before plugging headers
- MIDI host (computer or sequencer) with USB MIDI output

### Firmware Installation
1. Clone or download this repository.
2. Open `retrorchestre.ino` with the Arduino IDE or PlatformIO configured for your Teensy board.
3. Ensure the `src/` folder stays alongside the sketch so `config.h` and `lut.h` are found.
4. Select your Teensy target, enable `USB Type: MIDI`, and upload the sketch.
5. Watch the drives home in the setup routine, then confirm the Teensy enumerates as a MIDI device.

### Hooking Up to a DAW
- **Teensy native USB**: the board appears as a class-compliant MIDI device. Enable it in your DAW (e.g. Reaper) and route the track output to "Teensy MIDI".
- **Teensy via serial (legacy Arduinos)**: when using a board that only exposes a serial port, bridge it with `ttymidi`.
  1. Identify the serial device with `dmesg` (e.g. `/dev/ttyUSB0`).
  2. Start the bridge: `ttymidi -s /dev/ttyUSB0 -b 115200 -v`.
  3. List available MIDI ports with `aconnect -l`.
  4. Patch your DAW output to the bridge, e.g. `aconnect 129:0 128:1`.

## Configuring the Orchestra
Primary configuration lives in `src/config.h`:
- `NUMBER_FDDS`, `NUMBER_SCANNERS`, `NUMBER_HDDS`: declare how many of each instrument you have wired. Channels are allocated in that order.
- `FDD_LIMIT`, `SCANNER_LIMIT`: maximum travel before automatically reversing to protect the mechanics.
- `MINIMUM_*_NOTE` / `MAXIMUM_*_NOTE`: clamp playable MIDI note ranges per instrument family.
- `TIMER_RESOLUTION`: microseconds between scheduler ticks; affects both precision and CPU load.
- `DEBUG`: toggle verbose serial output for the first instrument of each family.

Upcoming hard-disk drive support will plug into the same structure--set `NUMBER_HDDS` ahead of time to reserve channels and future-proof your wiring.

## Wiring Notes (Work in Progress)
- **Power**: treat each floppy drive as a 500 mW load; group them on a dedicated 5V rail from an ATX PSU. Bridge PS_ON to ground, add fuses if possible, and share grounds between logic and drivers.
- **Scanners with CNC Shield V3**: map each stepper to an A4988 slot, match coil pairs with a multimeter before connecting, and decide whether the driver logic VCC comes from the Teensy or from the shield. Microstep jumpers (MS1-3) allow finer steps with lower acoustic output.
- **Control panel**: homing buttons and kill switches will eventually surface on a front panel. `place_scanners.ino` is the legacy helper for positioning; it will give way to manual jog buttons or toggle switches.
- **Grounding**: always join grounds between the Teensy, drivers, and power supply. Tie shield drain wires where needed to reduce noise.

Add photos, schematics, and BOM here when the harness is finalised.

## MIDI Channel Mapping
In the firmware, MIDI channels are used as instrument selectors:
- Channels `1...NUMBER_FDDS` drive the floppy motors.
- Channels following those (`NUMBER_FDDS+1...NUMBER_FDDS+NUMBER_SCANNERS`) run the scanners.
- The remaining channels are reserved for HDDs once implemented.

Send standard MIDI `NOTE_ON` / `NOTE_OFF` messages with velocity; the firmware converts pitch to travel speed and direction using the lookup tables.

## Timing Internals & Half-Period LUT
Precise motion depends on translating MIDI notes into step timings. For MIDI note `n`:
- Frequency `f = 440 x 2^((n - 69) / 12)` Hz
- Full period in microseconds `T = 1,000,000 / f`
- Half-period `T/2 = 500,000 / f`, because each step toggles the direction pin twice per full waveform
- Scheduler ticks `tick = (T/2) / TIMER_RESOLUTION`

`src/lut.h` contains two 128-entry tables with these values precomputed for every MIDI pitch. When you tweak `TIMER_RESOLUTION` or want to extend the range, regenerate the tables with `python3 scripts/compute_LUTs.py` and paste the output back into the header. This keeps the ISR lightweight while maintaining musically accurate tuning.

## Live Sets & Inspiration
The `live/` folder currently houses MIDI sketches and Reaper sessions used during prototyping. They are placeholders until a full stage show exists, but they help exercise the firmware under different rhythmic patterns and instrument combinations.

## HDD Sound Experiments on the Horizon
Two approaches are under evaluation:
1. **Voice-coil actuation**: drive the actuator coils with controlled audio signals so the slider behaves like a small speaker.
2. **Impulse percussion**: deliver short high-voltage pulses so the arm strikes the platter hub. Standard 3.5-inch and larger 5.25-inch drives yield distinct percussive responses.

Once the firmware pathways and safety interlocks are ready, HDD counts will be exposed alongside FDDs and scanners in `config.h`.

## Roadmap
- Finalise HDD support with dedicated timers and soft-landing routines
- Parameterise homing sequences for scanners and expose jog buttons on the control panel
- Add pitch-bend and CC handling for expressive control
- Document power distribution best practices, shielding tips, and fuse sizing

Contributions, ideas for new repertoires, or photos of your build are very welcome. May your drives stay in sync and your beats stay on tempo.
