/*
  ==============================================================================

    SimpleSampler - WAV Sampler Plugin with Virtual Keyboard
    PluginProcessor.cpp - Audio Processing Implementation

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSamplerAudioProcessor::SimpleSamplerAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, juce::Identifier ("SimpleSampler"),
                  {
                      std::make_unique<juce::AudioParameterFloat> ("volume",
                                                                    "Volume",
                                                                    juce::NormalisableRange<float> (0.0f, 1.0f),
                                                                    0.7f),
                      std::make_unique<juce::AudioParameterFloat> ("reverb",
                                                                    "Reverb",
                                                                    juce::NormalisableRange<float> (0.0f, 1.0f),
                                                                    0.0f)
                  })
{
    // Initialize synthesiser with 8 voices
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SamplerVoice());

    // Register audio formats (WAV, AIFF, etc.)
    formatManager.registerBasicFormats();

    // Get parameter pointers for efficient access
    volumeParameter = parameters.getRawParameterValue ("volume");
    reverbParameter = parameters.getRawParameterValue ("reverb");
}

SimpleSamplerAudioProcessor::~SimpleSamplerAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleSamplerAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SimpleSamplerAudioProcessor::producesMidi() const
{
    return false;
}

bool SimpleSamplerAudioProcessor::isMidiEffect() const
{
    return false;
}

double SimpleSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleSamplerAudioProcessor::getNumPrograms()
{
    return 1;
}

int SimpleSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleSamplerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SimpleSamplerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SimpleSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void SimpleSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize synthesiser
    synth.setCurrentPlaybackSampleRate (sampleRate);

    // Initialize reverb
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    reverb.prepare (spec);
}

void SimpleSamplerAudioProcessor::releaseResources()
{
    // Release any resources that were allocated in prepareToPlay()
}

bool SimpleSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only supports mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void SimpleSamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Merge virtual keyboard MIDI with incoming MIDI
    {
        juce::ScopedLock lock (virtualKeyboardLock);
        if (!virtualKeyboardMidi.isEmpty())
        {
            midiMessages.addEvents (virtualKeyboardMidi, 0, buffer.getNumSamples(), 0);
            virtualKeyboardMidi.clear();
        }
    }

    // Render synthesiser audio
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // Apply volume control
    float volume = volumeParameter->load();
    buffer.applyGain (volume);

    // Apply reverb effect
    float reverbMix = reverbParameter->load();

    if (reverbMix > 0.01f) // Only process if reverb is actually being used
    {
        // Set reverb parameters
        juce::Reverb::Parameters reverbParams;
        reverbParams.roomSize   = 0.5f;
        reverbParams.damping    = 0.5f;
        reverbParams.wetLevel   = reverbMix;
        reverbParams.dryLevel   = 1.0f - reverbMix;
        reverbParams.width      = 1.0f;
        reverbParams.freezeMode = 0.0f;

        reverb.state->setParameters (reverbParams);

        // Process reverb
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        reverb.process (context);
    }
}

//==============================================================================
bool SimpleSamplerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SimpleSamplerAudioProcessor::createEditor()
{
    return new SimpleSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save parameters to memory block
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SimpleSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters from memory block
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// Sample loading
bool SimpleSamplerAudioProcessor::loadSample (const juce::File& file)
{
    // Check if file exists
    if (!file.existsAsFile())
        return false;

    // Try to create a reader for this file
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));

    if (reader.get() != nullptr)
    {
        // Clear existing sounds
        synth.clearSounds();

        // Create a BigInteger to represent which MIDI notes should trigger this sound
        // Setting all bits to 1 means any MIDI note will trigger the sample
        juce::BigInteger allNotes;
        allNotes.setRange (0, 128, true);

        // Add the new sample sound
        // Parameters: name, reader, notes, root note (C4 = 60), attack, release, max length
        synth.addSound (new SamplerSound ("Sample",
                                          *reader,
                                          allNotes,
                                          60,           // Middle C as root note
                                          0.01,         // 10ms attack
                                          0.1,          // 100ms release
                                          10.0));       // Max 10 seconds

        // Store the filename
        loadedFileName = file.getFileName();

        return true;
    }

    return false;
}

//==============================================================================
// Virtual keyboard MIDI injection
void SimpleSamplerAudioProcessor::addNoteOn (int midiNote, float velocity)
{
    juce::ScopedLock lock (virtualKeyboardLock);

    // Add note on message to virtual keyboard buffer
    auto message = juce::MidiMessage::noteOn (1, midiNote, velocity);
    virtualKeyboardMidi.addEvent (message, 0);
}

void SimpleSamplerAudioProcessor::addNoteOff (int midiNote)
{
    juce::ScopedLock lock (virtualKeyboardLock);

    // Add note off message to virtual keyboard buffer
    auto message = juce::MidiMessage::noteOff (1, midiNote);
    virtualKeyboardMidi.addEvent (message, 0);
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleSamplerAudioProcessor();
}
