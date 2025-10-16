#ifndef __FIFO_PIPE_H__
#define __FIFO_PIPE_H__

#include <switch.h>
#include <string>
#include <mutex>
#include <fstream>

class FifoPipe {
public:
    FifoPipe(const char* sessionId);
    ~FifoPipe();

    // Initialize the FIFO pipe with the given file type and sample rate
    bool initialize(const char* fileType, int sampleRate);

    // Append audio data to the FIFO
    bool appendAudio(const std::string& audioData);

    // Get the file path of the FIFO
    const char* getFilePath() const;

    // Check if the FIFO is initialized
    bool isInitialized() const;

    // Close and clean up the FIFO
    void close();

private:
    std::string sessionId_;
    std::string filePath_;
    std::string fileType_;
    int sampleRate_;
    bool initialized_;
    std::ofstream fileStream_;
    std::mutex mutex_;

    // Generate the file path based on session ID and file type
    void generateFilePath();
};

#endif // __FIFO_PIPE_H__

