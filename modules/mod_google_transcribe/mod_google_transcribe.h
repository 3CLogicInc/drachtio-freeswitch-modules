#ifndef __MOD_GOOGLE_TRANSCRIBE_H__
#define __MOD_GOOGLE_TRANSCRIBE_H__

#include <switch.h>
#include <speex/speex_resampler.h>

#include <unistd.h>

#define MAX_SESSION_ID (256)

#define MY_BUG_NAME "google_transcribe"
#define TRANSCRIBE_EVENT_RESULTS "google_transcribe::transcription"
#define TRANSCRIBE_EVENT_END_OF_UTTERANCE "google_transcribe::end_of_utterance"
#define TRANSCRIBE_EVENT_START_OF_TRANSCRIPT "google_transcribe::start_of_transcript"
#define TRANSCRIBE_EVENT_END_OF_TRANSCRIPT "google_transcribe::end_of_transcript"
#define TRANSCRIBE_EVENT_NO_AUDIO_DETECTED "google_transcribe::no_audio_detected"
#define TRANSCRIBE_EVENT_MAX_DURATION_EXCEEDED "google_transcribe::max_duration_exceeded"
#define TRANSCRIBE_EVENT_PLAY_INTERRUPT "google_transcribe::play_interrupt"

#define TRANSCRIBE_EVENT_RESULTS_2 "google_transcribe::transcriptionapi"
#define TRANSCRIBE_EVENT_END_OF_UTTERANCE_2 "google_transcribe::end_of_utterance2api"
#define TRANSCRIBE_EVENT_START_OF_TRANSCRIPT_2 "google_transcribe::start_of_transcriptapi"
#define TRANSCRIBE_EVENT_END_OF_TRANSCRIPT_2 "google_transcribe::end_of_transcriptapi"
#define TRANSCRIBE_EVENT_NO_AUDIO_DETECTED_2 "google_transcribe::no_audio_detectedapi"
#define TRANSCRIBE_EVENT_MAX_DURATION_EXCEEDED_2 "google_transcribe::max_duration_exceededapi"
#define TRANSCRIBE_EVENT_PLAY_INTERRUPT_2 "google_transcribe::play_interruptapi"

// simply write a wave file
//#define DEBUG_TRANSCRIBE 0


#ifdef DEBUG_TRANSCRIBE

/* per-channel data */
struct cap_cb {
	switch_buffer_t *buffer;
	switch_mutex_t *mutex;
	char *base;
    SpeexResamplerState *resampler;
    FILE* fp;
};
#else
/* per-channel data */
typedef void (*responseHandler_t)(switch_core_session_t* session, const char* json);

struct cap_cb {
	switch_mutex_t *mutex;
	char sessionId[MAX_SESSION_ID];
	char *base;
  	SpeexResamplerState *resampler;
	void* streamer;
	responseHandler_t responseHandler;
	switch_thread_t* thread;
	int end_of_utterance;
	int play_file;
};
#endif

#endif