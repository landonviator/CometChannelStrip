//
// Created by Landon Viator on 11/26/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Fonts.h"

namespace viator::gui::laf
{
    class DialLAF final : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics &g, const int x, const int y, const int width, const int height,
                              const float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                              juce::Slider &slider) override
        {
            const auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
            const auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

            const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

            const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            const auto lineW = static_cast<float>(height) * 0.05f;
            const auto thumbW = static_cast<float>(height) * 0.025f;
            const auto arcRadius = radius - lineW * 0.5f;
            const auto center = bounds.getCentre();

            // Draw drop shadow
            const auto knobBounds = bounds.withSizeKeepingCentre(radius * 1.6f, radius * 1.6f);
            const juce::Rectangle<float> shadowBounds = knobBounds.translated(1.0f, 2.0f);
            g.setColour(juce::Colours::black.withAlpha(0.3f));
            g.fillEllipse(shadowBounds);

            // center
            g.setColour(outline);
            const float cx = knobBounds.getCentreX();
            const float cy = knobBounds.getCentreY();
            const float r  = knobBounds.getWidth() * 0.5f;

            juce::ColourGradient gradient(
                    outline,
                    cx + lineW * 0.9f, cy - lineW * 4.0f,
                    outline.brighter(0.25f),
                    cx + r, cy + r,
                    true
            );

            g.setGradientFill(gradient);
            g.fillEllipse(knobBounds);

            // Background arc
            juce::Path backgroundArc;
            backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f,
                                        rotaryStartAngle, rotaryEndAngle, true);

            g.setColour(juce::Colours::black);
            g.strokePath(backgroundArc,
                         juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

            // Value arc
            if (slider.isEnabled()) {
                juce::Path valueArc;
                valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f,
                                       rotaryStartAngle, toAngle, true);

                g.setColour(fill.withAlpha(0.5f));
                g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                                            juce::PathStrokeType::rounded));
            }

            // Thumb
            const auto thumbWidth = thumbW * 5.0f;
            const float knobFaceRadius = (radius * 1.25f) / 2.0f;
            const juce::Point<float> thumbPoint(
                center.x + knobFaceRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                center.y + knobFaceRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

            g.setColour(slider.findColour(juce::Slider::thumbColourId).withAlpha(0.8f));
            g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
        }
    };
}
