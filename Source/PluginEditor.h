/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./GUI/SuperSlider.h"
#include "./GUI/ThresholdSlider.h"
#include "./GUI/GainMeter.h"

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
        thresholdSlider(audioProcessor.apvts.getParameter("threshold")),
        processor(audioProcessor)
    {
        addAndMakeVisible(SliderContainer);
        addAndMakeVisible(rotaryParamsContainer);
        addAndMakeVisible(thresholdSlider);

        startTimerHz(36);
    };


    void paint(juce::Graphics& g) override {

    };

    void resized() override {
        auto bounds = getLocalBounds();
        auto boundsWidth = bounds.getWidth();

        rotaryParamsContainer.setBounds(bounds.removeFromLeft((boundsWidth / 4) * 3));
        thresholdSlider.setBounds(bounds.removeFromRight(boundsWidth / 4));
    };

    void timerCallback() override {
        thresholdSlider.setLevel(processor.getRmsInputValue());
        thresholdSlider.repaint();
    };

    BubbaCompAudioProcessor& processor;

    juce::Component SliderContainer;
    RotaryParamsContainer rotaryParamsContainer;
    ThresholdSlider thresholdSlider;
};


struct Bottom : public juce::Component, public juce::Timer {

    Bottom(BubbaCompAudioProcessor& audioProcessor) :
        outputMeter("output", 24.f, -60.f),
        gainReduction("Gain Reduction", 80.f, 0.f),
        processor(audioProcessor)
    {
        addAndMakeVisible(outputMeter);
        addAndMakeVisible(gainReduction);

        startTimerHz(36);
    };


    GainMeter outputMeter, gainReduction;
    BubbaCompAudioProcessor& processor;

    void paint(juce::Graphics& g) override {
    };

    void resized() override {
        auto bounds = getLocalBounds().withTrimmedBottom(10);

        auto left = bounds.removeFromRight(bounds.getWidth() / 2).withTrimmedRight(40);


        using namespace juce;

        FlexBox leftFlexBox;
        leftFlexBox.flexDirection = FlexBox::Direction::column;
        leftFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexEnd;  
        leftFlexBox.alignContent = juce::FlexBox::AlignContent::center;

        leftFlexBox.items.add(FlexItem(outputMeter).withWidth(left.getWidth()).withHeight(40));
        leftFlexBox.items.add(FlexItem().withFlex(0.f).withHeight(5));
        leftFlexBox.items.add(FlexItem(gainReduction).withWidth(left.getWidth()).withHeight(40));

        leftFlexBox.performLayout(left);
    };


    void timerCallback() override {
        outputMeter.setLevel(processor.getRmsOutputValue());
        gainReduction.setLevel(processor.getRmsGainReductionValue());
        outputMeter.repaint();
        gainReduction.repaint();

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



