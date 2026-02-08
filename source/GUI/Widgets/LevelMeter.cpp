#include "LevelMeter.h"

#include "../../Globals/Globals.h"
#include "../Style/Colors.h"

namespace viator
{
    void LevelMeter::paint(juce::Graphics& g)
    {
        const float rawDB = juce::Decibels::gainToDecibels(smoothedGainReduction, -60.0f);
        const float dBValue = juce::jlimit(-60.0f, 0.0f, rawDB);

        const auto W = static_cast<float>(getWidth());
        const auto H = static_cast<float>(getHeight());

        g.fillAll(viator::gui_utils::Colors::light_bg());

        const float greenW  = W * 0.6f;
        const float yellowW = W * 0.25f;
        const float redW    = W * 0.15f;

        float fillNorm = 0.0f;
        if (constexpr float greenTopDB = -18.0f; dBValue <= greenTopDB)
            fillNorm = juce::jmap(dBValue, -60.0f, greenTopDB, 0.0f, 0.6f);
        else if (constexpr float yellowTopDB = -3.0f; dBValue <= yellowTopDB)
            fillNorm = juce::jmap(dBValue, greenTopDB, yellowTopDB, 0.6f, 0.85f);
        else
            fillNorm = juce::jmap(dBValue, yellowTopDB, 0.0f, 0.85f, 1.0f);

        const float fillW = W * fillNorm;
        float remainingW = fillW;

        float x = (fill_direction == FillDirection::LeftToRight) ? 0.0f : W;

        auto drawSeg = [&](const juce::Colour c, const float segMaxW)
        {
            if (remainingW <= 0.0f) return;

            const float w = juce::jmin(remainingW, segMaxW);
            g.setColour(c);

            if (fill_direction == FillDirection::LeftToRight)
            {
                g.fillRect(x, 0.0f, w, H);
                x += w;
            }
            else
            {
                x -= w;
                g.fillRect(x, 0.0f, w, H);
            }

            remainingW -= w;
        };

        drawSeg(gui_utils::Colors::meter_green(),  greenW);
        drawSeg(gui_utils::Colors::meter_yellow(), yellowW);
        drawSeg(gui_utils::Colors::meter_red(),    redW);
    }

    void LevelMeter::setLevel(const float newLevel)
    {
        smoothedGainReduction = newLevel;
        repaint();
    }
}
