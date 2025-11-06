# SimpleSampler - Quick Start Guide

Get up and running in 5 minutes!

## For macOS Users

### Prerequisites Check (2 minutes)

Open Terminal and run these commands:

```bash
# Check Xcode Command Line Tools
xcode-select -p
# If error, install: xcode-select --install

# Check CMake
cmake --version
# If not installed: brew install cmake
```

### Build the Plugin (2 minutes)

```bash
# Navigate to project
cd /path/to/FunVST/SimpleSampler

# Run build script
./build.sh
```

That's it! The plugin will build and install automatically.

### Use the Plugin (1 minute)

1. **Open your DAW** (Logic Pro, Ableton, Reaper, GarageBand)
2. **Rescan plugins** if needed
3. **Create instrument track** and load "SimpleSampler"
4. **Click "Load Sample"** and choose a WAV file
5. **Play the virtual keyboard** with your mouse!

## Virtual Keyboard Controls

### Piano Keys
- **White keys**: C, D, E, F, G, A, B
- **Black keys**: C#, D#, F#, G#, A#
- Click to play, release to stop

### Octave Controls
- **< button**: Decrease octave (minimum: 0)
- **> button**: Increase octave (maximum: 8)
- **Default**: Octave 4 (Middle C = C4)

### Effect Controls
- **Volume slider**: Adjust output level
- **Reverb slider**: Add reverb effect (0% = dry, 100% = wet)

## Tips

- **Green keys** = Currently pressed
- **Any MIDI keyboard** also works with the plugin
- **Default octave 4** means middle C on the virtual keyboard = C4 (MIDI 60)
- **Change octaves** to access different pitch ranges

## Troubleshooting

**Plugin not showing in DAW?**
- Rescan plugins in your DAW preferences
- Check: `~/Library/Audio/Plug-Ins/VST3/` or `~/Library/Audio/Plug-Ins/Components/`

**No sound?**
- Make sure a sample is loaded (check filename label)
- Check volume slider is not at 0%
- Verify DAW track is enabled

**Build failed?**
- Ensure you're in the SimpleSampler directory
- Check JUCE is in parent directory (`../JUCE`)
- Verify CMake is installed: `brew install cmake`

## Sample WAV Files

Need test samples? Try these free sources:
- **Freesound.org** - Free sound effects and samples
- **GarageBand** - Comes with sample files on macOS
- **Your own recordings** - Record anything and save as WAV!

---

For detailed information, see the full **README.md**

Happy sampling! 🎹
