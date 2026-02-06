//
// Created by Landon Viator on 11/26/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Fonts.h"

namespace viator::gui::laf
{
    class DialLAF final : public juce::LookAndFeel_V4 {
    public:
        enum class FillMode { UnipolarMinToValue, BipolarZeroToValue, None };

        void drawRotarySlider(juce::Graphics &g, const int x, const int y, const int width, const int height,
                              const float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                              juce::Slider &slider) override
        {
            auto b = juce::Rectangle<float>((float) x, (float) y, (float) width, (float) height).reduced(2.0f);
            auto centre = b.getCentre();
            auto r = juce::jmin(b.getWidth(), b.getHeight()) * 0.5f;

            const float start = rotaryStartAngle;
            const float end = rotaryEndAngle;

            auto getFillMode = [&]() -> FillMode
            {
                const auto minV = (float) slider.getRange().getStart();
                const auto maxV = (float) slider.getRange().getEnd();

                if (slider.getProperties().contains("fillMode"))
                {
                    const int m = (int) slider.getProperties()["fillMode"];
                    if (m == 1) return FillMode::BipolarZeroToValue;
                    if (m == 2) return FillMode::None;
                    return FillMode::UnipolarMinToValue;
                }

                if (minV < 0.0f && maxV > 0.0f)
                    return FillMode::BipolarZeroToValue;

                return FillMode::UnipolarMinToValue;
            };

            auto normalise = [&](float v) -> float
            {
                const auto minV = (float) slider.getRange().getStart();
                const auto maxV = (float) slider.getRange().getEnd();
                const auto denom = (maxV - minV);
                if (denom == 0.0f) return 0.0f;
                return juce::jlimit(0.0f, 1.0f, (v - minV) / denom);
            };

            auto angleForValue = [&](float v) -> float
            {
                const float t = normalise(v);
                return rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            };

            auto strokeArc = [&](float a0, float a1, float radius, float thickness, juce::Colour c)
            {
                if (a1 < a0) std::swap(a0, a1);
                juce::Path p;
                p.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, a0, a1, true);
                g.setColour(c);
                g.strokePath(p, juce::PathStrokeType(thickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            };

            const juce::Colour defaultTrack = juce::Colours::black.brighter(0.1f);
            const auto defaultValue = juce::Colour(211, 218, 217);

            constexpr auto trackId = juce::Slider::trackColourId;
            constexpr auto valueId = juce::Slider::rotarySliderFillColourId;

            const auto trackColour = slider.isColourSpecified(trackId) ? slider.findColour(trackId) : defaultTrack;
            const auto valueColour = slider.isColourSpecified(valueId) ? slider.findColour(valueId) : defaultValue;

            const float trackThickness = juce::jlimit(3.0f, r * 0.22f, r * 0.14f);
            const float trackRadius = r - trackThickness * 0.5f;

            const float faceRadius = trackRadius - trackThickness * 0.95f;
            const auto faceBounds = juce::Rectangle<float>(faceRadius * 2.0f, faceRadius * 2.0f).withCentre(centre);

            const float v = (float) slider.getValue();
            const float minV = (float) slider.getRange().getStart();
            const float maxV = (float) slider.getRange().getEnd();

            const FillMode mode = getFillMode();

            const float valueAngle = angleForValue(v);

            strokeArc(start, end, trackRadius, trackThickness, trackColour.withAlpha(0.75f));

            if (mode != FillMode::None)
            {
                float anchorV = minV;
                if (mode == FillMode::BipolarZeroToValue)
                    anchorV = 0.0f;

                anchorV = juce::jlimit(minV, maxV, anchorV);

                const float anchorAngle = angleForValue(anchorV);

                if (const bool atAnchor = std::abs(v - anchorV) <= 1.0e-6f; !atAnchor)
                {
                    strokeArc(anchorAngle, valueAngle, trackRadius, trackThickness, valueColour);
                }
            }

            const float rimW = juce::jlimit(1.5f, faceRadius * 0.12f, faceRadius * 0.075f);
            auto outline = faceBounds.reduced(rimW * 0.15f);

            {
                const auto shadowBase = juce::Colours::black;
                const float totalAlpha = 0.18f;
                const float step = 0.2f;

                for (int i = 0; i < 10; ++i)
                {
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
            g.drawEllipse(outline, 1.0f);

            {
                auto bevelBounds = faceBounds.reduced(rimW * 0.75f);
                juce::Path ring;
                ring.addEllipse(bevelBounds);

                auto bw = juce::jlimit(1.0f, faceRadius * 0.10f, faceRadius * 0.055f);

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

                g.setColour(juce::Colours::white);
                g.fillEllipse(dotBounds);
            }
        }
    };

    class MacroLAF final : public juce::LookAndFeel_V4 {
    public:
        explicit MacroLAF(const int num_decimal_places)
        {
            m_num_decimals = num_decimal_places;
        }

        void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                              const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override
        {
            auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
            auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

            auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

            auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto lineW = 2.0f;
            auto arcRadius = radius - lineW * 0.5f;

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

            if (slider.isEnabled())
            {
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

            auto thumbWidth = lineW * 2.0f;
            juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
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

    class Billboard final : public juce::LookAndFeel_V4 {
    public:
        void drawLabel(juce::Graphics &g, juce::Label &label) override
        {
            g.setColour(label.findColour(juce::Label::backgroundColourId));
            g.fillRoundedRectangle(label.getLocalBounds().toFloat().reduced(3.0f, 3.0f), 9.0f);

            g.setColour(label.findColour(juce::Label::outlineColourId));
            g.drawRoundedRectangle(label.getLocalBounds().toFloat().reduced(3.0f, 3.0f), 9.0f, 2.0f);
        }
    };

    class BevelButtonLAF : public juce::LookAndFeel_V4 {
    public:
        float corner = 6.0f;

        void drawButtonBackground(juce::Graphics &g,
                                  juce::Button &button,
                                  const juce::Colour & /*backgroundColour*/,
                                  bool isMouseOverButton,
                                  bool isButtonDown) override
        {
            auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

            juce::Path shape;
            shape.addRoundedRectangle(bounds, corner);

            auto base = button.findColour(juce::TextButton::buttonColourId);
            auto on = button.findColour(juce::TextButton::buttonOnColourId);
            auto fillBase = button.getToggleState() ? on : base;

            if (!button.isEnabled())
                fillBase = fillBase.withMultipliedSaturation(0.15f).withMultipliedAlpha(0.55f);

            if (isMouseOverButton)
                fillBase = fillBase.brighter(0.06f);

            if (isButtonDown)
                fillBase = fillBase.darker(0.10f);

            auto topCol = fillBase.brighter(isButtonDown ? 0.25f : 0.1f);
            auto botCol = fillBase;

            juce::ColourGradient fillGrad(topCol, bounds.getX(), bounds.getY(),
                                          botCol, bounds.getX(), bounds.getBottom(), false);
            g.setGradientFill(fillGrad);
            g.fillPath(shape);

            const auto w = bounds.getWidth();
            const auto h = bounds.getHeight();
            const auto r = 0.5f * juce::jmin(w, h);

            const auto rimW = juce::jlimit(1.5f, r * 0.22f, r * 0.12f);
            auto bevelBounds = bounds.reduced(rimW * 0.75f);

            juce::Path ring;
            ring.addRoundedRectangle(bevelBounds, juce::jmax(0.0f, corner - rimW * 0.75f));

            const auto bw = juce::jlimit(1.0f, r * 0.10f, r * 0.055f);

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

            g.setColour(juce::Colours::black.withAlpha(button.isEnabled() ? 0.25f : 0.15f));
            g.strokePath(shape, juce::PathStrokeType(1.0f));
        }

        void drawButtonText(juce::Graphics &g,
                            juce::TextButton &button,
                            bool /*isMouseOverButton*/,
                            bool /*isButtonDown*/) override
        {
            auto font = getTextButtonFont(button, button.getHeight());
            g.setFont(font);

            auto textCol = button.findColour(button.getToggleState()
                                                 ? juce::TextButton::textColourOnId
                                                 : juce::TextButton::textColourOffId);

            if (!button.isEnabled())
                textCol = textCol.withMultipliedAlpha(0.5f);

            auto r = button.getLocalBounds();

            // tiny "lift" so text feels centered against shadow
            r = r.translated(0, -1);

            g.setColour(textCol);
            g.drawFittedText(button.getButtonText(), r, juce::Justification::centred, 1);
        }

        juce::Font getTextButtonFont(juce::TextButton & /*button*/, int buttonHeight) override
        {
            return juce::Font((float) juce::jlimit(12, 22, (int) (buttonHeight * 0.45f)), juce::Font::bold);
        }
    };
}
