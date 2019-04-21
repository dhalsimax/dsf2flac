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
        numSamples = 0;
        numChannels = 0;
    }


    //=============================================================

    /** @Returns the sample rate */
    uint32_t getSampleRate() const {
        return sampleRate;
    }

    /** @Returns the number of audio channels in the buffer */
    uint16_t getNumChannels() const {
        return numChannels;
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
    uint16_t getBitDepth() const {
        return bitDepth;
    }

    /** @Returns the number of samples per channel */
    size_t getNumSamplesPerChannel() const {
        return numSamples;
    }

    /** @Returns the length in seconds of the audio file based on the number of samples and sample rate */
    double getLengthInSeconds() const {
        return (double) getNumSamplesPerChannel() / (double) sampleRate;
    }

    /** Prints a summary of the audio file to the console */
    void printSummary() const {
        std::cerr << "|=============WAVE FILE SUMMARY=============|" << std::endl;
        std::cerr << "Num Channels: " << getNumChannels() << std::endl;
        std::cerr << "Num Samples Per Channel: " << getNumSamplesPerChannel() << std::endl;
        std::cerr << "Sample Rate: " << getSampleRate() << std::endl;
        std::cerr << "Bit Depth: " << getBitDepth() << std::endl;
        std::cerr << "Length in Seconds: " << getLengthInSeconds() << std::endl;
        std::cerr << "Sample size in bytes: " << sizeof (T) << std::endl;
        std::cerr << "|===========================================|" << std::endl;
    }

    //=============================================================

    /** Sets the number of samples per channel in the audio buffer. This will try to preserve
     * the existing audio, adding zeros to new samples in a given channel if the number of samples is increased.
     */
    void setNumSamples(uint32_t aNumSamples) {
        numSamples = aNumSamples;
    }

    /** Sets the number of channels. New channels will have the correct number of samples and be initialised to zero */
    void setNumChannels(uint32_t aNumChannels) {
        numChannels = aNumChannels;
    }

    /** Sets the bit depth for the audio file. If you use the save() function, this bit depth rate will be used */
    void setBitDepth(uint16_t numBitsPerSample) {
        bitDepth = numBitsPerSample;
    }

    /** Sets the sample rate for the audio file. If you use the save() function, this sample rate will be used */
    void setSampleRate(uint32_t newSampleRate) {
        sampleRate = newSampleRate;
    }


    //=============================================================

    void getHeaderData(std::vector<uint8_t>& fileData) {
        fileData.clear();


        size_t dataChunkSize = getNumSamplesPerChannel() * getNumChannels() * getBitDepth() / 8;

        // -----------------------------------------------------------
        // HEADER CHUNK
        addStringToFileData(fileData, "RIFF");


        size_t fileSizeInBytes = dataChunkSize + 44 - 8;
        addInt32ToFileData(fileData, fileSizeInBytes);

        addStringToFileData(fileData, "WAVE");

        // -----------------------------------------------------------
        // FORMAT CHUNK
        addStringToFileData(fileData, "fmt ");
        addInt32ToFileData(fileData, 16); // format chunk size (16 for PCM)
        addInt16ToFileData(fileData, 1); // audio format = 1
        addInt16ToFileData(fileData, getNumChannels()); // num channels
        addInt32ToFileData(fileData, getSampleRate()); // sample rate

        uint32_t numBytesPerSecond = getNumChannels() * getSampleRate() * getBitDepth() / 8;
        addInt32ToFileData(fileData, numBytesPerSecond);

        uint16_t numBytesPerBlock = getNumChannels() * getBitDepth() / 8;

        addInt16ToFileData(fileData, numBytesPerBlock);

        addInt16ToFileData(fileData, getBitDepth());


        // -----------------------------------------------------------
        // DATA CHUNK
        addStringToFileData(fileData, "data");
        addInt32ToFileData(fileData, dataChunkSize);


    }

    bool save(std::string filePath, const AudioBuffer& samples) {

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

        std::vector<uint8_t> fileData;

        getHeaderData(fileData);

        setNumChannels(samples.size());
        if (getNumChannels() == 0) {
            return false;
        }

        setNumSamples(samples[0].size());

        if (out != NULL) {
            out->write((char*) fileData.data(), fileData.size());
        } else {
            std::cout.write((char*) fileData.data(), fileData.size());
        }

        for (uint32_t i = 0; i < getNumSamplesPerChannel(); i++) {
            for (uint16_t channel = 0; channel < getNumChannels(); channel++) {

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
    uint16_t bitDepth;
    size_t numSamples;
    uint16_t numChannels;
};


#endif /* AudioFile_h */
