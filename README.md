# Music Spectrum Analyzer
## Description
A small SMT32 Discovery Board based spectrum analyzer used to pick up audio
frequencies.

The current implementation provides a power spectrum of 10 different frequencies
ranging from 24 Hz to 24,000 Hz. Samples are obtained on 2 offboard I2S MEMS
microphones and summed. The FFT processing is then done using the CMSIS DSP
library. The results are converted to decibel and displayed on a SSD1306 SPI
display using the U8g2 graphics library.

## Git Setup (For Future Contributors)
This repository uses clang-format for consistent code formatting. If
contributions are to be made to this repository, please ensure that the
pre-commit hook is setup. The pre-commit hook will fail commits that do not
follow proper formatting.

After "git clone", add the the .gitconfig file to the local configuration:

```
git config --local include.path ../.gitconfig
```
