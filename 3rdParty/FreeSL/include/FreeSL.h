
// Product: FreeSL v2.4.0
// By: Lukas Heise
// Date: 2006-05-16
// Website: http://www.lukasheise.com
// Last Updated: 2007-01-30
// Description: OpenAL wrapper. Load and play sound with OpenAL and EAX 2.0

#ifndef __FREESL_H
#define __FREESL_H


#ifdef FREESL_EXPORTS
#define FSLAPI __declspec(dllexport)
#define FSLAPIENTRY __cdecl
#else
#define FSLAPI __declspec(dllimport)
#define FSLAPIENTRY __cdecl
#endif


// Typedefs
typedef unsigned int FSLenum;
typedef unsigned int FSLsound;
typedef unsigned int FSLbuffer;
typedef void FSLerror_callback(const char* str, bool is_critical);	// if (is_critical) then we must shutdown FreeSL


#define FSL_VERSION			int(240)


// Distance Models
#define FSL_INVERSE_DISTANCE                       0xD001
#define FSL_INVERSE_DISTANCE_CLAMPED               0xD002
#define FSL_LINEAR_DISTANCE                        0xD003
#define FSL_LINEAR_DISTANCE_CLAMPED                0xD004
#define FSL_EXPONENT_DISTANCE                      0xD005
#define FSL_EXPONENT_DISTANCE_CLAMPED              0xD006


// Sound Format
#define FSL_FORMAT_MONO8                           0x1100
#define FSL_FORMAT_MONO16                          0x1101
#define FSL_FORMAT_STEREO8                         0x1102
#define FSL_FORMAT_STEREO16                        0x1103


// FSL_EAX_LISTENER_PROPERTIES dwFlags (use: dwFlags = A | B | ....)
#define FSL_EAXLISTENERFLAGS_DECAYTIMESCALE        0x00000001		// reverberation decay time
#define FSL_EAXLISTENERFLAGS_REFLECTIONSSCALE      0x00000002		// reflection level
#define FSL_EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE 0x00000004		// initial reflection delay time
#define FSL_EAXLISTENERFLAGS_REVERBSCALE           0x00000008		// reflections level
#define FSL_EAXLISTENERFLAGS_REVERBDELAYSCALE      0x00000010		// late reverberation delay time
#define FSL_EAXLISTENERFLAGS_DECAYHFLIMIT          0x00000020		// this flag limits high-frequency decay time according to air absorption


// FSL_EAX_BUFFER_PROPERTIES dwFlags (use: dwFlags = A | B | ....)
#define FSL_EAXBUFFERFLAGS_DIRECTHFAUTO				0x00000001		// affects DSPROPERTY_EAXBUFFER_DIRECTHF
#define FSL_EAXBUFFERFLAGS_ROOMAUTO					0x00000002		// affects DSPROPERTY_EAXBUFFER_ROOM
#define FSL_EAXBUFFERFLAGS_ROOMHFAUTO				0x00000004		// affects DSPROPERTY_EAXBUFFER_ROOMHF


// Buffer Environment Material Transmission Presets
// 3 values in this order:
//     1: occlusion (or obstruction)
//     2: occlusion LF Ratio (or obstruction LF Ratio)
//     3: occlusion Room Ratio

// Single window material preset
#define FSL_EAX_MATERIAL_SINGLEWINDOW			(-2800)
#define FSL_EAX_MATERIAL_SINGLEWINDOWLF			0.71f
#define FSL_EAX_MATERIAL_SINGLEWINDOWROOMRATIO	0.43f

// Double window material preset
#define FSL_EAX_MATERIAL_DOUBLEWINDOW			(-5000)
#define FSL_EAX_MATERIAL_DOUBLEWINDOWHF			0.40f
#define FSL_EAX_MATERIAL_DOUBLEWINDOWROOMRATIO	0.24f

// Thin door material preset
#define FSL_EAX_MATERIAL_THINDOOR				(-1800)
#define FSL_EAX_MATERIAL_THINDOORLF				0.66f
#define FSL_EAX_MATERIAL_THINDOORROOMRATIO		0.66f

// Thick door material preset
#define FSL_EAX_MATERIAL_THICKDOOR				(-4400)
#define FSL_EAX_MATERIAL_THICKDOORLF			0.64f
#define FSL_EAX_MATERIAL_THICKDOORROOMRTATION	0.27f

// Wood wall material preset
#define FSL_EAX_MATERIAL_WOODWALL				(-4000)
#define FSL_EAX_MATERIAL_WOODWALLLF				0.50f
#define FSL_EAX_MATERIAL_WOODWALLROOMRATIO		0.30f

// Brick wall material preset
#define FSL_EAX_MATERIAL_BRICKWALL				(-5000)
#define FSL_EAX_MATERIAL_BRICKWALLLF			0.60f
#define FSL_EAX_MATERIAL_BRICKWALLROOMRATIO		0.24f

// Stone wall material preset
#define FSL_EAX_MATERIAL_STONEWALL				(-6000)
#define FSL_EAX_MATERIAL_STONEWALLLF			0.68f
#define FSL_EAX_MATERIAL_STONEWALLROOMRATIO		0.20f

// Curtain material preset
#define FSL_EAX_MATERIAL_CURTAIN				(-1200)
#define FSL_EAX_MATERIAL_CURTAINLF				0.15f
#define FSL_EAX_MATERIAL_CURTAINROOMRATIO		1.00f


// Listener Environments
enum FSL_LISTENER_ENVIRONMENT
{
	FSL_ENVIRONMENT_GENERIC,
    FSL_ENVIRONMENT_PADDEDCELL,
    FSL_ENVIRONMENT_ROOM,
    FSL_ENVIRONMENT_BATHROOM,
    FSL_ENVIRONMENT_LIVINGROOM,
    FSL_ENVIRONMENT_STONEROOM,
    FSL_ENVIRONMENT_AUDITORIUM,
    FSL_ENVIRONMENT_CONCERTHALL,
    FSL_ENVIRONMENT_CAVE,
    FSL_ENVIRONMENT_ARENA,
    FSL_ENVIRONMENT_HANGAR,
    FSL_ENVIRONMENT_CARPETEDHALLWAY,
    FSL_ENVIRONMENT_HALLWAY,
    FSL_ENVIRONMENT_STONECORRIDOR,
    FSL_ENVIRONMENT_ALLEY,
    FSL_ENVIRONMENT_FOREST,
    FSL_ENVIRONMENT_CITY,
    FSL_ENVIRONMENT_MOUNTAINS,
    FSL_ENVIRONMENT_QUARRY,
    FSL_ENVIRONMENT_PLAIN,
    FSL_ENVIRONMENT_PARKINGLOT,
    FSL_ENVIRONMENT_SEWERPIPE,
    FSL_ENVIRONMENT_UNDERWATER,
    FSL_ENVIRONMENT_DRUGGED,
    FSL_ENVIRONMENT_DIZZY,
    FSL_ENVIRONMENT_PSYCHOTIC,

    FSL_ENVIRONMENT_COUNT
};


// Sound System
enum FSL_SOUND_SYSTEM
{
	FSL_SS_EAX2,							// EAX 2.0 (Direct Sound 3D)
	FSL_SS_DIRECTSOUND3D,					// Direct Sound 3D
	FSL_SS_DIRECTSOUND,						// Direct Sound
	FSL_SS_NVIDIA_NFORCE_2,					// nVidia nForce 2
	FSL_SS_CREATIVE_AUDIGY_2,				// Creative Audigy 2
	FSL_SS_MMSYSTEM,						// Microsoft
	FSL_SS_ALUT,							// ALUT
	
	FSL_SS_NOSYSTEM							// no sound system
};


// Structs
struct FSL_EAX_LISTENER_PROPERTIES
{
	long lRoom;						// room effect level at low frequencies
	long lRoomHF;					// room effect high-frequency level re. low frequency level
	float flRoomRolloffFactor;		// like DS3D flRolloffFactor but for room effect
    float flDecayTime;				// reverberation decay time at low frequencies
    float flDecayHFRatio;			// high-frequency to low-frequency decay time ratio
    long lReflections;				// early reflections level relative to room effect
    float flReflectionsDelay;		// initial reflection delay time
    long lReverb;					// late reverberation level relative to room effect
    float flReverbDelay;			// late reverberation delay time relative to initial reflection
    unsigned long dwEnvironment;	// sets all listener properties
    float flEnvironmentSize;		// environment size in meters
    float flEnvironmentDiffusion;	// environment diffusion
    float flAirAbsorptionHF;		// change in level per meter at 5 kHz
    unsigned long dwFlags;			// modifies the behavior of properties
};


struct FSL_EAX_BUFFER_PROPERTIES
{
	long lDirect;					// direct path level
    long lDirectHF;					// direct path level at high frequencies
    long lRoom;						// room effect level
    long lRoomHF;					// room effect level at high frequencies
    float flRoomRolloffFactor;		// like DS3D flRolloffFactor but for room effect
    long lObstruction;				// main obstruction control (attenuation at high frequencies) 
    float flObstructionLFRatio;		// obstruction low-frequency level re. main control
    long lOcclusion;				// main occlusion control (attenuation at high frequencies)
    float flOcclusionLFRatio;		// occlusion low-frequency level re. main control
    float flOcclusionRoomRatio;		// occlusion room effect level re. main control
    long lOutsideVolumeHF;			// outside sound cone level at high frequencies
    float flAirAbsorptionFactor;	// multiplies DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
    unsigned long dwFlags;			// modifies the behavior of properties
};


struct FSL_SOURCE_INFO
{
	unsigned int uiChannels;		// example: 1 = mono, 2 = stereo
	unsigned int uiSampling;		// example: 16 bit
	unsigned int uiFrequency;		// example: 44100 Hz
};


// Initialization, Shutdown and Common Functions
FSLAPI bool FSLAPIENTRY fslInit(FSL_SOUND_SYSTEM val);									// main initialization function
FSLAPI void FSLAPIENTRY fslShutDown();													// shutdown FreeSL and free any allocated memory
FSLAPI void FSLAPIENTRY fslUpdate();													// per-frame update stuff (must be called if there are streaming sounds)
FSLAPI void FSLAPIENTRY fslSetAutoUpdate(bool do_automaticly);							// dont want to call fslUpdate? Then let FreeSL do it for you
FSLAPI int FSLAPIENTRY fslGetVersion();													// get the current version of FreeSL (should be equal to FSL_VERSION)
FSLAPI void FSLAPIENTRY fslSetErrorCallback(FSLerror_callback* func);					// set error callback function
FSLAPI void FSLAPIENTRY fslSleep(float duration);										// delay the execution of the current thread for a number of seconds


// General Functions
FSLAPI void FSLAPIENTRY fslSetVolume(float gain_mult);									// multiply all sounds with the following gain value
FSLAPI int FSLAPIENTRY fslCountSoundsStreaming();										// get the number of sounds sources that are playing and that are stream sounds
FSLAPI int FSLAPIENTRY fslCountSoundsTotal();											// get the total number of sound sources
FSLAPI unsigned long FSLAPIENTRY fslGetSoundMemoryUsage();								// return the memory usage for the sounds (not including stream sounds)


// Sound Creation and Loading
FSLAPI char* FSLAPIENTRY fslGetSupportedSoundFileFormats();								// returns a string with the extension names of the supported sound files
FSLAPI char* FSLAPIENTRY fslGetSupportedSoundStreamFormats();							// returns a string with the extension names of the supported stream sound files
FSLAPI int FSLAPIENTRY fslGetMaxSources();												// return the amount of sound sources supported by the current hardware
FSLAPI FSLsound FSLAPIENTRY fslCreateEmptySound();										// create a sound object that has not buffer
FSLAPI FSLsound FSLAPIENTRY fslCreateDummySound();										// create an empty sound object
FSLAPI FSLsound FSLAPIENTRY fslCreateSoundFromBuffer(FSLbuffer buffer);					// create a sound source using a pre-defined sound buffer
FSLAPI FSLsound FSLAPIENTRY fslLoadSound(const char* strFile);							// load a sound file
FSLAPI FSLsound FSLAPIENTRY fslLoadSoundFromZip(const char* strPackage,
												const char* strFile);					// load sound from *.zip file
FSLAPI FSLsound FSLAPIENTRY fslLoadSoundFromData(const char* strFileFormat,
												 const void* pData, unsigned int size);	// load a sound from data
FSLAPI FSLsound FSLAPIENTRY fslCreateSound(const void* pData, FSLenum format,
										   unsigned int size, unsigned int freq);		// creates a sound from data
FSLAPI FSLsound FSLAPIENTRY fslStreamSound(const char* strFile);						// load sound as a stream
FSLAPI FSLsound FSLAPIENTRY fslCloneSound(FSLsound obj);								// create a new sound source, copy the buffer
FSLAPI void FSLAPIENTRY fslFreeSound(FSLsound obj, bool remove_buffer = true);			// destroy sound source
FSLAPI int FSLAPIENTRY fslFreeAllSounds();												// destroy all loaded/created sound sources (returns remove count)


// Sound Buffer Functions
FSLAPI FSLbuffer FSLAPIENTRY fslGetBufferFromSound(FSLsound obj);						// retrieve the sound's buffer
FSLAPI void FSLAPIENTRY fslSetSoundBuffer(FSLsound obj, FSLbuffer buffer);				// set the buffer for a sound (if you want to replace the current buffer then you have to destroy it)
FSLAPI FSLbuffer FSLAPIENTRY fslCreateBuffer();											// create an empty buffer
FSLAPI void FSLAPIENTRY fslDestroyBuffer(FSLbuffer obj);								// destroy buffer
FSLAPI void FSLAPIENTRY fslSetBufferData(FSLbuffer obj, FSLenum format,
										 const void* pData, unsigned int size,
										 unsigned int freq);							// set the data for a buffer
FSLAPI void FSLAPIENTRY fslSoundQueueBuffers(FSLsound obj, unsigned int num_buffers,
											 FSLbuffer* buffers);						// queue buffers for a sound
FSLAPI void FSLAPIENTRY fslSoundUnqueueBuffers(FSLsound obj, unsigned int num_buffers,
											   FSLbuffer* buffers);						// unqueue buffers for a sound
FSLAPI FSLbuffer FSLAPIENTRY fslLoadBuffer(const char* strFile);						// load a sound buffer from file
FSLAPI FSLbuffer FSLAPIENTRY fslLoadBufferFromZip(const char* strPackage,
												  const char* strFile);					// load a sound buffer from *.zip file
FSLAPI FSLbuffer FSLAPIENTRY fslLoadBufferFromData(const char* strFileFormat,
												   const void* pData, unsigned int size);// load a sound buffer from data
FSLAPI int FSLAPIENTRY fslFreeAllBuffers();												// destroy all loaded/created buffers (returns remove count)
FSLAPI FSL_SOURCE_INFO FSLAPIENTRY fslBufferGetInfo(FSLbuffer obj);						// get info about the sound buffer


// Sound Functions
FSLAPI void FSLAPIENTRY fslSoundPlay(FSLsound obj);										// play the sound
FSLAPI void FSLAPIENTRY fslSoundRewind(FSLsound obj);									// rewind the sound
FSLAPI void FSLAPIENTRY fslSoundStop(FSLsound obj);										// stop the sound from playing
FSLAPI void FSLAPIENTRY fslSoundPause(FSLsound obj);									// pause the sound
FSLAPI bool FSLAPIENTRY fslSoundIsPlaying(FSLsound obj);								// is the sound playing?
FSLAPI bool FSLAPIENTRY fslSoundIsPaused(FSLsound obj);									// is the sound paused?

FSLAPI void FSLAPIENTRY fslSoundSetLooping(FSLsound obj, bool loop_sound);				// should the sound loop?
FSLAPI void FSLAPIENTRY fslSoundSetPositionVec(FSLsound obj, float* position);			// set position (float[3])
FSLAPI void FSLAPIENTRY fslSoundSetPosition(FSLsound obj, float x, float y, float z);	// set position
FSLAPI void FSLAPIENTRY fslSoundSetVelocityVec(FSLsound obj, float* velocity);			// set velocity (float[3])
FSLAPI void FSLAPIENTRY fslSoundSetVelocity(FSLsound obj, float x, float y, float z);	// set velocity
FSLAPI void FSLAPIENTRY fslSoundSetPitch(FSLsound obj, float pitch);					// set pitch (0,1]
FSLAPI void FSLAPIENTRY fslSoundSetGain(FSLsound obj, float gain);						// set gain (0,oo]
FSLAPI void FSLAPIENTRY fslSoundSetMaxDistance(FSLsound obj, float max_distance);		// set max distance
FSLAPI void FSLAPIENTRY fslSoundSetReferenceDistance(FSLsound obj, float ref_distance);	// set reference distance
FSLAPI void FSLAPIENTRY fslSoundSetConeOrientationVec(FSLsound obj, float* direction);	// set cone direction
FSLAPI void FSLAPIENTRY fslSoundSetConeOrientation(FSLsound obj,
												   float x, float y, float z);			// set cone direction
FSLAPI void FSLAPIENTRY fslSoundSetConeInsideAngle(FSLsound obj, unsigned int val);		// set inside angle
FSLAPI void FSLAPIENTRY fslSoundSetConeOutsideAngle(FSLsound obj, unsigned int val);	// set outside angle
FSLAPI void FSLAPIENTRY fslSoundSetConeOutsideVolume(FSLsound obj, int val);			// set outside volume

FSLAPI bool FSLAPIENTRY fslSoundIsLooping(FSLsound obj);								// is sound looping?
FSLAPI void FSLAPIENTRY fslSoundGetPosition(FSLsound obj, 
											float& x, float& y, float& z);				// get position
FSLAPI void FSLAPIENTRY fslSoundGetVelocity(FSLsound obj, 
											float& x, float& y, float& z);				// get velocity
FSLAPI float FSLAPIENTRY fslSoundGetPitch(FSLsound obj);								// get pitch
FSLAPI float FSLAPIENTRY fslSoundGetGain(FSLsound obj);									// get gain
FSLAPI float FSLAPIENTRY fslSoundGetMaxDistance(FSLsound obj);							// get max distance
FSLAPI float FSLAPIENTRY fslSoundGetReferenceDistance(FSLsound obj);					// get reference distance
FSLAPI void FSLAPIENTRY fslSoundGetConeOrientation(FSLsound obj, 
												   float& x, float& y, float& z);		// get cone orientation
FSLAPI unsigned int FSLAPIENTRY fslSoundGetConeInsideAngle(FSLsound obj);				// get cone inside angle
FSLAPI unsigned int FSLAPIENTRY fslSoundGetConeOutsideAngle(FSLsound obj);				// get cone outside angle
FSLAPI int FSLAPIENTRY fslSoundGetConeOutsideVolume(FSLsound obj);						// get cone outside volume
FSLAPI void FSLAPIENTRY fslSoundSetSourceRelative(FSLsound obj, bool is_relative);		// should the sound be relative to the listener
FSLAPI bool FSLAPIENTRY fslSoundIsSourceRelative(FSLsound obj);							// is the sound relative to the listener
FSLAPI void FSLAPIENTRY fslSoundSetRolloff(FSLsound obj, float val);					// set rolloff factor (default: 1.0)
FSLAPI float FSLAPIENTRY fslSoundGetRolloff(FSLsound obj);								// get rolloff factor

FSLAPI int FSLAPIENTRY fslSoundGetBufferLength(FSLsound obj);							// get length in milisec
FSLAPI int FSLAPIENTRY fslSoundGetBufferPosition(FSLsound obj);							// get the current position in milisec
FSLAPI void FSLAPIENTRY fslSoundSetBufferPosition(FSLsound obj, int m);					// set the position in milisec (will be applied on the next fslSoundPlay call)
FSLAPI FSL_SOURCE_INFO FSLAPIENTRY fslSoundGetInfo(FSLsound obj);						// get info about the sound buffer

FSLAPI void FSLAPIENTRY fslSoundPauseAllSounds();										// pause all sounds that are currently playing
FSLAPI void FSLAPIENTRY fslSoundUnPauseAllSounds();										// unpause all sounds that have been paused
FSLAPI void FSLAPIENTRY fslSoundStopAllSounds();										// stop playing all sounds that are playing
FSLAPI void FSLAPIENTRY fslSoundSetPitchAllSounds(float pitch);							// apply the following pitch to all sounds
FSLAPI void FSLAPIENTRY fslSoundSetGainAllSounds(float gain);							// apply the following gain to all sounds


// Listener Functions
FSLAPI void FSLAPIENTRY fslSetListenerPositionVec(float* position);						// set listener position (float[3])
FSLAPI void FSLAPIENTRY fslSetListenerVelocityVec(float* velocity);						// set listener velocity (float[3])
FSLAPI void FSLAPIENTRY fslSetListenerOrientationVec(float* at, float* up);				// set listener orientation (view direction) (float[3],float[3])
FSLAPI void FSLAPIENTRY fslSetListenerPosition(float x, float y, float z);				// set listener position
FSLAPI void FSLAPIENTRY fslSetListenerVelocity(float x, float y, float z);				// set listener velocity
FSLAPI void FSLAPIENTRY fslSetListenerOrientation(float atx, float aty, float atz,
												  float upx, float upy, float upz);		// set listener orientation (view direction)
FSLAPI void FSLAPIENTRY fslSetListenerDistanceModel(FSLenum model);						// set distance model
FSLAPI void FSLAPIENTRY fslSetDopplerParameters(float factor, float velocity);			// set doppler factor and velocity (default: 1.0)
FSLAPI void FSLAPIENTRY fslSetSpeedOfSound(float val);									// set reference speed used in doppler calculation(default: 343.3)
FSLAPI void FSLAPIENTRY fslSetMetersPerUnit(float meters);								// set listener distnace representation


// Environmental Audio Functions
FSLAPI void FSLAPIENTRY fslSetListenerEnvironment(FSL_EAX_LISTENER_PROPERTIES* lpData);						// setup listener environment
FSLAPI void FSLAPIENTRY fslSetListenerEnvironmentPreset(FSL_LISTENER_ENVIRONMENT type);						// set environment from type
FSLAPI void FSLAPIENTRY fslSetListenerDefaultEnvironment();													// set the environment to deafult
FSLAPI FSL_EAX_LISTENER_PROPERTIES FSLAPIENTRY fslGetCurrentListenerEnvironment();							// get current listener environment
FSLAPI FSL_EAX_LISTENER_PROPERTIES FSLAPIENTRY fslLoadListenerEnvironment(const char* strFile);				// load listener environment from script file
FSLAPI FSL_EAX_LISTENER_PROPERTIES FSLAPIENTRY fslLoadListenerEnvironmentFromZip(const char* strPackage, 
																				 const char* strFile);		// load listener environment from script file located in a *.zip file
FSLAPI FSL_EAX_LISTENER_PROPERTIES FSLAPIENTRY fslCreateListenerEnvironment(const char* strData, 
																			unsigned int size);				// create listener environment from script data


// Buffer Environmental Audio Functions
FSLAPI void FSLAPIENTRY fslSetBufferEnvironment(FSL_EAX_BUFFER_PROPERTIES* lpData, FSLsound source);		// setup buffer environment
FSLAPI void FSLAPIENTRY fslSetBufferDefaultEnvironment(FSLsound source);									// set the environment to deafult
FSLAPI FSL_EAX_BUFFER_PROPERTIES FSLAPIENTRY fslGetCurrentBufferEnvironment(FSLsound source);				// get current buffer environment
FSLAPI FSL_EAX_BUFFER_PROPERTIES FSLAPIENTRY fslLoadBufferEnvironment(const char* strFile);					// load buffer environment from script file
FSLAPI FSL_EAX_BUFFER_PROPERTIES FSLAPIENTRY fslLoadBufferEnvironmentFromZip(const char* strPackage, 
																			 const char* strFile);			// load buffer environment from script file located in a *.zip file
FSLAPI FSL_EAX_BUFFER_PROPERTIES FSLAPIENTRY fslCreateBufferEnvironment(const char* strData, 
																		unsigned int size);					// create buffer environment from script data

// Source Manager
// Enables you to create more than fslGetMaxSources of sound objects. This is particulary usefull
// in computer games where you may want to handle more than fslGetMaxSources sound objects.
// A sound will automaticly be considered as free if it is not playing and is not paused.
// Notice that just as with sound streaming you need to call fslUpdate if you use the Source Manager
FSLAPI void FSLAPIENTRY fslInitSourceManager(int max_sources);												// initialize source manager
FSLAPI void FSLAPIENTRY fslShutDownSourceManager();															// shutdown source manager
FSLAPI FSLsound FSLAPIENTRY fslSourceManagerGetSound(FSLbuffer b);											// get the handle to a free FSLsound slot (returns 0 if there arent any free slots)
FSLAPI int FSLAPIENTRY fslSourceManagerCountFreeSlots();													// return the number of free slots

#endif	// __FREESL_H