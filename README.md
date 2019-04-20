# dsf2flac
Updated conversion tool for DSD files

This tool is a fork from https://code.google.com/archive/p/dsf2flac/
Wav file writing from AudioFile.h is a modified version from https://github.com/adamstark/AudioFile

I wish many thanks to all the authors involved in such tool. For me this is the only free way to play DSD256 files on my dac without getting mad.

Support conversion from dsf or dff to wav files via DoP. DSD256 is supported by converting source dsf or dff to a DoP stream encapsulated in a 2 channels wav file 705600hz, 24 bit as flac actually does not support sample rates above 655350.

# Compiling

download zip or clone. Make directory "build" under dsf2flac root dir then:

ds2flac/build # cmake ..

and:

ds2flac/build # make

# FFmpeg ADI2 DAC pipe example:

dsf2flac -d -w -i "pathtofile" -o -  | ffmpeg -i - -c pcm_s32le -f alsa iec958:CARD=DAC57750571,DEV=0

Well tested up to DSD256 dsf to DoP wav conversion with rme ADI 2 DAC.
