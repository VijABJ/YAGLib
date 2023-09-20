FreeSL (Free Sound Library) License and Info
============================================
* FreeSL uses the following libraries: OpenAL, ALUT, zlib, Ogg-Vorbis and Creative EAX 2.0
* FreeSL has been created by Lukas Heise (http://www.lukasheise.com, axodoss@hotmail.com).
* FreeSL was originally created for openFrag (an open source FPS game).
* FreeSL  is free. You do not have to pay me (Lukas Heise) any money for using it in your project
  no mater if the project is free or not.
* You may use FreeSL as you want in your projects. You can change any part of the source code.
* If you change the source code and redistribute FreeSL then you have to let people know
  what changes you made to it.
* FreeSL is provided "AS IS" you are using it at your own risk. It may have bugs, it may be slow,
  it may not work at all.
* You do not have to give me (Lukas Heise) credit for using this library if you do not want to.

version 2.4.0
=============
* added function "fslBufferGetInfo"

version 2.3.0
=============
* added AVI stream file support (fslStreamSound("myfile.avi"))
* in the zip file reading class, fixed the Read functions
* in function fslUpdate() added case AL_INVALID_NAME:... and print error number on default
* made some fixes to "basictimer.h"
* made some tweaks here and there
* fslInit(FSL_SS_NOSYSTEM) will no longer result in a failure, you may initialize and use FreeSL with FSL_SS_NOSYSTEM, it will simply do nothing

version 2.2.0
=============
* function "fslFreeAllSounds" returns the number of sources it removed
* when shuting down FreeSL it will report how many sounds and sound buffers where not destroyed (it will destroy them)
* added function "fslSetAutoUpdate"
* added function "fslGetMaxSources" which returns the maximum number of sound sources you may have at one time
* added function "fslLoadBuffer"
* added function "fslLoadBufferFromZip"
* added function "fslLoadBufferFromData"
* added function "fslCreateEmptySound"
* added function "fslFreeAllBuffers"
* added "Source Manager" functions (see bottom of file FreeSL.h)
* added Sample6.cpp which shows how to use the "Source Manager"

Version 2.1.0
=============
* added function "void fslSoundSetRolloff(FSLsound obj, float val)"
* added function "float fslSoundGetRolloff(FSLsound obj)"
* removed function "void fslSetRolloff(float val)"
* fixed a major bug in the function "fslSetListenerDistanceModel"

Version 2.0.0
=============
* Ogg stream read buffer has changed to 32kb (fsl_stream.h)
* fixed: If a stream finishes playing then there might be a problem when you want to play it again (FreeSL.cpp::fslSoundPlay())
* added: read ALUT versions and report an error if there is a mismatch (FreeSL.cpp::fslInit())
* InitDefaults was not called when context creation was valid (fsl_system.cpp::ContextInit())
* added bStreamCreated (true if stream file was created) to prevent major errors on other stream files if
  a stream file was not created correctly (fsl_stream.cpp::fslStream_OGG::~fslStream_OGG())
* included sample5.cpp into the distribution package
* changed a small typo in sample5.cpp. The sample would exit if fslInit(FSL_SS_EAX2) failed
