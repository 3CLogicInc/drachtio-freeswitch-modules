#include "fifo_pipe.hpp"
#include <switch.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

FifoPipe::FifoPipe(const char* sessionId)
    : sessionId_(sessionId)
    , sampleRate_(16000)
    , initialized_(false)
{
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, 
        "FifoPipe created for session: %s\n", sessionId);
}

FifoPipe::~FifoPipe() {
    close();
}

void FifoPipe::generateFilePath() {
    char szFilePath[256];
    switch_snprintf(szFilePath, 256, "%s%s%s_fifo%s", 
        SWITCH_GLOBAL_dirs.temp_dir, 
        SWITCH_PATH_SEPARATOR, 
        sessionId_.c_str(), 
        fileType_.c_str());
    filePath_ = szFilePath;
}

bool FifoPipe::initialize(const char* fileType, int sampleRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, 
            "FifoPipe already initialized for session: %s\n", sessionId_.c_str());
        return true;
    }

    fileType_ = fileType;
    sampleRate_ = sampleRate;
    generateFilePath();

    // Open the file in binary append mode
    fileStream_.open(filePath_, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!fileStream_.is_open()) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, 
            "Failed to create FIFO file: %s for session: %s\n", 
            filePath_.c_str(), sessionId_.c_str());
        return false;
    }

    initialized_ = true;
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, 
        "FifoPipe initialized: %s (sample rate: %d) for session: %s\n", 
        filePath_.c_str(), sampleRate_, sessionId_.c_str());
    
    return true;
}

bool FifoPipe::appendAudio(const std::string& audioData) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_ || !fileStream_.is_open()) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, 
            "FifoPipe not initialized or file not open for session: %s\n", 
            sessionId_.c_str());
        return false;
    }

    // Write the audio data to the file
    fileStream_.write(audioData.c_str(), audioData.size());
    fileStream_.flush(); // Ensure data is written immediately
    
    if (fileStream_.fail()) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, 
            "Failed to append audio data to FIFO for session: %s\n", 
            sessionId_.c_str());
        return false;
    }

    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, 
        "Appended %zu bytes to FIFO for session: %s\n", 
        audioData.size(), sessionId_.c_str());
    
    return true;
}

const char* FifoPipe::getFilePath() const {
    return filePath_.c_str();
}

bool FifoPipe::isInitialized() const {
    return initialized_;
}

void FifoPipe::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        return;
    }

    // Close the file stream
    if (fileStream_.is_open()) {
        fileStream_.close();
    }

    // Delete the temporary file
    if (!filePath_.empty()) {
        std::remove(filePath_.c_str());
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, 
            "FifoPipe closed and file removed: %s for session: %s\n", 
            filePath_.c_str(), sessionId_.c_str());
    }

    initialized_ = false;
    filePath_.clear();
}

