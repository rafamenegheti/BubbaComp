/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SliderAttempAudioProcessorEditor::SliderAttempAudioProcessorEditor (SliderAttempAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), gainSlider(tooltip, p.apvts.getParameter("gain"))
{

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (650, 300);
    addAndMakeVisible(gainSlider);
}

SliderAttempAudioProcessorEditor::~SliderAttempAudioProcessorEditor()
{
}

//==============================================================================
void SliderAttempAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    gainSlider.setBounds(bounds);
}

void SliderAttempAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
