#ifndef _DTSOUND_
#define	_DTSOUND_
#ifdef DTSOUND_EXPORTS
#define DLLAPI __declspec(dllexport)
#else
#define DLLAPI	__declspec(dllimport)
#pragma comment(lib,"DTSOUND.lib" )
#endif
#include "windows.h"
typedef DWORD (*STREAMCALLBACK)(char *pAddr0,DWORD size0,char *pAddr1, DWORD size1,BOOL bLoop);//the callback for stream BGM buffer
//in the call back, APP need first fill data as many as size 0 to pAddr0, then to pAddr1 if pAddr1 is valid
//the call back  runs in a thread other than main thread of APP
//bLoop: true means the APP should fill data from head of file again. false means it should fill 0 to the rest memory
//return the actual writen bytes. e.g, APP think the sound is end, so only write a few bytes less than size0
//APP determine whether to write more head data after write end data based on bLoop
//if pAddr0==0&&size0==0, it measn from now on, this lib won't call this function any more unless APP re-load music and play again. so APP can safely release music data
struct DTVECTOR
{
	float x;
	float y;
	float z;
};

//world axis system: unit:meter; min resolution: mm; abs max size:10km


//all sound file should be .wav format
//2 channels. but if  need played in 3D space, 1 channel
//16bit
//PCM data
class DTSOUND
{
public:
	virtual BOOL Init(HWND h_TopWindow/*, STREAMCALLBACK OnNeedMoreSoundData,PVOID pSoundFmt*/ )=0;//pSoundFmt is pointer to WAVEFORMATEX
//	virtual  LoadSounds()=0;//load short sounds which will be mixed. the duration  should be<=4 seconds
	virtual	BOOL LoadBkMusicFromDisk(char *pFileName)=0;//the back music is recommended <10 minutes
	//the music format should be PCM,
	//pFileName should be <128 chars is the music file name
	//no need care previous music, system automatically release resource before load new music
	virtual BOOL LoadBkMusicFromMem(const char *pData, DWORD len)=0;
	//pData:total raw data of the file
	//len: the length of all the file data
	virtual BOOL PlayBGM(BOOL bLoop=FALSE)=0;//if failed, APP need re-call in next loop. the calling thread must not exit..   Volume:0~100)
	//thread safe, can be repeatedly called
	virtual void SetBGMVolume(WORD Volume)=0;//0~100
	//thread safe
	virtual BOOL PauseBGMPlay()=0;
	//thread safe
	//equal to stop play
	virtual void LoadSoundSlice(const char** ppFileName,BYTE *pIs3DSound, BYTE *pMaxInstance, BYTE Num)=0;
	//load the short sound which is less than 4 seconds, typically used in game for character, monster
	//must be monolithic channel, include 2D and 3D
	//this functiom automatically free previous resource, then load new ones
	//ppFileName: point number of waveform file names, they are listed in the order specified by user
	//pIs3DSound: store falg indicating whether the sound is 3D. in order of index same as Play(WORD index,WORD Volume)
	//return:pIs3DSound also return the execution result of corresponding load operation
	//pMaxInstance:store input for max instance(played at same time) for each sound, sugguest the max_bum  is<=8 because ear can't dsitinglish and rise CPU overhead
	//pMaxInstance value can be 254 max
	//Num: max 255 sound
	//all word position 1.0 means meter
	virtual BYTE Play(DTVECTOR *pPos, DTVECTOR *pFront, DTVECTOR *pTop,BYTE index, BYTE bLoop=0) = 0;//index:which sound slice to play, in order of LoadSoundSlice
	//return the voice instance num; we know many of same sound can be played at same time
	//user need instance num for future operation like UpdateVoicePos
	//0xff is invalid;
	//argues are the source sound position
	//the pos are ignored if sound is non 3D
	//注意，如果是循环播放建议不适用多个实例播放
	virtual void UpdateVoicePos(BYTE index, BYTE InstanceNum,DTVECTOR *pPos, DTVECTOR *pFront, DTVECTOR *pTop)=0;//if app need the src sound move when voice he can call this 
	//function before play
	//
	virtual BOOL SetEarPos(DTVECTOR *pPos, DTVECTOR *pFront, DTVECTOR *pTop) = 0;//set the EarPos in the 3D space if 3D sound is enabled
	virtual void SetSliceVolume(WORD Volume) = 0;//0~100
											   //thread safe
	virtual void StopSlice(BYTE index, DWORD InstanceNum)=0;
};

DLLAPI BOOL CreateSoundObject(DTSOUND **ppNetObj);
DLLAPI void ReleaseSoundObject(DTSOUND *pNetObj);
#endif