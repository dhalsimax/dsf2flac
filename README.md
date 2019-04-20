# dsf2flac
Updated conversion tool for DSD files

This tool is a fork from https://code.google.com/archive/p/dsf2flac/

Support conversion from dsf or dff to wav files via DoP. DSD256 is also supported by converting in a wav file (flac does not support sample rates above 655350).

Compiling

download zip or clone. Make directory build then:

cmake ..

and:

make

Tested up to DSD256 dsf to wav conversion with ADI 2 DAC.
