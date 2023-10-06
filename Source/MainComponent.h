#pragma once

#include <JuceHeader.h>

enum TransportState
{
    Stopped, // Audio playback is stopped and ready to be started.
    Starting, // Audio playback hasn't yet started but it has been told to start.
    Playing, // Audio is playing.
    Stopping // Audio is playing but playback has been told to stop, after this it will return to the Stopped state.
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                        public juce::ChangeListener,
                        public juce::Button::Listener
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
    
    //==============================================================================
    // ChangeListener class methods
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;
    
    //==============================================================================
    // Button::Listener class methods
    void buttonClicked (juce::Button *button) override;

private:
    //==============================================================================
    TransportState state;
    
    /** contains a list of audio formats and creates suitable objects for reading audio data from these formats */
    juce::AudioFormatManager formatManager;
    /** reads audio data from an AudioFormatReader object and render the audio */
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    /** control the playback of an AudioFormatReaderSource object */
    juce::AudioTransportSource transportSource;
    
    // Playback control buttons
    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};
    
    // File load button
    juce::TextButton loadButton{"LOAD"};
    
    void changeState(TransportState newState);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
