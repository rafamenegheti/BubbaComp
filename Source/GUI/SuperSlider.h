#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../helpers/misc.h"

//==============================================================================
/**
*/

namespace Comp
{
    static constexpr float Tau = 6.28318530718f;

    struct Comp :
        public juce::Component
    {
        Comp(juce::String&& tooltp = "") :
            juce::Component(),
            tooltip(tooltp)
        {
            setBufferedToImage(true);
        }
    protected:
        juce::String tooltip;

        void paint(juce::Graphics& g) override
        {
            const float t = 2.5f;
            const auto col = juce::Colour::fromRGBA(56, 163, 165, 255);
            g.setColour(col);
            g.drawRoundedRectangle(getLocalBounds().toFloat(), t, t);
        }
    };

    struct Label :
        public Comp
    {
        Label(juce::String txt) :
            Comp(),
            text(txt)
        {};
        juce::String text;
        juce::Font font;
    protected:

        void paint(juce::Graphics& g) override
        {
            g.setFont(font);
            auto bounds = getLocalBounds().toFloat().reduced(2.f);
            g.setColour(juce::Colours::limegreen);
            auto strWidth = font.getStringWidthFloat(text);
            g.setFont(4.f * (float)getWidth() / strWidth);
            g.drawFittedText(text, bounds.toNearestInt(), juce::Justification::centred, 1);
            g.drawRoundedRectangle(bounds, 2.f, 2.f);
            for (auto i = 0; i < 2; ++i)
            {
                bounds = bounds.reduced(4.f);
                g.drawRoundedRectangle(bounds, 2.f, 2.f);
            }
        }
    };

    struct Knob :
        public Comp
    {
        static constexpr float StartAngle = Tau * .5f + Tau;
        static constexpr float AngleWidth = Tau;

        static constexpr float Sensitive = .2f;
        static constexpr float Wheel = 0.01f;



        inline std::function<void(Knob&, juce::Graphics& g)> makeOnPaint0()
        {
            return [](Knob& k, juce::Graphics& g)
            {
                g.setFont(Misc::Shared::shared.font);

                g.setColour(Misc::Shared::shared.colors.green2);
                g.setFont(24.f);
                const auto value = k.rap.getValue();
                const auto nameAndVal = k.rap.getName(20) + "\n" + k.rap.getCurrentValueAsText();
                g.drawFittedText(nameAndVal, k.getLocalBounds(), juce::Justification::centred, 1);

                float mappedValue = juce::jmap(k.rap.getNormalisableRange().convertFrom0to1(value), k.rap.getNormalisableRange().start, k.rap.getNormalisableRange().end, 1.f, 0.f);

                const auto thickness = 2.f;
                const auto width = (float)k.getWidth();
                const auto height = (float)k.getHeight();
                juce::Rectangle<float> bounds;
                {
                    const auto minDimen = std::min(width, height);
                    const auto x = (width - minDimen) * .5f;
                    const auto y = (height - minDimen) * .5f;
                    bounds.setBounds(x, y, minDimen, minDimen);
                }

                g.drawEllipse(bounds, thickness);

                const auto thickness2 = thickness * 2.f;
                const auto thickness4 = thickness * 4.f;

                juce::PathStrokeType strokeType(thickness2, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle());

                const auto radius = .5f * bounds.getWidth() - thickness4;
                const juce::Point<float> centre(
                    bounds.getWidth() * .5f + bounds.getX(),
                    bounds.getHeight() * .5f + bounds.getY()
                );

                const auto valAngle = StartAngle + value * AngleWidth;

                //juce::ColourGradient gradient(green2, 0, 0, green1, 0, radius, false);
                //g.setGradientFill(gradient);

                auto red = (static_cast<int>(255 * mappedValue));
                auto green = (Misc::Shared::shared.colors.green1.getGreen());
                auto blue = (Misc::Shared::shared.colors.green1.getBlue());

                juce::Path arc;

                g.setColour(juce::Colours::white);
                arc.addCentredArc(
                    centre.x, centre.y,
                    radius, radius,
                    0.f,
                    value,
                    valAngle,
                    true
                );

                g.strokePath(arc, strokeType);


                juce::Path arc1;

                g.setColour(juce::Colour(red, green, blue));

                arc1.addCentredArc(
                    centre.x, centre.y,
                    radius, radius,
                    0.f,
                    StartAngle,
                    valAngle,
                    true
                );

                //auto line = juce::Line<float>::fromStartAndAngle(centre, radius, valAngle);
                //arc.lineTo(line.withShortenedStart(radius - thickness4).getStart());

                g.strokePath(arc1, strokeType);

            };
        };

        Knob(juce::String& tooltp, juce::RangedAudioParameter* _rap) :
            Comp(std::move(tooltp)),
            rap(*_rap),
            attach(rap, [this](float) { repaint(); }, nullptr),
            onPaint(makeOnPaint0()),
            dragY(0.f)
        {
            attach.sendInitialUpdate();
        }

    protected:
        juce::RangedAudioParameter& rap;
        juce::ParameterAttachment attach;
        std::function<void(Knob&, juce::Graphics& g)> onPaint;
        float dragY;

        void paint(juce::Graphics& g) override
        {
            onPaint(*this, g);
        }

        void mouseDown(const juce::MouseEvent& evt) override
        {
            attach.beginGesture();
            dragY = evt.position.y / (float)getTopLevelComponent()->getHeight();
        }

        void mouseDrag(const juce::MouseEvent& evt) override
        {
            const auto newDragY = evt.position.y / (float)getTopLevelComponent()->getHeight();
            const auto dif = dragY - newDragY;
            const auto sens = evt.mods.isShiftDown() ? Sensitive : 1.f;
            const auto val = juce::jlimit(0.f, 1.f, rap.getValue() + dif * sens);
            attach.setValueAsPartOfGesture(rap.convertFrom0to1(val));
            dragY = newDragY;
        }

        void mouseUp(const juce::MouseEvent& evt) override
        {
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
    };
}
