/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SuperSlider.h"
#include "GainReduceMetter.h"

//==============================================================================
/**
*/

class SliderAttempAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SliderAttempAudioProcessorEditor (SliderAttempAudioProcessor& p);
    ~SliderAttempAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SliderAttempAudioProcessor& audioProcessor;
    juce::String tooltip = "My Knob";

    Comp::Knob gainSlider;

    GainReduceMetter metter;

    juce::Rectangle<int> left, rigth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderAttempAudioProcessorEditor)
};



