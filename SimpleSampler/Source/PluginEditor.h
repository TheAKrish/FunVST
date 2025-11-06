/*
  ==============================================================================

    SimpleSampler - WAV Sampler Plugin with Virtual Keyboard
    PluginEditor.h - User Interface

    Features:
    - 12-key virtual keyboard with piano layout
    - Octave switching (0-8)
    - Volume and Reverb controls
    - Sample loading with file browser

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * Custom Piano Key Button Component
 */
class PianoKeyButton : public juce::TextButton
{
public:
    PianoKeyButton (const juce::String& keyName, bool isBlackKey)
        : juce::TextButton (keyName), blackKey (isBlackKey)
    {
        setClickingTogglesState (false);
    }

    bool isBlackKey() const { return blackKey; }

    void setPressed (bool shouldBePressed)
    {
        isPressed = shouldBePressed;
        repaint();
    }

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Determine button color
        juce::Colour buttonColour;
        if (isPressed || shouldDrawButtonAsDown)
            buttonColour = juce::Colours::green;
        else if (blackKey)
            buttonColour = juce::Colours::black;
        else
            buttonColour = juce::Colours::white;

        // Draw button background
        g.setColour (buttonColour);
        g.fillRoundedRectangle (bounds, 4.0f);

        // Draw border
        g.setColour (blackKey ? juce::Colours::grey : juce::Colours::black);
        g.drawRoundedRectangle (bounds.reduced (1.0f), 4.0f, 1.5f);

        // Draw text
        g.setColour (blackKey ? juce::Colours::white : juce::Colours::black);
        if (isPressed || shouldDrawButtonAsDown)
            g.setColour (juce::Colours::white);

        g.setFont (12.0f);
        g.drawText (getButtonText(), bounds, juce::Justification::centred, true);
    }

private:
    bool blackKey;
    bool isPressed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoKeyButton)
};

//==============================================================================
/**
 * Main Plugin Editor - User Interface
 */
class SimpleSamplerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    SimpleSamplerAudioProcessorEditor (SimpleSamplerAudioProcessor&);
    ~SimpleSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Timer callback for updating UI
    void timerCallback() override;

    // Button click handlers
    void loadButtonClicked();
    void octaveUpClicked();
    void octaveDownClicked();
    void pianoKeyPressed (int noteOffset);
    void pianoKeyReleased (int noteOffset);

    // Helper method to calculate MIDI note from octave and offset
    int getMidiNote (int noteOffset) const;

    //==============================================================================
    // Reference to processor
    SimpleSamplerAudioProcessor& audioProcessor;

    // UI Components - Virtual Keyboard
    std::unique_ptr<PianoKeyButton> keyC, keyCSharp, keyD, keyDSharp, keyE;
    std::unique_ptr<PianoKeyButton> keyF, keyFSharp, keyG, keyGSharp, keyA, keyASharp, keyB;

    std::unique_ptr<juce::TextButton> octaveDownButton;
    std::unique_ptr<juce::TextButton> octaveUpButton;
    std::unique_ptr<juce::Label> octaveLabel;

    // UI Components - Controls
    std::unique_ptr<juce::Slider> volumeSlider;
    std::unique_ptr<juce::Slider> reverbSlider;
    std::unique_ptr<juce::Label> volumeLabel;
    std::unique_ptr<juce::Label> reverbLabel;

    std::unique_ptr<juce::TextButton> loadButton;
    std::unique_ptr<juce::Label> fileNameLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbAttachment;

    // State
    int currentOctave = 4;  // Middle octave (C4 = MIDI 60)
    std::array<int, 12> activeNotes; // Track which notes are currently playing

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSamplerAudioProcessorEditor)
};
