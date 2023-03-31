/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SliderAttempAudioProcessorEditor::SliderAttempAudioProcessorEditor(SliderAttempAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), gainSlider(tooltip, p.apvts.getParameter("gain")), metter(p.apvts.getParameter("threshold"))
{

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (650, 300);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(metter);

    startTimerHz(24);
}

SliderAttempAudioProcessorEditor::~SliderAttempAudioProcessorEditor()
{
}

void SliderAttempAudioProcessorEditor::timerCallback() {
    metter.setLevel(audioProcessor.getRmsValue(0));
    metter.repaint();
}

//==============================================================================
void SliderAttempAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.fillAll(juce::Colour(34, 34, 59));

    gainSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2).reduced(50));
    metter.setBounds(bounds.removeFromRight(bounds.getWidth() / 2).reduced(50));
}

void SliderAttempAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
