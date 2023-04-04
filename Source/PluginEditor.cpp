/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BubbaCompAudioProcessorEditor::BubbaCompAudioProcessorEditor(BubbaCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), gainSlider(tooltip, p.apvts.getParameter("gain")), metter(p.apvts.getParameter("threshold")), mid(p)
{

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(750, 450);
    setResizable(true, true);

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(metter);

    addAndMakeVisible(top);
    addAndMakeVisible(mid);
    addAndMakeVisible(bottom);

    startTimerHz(36);
}

BubbaCompAudioProcessorEditor::~BubbaCompAudioProcessorEditor()
{
}

void BubbaCompAudioProcessorEditor::timerCallback() {
    metter.setLevel(audioProcessor.getRmsValue(0));
    metter.repaint();
}

//==============================================================================
void BubbaCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    //auto bounds = getLocalBounds();
    //
    //g.fillAll(juce::Colour(34, 34, 59));

    //gainSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2).reduced(50));
    //metter.setBounds(bounds.removeFromRight(bounds.getWidth() / 2).reduced(50));
}

void BubbaCompAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto boundsHeight = bounds.getHeight();
    

    top.setBounds(bounds.removeFromTop(juce::jmax(boundsHeight / 12, 40)));
    bottom.setBounds(bounds.removeFromBottom(juce::jmax(boundsHeight / 4, 80)));
    mid.setBounds(bounds);
}
