/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#ifdef ENABLE_AUDIO_OPENAL

#include "Engine/Resources/Sound.h"
#include "Core/Math/Vector.h"

FWD_DECL_INTRUSIVE(AudioBuffer)
FWD_DECL_INTRUSIVE(AudioContext)

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef int ALenum;

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

class AudioDevice;

// Initializes the audio subsystem.
API_AUDIO void AudioInitialize();

// Shutdowns the audio subsystem.
API_AUDIO void AudioShutdown();

// Gets the main audio device.
API_AUDIO AudioDevice* GetAudioDevice();

// Gets a list of available audio devices.
API_AUDIO bool AudioGetDevices(Array<String>& devices);

// Creates an internal AL device.
API_AUDIO AudioDevice* AudioCreateDevice(const String& device);

// Gets a string with the version of OpenAL.
API_AUDIO const String AudioGetVersion();

// Gets the AL format matching the sound.
API_AUDIO int AudioGetFormat(Sound* sound);

// Return the last error as a char array.
API_AUDIO const char* AudioGetError();

// Checks if the last operation was successful.
API_AUDIO bool AudioCheckError();

/**
 * Audio device to play sound data using OpenAL as backend.
 */

typedef HashMap<AudioBufferPtr> SoundBufferMap; // keyed by Sound*
struct ResourceEvent;

class API_AUDIO AudioDevice
{
	DECLARE_UNCOPYABLE(AudioDevice)

	friend class AudioContext;
	friend class AudioSource;
	friend class AudioBuffer;

public:

	AudioDevice(ALCdevice* device);
	~AudioDevice();

	// Gets a list of available extensions.
	bool getExtensions(Array<String>& extensions);

	// Creates the main context.
	bool createMainContext();

	// Creates a new context.
	AudioContext* createContext();
	
	// Sets the global audio volume
	void setVolume(float volume);

	// Creates a new audio buffer.
	AudioBufferPtr createBuffer();

	// Prepares a buffer for AL usage.
	AudioBufferPtr prepareBuffer(Sound* sound);

	// Gets the main audio context.
	GETTER(MainContext, AudioContext*, mainContext)

protected:

	// Callback when a resource is loaded.
	void onResourceLoaded(const ResourceEvent&);

	// Audio device.
	ALCdevice* device;

	// Maps each sound to a OpenAL sound buffer.
	SoundBufferMap soundBuffers;

	// Main audio context.
	AudioContext* mainContext;
};

//-----------------------------------//

NAMESPACE_ENGINE_END

#endif