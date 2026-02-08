#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace viator
{
    class LevelMeter : public juce::Component {
    public:
        LevelMeter() = default;

        void paint(juce::Graphics &) override;

        void setLevel(float newLevel);

        float getReduction() const
        {
            return smoothedGainReduction;
        }

        enum class FillDirection { LeftToRight, RightToLeft };
        void setFillDirection(FillDirection dir) { fill_direction = dir; repaint(); }

    private:
        float smoothedGainReduction = 0.0f;

        FillDirection fill_direction = FillDirection::LeftToRight;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
    };
}
