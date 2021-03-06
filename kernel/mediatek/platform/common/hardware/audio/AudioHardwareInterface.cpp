

#include <cutils/properties.h>
#include <string.h>
#include <unistd.h>
//#define LOG_NDEBUG 0

#define LOG_TAG "AudioHardwareInterface"
#include <utils/Log.h>
#include <utils/String8.h>

#include "AudioHardwareStub.h"
#include "AudioHardwareGeneric.h"

#ifndef GENERIC_AUDIO
#include "AudioMTKHardware.h"
#endif

#if 0
#undef ALOGV
#define ALOGV(...) ALOGW(__VA_ARGS__)
#endif


#ifdef WITH_A2DP
#include "A2dpAudioInterface.h"
#endif

#ifdef ENABLE_AUDIO_DUMP
#include "AudioDumpInterface.h"
#endif


// change to 1 to log routing calls
#define LOG_ROUTING_CALLS 1

namespace android_audio_legacy
{

#if LOG_ROUTING_CALLS
static const char *routingModeStrings[] = {
    "OUT OF RANGE",
    "INVALID",
    "CURRENT",
    "NORMAL",
    "RINGTONE",
    "IN_CALL",
    "IN_COMMUNICATION"
};

static const char *routeNone = "NONE";

static const char *displayMode(int mode)
{
    if ((mode < AudioSystem::MODE_INVALID) || (mode >= AudioSystem::NUM_MODES))
        return routingModeStrings[0];
    return routingModeStrings[mode + 3];
}
#endif

// ----------------------------------------------------------------------------

AudioHardwareInterface *AudioHardwareInterface::create()
{
    /*
     * FIXME: This code needs to instantiate the correct audio device
     * interface. For now - we use compile-time switches.
     */
    AudioHardwareInterface *hw = 0;
    char value[PROPERTY_VALUE_MAX];

#ifdef GENERIC_AUDIO
    ALOGD("GENERIC_AUDIO - using generic audio driver");
    hw = new AudioHardwareGeneric();
#else
    // if running in emulation - use the emulator driver
    if (property_get("ro.kernel.qemu", value, 0)) {
        ALOGD("Running in emulation - using generic audio driver");
        hw = new AudioHardwareGeneric();
    }
    else {
        ALOGV("Creating Vendor Specific AudioHardware");
        hw = new android::AudioMTKHardware();
        //hw = new AudioHardwareGeneric();
        ALOGD("Creating Vendor Specific AudioHardware = %p", hw);
    }
#endif


    if (hw->initCheck() != NO_ERROR) {
        ALOGW("Using stubbed audio hardware. No sound will be produced.");
        delete hw;
        hw = new AudioHardwareStub();
    }

#ifdef ENABLE_AUDIO_DUMP
    // This code adds a record of buffers in a file to write calls made by AudioFlinger.
    // It replaces the current AudioHardwareInterface object by an intermediate one which
    // will record buffers in a file (after sending them to hardware) for testing purpose.
    // This feature is enabled by defining symbol ENABLE_AUDIO_DUMP.
    // The output file is set with setParameters("test_cmd_file_name=<name>"). Pause are not recorded in the file.
    ALOGV("opening PCM dump interface");
    hw = new AudioDumpInterface(hw);    // replace interface
#endif
    return hw;

    // return NULL;
}

AudioHardwareInterface *AudioHardwareInterface::A2DPcreate()
{
    /*
     * FIXME: This code needs to instantiate the correct audio device
     * interface. For now - we use compile-time switches.
     */
    AudioHardwareInterface *hw = 0;
    char value[PROPERTY_VALUE_MAX];

#ifdef GENERIC_AUDIO
    ALOGD("GENERIC_AUDIO - using generic audio driver");
    hw = new AudioHardwareGeneric();
#else
    // if running in emulation - use the emulator driver
    if (property_get("ro.kernel.qemu", value, 0)) {
        ALOGD("Running in emulation - using generic audio driver");
        hw = new AudioHardwareGeneric();
    }
    else {
        ALOGV("A2DPcreate AudioHardwareGeneric ");
        hw = new AudioHardwareGeneric();
    }
#endif

    if (hw->initCheck() != NO_ERROR) {
        ALOGW("Using stubbed audio hardware. No sound will be produced.");
        delete hw;
        hw = new AudioHardwareStub();
    }
    return hw;
}


AudioStreamOut::~AudioStreamOut()
{
}

// default implementation is unsupported
status_t AudioStreamOut::getNextWriteTimestamp(int64_t *timestamp)
{
    return INVALID_OPERATION;
}

AudioStreamIn::~AudioStreamIn() {}

AudioHardwareBase::AudioHardwareBase()
{
    mMode = 0;
}

status_t AudioHardwareBase::setMode(int mode)
{
#if LOG_ROUTING_CALLS
    ALOGD("setMode(%s)", displayMode(mode));
#endif
    if ((mode < 0) || (mode >= AudioSystem::NUM_MODES))
        return BAD_VALUE;
    if (mMode == mode)
        return ALREADY_EXISTS;
    mMode = mode;
    return NO_ERROR;
}
#ifndef ANDROID_DEFAULT_CODE
status_t AudioHardwareBase::SetEMParameter(void *ptr , int len)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::GetEMParameter(void *ptr , int len)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::SetAudioCommand(int par1, int par2)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::GetAudioCommand(int par1)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::SetAudioData(int par1, size_t len, void *ptr)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::GetAudioData(int par1, size_t len, void *ptr)
{
    return NO_ERROR;
}

status_t AudioHardwareBase::SetACFPreviewParameter(void *ptr , int len)
{
    return NO_ERROR;
}
status_t AudioHardwareBase::SetHCFPreviewParameter(void *ptr , int len)
{
    return NO_ERROR;
}
/////////////////////////////////////////////////////////////////////////
//    for PCMxWay Interface API ...
/////////////////////////////////////////////////////////////////////////
int AudioHardwareBase::xWayPlay_Start(int sample_rate)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayPlay_Stop(void)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayPlay_Write(void *buffer, int size_bytes)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayPlay_GetFreeBufferCount(void)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayRec_Start(int sample_rate)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayRec_Stop(void)
{
    return NO_ERROR;
}
int AudioHardwareBase::xWayRec_Read(void *buffer, int size_bytes)
{
    return NO_ERROR;
}
//add by wendy
int AudioHardwareBase:: ReadRefFromRing(void*buf, uint32_t datasz,void* DLtime)
{
   return NO_ERROR;
}
int AudioHardwareBase:: GetVoiceUnlockULTime(void* DLtime)
{
   return NO_ERROR;
}
int AudioHardwareBase:: SetVoiceUnlockSRC(uint outSR, uint outChannel)
{
   return NO_ERROR;
}

bool AudioHardwareBase:: startVoiceUnlockDL()
{
   return NO_ERROR;
}

bool AudioHardwareBase::stopVoiceUnlockDL()
{
   return NO_ERROR;
}

void AudioHardwareBase::freeVoiceUnlockDLInstance()
{
    return;
}

int AudioHardwareBase::GetVoiceUnlockDLLatency()
{
 return NO_ERROR;
}
bool AudioHardwareBase::getVoiceUnlockDLInstance()
{
 return 0;
}
#endif
// default implementation
status_t AudioHardwareBase::setParameters(const String8 &keyValuePairs)
{
    return NO_ERROR;
}

// default implementation
String8 AudioHardwareBase::getParameters(const String8 &keys)
{
    AudioParameter param = AudioParameter(keys);
    return param.toString();
}

// default implementation
size_t AudioHardwareBase::getInputBufferSize(uint32_t sampleRate, int format, int channelCount)
{
    //if (sampleRate != 8000) {
    //: supporting 16kHz recording
    if (sampleRate > 16000) {
        ALOGW("getInputBufferSize bad sampling rate: %d", sampleRate);
        return 0;
    }
    if (format != AudioSystem::PCM_16_BIT) {
        ALOGW("getInputBufferSize bad format: %d", format);
        return 0;
    }
    if (channelCount != 1) {
        ALOGW("getInputBufferSize bad channel count: %d", channelCount);
        return 0;
    }

    return 320;
}

// default implementation is unsupported
status_t AudioHardwareBase::getMasterVolume(float *volume)
{
    return INVALID_OPERATION;
}

status_t AudioHardwareBase::dumpState(int fd, const Vector<String16> &args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, SIZE, "AudioHardwareBase::dumpState\n");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmMode: %d\n", mMode);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    dump(fd, args);  // Dump the state of the concrete child.
    return NO_ERROR;
}

// ----------------------------------------------------------------------------
extern "C" AudioHardwareInterface *createAudioHardware()
{
    return AudioHardwareInterface::create();
}

}; // namespace android
