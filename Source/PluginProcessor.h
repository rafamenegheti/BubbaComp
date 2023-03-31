/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SliderAttempAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SliderAttempAudioProcessor();
    ~SliderAttempAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

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

    float getRmsValue(const int channel) const;
    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================

    float rmsLevel;

    template<typename T>
    float computeRMSLevel(const T& buffer)
    {
        //int numChannels = static_cast<int>(buffer.getNumChannels());
        //int numSamples = static_cast<int>(buffer.getNumSamples());
        //auto rms = 0.f;
        //for (int chan = 0; chan < numChannels; ++chan)
        //{
        //    rms += buffer.getRMSLevel(chan, 0, numSamples);
        //}

        //rms /= static_cast<float>(numChannels);
        //return rms;

        int numChannels = static_cast<int>(buffer.getNumChannels());
        int numSamples = static_cast<int>(buffer.getNumSamples());
        auto maxLevel = 0.f;
        for (int chan = 0; chan < numChannels; ++chan)
        {
            auto* channelData = buffer.getReadPointer(chan);
            for (int i = 0; i < numSamples; ++i)
            {
                auto absLevel = std::abs(channelData[i]);
                if (absLevel > maxLevel)
                    maxLevel = absLevel;
            }
        }
        return maxLevel;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderAttempAudioProcessor)
};
