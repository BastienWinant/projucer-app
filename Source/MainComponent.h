#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    // AudioSource class methods
    /** called just before audio processing starts */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    /** called each time the audio hardware needs a new block of audio data */
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    /** called when audio processing has finished */
    void releaseResources() override;

    //==============================================================================
    // Component class methods
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    void logMessage(juce::String message, const char* callingObjectClass = __builtin_FUNCTION());


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
