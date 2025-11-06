/*
  ==============================================================================

    SimpleSampler - WAV Sampler Plugin with Virtual Keyboard
    PluginProcessor.h - Audio Processing Engine

    Features:
    - Single WAV file sample playback
    - MIDI triggered sample playback
    - Virtual keyboard MIDI injection
    - Volume control
    - Reverb effect

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Custom Sampler Sound - stores the audio sample data
 */
class SamplerSound : public juce::SynthesiserSound
{
public:
    SamplerSound (const juce::String& name,
                  juce::AudioFormatReader& source,
                  const juce::BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds)
        : sourceSampleRate (source.sampleRate),
          midiNotes (midiNotes),
          midiRootNote (midiNoteForNormalPitch)
    {
        if (sourceSampleRate > 0 && source.lengthInSamples > 0)
        {
            length = juce::jmin ((int) source.lengthInSamples,
                                (int) (maxSampleLengthSeconds * sourceSampleRate));

            data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

            source.read (data.get(), 0, length + 4, 0, true, true);

            params.attack  = static_cast<float> (attackTimeSecs);
            params.release = static_cast<float> (releaseTimeSecs);
        }
    }

    bool appliesToNote (int midiNoteNumber) override
    {
        return midiNotes[midiNoteNumber];
    }

    bool appliesToChannel (int /*midiChannel*/) override
    {
        return true;
    }

    juce::AudioBuffer<float>* getAudioData() const noexcept { return data.get(); }

    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int midiRootNote, length;

    std::unique_ptr<juce::AudioBuffer<float>> data;
    juce::ADSR::Parameters params;

private:
    JUCE_LEAK_DETECTOR (SamplerSound)
};

//==============================================================================
/**
 * Custom Sampler Voice - handles playback of the sample
 */
class SamplerVoice : public juce::SynthesiserVoice
{
public:
    SamplerVoice() {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<const SamplerSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* s, int /*currentPitchWheelPosition*/) override
    {
        if (auto* sound = dynamic_cast<const SamplerSound*> (s))
        {
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                        * sound->sourceSampleRate / getSampleRate();

            sourceSamplePosition = 0.0;
            lgain = velocity;
            rgain = velocity;

            adsr.setSampleRate (sound->sourceSampleRate);
            adsr.setParameters (sound->params);

            adsr.noteOn();
        }
        else
        {
            jassertfalse; // This should never happen!
        }
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
        }
    }

    void pitchWheelMoved (int /*newValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (auto* playingSound = static_cast<SamplerSound*> (getCurrentlyPlayingSound().get()))
        {
            auto& data = *playingSound->getAudioData();
            const float* const inL = data.getReadPointer (0);
            const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;

            float* outL = outputBuffer.getWritePointer (0, startSample);
            float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

            while (--numSamples >= 0)
            {
                auto pos = (int) sourceSamplePosition;
                auto alpha = (float) (sourceSamplePosition - pos);
                auto invAlpha = 1.0f - alpha;

                // Simple linear interpolation
                float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
                float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha) : l;

                auto envelopeValue = adsr.getNextSample();

                l *= lgain * envelopeValue;
                r *= rgain * envelopeValue;

                if (outR != nullptr)
                {
                    *outL++ += l;
                    *outR++ += r;
                }
                else
                {
                    *outL++ += (l + r) * 0.5f;
                }

                sourceSamplePosition += pitchRatio;

                if (sourceSamplePosition > playingSound->length)
                {
                    stopNote (0.0f, false);
                    break;
                }
            }
        }
    }

private:
    double pitchRatio = 0.0;
    double sourceSamplePosition = 0.0;
    float lgain = 0.0f, rgain = 0.0f;

    juce::ADSR adsr;

    JUCE_LEAK_DETECTOR (SamplerVoice)
};

//==============================================================================
/**
 * Main Audio Processor - handles all audio and MIDI processing
 */
class SimpleSamplerAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleSamplerAudioProcessor();
    ~SimpleSamplerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Sample loading
    bool loadSample (const juce::File& file);
    juce::String getLoadedFileName() const { return loadedFileName; }

    //==============================================================================
    // Virtual keyboard MIDI injection
    void addNoteOn (int midiNote, float velocity);
    void addNoteOff (int midiNote);

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }

private:
    //==============================================================================
    // Audio processing components
    juce::Synthesiser synth;
    juce::AudioFormatManager formatManager;

    // DSP processing
    juce::dsp::ProcessorDuplicator<juce::dsp::Reverb, juce::dsp::Reverb::Parameters> reverb;

    // Parameters
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* volumeParameter = nullptr;
    std::atomic<float>* reverbParameter = nullptr;

    // Virtual keyboard MIDI buffer
    juce::MidiBuffer virtualKeyboardMidi;
    juce::CriticalSection virtualKeyboardLock;

    // Sample info
    juce::String loadedFileName;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSamplerAudioProcessor)
};
