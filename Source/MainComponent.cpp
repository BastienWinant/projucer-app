#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: state(Stopped)
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
        setAudioChannels (2, 2);
    }
    
    // initialise the AudioFormatManager object to register a list of standard formats
    formatManager.registerBasicFormats();
    
    // add MainContentComponent object as listener to the AudioTransportSource object so that we can respond to changes in its state
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
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    // Clear the buffer so as to prevent random noise
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

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
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::changeState(AppState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch(state)
        {
            case Stopped:
                break;
            case Starting:
                break;
            case Playing:
                break;
            case Stopping:
                break;
        }
    }
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster *source)
{
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
