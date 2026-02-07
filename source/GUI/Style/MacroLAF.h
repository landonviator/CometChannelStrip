//
// Created by Landon Viator on 2/7/26.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Fonts.h"

namespace viator::laf
{
    class MacroLAF final : public juce::LookAndFeel_V4
    {
    public:
        explicit MacroLAF(const int num_decimal_places)
        {
            m_num_decimals = num_decimal_places;
        }

        void drawRotarySlider(juce::Graphics &g, const int x, const int y, const int width, const int height, const float sliderPos,
                              const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override
        {
            const auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
            const auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

            const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

            const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            constexpr auto lineW = 2.0f;
            const auto arcRadius = radius - lineW * 0.5f;

            juce::Path backgroundArc;
            backgroundArc.addCentredArc(bounds.getCentreX(),
                                        bounds.getCentreY(),
                                        arcRadius,
                                        arcRadius,
                                        0.0f,
                                        rotaryStartAngle,
                                        rotaryEndAngle,
                                        true);

            g.setColour(outline);
            g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            if (slider.isEnabled()) {
                juce::Path valueArc;
                valueArc.addCentredArc(bounds.getCentreX(),
                                       bounds.getCentreY(),
                                       arcRadius,
                                       arcRadius,
                                       0.0f,
                                       rotaryStartAngle,
                                       toAngle,
                                       true);

                g.setColour(fill);
                g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            }

            constexpr auto thumbWidth = lineW * 2.0f;
            const juce::Point<float> thumbPoint(
                bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

            const auto text = slider.isMouseOverOrDragging() ? juce::String(slider.getValue(), m_num_decimals) : slider.getName();
            g.setColour(juce::Colours::whitesmoke);
            g.setFont(gui_utils::Fonts::bold(static_cast<float>(width) * 0.16f));
            g.drawFittedText(text, x, y, width, height, juce::Justification::centred, 2);
        }

    private:
        int m_num_decimals{0};
    };
}