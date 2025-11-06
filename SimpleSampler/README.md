# SimpleSampler - WAV Sampler VST Plugin with Virtual Keyboard

A simple but fully functional audio plugin built with JUCE framework that loads and plays WAV samples with a built-in virtual keyboard, volume control, and reverb effect.

## Features

- **Virtual Piano Keyboard**: 12 keys (one octave) with piano-style layout
- **Octave Switching**: Navigate octaves 0-8 to access full MIDI range
- **WAV Sample Loading**: Load any mono or stereo WAV file
- **MIDI Triggered Playback**: Play samples with virtual keyboard or external MIDI controller
- **Volume Control**: Smooth volume adjustment (0-100%)
- **Reverb Effect**: Adjustable reverb with dry/wet control
- **Cross-Platform**: Builds on macOS (AU, VST3), Windows (VST3), and Linux (VST3)

## Screenshot / UI Layout

```
┌────────────────────────────────────────────────────────┐
│                    SimpleSampler                       │
├────────────────────────────────────────────────────────┤
│  Virtual Keyboard Section:                            │
│  ┌──┬──┬──┬──┬──┐  Octave Controls: < > [Octave: 4]  │
│  │C#│D#│  │F#│G#│A#│                                  │
│  ├┬─┴┬─┴┬─┬┴─┬┴─┬┴─┬┐  (Black keys)                  │
│  ││C ││D ││E││F ││G ││A ││B │  (White keys)          │
│  │└──┘└──┘└──┘└──┘└──┘└──┘└──┘                       │
├────────────────────────────────────────────────────────┤
│  Sample Loading:                                       │
│  [ Load Sample ]                                       │
│  [ No sample loaded ]                                  │
├────────────────────────────────────────────────────────┤
│  Controls:                                             │
│  Volume    Reverb                                      │
│  [slider]  [slider]                                    │
└────────────────────────────────────────────────────────┘
```

## Requirements

### macOS (Recommended)
- **macOS**: 10.13 (High Sierra) or later
- **Xcode**: Version 12.0 or later
- **Xcode Command Line Tools**: Required
- **Homebrew**: Package manager for macOS
- **CMake**: Version 3.15 or later

### Other Platforms
- **Windows**: Windows 10 or later, Visual Studio 2019 or later, CMake
- **Linux**: Ubuntu 18.04+, GCC 7+, CMake, ALSA/JACK development libraries

## Building on macOS

### Step 1: Install Prerequisites

1. **Install Xcode Command Line Tools:**
   ```bash
   xcode-select --install
   ```

2. **Install Homebrew** (if not installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install CMake:**
   ```bash
   brew install cmake
   ```

4. **Verify installations:**
   ```bash
   xcode-select -p          # Should show Xcode path
   brew --version           # Should show Homebrew version
   cmake --version          # Should show CMake 3.15+
   ```

### Step 2: Get the Source Code

The project is already set up with JUCE included. Navigate to the project directory:

```bash
cd /path/to/FunVST
ls -la
# You should see: JUCE/ SimpleSampler/ LICENSE
```

### Step 3: Configure the Build

1. **Create a build directory:**
   ```bash
   cd SimpleSampler
   mkdir build
   cd build
   ```

2. **Generate build files with CMake:**
   ```bash
   cmake .. -G Xcode
   ```

   Or for faster command-line builds:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

### Step 4: Build the Plugin

**Option A: Using Xcode (GUI)**
```bash
open SimpleSampler.xcodeproj
# Then in Xcode: Product > Build (⌘B)
```

**Option B: Using CMake (Command Line)**
```bash
cmake --build . --config Release
```

Build time: ~2-5 minutes depending on your system.

### Step 5: Install the Plugin

The plugin is automatically copied to the correct locations if `COPY_PLUGIN_AFTER_BUILD` is enabled in CMakeLists.txt (default: ON).

**Manual installation** (if needed):

1. **VST3 Plugin:**
   ```bash
   cp -r build/SimpleSampler_artefacts/Release/VST3/SimpleSampler.vst3 \
       ~/Library/Audio/Plug-Ins/VST3/
   ```

2. **AU Plugin:**
   ```bash
   cp -r build/SimpleSampler_artefacts/Release/AU/SimpleSampler.component \
       ~/Library/Audio/Plug-Ins/Components/
   ```

3. **Verify installation:**
   ```bash
   ls ~/Library/Audio/Plug-Ins/VST3/
   ls ~/Library/Audio/Plug-Ins/Components/
   ```

### Step 6: Validate the Plugin (Optional but Recommended)

**For AU plugins:**
```bash
auval -v aumu Ssmp Ynam
```

This validates that the Audio Unit is properly installed and functioning.

## Usage Guide

### Loading a Sample

1. **Launch your DAW** (Logic Pro, Ableton Live, Reaper, GarageBand, etc.)
2. **Create a new Software Instrument track**
3. **Load SimpleSampler** from your plugin list
4. **Click "Load Sample"** button in the plugin UI
5. **Select a WAV file** from your computer
6. The filename will display below the button

### Playing Samples

**Virtual Keyboard:**
- Click on the piano keys to play notes
- Use **< >** buttons to change octaves (0-8)
- Current octave displays next to the buttons
- Green highlight indicates pressed keys

**External MIDI Controller:**
- Connect your MIDI keyboard
- Play notes directly - they will trigger the sample
- Works simultaneously with the virtual keyboard

### Controls

**Volume Slider:**
- Adjust output level (0-100%)
- Default: 70%
- Affects both virtual keyboard and external MIDI

**Reverb Slider:**
- Add reverb effect to the sound
- 0% = completely dry signal
- 100% = completely wet signal
- Smooth blend between dry and wet

### MIDI Note Mapping

- **Middle C (C4)** = MIDI note 60 = root pitch of sample
- **Higher notes** = sample plays at higher pitch
- **Lower notes** = sample plays at lower pitch
- Pitch shifting is calculated automatically

### Octave Ranges

- **Octave 0**: C0-B0 (MIDI 12-23) - Very low
- **Octave 4**: C4-B4 (MIDI 60-71) - Middle (default)
- **Octave 8**: C8-B8 (MIDI 108-119) - Very high

## Troubleshooting

### Build Issues

**"JUCE not found" error:**
```bash
# Verify JUCE location
ls ../JUCE
# Should show JUCE files and folders
```

**CMake version error:**
```bash
brew upgrade cmake
```

**Xcode errors:**
```bash
# Reset Xcode command line tools
sudo xcode-select --reset
xcode-select --install
```

### Plugin Issues

**Plugin not appearing in DAW:**

1. **Rescan plugins** in your DAW:
   - Logic Pro: Preferences > Plug-in Manager > Reset & Rescan
   - Ableton Live: Preferences > Plug-ins > Rescan
   - Reaper: Preferences > Plugins > VST > Re-scan

2. **Check installation paths:**
   ```bash
   ls ~/Library/Audio/Plug-Ins/VST3/SimpleSampler.vst3
   ls ~/Library/Audio/Plug-Ins/Components/SimpleSampler.component
   ```

3. **AU validation failed:**
   ```bash
   # Run validation again with verbose output
   auval -v aumu Ssmp Ynam
   ```

**No sound when clicking keys:**

1. **Verify sample is loaded** - check filename label
2. **Check volume slider** - ensure it's not at 0%
3. **Check DAW track** - ensure track is armed/enabled
4. **Check MIDI routing** in your DAW

**Sample won't load:**

1. **Verify file format** - must be WAV file
2. **Check file permissions** - ensure file is readable
3. **Try a different WAV file** - test with a known-good sample

**Stuck notes:**

- Click the stuck key again to release
- Or restart the plugin instance

### Performance Issues

**High CPU usage:**
- Reduce reverb amount (reverb is CPU-intensive)
- Use shorter samples
- Reduce number of simultaneous notes

**Audio crackling:**
- Increase audio buffer size in your DAW settings
- Close other applications using audio

## Testing Checklist

- [x] Plugin loads in DAW without errors
- [x] File browser opens when clicking "Load Sample"
- [x] WAV file loads successfully
- [x] Filename displays correctly
- [x] Virtual keyboard keys trigger sample playback
- [x] Octave buttons change pitch range
- [x] Current octave displays correctly (0-8)
- [x] External MIDI keyboard triggers sample
- [x] Volume slider controls output level
- [x] Reverb slider adds reverb effect
- [x] Keys turn green when pressed
- [x] Keys release properly (no stuck notes)
- [x] Multiple simultaneous notes work
- [x] Plugin state saves/loads in DAW project

## Customization

### Changing Plugin Name

Edit `CMakeLists.txt`:
```cmake
COMPANY_NAME "YourName"          # Line 16
PLUGIN_MANUFACTURER_CODE Ynam    # Line 17 (4 unique chars)
BUNDLE_ID com.yourname.SimpleSampler  # Line 34
```

### Adjusting Number of Voices

Edit `PluginProcessor.cpp`, line 44:
```cpp
for (int i = 0; i < 8; ++i)  // Change 8 to desired number
    synth.addVoice (new SamplerVoice());
```

### Changing Default Parameters

Edit `PluginProcessor.cpp`, lines 46-55:
```cpp
0.7f  // Default volume (0.0 - 1.0)
0.0f  // Default reverb (0.0 - 1.0)
```

### Modifying Keyboard Layout

Edit `PluginEditor.cpp`, `resized()` method (lines 160-220) to adjust:
- Key sizes
- Key positions
- Colors
- Layout

## Technical Details

### Architecture

- **Audio Engine**: JUCE Synthesiser with custom SamplerVoice
- **Sample Format**: WAV (mono/stereo), max 10 seconds
- **Sample Rate**: Matches host DAW sample rate
- **Polyphony**: 8 voices
- **MIDI**: Channel 1, notes 0-127
- **Latency**: Near-zero (synthesizer, not processor)
- **DSP**: JUCE dsp::Reverb module

### File Structure

```
SimpleSampler/
├── Source/
│   ├── PluginProcessor.h       # Audio engine header
│   ├── PluginProcessor.cpp     # Audio engine implementation
│   ├── PluginEditor.h          # UI header
│   └── PluginEditor.cpp        # UI implementation
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
└── build/                      # Generated build files
```

### Parameters

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| Volume | Float | 0.0 - 1.0 | 0.7 | Output gain |
| Reverb | Float | 0.0 - 1.0 | 0.0 | Reverb wet/dry mix |

### MIDI Implementation

- **Note On**: Triggers sample playback with pitch shifting
- **Note Off**: Stops note with release envelope (100ms)
- **Velocity**: Controls note volume
- **Pitch Bend**: Not implemented
- **CC**: Not implemented

## Future Enhancements

Possible improvements for learning:

- [ ] Multi-sample support (multiple WAV files)
- [ ] ADSR envelope controls
- [ ] Filter (low-pass, high-pass, band-pass)
- [ ] LFO for modulation
- [ ] Pitch bend support
- [ ] Sample preview in file browser
- [ ] Drag-and-drop sample loading
- [ ] Preset management
- [ ] Velocity layers
- [ ] Loop points
- [ ] Visual waveform display

## Resources

- **JUCE Documentation**: https://juce.com/learn/documentation
- **JUCE Forum**: https://forum.juce.com/
- **JUCE Tutorials**: https://juce.com/learn/tutorials
- **CMake Documentation**: https://cmake.org/documentation/

## License

This project uses the JUCE framework, which is licensed under GPL v3 or commercial license.
Your plugin code can use any license compatible with JUCE.

See LICENSE file for details.

## Credits

Built with:
- **JUCE Framework** by ROLI Ltd.
- **CMake** build system
- Developed as a learning project

## Support

For issues or questions:
1. Check the Troubleshooting section above
2. Review JUCE documentation
3. Ask on JUCE Forum
4. Check CMake configuration

---

**Version**: 1.0.0
**Last Updated**: November 2025
**Target Platforms**: macOS (AU, VST3), Windows (VST3), Linux (VST3)

Happy sampling! 🎹🎵
