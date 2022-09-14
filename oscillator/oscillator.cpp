#include "daisysp.h"
#include "daisy_seed.h"

using namespace daisysp;
using namespace daisy;

static DaisySeed  hw;
static Oscillator osc;


enum AdcChannel {
   pitchKnob = 0,
   ampKnob,
   waveFormKnob,
   NUM_ADC_CHANNELS
};

static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    float sig;
    for(size_t i = 0; i < size; i += 2)
    {
        float knob = hw.adc.GetFloat(pitchKnob);
        osc.SetFreq(knob * 600.0f + 440.0f);

        float amp = hw.adc.GetFloat(ampKnob);
        osc.SetAmp(amp);

        sig = osc.Process();

        // left out
        out[i] = sig;

        // right out
        out[i + 1] = sig;
    }
}

float pitchKnobTest = 0.0;
float ampKnobTest = 0.0;

int main(void)
{
    // initialize seed hardware and oscillator daisysp module
    float sample_rate;
    hw.Configure();
    hw.Init();

    // Send log messages and enable serial messages
    hw.StartLog();

    AdcChannelConfig adcConfig[NUM_ADC_CHANNELS];
    adcConfig[pitchKnob].InitSingle(seed::A0);
    adcConfig[ampKnob].InitSingle(seed::A1);
    adcConfig[waveFormKnob].InitSingle(seed::A2);
    hw.adc.Init(adcConfig, NUM_ADC_CHANNELS);

    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();
    osc.Init(sample_rate);

    // Set parameters for oscillator
    osc.SetWaveform(osc.WAVE_SAW);
    osc.SetFreq(440);
    osc.SetAmp(0.5);

    // start callback
    hw.adc.Start();
    hw.StartAudio(AudioCallback);

    while(1) {
        System::Delay(5000);

        float waveForm = hw.adc.GetFloat(waveFormKnob);
        uint8_t mappedWaveForm = fmap(waveForm, 1, 8);

        //osc.SetWaveform(waveForm);
        hw.Print("waveFormKnob:" FLT_FMT3 "\n", FLT_VAR3(waveForm));
        hw.Print("WAVE_SIN value:" FLT_FMT3 "\n", FLT_VAR3(osc.WAVE_SIN));
        hw.Print("waveForm mapping: %d", mappedWaveForm);

        osc.SetWaveform(mappedWaveForm);
    }
}