/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./GUI/SuperSlider.h"
#include "./GUI/GainReduceMetter.h"

//==============================================================================
/**
*/



struct Top : public juce::Component {

    Top() {

    };

    void paint(juce::Graphics& g) override {

    };

    void resized() override {
    };

};

struct RotaryParamsContainer : public juce::Component {

    RotaryParamsContainer(BubbaCompAudioProcessor& p) : 
        releaseSlider(tooltip, p.apvts.getParameter("release")),
        ratioSlider(tooltip, p.apvts.getParameter("ratio")),
        attackSlider(tooltip, p.apvts.getParameter("attack")),
        kneeSlider(tooltip, p.apvts.getParameter("knee")),
        gainSlider(tooltip, p.apvts.getParameter("gain"))
    {
        addAndMakeVisible(releaseSlider);
        addAndMakeVisible(ratioSlider);
        addAndMakeVisible(attackSlider);
        addAndMakeVisible(kneeSlider);
        addAndMakeVisible(gainSlider);
    };
    juce::String tooltip = "My Knob";


    void resized() override {
        auto bounds = getLocalBounds().reduced(5);
        auto boundsHeight = bounds.getHeight();
        auto topBounds = bounds.removeFromTop(boundsHeight / 2);
        auto bottomBounds = bounds.removeFromBottom(boundsHeight / 2);

        using namespace juce;

        FlexBox topFlexBox;
        topFlexBox.flexDirection = FlexBox::Direction::row;
        topFlexBox.flexWrap = FlexBox::Wrap::noWrap;


        topFlexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
        topFlexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
        topFlexBox.items.add(FlexItem(attackSlider).withFlex(1.f));

        topFlexBox.performLayout(topBounds);

        FlexBox bottomFlexBox;
        bottomFlexBox.flexDirection = FlexBox::Direction::row;
        bottomFlexBox.flexWrap = FlexBox::Wrap::noWrap;

        bottomFlexBox.items.add(FlexItem(kneeSlider).withFlex(1.f));
        bottomFlexBox.items.add(FlexItem(gainSlider).withFlex(1.f));

        bottomFlexBox.performLayout(bottomBounds.withTrimmedLeft(bottomBounds.getWidth() / 6).withTrimmedRight(bottomBounds.getWidth() / 6));
    };

    Comp::Knob releaseSlider, ratioSlider, attackSlider, kneeSlider, gainSlider;
};



struct Mid : public juce::Component, public juce::Timer {

    Mid(BubbaCompAudioProcessor& audioProcessor) :
        rotaryParamsContainer(audioProcessor),
        metter(audioProcessor.apvts.getParameter("threshold")),
        processor(audioProcessor)
    {
        addAndMakeVisible(SliderContainer);
        addAndMakeVisible(rotaryParamsContainer);
        addAndMakeVisible(metter);

        startTimerHz(36);
    };


    void paint(juce::Graphics& g) override {

    };

    void resized() override {
        auto bounds = getLocalBounds();
        auto boundsWidth = bounds.getWidth();

        rotaryParamsContainer.setBounds(bounds.removeFromLeft((boundsWidth / 4) * 3));
        metter.setBounds(bounds.removeFromRight(boundsWidth / 4));
    };

    void timerCallback() override {
        metter.setLevel(processor.getRmsValue(0));
        metter.repaint();
    };

    BubbaCompAudioProcessor& processor;

    juce::Component SliderContainer;
    RotaryParamsContainer rotaryParamsContainer;
    GainReduceMetter metter;
};


struct Bottom : public juce::Component {

    Bottom() {};

    void paint(juce::Graphics& g) override {

    };

    void resized() override {
    };
};


class BubbaCompAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    BubbaCompAudioProcessorEditor (BubbaCompAudioProcessor& p);
    ~BubbaCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BubbaCompAudioProcessor& audioProcessor;
    juce::String tooltip = "My Knob";

    Top top;
    Mid mid; 
    Bottom bottom;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BubbaCompAudioProcessorEditor)
};



