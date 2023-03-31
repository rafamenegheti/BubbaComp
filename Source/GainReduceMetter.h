/*
  ==============================================================================

    GainReduceMetter.h
    Created: 28 Mar 2023 8:51:36pm
    Author:  rafae

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "PluginProcessor.h"


struct GainReduceMetter : public juce::Component {

    GainReduceMetter(juce::RangedAudioParameter* _rap) :
        rap(*_rap),
        attach(rap, [this](float) { repaint(); }, nullptr)
    {

    }

public: 
    void setLevel(const float value) {
        level = value;
    }

protected:

    float level = -60.f;

    int metterWidth = 20;
    int thicknes = 1;
    int textContainerSpace = 20;

    juce::RangedAudioParameter& rap;
    juce::ParameterAttachment attach;

    std::string formatFloat(float f)
    {
        if (f == -60.f) return "";
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << f;
        return stream.str() + "  dB";
    }

    void paint(juce::Graphics& g) override {
        // define colors
        auto green1 = juce::Colour(87, 204, 153);
        auto green2 = juce::Colour(56, 163, 165);
        auto green3 = juce::Colour(199, 249, 204);

        // define font
        auto font = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::Symtext_ttf, BinaryData::Symtext_ttfSize));
        g.setFont(font);

        auto bounds = getLocalBounds();

        // define full componente bounds
        juce::Rectangle<float> compBounds(bounds.getCentreX() - (metterWidth / 2), bounds.getY(), metterWidth, bounds.getHeight());

        // define meter bounds
        auto metterBounds = compBounds.withTrimmedTop(textContainerSpace).withTrimmedBottom(textContainerSpace).removeFromRight(bounds.getWidth() / 2);
        // draw meter bounds
        g.setColour(juce::Colours::azure);
        g.drawRoundedRectangle(metterBounds, 3.f, 1.f);

        // calculate heights
        const auto fillHeight = metterBounds.getHeight();
        const auto sla = metterBounds.reduced(1).getHeight();
        const float clippedLevel = juce::jlimit(-60.f, 20.f, level);
        const auto scaledLevel = juce::jmap(clippedLevel, -60.f, 20.f, fillHeight, 0.f);
        const auto zerodBHeight = juce::jmap(0.f, -60.f, 20.f, sla, 0.f);

        // define fill bounds
        juce::Rectangle<float> fillBounds(
            metterBounds
            .removeFromBottom(metterBounds.getHeight() * (1.f - scaledLevel / fillHeight))
            .reduced(1));


        // draw fill bar
        g.setColour(juce::Colour(green1));
        g.fillRoundedRectangle(fillBounds, 1.f);


        // define 0dB clip line
        juce::Line<float> clipLine(metterBounds.getX(),
            metterBounds.getY() + zerodBHeight,
            metterBounds.getX() + metterWidth,
            metterBounds.getY() + zerodBHeight);
        // draw 0dB clip line
        g.setColour(juce::Colours::red);
        g.drawLine(clipLine, 1);


        // draw text
        auto formatedValue = formatFloat(clippedLevel);
        g.setColour(juce::Colour(green1));
        g.setFont(16.f);
        g.drawFittedText("Output", bounds.withTrimmedBottom(bounds.getHeight() - textContainerSpace), juce::Justification::centred, 1);
        g.drawFittedText(formatedValue, bounds.withTrimmedTop(bounds.getHeight() - textContainerSpace), juce::Justification::centred, 1);
    }


    //void paint(juce::Graphics& g) override {

    //    auto bounds = getLocalBounds();

    //    auto lineHeight = bounds.getHeight() / 2; // half the height of the rectangle
    //    juce::Line<float> line(
    //        bounds.getX(), bounds.getY() + lineHeight, // start point at the left center of the rectangle
    //        bounds.getRight(), bounds.getY() + lineHeight // end point at the right center of the rectangle
    //    );

    //    g.drawLine(line);
    //}
};