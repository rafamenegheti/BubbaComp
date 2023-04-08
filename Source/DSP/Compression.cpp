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
    reductionCalc.setRatio(ratioInDb);
}
// threshold
void Compression::setThreshold(float thresholdInDb)
{
    reductionCalc.setThreshold(thresholdInDb);
}
const float Compression::getThreshold() { return reductionCalc.getThreshold(); }
// attack
void Compression::setAttack(float attackInMs)
{
    reductionCalc.setAttackTime(attackInMs / 1000);
}
// release
void Compression::setRelease(float releaseInMs)
{
    reductionCalc.setReleaseTime(releaseInMs / 1000);
}
// knee
void Compression::setKnee(float kneeInDb)
{
    reductionCalc.setKnee(kneeInDb);
}
const float Compression::getKnee()
{
    return reductionCalc.getKnee();
}
// makeUpGain
void Compression::setMakeUpGain(const float makeUpGainInDecibels)
{
    reductionCalc.setMakeUpGain(makeUpGainInDecibels);
}
const float Compression::getMakeUpGain() { return reductionCalc.getMakeUpGain(); }
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

    /** STEP 1: compute sidechain-signal */
    // copy the absolute values from the first input channel to the sideChainBuffer
    juce::FloatVectorOperations::abs(sideChainBuffer.getWritePointer(0), outBlock.getChannelPointer(0), numSamples);

    // copy all other channels to the second channel of the sideChainBuffer and write the maximum of both channels to the first one
    for (int ch = 1; ch < nChannels; ++ch)
    {
        juce::FloatVectorOperations::abs(sideChainBuffer.getWritePointer(1), outBlock.getChannelPointer(ch), numSamples);
        juce::FloatVectorOperations::max(sideChainBuffer.getWritePointer(0), sideChainBuffer.getReadPointer(0), sideChainBuffer.getReadPointer(1), numSamples);
    }

    /** STEP 2: calculate gain reduction */
    reductionCalc.computeLinearGainFromSidechainSignal(sideChainBuffer.getReadPointer(0), sideChainBuffer.getWritePointer(1), numSamples);
    // gain-reduction is now in the second channel of our sideChainBuffer


    /** STEP 3: apply gain-reduction to all channels */
    for (int ch = 0; ch < nChannels; ++ch)
        juce::FloatVectorOperations::multiply(outBlock.getChannelPointer(ch), sideChainBuffer.getReadPointer(1), numSamples);
}




