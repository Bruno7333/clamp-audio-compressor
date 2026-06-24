# Clamp — Dynamic Range Compressor

Clamp is a real-time desktop audio processor built with [JUCE](https://juce.com/) and C++. It takes live audio from an input device, applies dynamic range processing, and sends the result to an output device. Beyond a standard compressor, it also offers a brick-wall limiter and a "constant volume" leveler mode.

## What it does

Audio flows through Clamp in real time:

```
Input device  ->  AudioEngine (pass-through)  ->  CompressorProcessor (DSP)  ->  Output device
```

The `AudioEngine` copies the selected input into the output buffer, then the `CompressorProcessor` applies the chosen dynamics processing in place before it reaches your speakers/headphones.

## Processing modes

Selectable from the **Mode** dropdown:

- **Compressor** — Standard dynamic range compression. Signal above the threshold is attenuated according to the ratio, smoothed by the attack/release times.
- **Limiter** — A brick-wall limiter (effectively an infinite ratio). Anything above the threshold is clamped down *to* the threshold, so peaks never exceed that level.
- **Constant Volume** — A leveler/AGC: it pushes quiet material *up* and loud material *down* toward a single target loudness (set by the Threshold knob). A built-in noise floor (-60 dB) prevents it from boosting silence.

## Controls

**Routing**

- **Input Device** / **Output Device** — choose your audio interfaces.
- **Apply** — commits the selected devices *and* the current compressor settings.

**Compressor**

- **Threshold (dB)** — the level above which processing kicks in. In Constant Volume mode this acts as the target loudness.
- **Ratio** — how much to compress above the threshold (unused in Limiter / Constant Volume modes).
- **Attack (ms)** — how quickly gain is reduced when the signal exceeds the threshold.
- **Release (ms)** — how quickly gain returns to normal once the signal falls back below the threshold.
- **Makeup (dB)** — output gain applied after processing to compensate for level lost to compression.

**Buttons**

- **Bypass** — toggles processing on/off instantly (audio still passes through, unprocessed).
- **Apply** — commits routing + compressor settings.
- **Reset to Defaults** — restores the knobs to their default values (then press Apply to commit).

## How it works

The project is organized into a few focused classes:

- **`AudioEngine`** — implements `juce::AudioIODeviceCallback`. Performs the input-to-output pass-through and owns the DSP. Parameters set from the GUI are stored in `std::atomic` members, making the GUI-to-audio-thread handoff lock-free and real-time safe.
- **`CompressorProcessor`** — the DSP core. For each sample it detects the peak level across channels (stereo-linked), converts to decibels, computes the required gain reduction for the active mode, smooths it with a one-pole attack/release envelope, and applies the result plus makeup gain.
- **`MainComponent`** — the GUI: device routing, parameter knobs, the mode selector, and the buttons. It pushes committed values into the `AudioEngine`.
- **`CompressorLookAndFeel`** — custom theming for the interface.

## Requirements

- A C++ compiler with C++17 support (MSVC on Windows).
- [CMake](https://cmake.org/) 3.22 or newer.
- The [JUCE](https://github.com/juce-framework/JUCE) framework available locally.

> **Note:** `CMakeLists.txt` references JUCE via a local path (`add_subdirectory(C:/Users/bruno/JUCE ...)`). Adjust this path to point at your own JUCE checkout before building.

## Building

From the project root:

```sh
cmake -B build -S .
cmake --build build --config Debug
```

The executable is produced at:

```
build/AudioLimiter_artefacts/Debug/Clamp.exe
```

## Usage

1. Launch `Clamp.exe`.
2. Select an **Input Device** and **Output Device**.
3. Choose a **Mode** and dial in the knobs.
4. Click **Apply** to commit the routing and settings.
5. Use **Bypass** to A/B the processed vs. unprocessed signal.

### Tip: processing system or application audio

To compress audio from another app (music, a game, etc.) rather than a microphone, route that app's output through a virtual audio cable (such as [VB-CABLE](https://vb-audio.com/Cable/)):

1. Set the source app's playback device to the virtual cable's input.
2. In Clamp, select the virtual cable's output as your **Input Device** and your headphones as the **Output Device**.

Use headphones when testing with a microphone to avoid feedback.

## Project structure

```
audio-compressor/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp                  # JUCE application + window
    ├── MainComponent.h/.cpp      # GUI, routing, controls
    ├── AudioEngine.h/.cpp        # Audio callback, threading, pass-through
    ├── CompressorProcessor.h/.cpp# DSP: compressor / limiter / leveler
    └── CompressorLookAndFeel.h   # Custom UI theme
```

## Status

Functional: real-time pass-through, all three processing modes, live parameter control, and bypass. Built and tested on Windows with JUCE.