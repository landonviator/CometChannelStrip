//
// Created by Landon Viator on 11/21/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace viator::gui::widgets
{
class BaseSlider : public juce::Slider, public juce::ChangeBroadcaster
    {
    public:
        BaseSlider();

        ~BaseSlider() override;

        void paint(juce::Graphics &g) override;

        bool getIsMapped() const { return m_is_mapped; }
        void setIsMapped(const bool isMapped);
        void showMapping(const bool shouldShowMapped);
    private:
        void mouseDown(const juce::MouseEvent &event) override;
        bool m_is_mapped { false };
        bool m_show_mapped { false };
    };
}

