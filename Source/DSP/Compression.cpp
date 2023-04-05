/*
  ==============================================================================

    Compression.cpp
    Created: 12 Mar 2023 6:11:49pm
    Author:  rafae

  ==============================================================================
*/

#include "Compression.h"


// ratio
void Compression::setRatio(float ratioInDb)
{
    ReductionCalc.setRatio(ratioInDb);
}
// threshold
void Compression::setThreshold(float thresholdInDb)
{
    ReductionCalc.setThreshold(thresholdInDb);
}
const float Compression::getThreshold() { return ReductionCalc.getThreshold(); }
// attack
void Compression::serAttack(float attackInMs)
{
    ReductionCalc.setAttackTime(attackInMs / 1000);
}
// release
void Compression::setRelease(float releaseInMs)
{
    ReductionCalc.setReleaseTime(releaseInMs / 1000);
}
// knee
void Compression::setKnee(float kneeInDb)
{
    ReductionCalc.setKnee(kneeInDb);
}
const float Compression::getKnee()
{
    return ReductionCalc.getKnee();
}
// makeUpGain
void Compression::setMakeUpGain(const float makeUpGainInDecibels)
{
    ReductionCalc.setMakeUpGain(makeUpGainInDecibels);
}
const float Compression::getMakeUpGain() { return ReductionCalc.getMakeUpGain(); }
// bypassed
void Compression::setBypassed(bool isBypassed)
{
    bypassed = isBypassed;
}


void Compression::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto outBlock = context.getOutputBlock();
    const int nChannels = static_cast<int> (outBlock.getNumChannels());
    const int numSamples = static_cast<int> (outBlock.getNumSamples());


    for (int ch = 0; ch < nChannels; ++ch) {
        juce::FloatVectorOperations::abs(sideChainBuffer.getWritePointer(ch), outBlock.getChannelPointer(ch), numSamples);
        ReductionCalc.computeLinearGainFromSidechainSignal(sideChainBuffer.getReadPointer(ch), sideChainBuffer.getWritePointer(ch), numSamples);
        juce::FloatVectorOperations::multiply(outBlock.getChannelPointer(ch), sideChainBuffer.getReadPointer(ch), numSamples);
    }
}




