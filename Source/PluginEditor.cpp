/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BubbaCompAudioProcessorEditor::BubbaCompAudioProcessorEditor(BubbaCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mid(p)
{

    setSize(750, 450);
    setResizable(true, true);
    
    addAndMakeVisible(top);
    addAndMakeVisible(mid);
    addAndMakeVisible(bottom);
}

BubbaCompAudioProcessorEditor::~BubbaCompAudioProcessorEditor()
{
}

//==============================================================================
void BubbaCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(34, 34, 59));
}

void BubbaCompAudioProcessorEditor::resized()
{

    auto bounds = getLocalBounds();
    auto boundsHeight = bounds.getHeight();

    top.setBounds(bounds.removeFromTop(juce::jmax(boundsHeight / 12, 40)));
    bottom.setBounds(bounds.removeFromBottom(juce::jmax(boundsHeight / 4, 80)));
    mid.setBounds(bounds);
}
