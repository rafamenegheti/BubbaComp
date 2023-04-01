
#pragma once
#include <JuceHeader.h>

namespace Misc {
	struct Shared
	{
		Shared() :
			font(juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::Symtext_ttf, BinaryData::Symtext_ttfSize))),
			colors()
		{

		}

		struct Colors {
			juce::Colour green1 = juce::Colour(87, 204, 153);
			juce::Colour green2 = juce::Colour(56, 163, 165);
			juce::Colour green3 = juce::Colour(199, 249, 204);
		};

		juce::Font font;
		Colors colors;

		static Shared shared;
	};

}