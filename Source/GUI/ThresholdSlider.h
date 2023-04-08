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


struct ThresholdSlider : public juce::Component {

    float level = -60.f;

    static constexpr int metterWidth = 20;
    static constexpr int thicknes = 1;
    static constexpr int textContainerSpace = 20;

    static constexpr float Sensitive = .2f;
    static constexpr float Wheel = 0.01f;

    inline std::function<void(ThresholdSlider&, juce::Graphics& g, float level, std::string(*formatFloat)(float))> makeOnPaint0()
    {
        return [](ThresholdSlider& k, juce::Graphics& g, float level, auto formatFloat)
        {
            // define font
            g.setFont(Misc::Shared::shared.font);

            auto bounds = k.getLocalBounds();

            // define full componente bounds
            juce::Rectangle<float> compBounds(bounds.getCentreX() - (metterWidth / 2), bounds.getY(), metterWidth, bounds.getHeight());

            // define meter bounds
            auto metterBounds = compBounds.withTrimmedTop(textContainerSpace).withTrimmedBottom(textContainerSpace).removeFromRight(bounds.getWidth() / 2);


            // draw meter bounds
            g.setColour(juce::Colours::azure);
            g.drawRoundedRectangle(metterBounds, 3.f, 1.f);

            // calculate heights
            auto paramValue = k.rap.convertFrom0to1(k.rap.getValue());
            auto maxParamValue = k.rap.getNormalisableRange().end;
            auto minParamValue = k.rap.getNormalisableRange().start;

            const auto fillHeight = metterBounds.getHeight();
            const float clippedLevel = juce::jlimit(minParamValue, maxParamValue, level);
            const auto scaledLevel = juce::jmap(clippedLevel, minParamValue, maxParamValue, fillHeight, 0.f);
            const auto zerodBHeight = juce::jmap(0.f, minParamValue, maxParamValue, fillHeight, 0.f);
            const auto paramValueHeight = juce::jmap(paramValue, minParamValue, maxParamValue, fillHeight, 0.f);


            // define fill bounds
            juce::Rectangle<float> fillBounds(
                metterBounds
                .removeFromBottom(metterBounds.getHeight() * (1.f - scaledLevel / fillHeight))
                .reduced(1));


            // draw fill bar
            g.setColour(juce::Colour(Misc::Shared::shared.colors.green1));
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
            auto name = k.rap.getName(20);
            auto paramValueText = k.rap.getCurrentValueAsText() + " dB";
            g.setColour(juce::Colour(Misc::Shared::shared.colors.green1));
            g.setFont(16.f);
            g.drawFittedText(name, bounds.withTrimmedBottom(bounds.getHeight() - textContainerSpace), juce::Justification::centred, 1);
            g.drawFittedText(paramValueText, bounds.withTrimmedTop(bounds.getHeight() - textContainerSpace), juce::Justification::centred, 1);


            g.setColour(Misc::Shared::shared.colors.green3);

            juce::Path trianglePath;
            trianglePath.addTriangle(
                (metterBounds.getX() + metterWidth) + 2,
                (metterBounds.getY()) + paramValueHeight,
                (metterBounds.getX() + metterWidth * 1.5),
                (metterBounds.getY() + 5) + paramValueHeight,
                (metterBounds.getX() + metterWidth * 1.5),
                (metterBounds.getY() - 5) + paramValueHeight
            );
            g.fillPath(trianglePath);
        };
    };

    ThresholdSlider(juce::RangedAudioParameter* _rap) :
        rap(*_rap),
        attach(rap, [this](float) { repaint(); }, nullptr),
        onPaint(makeOnPaint0()),
        dragY(0)
    {
        attach.sendInitialUpdate();
    }

public: 
    void setLevel(const float value) {
        level = value;
    }

protected:
    juce::RangedAudioParameter& rap;
    juce::ParameterAttachment attach;
    std::function<void(ThresholdSlider&, juce::Graphics& g, float level, std::string (*formatFloat)(float))> onPaint;
    float dragY;

    static std::string formatFloat(float f)
    {
        if (f == -60.f) return "";
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << f;
        return stream.str() + "  dB";
    };

    void paint(juce::Graphics& g) override
    {
        onPaint(*this, g, level, &formatFloat);
    }

    void mouseDown(const juce::MouseEvent& evt) override {
        attach.beginGesture();
        // Calculate new parameter value based on click position
        auto bounds = getLocalBounds();
        auto metterBounds = bounds.withTrimmedTop(textContainerSpace).withTrimmedBottom(textContainerSpace).removeFromRight(bounds.getWidth() / 2);
        auto fillHeight = metterBounds.getHeight();
        auto maxValue = rap.getNormalisableRange().end;
        auto minValue = rap.getNormalisableRange().start;
        auto newValue = juce::jmap<float>(juce::jlimit<float>(minValue, maxValue, juce::jmap<float>(fillHeight - evt.position.y + 18, 0.f, fillHeight, minValue, maxValue)), minValue, maxValue, 0.f, 1.f);

        // Set new parameter value
        rap.setValue(newValue);
    }

    void mouseDrag(const juce::MouseEvent& evt) override
    {
        setMouseCursor(juce::MouseCursor::StandardCursorType::NoCursor);
        auto bounds = getLocalBounds();
        auto metterBounds = bounds.withTrimmedTop(textContainerSpace).withTrimmedBottom(textContainerSpace).removeFromRight(bounds.getWidth() / 2);
        auto fillHeight = metterBounds.getHeight();
        auto maxValue = rap.getNormalisableRange().end;
        auto minValue = rap.getNormalisableRange().start;
        auto newValue = juce::jmap<float>(juce::jlimit<float>(minValue, maxValue, juce::jmap<float>(fillHeight - evt.position.y + 18, 0.f, fillHeight, minValue, maxValue)), minValue, maxValue, 0.f, 1.f);

        // Set new parameter value
        rap.setValue(newValue);
    }


    void mouseUp(const juce::MouseEvent& evt) override
    {
        setMouseCursor(juce::MouseCursor::StandardCursorType::PointingHandCursor);
        if (!evt.mouseWasDraggedSinceMouseDown() && evt.mods.isAltDown())
        {
            attach.setValueAsPartOfGesture(rap.convertFrom0to1(rap.getDefaultValue()));
        }
        attach.endGesture();
    }

    void mouseDoubleClick(const juce::MouseEvent& evt) override
    {
        attach.beginGesture();
        attach.setValueAsPartOfGesture(rap.convertFrom0to1(rap.getDefaultValue()));
        attach.endGesture();
    }

    void mouseWheelMove(const juce::MouseEvent& evt, const juce::MouseWheelDetails& wheel)
    {
        if (evt.mods.isAnyMouseButtonDown()) return;
        const auto direc = wheel.deltaY > 0.f ? 1.f : -1.f;
        const auto sens = evt.mods.isShiftDown() ? Sensitive : 1.f;
        auto val = juce::jlimit(0.f, 1.f, rap.getValue() + direc * Wheel * sens);
        attach.setValueAsCompleteGesture(rap.convertFrom0to1(val));
    }

    void mouseEnter(const juce::MouseEvent& evt) override
    {
        setMouseCursor(juce::MouseCursor::StandardCursorType::PointingHandCursor);
    }
};