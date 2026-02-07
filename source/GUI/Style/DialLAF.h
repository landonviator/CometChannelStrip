//
// Created by Landon Viator on 2/7/26.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Fonts.h"
#include "Colors.h"

namespace viator::laf
{
    class KnobUtils {
    public:
        static void draw_ticks(juce::Graphics &g, const juce::Slider &slider, const float radiusMult = 0.05f)
        {
            const auto bounds = slider.getLocalBounds().toFloat().reduced(static_cast<float>(slider.getWidth()) * 0.1f);
            const auto center = bounds.getCentre();
            const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 10.0f;

            constexpr int numSteps = 11;
            constexpr float startAngle = juce::MathConstants<float>::pi * 1.25f;
            constexpr float endAngle = juce::MathConstants<float>::pi * 2.75f;

            g.setColour(gui_utils::Colors::text());
            constexpr auto font_size = 10.0f;
            const auto font = gui_utils::Fonts::bold(font_size);
            g.setFont(font);

            if (slider.getName() != "Type")
            {
                for (int i = 0; i < numSteps; ++i)
                {
                    const float t = static_cast<float>(i) / (numSteps - 1);
                    const auto value = static_cast<float>(slider.getNormalisableRange().convertFrom0to1(t));
                    const auto angle = juce::jmap(t, 0.0f, 1.0f, startAngle, endAngle) - juce::MathConstants<
                                           float>::halfPi;

                    const float labelRadius = radius + static_cast<float>(slider.getHeight()) * 0.1f;
                    const float x = center.x + std::cos(angle) * labelRadius;
                    const float y = center.y + std::sin(angle) * labelRadius;

                    juce::String label;
                    if (std::abs(value) >= 1000.0f)
                    {
                        const int valueInK = static_cast<int>(std::round(value / 1000.0f));
                        label = juce::String(valueInK) + "k";
                    } else
                    {
                        label = juce::String(static_cast<int>(std::round(std::abs(value))));
                    }


                    const float textWidth = juce::TextLayout::getStringWidth(juce::AttributedString(label));
                    const float textHeight = g.getCurrentFont().getAscent();

                    if (i == 0 || i == 5 || i == 10)
                    {
                        const juce::Rectangle<float> textBounds(x - textWidth / 2.0f, y - textHeight / 2.0f, textWidth,
                                                                textHeight);
                        g.drawText(label, textBounds, juce::Justification::centred);
                    }
                }
            }

            // Draw tick marks
            for (int i = 0; i < numSteps; ++i)
            {
                const float t = static_cast<float>(i) / (numSteps - 1);
                const auto angle = juce::jmap(t, 0.0f, 1.0f, startAngle, endAngle) - juce::MathConstants<float>::halfPi;

                const auto height = static_cast<float>(slider.getHeight());
                const float innerRadius = radius - height * 0.04f;
                const float outerRadius = radius + height * radiusMult * 0.5f;

                const juce::Point<float> p1(center.x + std::cos(angle) * innerRadius,
                                            center.y + std::sin(angle) * innerRadius);
                const juce::Point<float> p2(center.x + std::cos(angle) * outerRadius,
                                            center.y + std::sin(angle) * outerRadius);

                g.drawLine({p1, p2}, 1.0f);
            }

            g.setColour(juce::Colours::white);
        }
    };

    class DialLAF final : public juce::LookAndFeel_V4 {
    public:
        enum class FillMode { UnipolarMinToValue, BipolarZeroToValue, None };

        static juce::String formatKnobValue(const double value, const juce::String &suffix)
        {
            const bool is_db = suffix.trim().equalsIgnoreCase("dB");

            if (const bool is_hz = suffix.containsIgnoreCase("hz"); !is_db && is_hz && std::abs(value) >= 1000.0)
            {
                const auto k = value / 1000.0;
                const auto k_trunc_1dp = std::floor(k * 10.0) / 10.0;
                return juce::String(k_trunc_1dp, 1) + "k";
            }

            const int num_places = (value >= 1.0 && !is_db) ? 0 : 2;
            return (num_places == 0)
                       ? juce::String(juce::roundToInt(value))
                       : juce::String(value, num_places);
        }

        void drawRotarySlider(juce::Graphics &g, const int x, const int y, const int width, const int height,
                              const float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                              juce::Slider &slider) override
        {
            KnobUtils::draw_ticks(g, slider);

            auto b = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)).
                    reduced(static_cast<float>(width) * 0.18f);
            auto centre = b.getCentre();
            auto r = juce::jmin(b.getWidth(), b.getHeight()) * 0.5f;

            auto normalise = [&](const float v) -> float
            {
                const auto minV = static_cast<float>(slider.getRange().getStart());
                const auto maxV = static_cast<float>(slider.getRange().getEnd());
                const auto denom = (maxV - minV);
                if (denom == 0.0f) return 0.0f;
                return juce::jlimit(0.0f, 1.0f, (v - minV) / denom);
            };

            auto angleForValue = [&](const float v) -> float
            {
                const float t = normalise(v);
                return rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            };

            const juce::Colour defaultTrack = juce::Colours::black.brighter(0.1f);
            constexpr auto trackId = juce::Slider::trackColourId;
            const auto trackColour = slider.isColourSpecified(trackId) ? slider.findColour(trackId) : defaultTrack;
            const float trackThickness = r * 0.14f;
            const float trackRadius = r - trackThickness * 0.5f;
            const float faceRadius = trackRadius - trackThickness * 0.95f;
            const auto faceBounds = juce::Rectangle<float>(faceRadius * 2.0f, faceRadius * 2.0f).withCentre(centre);

            const auto v = static_cast<float>(slider.getValue());
            const float valueAngle = angleForValue(v);
            const float rimW = faceRadius * 0.075f;
            auto outline = faceBounds.reduced(rimW * 0.15f);
            {
                const auto shadowBase = juce::Colours::black;

                for (int i = 0; i < 10; ++i)
                {
                    constexpr float step = 0.2f;
                    constexpr float totalAlpha = 0.18f;
                    const float t = static_cast<float>(i) / 9.0f;
                    const float a = totalAlpha * (1.0f - t) * (1.0f - t);
                    g.setColour(shadowBase.withAlpha(a));
                    g.drawEllipse(outline.translated(0.0f, (static_cast<float>(i) + 1.0f) * step), rimW);
                }
            }

            auto faceBase = slider.findColour(juce::Slider::ColourIds::backgroundColourId);
            juce::ColourGradient faceGrad(
                faceBase.brighter(0.08f),
                faceBounds.getX() + faceBounds.getWidth() * 0.25f, faceBounds.getY() + faceBounds.getHeight() * 0.20f,
                faceBase.darker(0.10f),
                faceBounds.getRight() - faceBounds.getWidth() * 0.15f, faceBounds.getBottom() - faceBounds.getHeight() * 0.10f,
                true
            );

            g.setGradientFill(faceGrad);
            g.fillEllipse(faceBounds);

            g.setColour(trackColour.withAlpha(0.85f));
            {
                auto bevelBounds = faceBounds.reduced(rimW * 0.75f);
                juce::Path ring;
                ring.addEllipse(bevelBounds);

                auto bw = faceRadius * 0.055f;

                juce::ColourGradient hi(
                    juce::Colours::white.withAlpha(0.22f),
                    bevelBounds.getX(), bevelBounds.getY(),
                    juce::Colours::transparentBlack,
                    bevelBounds.getCentreX(), bevelBounds.getCentreY(),
                    true
                );

                g.setGradientFill(hi);
                g.strokePath(ring, juce::PathStrokeType(bw, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

                juce::ColourGradient lo(
                    juce::Colours::transparentBlack,
                    bevelBounds.getCentreX(), bevelBounds.getCentreY(),
                    juce::Colours::black.withAlpha(0.18f),
                    bevelBounds.getRight(), bevelBounds.getBottom(),
                    true
                );

                g.setGradientFill(lo);
                g.strokePath(ring, juce::PathStrokeType(bw, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            }
            {
                const float dotAngle = valueAngle - juce::MathConstants<float>::halfPi;
                const float dotDist = faceRadius * 0.62f;
                const float dotR = faceRadius * 0.07f;

                juce::Point<float> p(
                    centre.x + dotDist * std::cos(dotAngle),
                    centre.y + dotDist * std::sin(dotAngle)
                );

                auto dotBounds = juce::Rectangle<float>(dotR * 2.0f, dotR * 2.0f).withCentre(p);

                g.setColour(trackColour);
                g.fillEllipse(dotBounds);
            }

            constexpr auto font_size = 10.0f;
            const auto font = gui_utils::Fonts::bold(font_size);
            const auto suffix = slider.getTextValueSuffix();
            const auto value = slider.getValue();

            const auto text = slider.isMouseOverOrDragging()
                                  ? formatKnobValue(value, suffix) + suffix
                                  : slider.getName();

            g.setColour(juce::Colour(255, 255, 255));
            g.setFont(font);
            g.drawText(text, 0, height - juce::roundToInt(font_size), width, juce::roundToInt(font_size),
                       juce::Justification::centredBottom);
        }
    };

    class PultecHandleDialLAF final : public juce::LookAndFeel_V4 {
    public:
        static juce::String formatKnobValue(const double value, const juce::String &suffix)
        {
            const bool is_db = suffix.trim().equalsIgnoreCase("dB");

            if (const bool is_hz = suffix.containsIgnoreCase("hz"); !is_db && is_hz && std::abs(value) >= 1000.0)
            {
                const auto k = value / 1000.0;
                const auto k_trunc_1dp = std::floor(k * 10.0) / 10.0;
                return juce::String(k_trunc_1dp, 1) + "k";
            }

            const int num_places = (value >= 1.0 && !is_db) ? 0 : 2;
            return (num_places == 0)
                       ? juce::String(juce::roundToInt(value))
                       : juce::String(value, num_places);
        }

        void drawRotarySlider(juce::Graphics &g, const int x, const int y, const int width, const int height,
                              const float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                              juce::Slider &slider) override
        {
            KnobUtils::draw_ticks(g, slider);

            auto b = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)).
                    reduced(static_cast<float>(width) * 0.18f);
            auto centre = b.getCentre();
            auto r = juce::jmin(b.getWidth(), b.getHeight()) * 0.5f;

            auto normalise = [&](const float v) -> float
            {
                const auto minV = static_cast<float>(slider.getRange().getStart());
                const auto maxV = static_cast<float>(slider.getRange().getEnd());
                const auto denom = (maxV - minV);
                if (denom == 0.0f) return 0.0f;
                return juce::jlimit(0.0f, 1.0f, (v - minV) / denom);
            };

            auto angleForValue = [&](const float v) -> float
            {
                const float t = normalise(v);
                return rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            };

            const juce::Colour defaultTrack = juce::Colours::black.brighter(0.1f);
            constexpr auto trackId = juce::Slider::trackColourId;
            const auto trackColour = slider.isColourSpecified(trackId) ? slider.findColour(trackId) : defaultTrack;
            const float trackThickness = r * 0.14f;
            const float trackRadius = r - trackThickness * 0.5f;
            const float faceRadius = trackRadius - trackThickness * 0.95f;
            const auto faceBounds = juce::Rectangle<float>(faceRadius * 1.0f, faceRadius * 1.0f).withCentre(centre);

            auto faceBase = slider.findColour(juce::Slider::ColourIds::rotarySliderOutlineColourId);
            juce::ColourGradient faceGrad(
                faceBase.brighter(0.15f),
                faceBounds.getX() + faceBounds.getWidth() * 0.25f, faceBounds.getY() + faceBounds.getHeight() * 0.20f,
                faceBase.darker(0.15f),
                faceBounds.getRight() - faceBounds.getWidth() * 0.15f, faceBounds.getBottom() - faceBounds.getHeight() * 0.10f,
                true
            );

            g.setGradientFill(faceGrad);
            g.fillEllipse(faceBounds);

            // Define the dimensions and position of the rounded rectangle
            float rectWidth = static_cast<float>(width) * 0.15f; // 10% of the component's width
            float rectHeight = static_cast<float>(height) * 0.5f; // 30% of the component's height
            float rectRadius = rectWidth * 0.25f; // Radius for rounded corners

            // Create a Path for the rounded rectangle
            juce::Path dialRect;
            dialRect.addRoundedRectangle(-rectWidth / 2, -rectHeight / 2, rectWidth, rectHeight, rectRadius);

            const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

            // Apply rotation based on the dial value
            float angle = toAngle; // Use the calculated angle
            juce::AffineTransform transform = juce::AffineTransform::rotation(angle)
                .translated(centre.getX(), centre.getY());
            dialRect.applyTransform(transform);

            // Draw the rounded rectangle
            faceBase = slider.findColour(juce::Slider::ColourIds::rotarySliderOutlineColourId);
            juce::ColourGradient grad(
                faceBase.brighter(0.05f),
                faceBounds.getX() + faceBounds.getWidth() * 0.25f, faceBounds.getY() + faceBounds.getHeight() * 0.20f,
                faceBase.darker(0.05f),
                faceBounds.getRight() - faceBounds.getWidth() * 0.15f, faceBounds.getBottom() - faceBounds.getHeight() * 0.10f,
                true
            );
            g.setGradientFill(grad);
            g.fillPath(dialRect);

            g.setColour(slider.findColour(juce::Slider::ColourIds::backgroundColourId));
            g.strokePath(dialRect, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            {
                const auto v = static_cast<float>(slider.getValue());
                const float valueAngle = angleForValue(v);
                const float dotAngle = valueAngle - juce::MathConstants<float>::halfPi;
                const float dotDist = faceRadius * 0.62f;
                const float dotR = faceRadius * 0.07f;

                juce::Point<float> p(
                    centre.x + dotDist * std::cos(dotAngle),
                    centre.y + dotDist * std::sin(dotAngle)
                );

                auto dotBounds = juce::Rectangle<float>(dotR * 2.0f, dotR * 2.0f).withCentre(p);

                g.setColour(trackColour);
                g.fillEllipse(dotBounds);
            }

            constexpr auto font_size = 10.0f;
            const auto font = gui_utils::Fonts::bold(font_size);
            const auto suffix = slider.getTextValueSuffix();
            const auto value = slider.getValue();

            const auto text = slider.isMouseOverOrDragging()
                                  ? formatKnobValue(value, suffix) + suffix
                                  : slider.getName();

            g.setColour(juce::Colour(255, 255, 255));
            g.setFont(font);
            g.drawText(text, 0, height - juce::roundToInt(font_size), width, juce::roundToInt(font_size),
                       juce::Justification::centredBottom);
        }
    };
}
