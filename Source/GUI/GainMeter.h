/*
  ==============================================================================

    GainReduceMetter.h
    Created: 28 Mar 2023 8:51:36pm
    Author:  rafae

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../helpers/misc.h"


struct GainMeter : public juce::Component, public juce::Timer {

    float level = -60.f;

    static constexpr int meterHeight = 20;
    static constexpr int thicknes = 1;
    static constexpr int textContainerSpace = 20;

    GainMeter(juce::String copy, float maxParamValue, float minParamValue) {
        this->copy = copy;
        this->maxParamValue = maxParamValue;
        this->minParamValue = minParamValue;

        startTimerHz(7);
    };

public:
    void setLevel(const float value) {
        level = value;
    }

protected:

    juce::String copy;

    float maxParamValue;
    float minParamValue;

    float displayLevel = 0;

    static std::string formatFloat(float f)
    {
        if (f == -60.f) return "";
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << f;
        return stream.str() + "  dB";
    };

    void timerCallback() override
    {
        // update the text value
        displayLevel = level;
    }

    void paint(juce::Graphics& g) override
    {

        // define font
        g.setFont(Misc::Shared::shared.font);

        auto bounds = getLocalBounds();

        // define full componente bounds
        juce::Rectangle<float> meterBounds(bounds.removeFromBottom(bounds.getHeight() / 2).toFloat());


        // draw meter bounds
        g.setColour(juce::Colours::azure);
        g.drawRoundedRectangle(meterBounds, 3.f, 1.f);
        g.drawFittedText(copy, bounds, juce::Justification::centredLeft, 1);

        const auto fillWidth = meterBounds.getWidth();
        const float clippedLevel = juce::jlimit(minParamValue, maxParamValue, level);
        const auto scaledLevel = juce::jmap(clippedLevel, minParamValue, maxParamValue, 0.f, fillWidth);
        const auto zerodB = juce::jmap(0.f, minParamValue, maxParamValue, 0.f, fillWidth);

        // define fill bounds
        juce::Rectangle<float> fillBounds(
            meterBounds
            .withWidth(scaledLevel)
            .reduced(2));

        // draw fill bar
        g.setColour(juce::Colour(Misc::Shared::shared.colors.green1));
        g.fillRoundedRectangle(fillBounds, 1.f);

        if(copy == "output") {
            // define 0dB clip line
            juce::Line<float> clipLine(meterBounds.getX() + zerodB,
            meterBounds.getY(),
            meterBounds.getX() + zerodB,
            meterBounds.getY() + meterBounds.getHeight());
            // draw 0dB clip line
            g.setColour(juce::Colours::red);
            g.drawLine(clipLine, 1);
        }


        // draw value
        auto formatedValue = formatFloat(displayLevel);
        g.setColour(juce::Colour(Misc::Shared::shared.colors.green1));
        g.setFont(16.f);
        g.drawText(formatedValue, bounds, juce::Justification::centredRight, 1);
    }
};