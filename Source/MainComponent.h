#pragma once

#include <JuceHeader.h>

enum AppState
{
    Initial, // Define a state for the intial state of the app
    Stopped, // Audio playback is stopped and ready to be started
    Starting, // Audio playback hasn't yet started but it has been told to start
    Playing, // Audio is playing
    Pausing,
    Paused,
    Stopping, // Audio is playing but playback has been told to stop, after this it will return to the Stopped state
};

class MainComponent  : public juce::AudioAppComponent,
                        public juce::ChangeListener,
                        public juce::Button::Listener,
                        public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    // AudioSource class methods
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
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
    
    //==============================================================================
    // Button::Listener class methods
    void sliderValueChanged (juce::Slider *slider) override;
    
    void changeState(AppState newState);
    
    void loadFile();

private:
    //==============================================================================
    AppState state;
    
    // Buttons and sliders
    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};
    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;
    juce::TextButton loadButton{"LOAD"};
    
    // Audio file reading and rendering components
    /** contains list of audio formats and can create suitable objects for reading audio data */
    juce::AudioFormatManager formatManager;
    /** reads audio data from an AudioFormatReader object and render the audio */
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    /** control the playback of an AudioFormatReaderSource object */
    juce::AudioTransportSource transportSource;
    
    std::unique_ptr<juce::FileChooser> chooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
