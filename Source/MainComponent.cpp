#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
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
        // Trigger the audio system to start up and call the prepareToPlay() function
        setAudioChannels (2, 2);
    }
    
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&stopButton);
    addAndMakeVisible(&loadButton);
    
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    
    formatManager.registerBasicFormats();
    
    transportSource.addChangeListener(this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    // Causes the AudioAppComponent::releaseResources() function to be called.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    juce::Logger::getCurrentLogger()->writeToLog("MainComponent::prepareToPlay - Preparing to play audio...");
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // checks if there is a valid AudioFormatReaderSource object first and simply zeros the output if not
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    juce::Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    transportSource.releaseResources();
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
    double rowH = getHeight() / 3;
    
    playButton.setBounds(0, 0, getWidth(), rowH);
    stopButton.setBounds(0, rowH, getWidth(), rowH);
    loadButton.setBounds(0, rowH * 2, getWidth(), rowH);
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
            default:
                break;
        }
    }
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster *source)
{
    juce::Logger::getCurrentLogger()->writeToLog("MainComponent::changeListenerCallback - Change event intercepted");
    
    if (source == &transportSource)
    {
        juce::Logger::getCurrentLogger()->writeToLog("MainComponent::changeListenerCallback: - TransportSource status has changed!");
        
        if (transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            changeState(Stopped);
        }
    }
}

void MainComponent::buttonClicked (juce::Button *button)
{
    if (button == &playButton)
    {
        juce::Logger::getCurrentLogger()->writeToLog("MainComponent::buttonClicked: - Play button clicked!");
    }
    
    if (button == &stopButton)
    {
        juce::Logger::getCurrentLogger()->writeToLog("MainComponent::buttonClicked: - Stop button clicked!");
    }
    
    if (button == &loadButton)
    {
        juce::Logger::getCurrentLogger()->writeToLog("MainComponent::buttonClicked: - Load button clicked!");
    }
}
