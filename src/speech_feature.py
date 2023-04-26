from python_speech_features import mfcc
from python_speech_features import delta
from python_speech_features import logfbank
import scipy.io.wavfile as wav
import numpy as np

def openWAV(wav_path):
    rate, sig = wav.read(wav_path)
    return sig, rate

def wav_inference(sig, rate):
    mfcc_feat = mfcc(sig, rate)
    d_mfcc_feat = delta(mfcc_feat, 2)
    fbank_feat = logfbank(sig, rate)
    return np.array(mfcc_feat)

if __name__ == '__main__':
    sig, rate = openWAV('../data/wav/BAC009S0002W0124.wav')
    print(wav_inference(sig, rate).shape)
    print(wav_inference(sig, rate).flatten().shape)