#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : state(Stopped)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        // Trigger the audio system to start up by call the prepareToPlay() function
        setAudioChannels (2, 2);
    }
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    
    // register basic format files
    formatManager.registerBasicFormats();
    
    // make the app reactive to changes in the playback state
    transportSource.addChangeListener(this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    playButton.setBounds(0, 0, getWidth(), getHeight() / 2);
    stopButton.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}

void MainComponent::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch (state) {
            case Stopped:
                stopButton.setEnabled(false);
                playButton.setEnabled(true);
                transportSource.setPosition(0.0);
                break;
            case Starting:
                playButton.setEnabled(false);
                transportSource.start();
                break;
            case Playing:
                stopButton.setEnabled(true);
                break;
            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    juce::String message;
    message << "Change event captured";
    juce::Logger::getCurrentLogger()->writeToLog(message);
    
    
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            changeState(Stopping);
        }
    }
}
