/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


inline juce::AudioProcessorValueTreeState::ParameterLayout createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    auto attackReleaseRange = juce::NormalisableRange<float>(1.f, 1000.f, 1, 1);

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -60.f, 20.f, 6.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", -60.f, 20.f, -10.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", attackReleaseRange, 50));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", attackReleaseRange, 50));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee", -60.f, 20.f, -10.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", -60.f, 20.f, -10.f));

    return { parameters.begin(), parameters.end() };
}

//==============================================================================
BubbaCompAudioProcessor::BubbaCompAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "params", createParameters())
#endif
{
}

BubbaCompAudioProcessor::~BubbaCompAudioProcessor()
{
}

//==============================================================================
const juce::String BubbaCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BubbaCompAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BubbaCompAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BubbaCompAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BubbaCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BubbaCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BubbaCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BubbaCompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BubbaCompAudioProcessor::getProgramName (int index)
{
    return {};
}

void BubbaCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BubbaCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BubbaCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BubbaCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BubbaCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;


    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };

    rmsLevel = convertToDb(computeRMSLevel(buffer));
}

//==============================================================================
bool BubbaCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BubbaCompAudioProcessor::createEditor()
{
    return new BubbaCompAudioProcessorEditor (*this);
}

//==============================================================================
void BubbaCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

float BubbaCompAudioProcessor::getRmsValue(const int channel) const
{
    return rmsLevel;
}


void BubbaCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BubbaCompAudioProcessor();
}
