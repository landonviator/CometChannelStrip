//
// Created by Landon Viator on 11/24/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

namespace viator::gui_utils
{
    class Fonts final
    {
    public:
        static juce::Font bold(const float height = 12.0f, const float kerning = 0.15f)
        {
            return fonts().f_bold.withHeight(height).withExtraKerningFactor(kerning);
        }

        static juce::Font regular(const float height = 12.0f, const float kerning = 0.15f)
        {
            return fonts().f_regular.withHeight(height).withExtraKerningFactor(kerning);
        }

    private:
        Fonts() = default;

        static Fonts &fonts()
        {
            static Fonts instance;
            return instance;
        }

        juce::Font f_bold{
            juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoBold_ttf,
                                                               BinaryData::JetBrainsMonoBold_ttfSize))
        };
        juce::Font f_regular{
            juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoLight_ttf,
                                                               BinaryData::JetBrainsMonoLight_ttfSize))
        };

        JUCE_DECLARE_NON_COPYABLE(Fonts)
    };
}
