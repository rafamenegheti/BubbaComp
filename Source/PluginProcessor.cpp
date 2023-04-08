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

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -36.f, 36.f, 0.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", -60.f, 20.f, -10.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.1f, 800.0f, 0.1f), 150.0f, "ms"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 30.0f, "ms"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", juce::NormalisableRange<float>(1.0f, 16.0f, 0.1f), 1.0f, " : 1",
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int maximumStringLength) { if (value > 15.9f) return juce::String("inf"); return juce::String(value, 2); }));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee", juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f), 0.0f, "dB"));

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
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    compressor.prepare(spec);
    gain.prepare(spec);
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

    attParams();

    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };

    inputRmsLevel = convertToDb(computePeakLevel(buffer));

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();

    // clear not needed output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    juce::dsp::AudioBlock<float> ab(buffer);
    juce::dsp::ProcessContextReplacing<float> context(ab);

    compressor.process(context);

    auto outputBeforeGainRmsLevel = convertToDb(computePeakLevel(buffer));
    gain.process(context);

    outputRmsLevel = convertToDb(computePeakLevel(buffer));
    gainReductionRmsLevel = inputRmsLevel - outputBeforeGainRmsLevel;

    DBG(inputRmsLevel);
    DBG(outputRmsLevel);
    DBG(gainReductionRmsLevel);
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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
}

float BubbaCompAudioProcessor::getRmsInputValue() const
{
    return inputRmsLevel;
}
float BubbaCompAudioProcessor::getRmsOutputValue() const
{
    return outputRmsLevel;
}
float BubbaCompAudioProcessor::getRmsGainReductionValue() const
{
    return gainReductionRmsLevel;
}

void BubbaCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BubbaCompAudioProcessor();
}

void BubbaCompAudioProcessor::attParams() {
    juce::RangedAudioParameter* thresholdValue = apvts.getParameter("threshold");
    float normalizedValue = thresholdValue->getValue();
    float actualValue = thresholdValue->convertFrom0to1(normalizedValue);

    compressor.setAttack(apvts.getRawParameterValue("attack")->load());
    compressor.setRelease(apvts.getRawParameterValue("release")->load());
    compressor.setThreshold(actualValue);

    compressor.setKnee(apvts.getRawParameterValue("knee")->load());

    auto ratio = apvts.getRawParameterValue("ratio")->load();

    if (ratio > 15.9f)
        compressor.setRatio(std::numeric_limits<float>::infinity());
    else
        compressor.setRatio(ratio);

    gain.setGainDecibels(apvts.getRawParameterValue("gain")->load());
}