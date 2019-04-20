//=======================================================================
/** @file AudioFile.h
 *  @author Adam Stark
 *  @copyright Copyright (C) 2017  Adam Stark
 *
 * This file is part of the 'AudioFile' library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//=======================================================================

#ifndef _AS_AudioFile_h
#define _AS_AudioFile_h

#include <iostream>
#include <vector>
#include <assert.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <iterator>

//=============================================================

template <class T>
class AudioFile {
public:

    //=============================================================
    typedef std::vector<std::vector<T> > AudioBuffer;

    //=============================================================

    /** Constructor */
    AudioFile() {
        bitDepth = 16;
        sampleRate = 44100;
        samples.resize(1);
        samples[0].resize(0);
    }


    //=============================================================

    /** @Returns the sample rate */
    uint32_t getSampleRate() const {
        return sampleRate;
    }

    /** @Returns the number of audio channels in the buffer */
    int getNumChannels() const {
        return (int) samples.size();
    }

    /** @Returns true if the audio file is mono */
    bool isMono() const {
        return getNumChannels() == 1;
    }

    /** @Returns true if the audio file is stereo */
    bool isStereo() const {
        return getNumChannels() == 2;
    }

    /** @Returns the bit depth of each sample */
    int getBitDepth() const {
        return bitDepth;
    }

    /** @Returns the number of samples per channel */
    int getNumSamplesPerChannel() const {
        if (samples.size() > 0)
            return (int) samples[0].size();
        else
            return 0;
    }

    /** @Returns the length in seconds of the audio file based on the number of samples and sample rate */
    double getLengthInSeconds() const {
        return (double) getNumSamplesPerChannel() / (double) sampleRate;
    }

    /** Prints a summary of the audio file to the console */
    void printSummary() const {
        std::cerr << "|======================================|" << std::endl;
        std::cerr << "Num Channels: " << getNumChannels() << std::endl;
        std::cerr << "Num Samples Per Channel: " << getNumSamplesPerChannel() << std::endl;
        std::cerr << "Sample Rate: " << sampleRate << std::endl;
        std::cerr << "Bit Depth: " << bitDepth << std::endl;
        std::cerr << "Length in Seconds: " << getLengthInSeconds() << std::endl;
        std::cerr << "|======================================|" << std::endl;
    }

    //=============================================================

    /** Set the audio buffer for this AudioFile by copying samples from another buffer.
     * @Returns true if the buffer was copied successfully.
     */
    bool setAudioBuffer(AudioBuffer& newBuffer) {
        int numChannels = (int) newBuffer.size();

        if (numChannels <= 0) {
            assert(false && "The buffer your are trying to use has no channels");
            return false;
        }

        std::swap(samples, newBuffer);

        return true;
    }

    /** Sets the audio buffer to a given number of channels and number of samples per channel. This will try to preserve
     * the existing audio, adding zeros to any new channels or new samples in a given channel.
     */
    void setAudioBufferSize(int numChannels, int numSamples) {
        samples.resize(numChannels);
        setNumSamplesPerChannel(numSamples);
    }

    /** Sets the number of samples per channel in the audio buffer. This will try to preserve
     * the existing audio, adding zeros to new samples in a given channel if the number of samples is increased.
     */
    void setNumSamplesPerChannel(int numSamples) {
        int originalSize = getNumSamplesPerChannel();

        for (int i = 0; i < getNumChannels(); i++) {
            samples[i].resize(numSamples);

            // set any new samples to zero
            if (numSamples > originalSize) {
                T Tmp;
                memset(&Tmp, 0, sizeof (Tmp));
                std::fill(samples[i].begin() + originalSize, samples[i].end(), Tmp);
            }
        }
    }

    /** Sets the number of channels. New channels will have the correct number of samples and be initialised to zero */
    void setNumChannels(int numChannels) {
        int originalNumChannels = getNumChannels();
        int originalNumSamplesPerChannel = getNumSamplesPerChannel();

        samples.resize(numChannels);

        // make sure any new channels are set to the right size
        // and filled with zeros
        if (numChannels > originalNumChannels) {
            for (int i = originalNumChannels; i < numChannels; i++) {
                samples[i].resize(originalNumSamplesPerChannel);
                T Tmp;
                memset(&Tmp, 0, sizeof (Tmp));
                std::fill(samples[i].begin(), samples[i].end(), Tmp);
            }
        }
    }

    /** Sets the bit depth for the audio file. If you use the save() function, this bit depth rate will be used */
    void setBitDepth(int numBitsPerSample) {
        bitDepth = numBitsPerSample;
    }

    /** Sets the sample rate for the audio file. If you use the save() function, this sample rate will be used */
    void setSampleRate(uint32_t newSampleRate) {
        sampleRate = newSampleRate;
    }


    //=============================================================
    /** A vector of vectors holding the audio samples for the AudioFile. You can
     * access the samples by channel and then by sample index, i.e:
     *
     *      samples[channel][sampleIndex]
     */
    AudioBuffer samples;

    //=============================================================

    bool save(std::string filePath) {
        std::vector<uint8_t> fileData;


        int32_t dataChunkSize = getNumSamplesPerChannel() * (getNumChannels() * bitDepth / 8);

        // -----------------------------------------------------------
        // HEADER CHUNK
        addStringToFileData(fileData, "RIFF");

        // The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
        // chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
        int32_t fileSizeInBytes = 4 + 24 + 8 + dataChunkSize;
        addInt32ToFileData(fileData, fileSizeInBytes);

        addStringToFileData(fileData, "WAVE");

        // -----------------------------------------------------------
        // FORMAT CHUNK
        addStringToFileData(fileData, "fmt ");
        addInt32ToFileData(fileData, 16); // format chunk size (16 for PCM)
        addInt16ToFileData(fileData, 1); // audio format = 1
        addInt16ToFileData(fileData, (int16_t) getNumChannels()); // num channels
        addInt32ToFileData(fileData, (int32_t) sampleRate); // sample rate

        int32_t numBytesPerSecond = (int32_t) ((getNumChannels() * sampleRate * bitDepth) / 8);
        addInt32ToFileData(fileData, numBytesPerSecond);

        int16_t numBytesPerBlock = getNumChannels() * (bitDepth / 8);
        addInt16ToFileData(fileData, numBytesPerBlock);

        addInt16ToFileData(fileData, (int16_t) bitDepth);

        // -----------------------------------------------------------
        // DATA CHUNK
        addStringToFileData(fileData, "data");
        addInt32ToFileData(fileData, dataChunkSize);

        std::ofstream* out = NULL;

        if (filePath != "-") {
            out = new ofstream(filePath, std::ios::binary);
            if (out == NULL) {
                return false;
            }
            if (!out->is_open()) {
                delete out;
                return false;
            }
        }

        if (out != NULL) {
            out->write((char*) fileData.data(), fileData.size());
        } else {
            std::cout.write((char*) fileData.data(), fileData.size());
        }

        for (int i = 0; i < getNumSamplesPerChannel(); i++) {
            for (int channel = 0; channel < getNumChannels(); channel++) {

                if (out != NULL) {
                    out->write((char*) &samples[channel][i], sizeof (T));
                } else {
                    std::cout.write((char*) &samples[channel][i], sizeof (T));
                }
            }
        }
        if (out != NULL) {
            out->close();
            delete out;
        }

        return true;
    }


private:

    //=============================================================

    enum class Endianness {
        LittleEndian,
        BigEndian
    };


    //=============================================================

    void clearAudioBuffer() {
        for (size_t i = 0; i < samples.size(); i++) {
            samples[i].clear();
        }

        samples.clear();
    }


    //=============================================================

    int32_t fourBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian) {
        int32_t result;

        if (endianness == Endianness::LittleEndian)
            result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
        else
            result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) | source[startIndex + 3];

        return result;
    }

    int16_t twoBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian) {
        int16_t result;

        if (endianness == Endianness::LittleEndian)
            result = (source[startIndex + 1] << 8) | source[startIndex];
        else
            result = (source[startIndex] << 8) | source[startIndex + 1];

        return result;
    }

    int getIndexOfString(std::vector<uint8_t>& source, std::string stringToSearchFor) {
        int index = -1;
        int stringLength = (int) stringToSearchFor.length();

        for (size_t i = 0; i < source.size() - stringLength; i++) {
            std::string section(source.begin() + i, source.begin() + i + stringLength);

            if (section == stringToSearchFor) {
                index = i;
                break;
            }
        }

        return index;
    }


    //=============================================================

    void addStringToFileData(std::vector<uint8_t>& fileData, std::string s) {
        for (size_t i = 0; i < s.length(); i++)
            fileData.push_back((uint8_t) s[i]);
    }

    void addInt32ToFileData(std::vector<uint8_t>& fileData, int32_t i, Endianness endianness = Endianness::LittleEndian) {
        uint8_t bytes[4];

        if (endianness == Endianness::LittleEndian) {
            bytes[3] = (i >> 24) & 0xFF;
            bytes[2] = (i >> 16) & 0xFF;
            bytes[1] = (i >> 8) & 0xFF;
            bytes[0] = i & 0xFF;
        } else {
            bytes[0] = (i >> 24) & 0xFF;
            bytes[1] = (i >> 16) & 0xFF;
            bytes[2] = (i >> 8) & 0xFF;
            bytes[3] = i & 0xFF;
        }

        for (int i = 0; i < 4; i++)
            fileData.push_back(bytes[i]);
    }

    void addInt16ToFileData(std::vector<uint8_t>& fileData, int16_t i, Endianness endianness = Endianness::LittleEndian) {
        uint8_t bytes[2];

        if (endianness == Endianness::LittleEndian) {
            bytes[1] = (i >> 8) & 0xFF;
            bytes[0] = i & 0xFF;
        } else {
            bytes[0] = (i >> 8) & 0xFF;
            bytes[1] = i & 0xFF;
        }

        fileData.push_back(bytes[0]);
        fileData.push_back(bytes[1]);
    }


    //=============================================================
    uint32_t sampleRate;
    int bitDepth;
};


#endif /* AudioFile_h */
