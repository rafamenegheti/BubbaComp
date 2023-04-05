/*
  ==============================================================================

    Compression.h
    Created: 12 Mar 2023 6:11:34pm
    Author:  rafae

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "./ReductionCalc.h"
#include <juce_dsp/juce_dsp.h>


class Compression : private juce::dsp::ProcessorBase
{

public:
    void setRatio(float ratioInDb);

    void setThreshold(float thresholdInDb);
    const float getThreshold();

    void serAttack(float attackInMs);

    void setRelease(float releaseInMs);

    void setKnee(float kneeInDb);
    const float getKnee();

    void setMakeUpGain(const float makeUpGainInDb);
    const float getMakeUpGain();

    void setBypassed(bool isBypassed);

    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    void reset() override {};

    ReductionCalc& getReductionCalc() { return ReductionCalc; };

    void prepare(const juce::dsp::ProcessSpec& specs) override
    {
        ReductionCalc.prepare(specs.sampleRate);
        sideChainBuffer.setSize(2, specs.maximumBlockSize);
    };

private:
    // parameters
    bool bypassed{ false };

    ReductionCalc ReductionCalc;
    juce::AudioBuffer<float> sideChainBuffer;
};