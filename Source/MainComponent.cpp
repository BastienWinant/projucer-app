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
        setAudioChannels (2, 2);
    }
    
    // initialise the AudioFormatManager object to register a list of standard formats
    formatManager.registerBasicFormats();
    
    // add MainContentComponent object as listener to the AudioTransportSource object so that we can respond to changes in its state
    transportSource.addChangeListener(this);
    
    // add the buttons and sliders to the display
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(loadButton);
    
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    
    // set application to initial state
    changeState(Initial);
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
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        // Clear the buffer so as to prevent random noise
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
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
    double rowH = getHeight() / 6;
    
    playButton.setBounds(0, 0, getWidth(), rowH);
    stopButton.setBounds(0, rowH, getWidth(), rowH);
    volSlider.setBounds(0, rowH * 2, getWidth(), rowH);
    speedSlider.setBounds(0, rowH * 3, getWidth(), rowH);
    posSlider.setBounds(0, rowH * 4, getWidth(), rowH);
    loadButton.setBounds(0, rowH * 5, getWidth(), rowH);
}

void MainComponent::changeState(AppState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch(state)
        {
            case Initial:
                stopButton.setEnabled(false);
                playButton.setEnabled(false);
                transportSource.setPosition(0.0);
                break;
                
            case Stopped:
                playButton.setButtonText("PLAY");
                stopButton.setButtonText("STOP");
                stopButton.setEnabled(false);
                transportSource.setPosition(0.0);
                break;
                
            case Starting:
                // starting the transport source will eventually transition to Playing state
                transportSource.start();
                break;
                
            case Playing:
                playButton.setButtonText("PAUSE");
                stopButton.setButtonText("STOP");
                stopButton.setEnabled(true);
                break;
                
            case Pausing:
                // pausing the transport source will eventually transition to Paused state
                transportSource.stop();
                break;
                
            case Paused:
                // change the buttons' text when reaching a paused state
                playButton.setButtonText("RESUME");
                stopButton.setButtonText("RETURN TO ZERO");
                break;
                
            case Stopping:
                transportSource.stop();
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
        else if ((state == Stopping) || (state == Playing))
        {
            changeState(Stopped);
        }
        else if (state == Pausing)
        {
            changeState(Paused);
        }
    }
}

void MainComponent::buttonClicked (juce::Button *button)
{
    if (button == &playButton)
    {
        if ((state == Stopped) || (state == Paused)) // resume play from the stopped or paused state
        {
            changeState(Starting);
        }
        else if (state == Playing) // the play button is a pause button in this scenario
        {
            changeState(Pausing);
        }
    }
    
    if (button == &stopButton)
    {
        if (state == Paused)
        {
            changeState(Stopped);
        }
        else
        {
            changeState(Stopping);
        }
    }
    
    if (button == &loadButton)
    {
        loadFile();
    }
}

void MainComponent::sliderValueChanged (juce::Slider *slider)
{}

void MainComponent::loadFile()
{
    chooser = std::make_unique<juce::FileChooser> ("Select an MP3 file to play...",
                                                   juce::File{}, "*.mp3");
    
    auto chooserFlags = juce::FileBrowserComponent::openMode |
                        juce::FileBrowserComponent::canSelectFiles;
    
    chooser->launchAsync(chooserFlags, [this] (const juce::FileChooser& fc)
    {
        juce::File file = fc.getResult();
        
        if (file != juce::File{})
        {
            auto* reader = formatManager.createReaderFor(file);
            
            if (reader != nullptr)
            {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled(true);
                readerSource.reset(newSource.release());
            }
        }
    });
}
