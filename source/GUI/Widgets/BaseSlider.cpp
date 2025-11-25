//
// Created by Landon Viator on 11/21/25.
//

#include "BaseSlider.h"

namespace viator::gui::widgets
{
    BaseSlider::BaseSlider()
    {

    }

    BaseSlider::~BaseSlider()
    {

    }

    void BaseSlider::paint(juce::Graphics &g)
    {
        juce::Slider::paint(g);

        g.setColour(juce::Colours::purple.withAlpha(m_is_mapped && m_show_mapped ? 0.25f : 0.0f));
        g.fillRect(getLocalBounds());
    }

    void BaseSlider::mouseDown(const juce::MouseEvent &event)
    {
        if (event.mods.isRightButtonDown())
        {
            sendChangeMessage();
        }
    }

    void BaseSlider::setIsMapped(const bool isMapped)
    {
        m_is_mapped = isMapped;
        repaint();
    };

    void BaseSlider::showMapping(const bool shouldShowMapped)
    {
        m_show_mapped = shouldShowMapped;
        repaint();
    }
}