//
// Created by Landon Viator on 10/15/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


namespace viator::gui
{
    class Images final {
    public:
        Images() = default;

        static const juce::Image &logo() { return images().m_logo; }
        static const juce::Image &texture() { return images().m_texture; }
        static const juce::Image &bd_d() { return images().m_bd_d; }

    private:
        static Images &images()
        {
            static Images instance;
            return instance;
        }

    protected:
        juce::Image m_logo{
            juce::ImageCache::getFromMemory(BinaryData::landon_png,
                                            BinaryData::landon_pngSize)
        };
        juce::Image m_texture{
            juce::ImageCache::getFromMemory(BinaryData::lightgrayconcretewall_jpg,
                                            BinaryData::lightgrayconcretewall_jpgSize)
        };
        juce::Image m_bd_d{
            juce::ImageCache::getFromMemory(BinaryData::back_d_png,
                                            BinaryData::back_d_pngSize)
        };

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}
