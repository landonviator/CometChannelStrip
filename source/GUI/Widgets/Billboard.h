//
// Created by Landon Viator on 11/21/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class RoundedBackdrop : public juce::Component
{
public:
    void setCornerRadius(float new_radius)            { corner_radius = new_radius; repaint(); }
    void setOutlineThickness(float new_thickness)     { outline_thickness = new_thickness; repaint(); }
    void setShadowRadius(int new_radius)              { shadow_radius = new_radius; repaint(); }
    void setShadowOffset(juce::Point<int> new_offset) { shadow_offset = new_offset; repaint(); }

    void setFillColour(juce::Colour c)                { fill_colour = c; repaint(); }
    void setOutlineColour(juce::Colour c)             { outline_colour = c; repaint(); }
    void setShadowColour(juce::Colour c)              { shadow_colour = c; repaint(); }

    void paint(juce::Graphics& g) override
    {
        auto r = getLocalBounds().toFloat();

        const float pad = (float)shadow_radius + (float)juce::jmax(std::abs(shadow_offset.x), std::abs(shadow_offset.y));
        r = r.reduced(pad);

        r = r.reduced(outline_thickness * 0.5f);

        juce::Path p;
        p.addRoundedRectangle(r, corner_radius);

        juce::DropShadow ds(shadow_colour, shadow_radius, shadow_offset);
        ds.drawForPath(g, p);

        g.setColour(fill_colour);
        g.fillPath(p);

        if (outline_thickness > 0.0f)
        {
            g.setColour(outline_colour);
            g.strokePath(p, juce::PathStrokeType(outline_thickness));
        }
    }

private:
    float corner_radius = 6.0f;
    float outline_thickness = 1.0f;

    int shadow_radius = 5;
    juce::Point<int> shadow_offset { 0, 2 };

    juce::Colour fill_colour   = juce::Colour(31, 31, 31);
    juce::Colour outline_colour = juce::Colour(73, 73, 73);
    juce::Colour shadow_colour  = juce::Colour(0, 0, 0);
};