#include "daisysp.h"
#include "daisy_seed.h"

using namespace daisysp;
using namespace daisy;

static DaisySeed  hw;
static Oscillator osc;

enum AdcChannel {
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
        sig = osc.Process();
        out[i] = sig;
        out[i + 1] = sig;
    }
}

int main(void)
{
    float sample_rate;
    hw.Configure();
    hw.Init();
    hw.StartLog();
    AdcChannelConfig adcConfig[NUM_ADC_CHANNELS];
    adcConfig[waveFormKnob].InitSingle( seed::A0 );
    hw.adc.Init(adcConfig, NUM_ADC_CHANNELS);
    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();
    osc.Init(sample_rate);
    osc.SetWaveform( osc.WAVE_SIN );
    osc.SetFreq( 440 );
    osc.SetAmp( 1.0 );
    hw.adc.Start();
    hw.StartAudio( AudioCallback );
    while(1) {
        System::Delay( 10 );
        float waveForm = hw.adc.GetFloat(waveFormKnob);
        uint8_t mappedWaveForm = fmap(waveForm, 0, 5);
        hw.PrintLine("waveFormKnob:" FLT_FMT3 "\n", FLT_VAR3(waveForm));
        hw.PrintLine("WAVE_SIN value:" FLT_FMT3 "\n", FLT_VAR3(osc.WAVE_SIN));
        hw.PrintLine("waveForm mapping: %d", mappedWaveForm);
        osc.SetWaveform(mappedWaveForm);
    }
}