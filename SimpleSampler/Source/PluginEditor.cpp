/*
  ==============================================================================

    SimpleSampler - WAV Sampler Plugin with Virtual Keyboard
    PluginEditor.cpp - User Interface Implementation

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSamplerAudioProcessorEditor::SimpleSamplerAudioProcessorEditor (SimpleSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Initialize active notes array
    activeNotes.fill (-1);

    // Create Virtual Keyboard - White Keys
    keyC = std::make_unique<PianoKeyButton> ("C", false);
    keyD = std::make_unique<PianoKeyButton> ("D", false);
    keyE = std::make_unique<PianoKeyButton> ("E", false);
    keyF = std::make_unique<PianoKeyButton> ("F", false);
    keyG = std::make_unique<PianoKeyButton> ("G", false);
    keyA = std::make_unique<PianoKeyButton> ("A", false);
    keyB = std::make_unique<PianoKeyButton> ("B", false);

    // Create Virtual Keyboard - Black Keys
    keyCSharp = std::make_unique<PianoKeyButton> ("C#", true);
    keyDSharp = std::make_unique<PianoKeyButton> ("D#", true);
    keyFSharp = std::make_unique<PianoKeyButton> ("F#", true);
    keyGSharp = std::make_unique<PianoKeyButton> ("G#", true);
    keyASharp = std::make_unique<PianoKeyButton> ("A#", true);

    // Set up white key callbacks
    keyC->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (0); };
    keyC->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (0); };
    keyD->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (2); };
    keyD->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (2); };
    keyE->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (4); };
    keyE->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (4); };
    keyF->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (5); };
    keyF->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (5); };
    keyG->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (7); };
    keyG->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (7); };
    keyA->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (9); };
    keyA->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (9); };
    keyB->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (11); };
    keyB->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (11); };

    // Set up black key callbacks
    keyCSharp->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (1); };
    keyCSharp->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (1); };
    keyDSharp->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (3); };
    keyDSharp->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (3); };
    keyFSharp->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (6); };
    keyFSharp->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (6); };
    keyGSharp->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (8); };
    keyGSharp->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (8); };
    keyASharp->onMouseDown = [this] (const juce::MouseEvent&) { pianoKeyPressed (10); };
    keyASharp->onMouseUp   = [this] (const juce::MouseEvent&) { pianoKeyReleased (10); };

    // Add white keys to editor
    addAndMakeVisible (keyC.get());
    addAndMakeVisible (keyD.get());
    addAndMakeVisible (keyE.get());
    addAndMakeVisible (keyF.get());
    addAndMakeVisible (keyG.get());
    addAndMakeVisible (keyA.get());
    addAndMakeVisible (keyB.get());

    // Add black keys to editor (must be added after white keys for proper Z-order)
    addAndMakeVisible (keyCSharp.get());
    addAndMakeVisible (keyDSharp.get());
    addAndMakeVisible (keyFSharp.get());
    addAndMakeVisible (keyGSharp.get());
    addAndMakeVisible (keyASharp.get());

    // Create Octave Controls
    octaveDownButton = std::make_unique<juce::TextButton> ("<");
    octaveUpButton = std::make_unique<juce::TextButton> (">");
    octaveLabel = std::make_unique<juce::Label> ("OctaveLabel", "Octave: 4");

    octaveDownButton->onClick = [this] { octaveDownClicked(); };
    octaveUpButton->onClick = [this] { octaveUpClicked(); };

    octaveLabel->setJustificationType (juce::Justification::centred);
    octaveLabel->setFont (juce::Font (14.0f, juce::Font::bold));

    addAndMakeVisible (octaveDownButton.get());
    addAndMakeVisible (octaveUpButton.get());
    addAndMakeVisible (octaveLabel.get());

    // Create Volume Slider and Label
    volumeSlider = std::make_unique<juce::Slider> (juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
    volumeSlider->setRange (0.0, 1.0, 0.01);
    volumeSlider->setValue (0.7);
    volumeLabel = std::make_unique<juce::Label> ("VolumeLabel", "Volume");
    volumeLabel->setJustificationType (juce::Justification::centred);
    volumeLabel->attachToComponent (volumeSlider.get(), false);

    addAndMakeVisible (volumeSlider.get());
    addAndMakeVisible (volumeLabel.get());

    // Create Reverb Slider and Label
    reverbSlider = std::make_unique<juce::Slider> (juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
    reverbSlider->setRange (0.0, 1.0, 0.01);
    reverbSlider->setValue (0.0);
    reverbLabel = std::make_unique<juce::Label> ("ReverbLabel", "Reverb");
    reverbLabel->setJustificationType (juce::Justification::centred);
    reverbLabel->attachToComponent (reverbSlider.get(), false);

    addAndMakeVisible (reverbSlider.get());
    addAndMakeVisible (reverbLabel.get());

    // Create Load Sample Button and File Name Label
    loadButton = std::make_unique<juce::TextButton> ("Load Sample");
    loadButton->onClick = [this] { loadButtonClicked(); };

    fileNameLabel = std::make_unique<juce::Label> ("FileNameLabel", "No sample loaded");
    fileNameLabel->setJustificationType (juce::Justification::centred);
    fileNameLabel->setColour (juce::Label::backgroundColourId, juce::Colours::darkgrey);
    fileNameLabel->setColour (juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible (loadButton.get());
    addAndMakeVisible (fileNameLabel.get());

    // Attach sliders to parameters
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.getValueTreeState(), "volume", *volumeSlider);

    reverbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.getValueTreeState(), "reverb", *reverbSlider);

    // Set window size
    setSize (600, 450);

    // Start timer for UI updates (60 Hz)
    startTimerHz (60);
}

SimpleSamplerAudioProcessorEditor::~SimpleSamplerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void SimpleSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (juce::Colours::darkslategrey);

    // Draw section separators
    g.setColour (juce::Colours::lightgrey);
    g.drawLine (10, 165, getWidth() - 10, 165, 2.0f);

    // Draw title
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (18.0f, juce::Font::bold));
    g.drawText ("SimpleSampler", getLocalBounds().removeFromTop (30), juce::Justification::centred, true);
}

void SimpleSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Title area
    area.removeFromTop (30);

    // Virtual Keyboard Section (top)
    auto keyboardArea = area.removeFromTop (140);
    keyboardArea.reduce (20, 10);

    // Octave controls at top of keyboard section
    auto octaveArea = keyboardArea.removeFromTop (30);
    octaveDownButton->setBounds (octaveArea.removeFromLeft (50).reduced (2));
    octaveUpButton->setBounds (octaveArea.removeFromLeft (50).reduced (2));
    octaveLabel->setBounds (octaveArea.removeFromLeft (100).reduced (2));

    keyboardArea.removeFromTop (5);

    // Piano key layout
    const int whiteKeyWidth = keyboardArea.getWidth() / 7;
    const int whiteKeyHeight = keyboardArea.getHeight();
    const int blackKeyWidth = whiteKeyWidth * 0.6f;
    const int blackKeyHeight = whiteKeyHeight * 0.6f;

    // Position white keys
    int xPos = keyboardArea.getX();
    keyC->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyD->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyE->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyF->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyG->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyA->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);
    xPos += whiteKeyWidth;
    keyB->setBounds (xPos, keyboardArea.getY(), whiteKeyWidth, whiteKeyHeight);

    // Position black keys (offset between white keys)
    xPos = keyboardArea.getX();
    keyCSharp->setBounds (xPos + whiteKeyWidth - blackKeyWidth/2, keyboardArea.getY(), blackKeyWidth, blackKeyHeight);
    xPos += whiteKeyWidth;
    keyDSharp->setBounds (xPos + whiteKeyWidth - blackKeyWidth/2, keyboardArea.getY(), blackKeyWidth, blackKeyHeight);
    xPos += whiteKeyWidth * 2;
    keyFSharp->setBounds (xPos + whiteKeyWidth - blackKeyWidth/2, keyboardArea.getY(), blackKeyWidth, blackKeyHeight);
    xPos += whiteKeyWidth;
    keyGSharp->setBounds (xPos + whiteKeyWidth - blackKeyWidth/2, keyboardArea.getY(), blackKeyWidth, blackKeyHeight);
    xPos += whiteKeyWidth;
    keyASharp->setBounds (xPos + whiteKeyWidth - blackKeyWidth/2, keyboardArea.getY(), blackKeyWidth, blackKeyHeight);

    // Separator space
    area.removeFromTop (15);

    // Sample Loading Section
    auto loadArea = area.removeFromTop (60);
    loadArea.reduce (20, 10);

    loadButton->setBounds (loadArea.removeFromTop (30));
    fileNameLabel->setBounds (loadArea);

    // Controls Section (bottom)
    auto controlsArea = area;
    controlsArea.reduce (40, 20);

    const int sliderWidth = 80;
    const int spacing = 40;

    auto volumeArea = controlsArea.removeFromLeft (sliderWidth);
    volumeArea.removeFromTop (20); // Space for label
    volumeSlider->setBounds (volumeArea);

    controlsArea.removeFromLeft (spacing);

    auto reverbArea = controlsArea.removeFromLeft (sliderWidth);
    reverbArea.removeFromTop (20); // Space for label
    reverbSlider->setBounds (reverbArea);
}

//==============================================================================
void SimpleSamplerAudioProcessorEditor::timerCallback()
{
    // Update file name label if sample was loaded
    auto fileName = audioProcessor.getLoadedFileName();
    if (fileName.isNotEmpty() && fileNameLabel->getText() != fileName)
    {
        fileNameLabel->setText (fileName, juce::dontSendNotification);
    }
}

//==============================================================================
void SimpleSamplerAudioProcessorEditor::loadButtonClicked()
{
    // Create file chooser for WAV files
    auto fileChooser = std::make_shared<juce::FileChooser> (
        "Select a WAV file to load...",
        juce::File::getSpecialLocation (juce::File::userHomeDirectory),
        "*.wav");

    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync (chooserFlags, [this, fileChooser] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != juce::File{})
        {
            if (audioProcessor.loadSample (file))
            {
                fileNameLabel->setText (file.getFileName(), juce::dontSendNotification);
            }
            else
            {
                fileNameLabel->setText ("Error loading file!", juce::dontSendNotification);
            }
        }
    });
}

void SimpleSamplerAudioProcessorEditor::octaveUpClicked()
{
    if (currentOctave < 8)
    {
        currentOctave++;
        octaveLabel->setText ("Octave: " + juce::String (currentOctave), juce::dontSendNotification);
    }
}

void SimpleSamplerAudioProcessorEditor::octaveDownClicked()
{
    if (currentOctave > 0)
    {
        currentOctave--;
        octaveLabel->setText ("Octave: " + juce::String (currentOctave), juce::dontSendNotification);
    }
}

void SimpleSamplerAudioProcessorEditor::pianoKeyPressed (int noteOffset)
{
    int midiNote = getMidiNote (noteOffset);

    // Send note on to processor
    audioProcessor.addNoteOn (midiNote, 0.8f);

    // Track active note
    activeNotes[noteOffset] = midiNote;

    // Update visual feedback
    switch (noteOffset)
    {
        case 0:  keyC->setPressed (true); break;
        case 1:  keyCSharp->setPressed (true); break;
        case 2:  keyD->setPressed (true); break;
        case 3:  keyDSharp->setPressed (true); break;
        case 4:  keyE->setPressed (true); break;
        case 5:  keyF->setPressed (true); break;
        case 6:  keyFSharp->setPressed (true); break;
        case 7:  keyG->setPressed (true); break;
        case 8:  keyGSharp->setPressed (true); break;
        case 9:  keyA->setPressed (true); break;
        case 10: keyASharp->setPressed (true); break;
        case 11: keyB->setPressed (true); break;
    }
}

void SimpleSamplerAudioProcessorEditor::pianoKeyReleased (int noteOffset)
{
    int midiNote = activeNotes[noteOffset];

    if (midiNote >= 0)
    {
        // Send note off to processor
        audioProcessor.addNoteOff (midiNote);

        // Clear active note
        activeNotes[noteOffset] = -1;
    }

    // Update visual feedback
    switch (noteOffset)
    {
        case 0:  keyC->setPressed (false); break;
        case 1:  keyCSharp->setPressed (false); break;
        case 2:  keyD->setPressed (false); break;
        case 3:  keyDSharp->setPressed (false); break;
        case 4:  keyE->setPressed (false); break;
        case 5:  keyF->setPressed (false); break;
        case 6:  keyFSharp->setPressed (false); break;
        case 7:  keyG->setPressed (false); break;
        case 8:  keyGSharp->setPressed (false); break;
        case 9:  keyA->setPressed (false); break;
        case 10: keyASharp->setPressed (false); break;
        case 11: keyB->setPressed (false); break;
    }
}

int SimpleSamplerAudioProcessorEditor::getMidiNote (int noteOffset) const
{
    // MIDI note calculation: C0 = 12, so C4 (middle C) = 60
    // Formula: 12 + (octave * 12) + noteOffset
    return 12 + (currentOctave * 12) + noteOffset;
}
