# Clamp — Dynamic Range Compressor

Clamp is a real-time desktop audio processor built with [JUCE](https://juce.com/) and C++. It takes live audio from an input device, applies dynamic range processing, and sends the result to an output device. Beyond a standard compressor, it also offers a brick-wall limiter and a "constant volume" leveler mode.

## What it does

Audio flows through Clamp in real time:

```
Input device  ->  AudioEngine (pass-through)  ->  CompressorProcessor (DSP)  ->  Output device
```

The `AudioEngine` copies the selected input into the output buffer, then the `CompressorProcessor` applies the chosen dynamics processing in place before it reaches your speakers/headphones.

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

## Processing modes

Selectable from the **Mode** dropdown:

- **Compressor** — Standard dynamic range compression. Signal above the threshold is attenuated according to the ratio, smoothed by the attack/release times.
- **Limiter** — A brick-wall limiter (effectively an infinite ratio). Anything above the threshold is clamped down *to* the threshold, so peaks never exceed that level.
- **Constant Volume** — A leveler/AGC: it pushes quiet material *up* and loud material *down* toward a single target loudness (set by the Threshold knob). A built-in noise floor (-60 dB) prevents it from boosting silence.


## Requirements

**To just run Clamp** (Option A — download the prebuilt app):

- **Windows 10 or 11** (64-bit). Nothing else is required.
- **A virtual audio cable** *(optional)* — needed only to process audio from other apps (games, browsers, etc.). This guide uses **[VB-CABLE](https://vb-audio.com/Cable/)**, installed below. Not needed if you only process a microphone.

**To build Clamp from source** (Option B), you additionally need:

- **Visual Studio 2022** with the **"Desktop development with C++"** workload (provides the MSVC compiler and the Windows SDK). [Download here](https://visualstudio.microsoft.com/downloads/).
- **[CMake](https://cmake.org/download/) 3.22 or newer** (Visual Studio 2022 already bundles a compatible CMake).
- **The [JUCE](https://github.com/juce-framework/JUCE) framework** (installed in Option B below).

---
# Installation Instructions

There are two ways to get Clamp. Pick whichever suits you:

- **Option A — Download the prebuilt app (easiest):** grab the ready-to-run executable from GitHub Releases. No JUCE, no compiler, no building required.
- **Option B — Build from source:** compile it yourself with CMake.

After getting Clamp either way, optionally install **VB-CABLE**, then **configure and run** — both are covered further below and apply to both options.

---

## Option A: Download the prebuilt release

1. Open the project's **[Releases](../../releases)** page on GitHub.
2. Under the latest release's **Assets**, download the **`Clamp-vX.Y.Z-win64.zip`** file (e.g. `Clamp-v0.1.0-win64.zip` — the version number matches the release).
3. Unzip it anywhere (for example, your Desktop).
4. Double-click **`Clamp.exe`** to run it — there is nothing to install.

> On first launch, Windows may show a blue **"Windows protected your PC"** box because the app isn't code-signed. Click **More info -> Run anyway**. This is expected for unsigned apps.

That's all you need to run Clamp. Skip ahead to **Install VB-CABLE** (optional) and **Configure and run**.

---

## Option B: Build from source

### Step B1: Install the JUCE library

Clamp builds JUCE from source via CMake, so you only need a local copy of the JUCE repository — no separate install or compilation step.

1. Download JUCE one of two ways:
   - **Git:** `git clone https://github.com/juce-framework/JUCE.git`
   - **Zip:** download from the [JUCE releases page](https://github.com/juce-framework/JUCE/releases) and extract it.
2. Place the resulting `JUCE` folder somewhere stable, for example `C:/JUCE` or `C:/Users/<you>/JUCE`.
3. Open `CMakeLists.txt` and point the JUCE path at that folder. Edit this line so the path matches where you put JUCE:

   ```cmake
   add_subdirectory(C:/Users/bruno/JUCE JUCE-build)
   ```

   For example, if you cloned JUCE to `C:/JUCE`, change it to:

   ```cmake
   add_subdirectory(C:/JUCE JUCE-build)
   ```

That's it — CMake will compile the JUCE modules it needs automatically the first time you build.

### Step B2: Build Clamp

From the project root:

```sh
cmake -B build -S .
cmake --build build --config Release
```

The finished, self-contained executable is produced at:

```
build/AudioLimiter_artefacts/Release/Clamp.exe
```

The Release build statically links the runtime, so `Clamp.exe` runs on any Windows PC without extra dependencies.

---

## Install VB-CABLE (virtual audio cable) — optional

A virtual audio cable lets Clamp capture the sound coming out of other applications (music players, browsers, games) so it can process it. Without it, you can still use a microphone as the input, but you can't process system/app audio. **This applies whether you downloaded Clamp (Option A) or built it (Option B).**

1. Download VB-CABLE from <https://vb-audio.com/Cable/>.
2. Extract the downloaded `.zip`.
3. **Right-click `VBCABLE_Setup_x64.exe` and choose "Run as administrator"**, then click **Install Driver**.
4. **Reboot** your PC so Windows registers the new devices.

After rebooting you will have two new audio devices:

- **CABLE Input (VB-Audio Virtual Cable)** — a *playback* device. Send audio here to capture it.
- **CABLE Output (VB-Audio Virtual Cable)** — a *recording* device. Clamp reads from here.

---

## Configure and run

### Processing game / app audio (via VB-CABLE)

This routes a program's sound through Clamp and out to your headphones/speakers:

1. **Send the source app's audio into the cable.** In Windows, open **Settings -> System -> Sound**, and under **Output** (or per-app in *Volume mixer*) set the playback device to **CABLE Input (VB-Audio Virtual Cable)**. Everything the app plays now goes into the cable instead of your speakers.
2. **Launch `Clamp.exe`.**
3. Set **Input Device** to **CABLE Output (VB-Audio Virtual Cable)** — this is the audio captured from step 1.
4. Set **Output Device** to your headphones or speakers — this is where the processed sound is sent.
5. Pick a **Mode** (Compressor, Limiter, or Constant Volume) and adjust the knobs.
6. Click **Apply** to commit the device routing and the compressor settings.

The signal now flows: **source app -> CABLE Input -> CABLE Output -> Clamp -> your headphones**.

> **Note:** while audio is routed into the cable, you'll only hear it through Clamp's output. To go back to normal, set the source app's playback device back to your speakers.

### Processing a microphone

1. Launch `Clamp.exe`.
2. Set **Input Device** to your microphone and **Output Device** to your headphones.
3. Choose a mode, set the knobs, and click **Apply**.

> **Use headphones** when the input is a microphone, otherwise the output will feed back into the mic and cause a loud feedback loop.

### Live controls

- **Bypass** toggles processing on/off instantly so you can A/B the processed vs. unprocessed sound.
- **Reset to Defaults** restores the knobs to their defaults (then click **Apply** to commit them).
- Changing the knobs or mode only takes effect after you click **Apply**.

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


## How it works

The project is organized into a few focused classes:

- **`AudioEngine`** — implements `juce::AudioIODeviceCallback`. Performs the input-to-output pass-through and owns the DSP. Parameters set from the GUI are stored in `std::atomic` members, making the GUI-to-audio-thread handoff lock-free and real-time safe.
- **`CompressorProcessor`** — the DSP core. For each sample it detects the peak level across channels (stereo-linked), converts to decibels, computes the required gain reduction for the active mode, smooths it with a one-pole attack/release envelope, and applies the result plus makeup gain.
- **`MainComponent`** — the GUI: device routing, parameter knobs, the mode selector, and the buttons. It pushes committed values into the `AudioEngine`.
- **`CompressorLookAndFeel`** — custom theming for the interface.


## Status

Functional: real-time pass-through, all three processing modes, live parameter control, and bypass. Built and tested on Windows with JUCE.