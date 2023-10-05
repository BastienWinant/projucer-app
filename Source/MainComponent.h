#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

enum TransportState
{
    Stopped, // Audio playback is stopped and ready to be started.
    Starting, // Audio playback hasn't yet started but it has been told to start.
    Playing, // Audio is playing.
    Stopping // Audio is playing but playback has been told to stop, after this it will return to the Stopped state.
};

class MainComponent  : public juce::AudioAppComponent,
                        public juce::ChangeListener
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
    /**  called when audio processing has finished */
    void releaseResources() override;

    //==============================================================================
    // Component class methods
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // ChangeListener class methods
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    // keeps track of the overall state of the application
    TransportState state;
    
    // contains a list of audio formats and can create suitable objects for reading audio data
    juce::AudioFormatManager formatManager;
    // reads audio data from an AudioFormatReader object and renders the audio via its getNextAudioBlock() function
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    // controls the playback of an AudioFormatReaderSource object
    juce::AudioTransportSource transportSource;
    
    // playback control buttons
    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};

    void changeState(TransportState newState);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
