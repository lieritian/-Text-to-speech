/*----------------------------------------------+
 |												|
 |	com_iflytek_tts_service_TtsJni.c.c 	        |
 |												|
 |		Copyright (c) 1999-2009, iFLYTEK Ltd.	|
 |		All rights reserved.					|
 |												|
 +----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>
#include <pthread.h>
#include "Inc/ivTTS.h"
//#include "ivTTSInUse.h"

#define LOGD(text) __android_log_write(ANDROID_LOG_DEBUG,"TtsServiceJni",text)

#define FILE_TTS_LOG "/sdcard/TTSlog.log"

#define FILE_TTS_RES  "/sdcard/Resource.irf"
#define MAX_PATH 256


/* heap size */
#define TTS_HEAP_SIZE		1024*1024
/* constant for cache allocation */
#define TTS_CACHE_SIZE	0
#define TTS_CACHE_COUNT	0
#define TTS_CACHE_EXT		0

/* resource file count */
#define TTS_RES_COUNT 	1

/* tts save log file */
#define TTS_SAVE_LOG    0

static pthread_t g_JavaSpeakThreadId;

typedef struct _StTtsParams{
	char is_set_pitch;
	char is_set_speed;
	char is_set_volume;
//	char is_set_en_role;
//	char is_set_language;
	ivUInt32 pitch;
	ivUInt32 speed;
	ivUInt32 volume;
//	ivUInt32 en_role;
//	ivUInt32 language;
}StTtsParams;

typedef struct _StJavaTtsClass{
	JNIEnv *env;
	jclass clazzTts;
	jmethodID onJniOutData;
	jmethodID onJniWatchCB;
	ivUInt32 nProcBegin;

}StJavaTtsClass;

enum {
	PLAY_STATUS_STOP = 0,
	PLAY_STATUS_SPEAK,
	PLAY_STATUS_FINISH
};

static ivHTTS m_hTTS = NULL;                     //Tts 实例 指针pointer
static ivPByte m_pHeap  = NULL;                  //TTS使用的堆 pointer
static ivPResPackDescExt m_pResPackDesc = NULL;  //资源结构 pointer
static StJavaTtsClass m_JavaTtsClass = {0};      //java 方法及jni env
static ivTTTsUserInfo	m_stUserInfo = {0};               //TTS 日志使用

static ivBool b_CreateStatus = ivFalse;		//创建对象状态
static ivBool b_StopAudioStatus = ivTrue;	//停止朗读状态
static ivTTSErrID m_lastErrId = 0;               //最后错误值
static int m_TtsPlayStatus = PLAY_STATUS_FINISH; //TTS 合成状态

static StTtsParams m_stCurParam = {0};           //当前设置
static StTtsParams m_stDefParam = {0};           //默认设置
static char m_sResFilename[MAX_PATH] = {0};      //资源文件全路径

//1. 读资源Callback Function
static ivBool ivCall ReadResCB(
		ivPointer		pParameter,		/* [in] user callback parameter */
		ivPointer		pBuffer,		/* [out] read resource buffer */
		ivResAddress	iPos,			/* [in] read start position */
		ivResSize		nSize )			/* [in] read size */
{
	memcpy((char* )pBuffer,(char* )(pParameter+iPos), nSize);
	return 1;
}

//2. 参数改变 Callback Function
static ivTTSErrID ivCall ParamChCB(
		ivPointer		pParameter,		/* [in] user callback parameter */
		ivUInt32		nParamID,		/* [in] parameter id */
		ivUInt32		nParamValue) 	/* [in] parameter value */
{
	LOGD("ParamChCB");
	return ivTTS_ERR_OK;
}

//3. 流式输入 Callback Function
static ivTTSErrID ivCall InputCB(
		ivPointer		pParameter,		/* [in] user callback parameter */
		ivPointer		pText,			/* [out] input text buffer */
		ivSize ivPtr	pnSize )		/* [out] input text size */
{
	LOGD("InputCB");
	return ivTTS_ERR_OK;
}

//4. 进度消息回调
static ivTTSErrID ivCall WatchCB(
		ivPointer		pParameter,		/* [in] user callback parameter */
		ivUInt16		nSylType,		/* [in] syllable type */
		ivCPointer		pcSylText,		/* [in] syllable text buffer */
		ivSize			nTextLen,		/* [in] syllable text length */
		ivCPointer		pcSylPhone,		/* [in] syllable phoneme buffer */
		ivSize			nPhoneLen )		/* [in] syllable phoneme length */
{
	// 一句结束

	//if ( ivTTS_SYL_ENDSENT == nSylType){
		//LOGD("ivTTS_SYL_ENDSENT");
	//}

	return ivTTS_ERR_OK;
}


//5. 合成数据输出回调
static ivTTSErrID ivCall OutputCB(
		ivPointer		pParameter,		/* [in] user callback parameter */
		ivUInt16		nCode,			/* [in] output data code */
		ivCPointer		pcData,			/* [in] output data buffer */
		ivSize			nSize )			/* [in] output data size */
{
	if(nSize == 0) {
		LOGD("OutputCB nSize == 0.");
		return ivTTS_ERR_OK;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, "TtsServiceJni22", "OutputCB----nSize: %u.", nSize);
	}
	__android_log_print(ANDROID_LOG_DEBUG, "TtsServiceJni22", "OutputCB----nSize: %ld.", (jint)(nSize));
	__android_log_print(ANDROID_LOG_DEBUG, "TtsServiceJni22", "OutputCB----nSize: 0x%x.", (jint)(nSize));

	pthread_t hThreadId = pthread_self();
	if(!pthread_equal(hThreadId, g_JavaSpeakThreadId)) {
		LOGD("OutputCB pthread not same id, this should never happend.");
	} else {
		LOGD("OutputCB pthread same id.");
	}

	LOGD("OutputCB 1");
	if( PLAY_STATUS_STOP == m_TtsPlayStatus){
		LOGD("OutputCB  play status is stop, return in outputcb");
		return ivTTS_ERR_EXIT;
	}

	if (m_JavaTtsClass.env && m_JavaTtsClass.clazzTts
		&& m_JavaTtsClass.onJniOutData && b_StopAudioStatus){
		jbyte * m_pBuff = NULL;
		__android_log_print(ANDROID_LOG_DEBUG, "Before new TtsServiceJni22", "OutputCB----nSize: %ld.", (jint)(nSize));
		m_pBuff =  (*m_JavaTtsClass.env)->NewByteArray(m_JavaTtsClass.env,(jint)(nSize));
		__android_log_print(ANDROID_LOG_DEBUG, "After new TtsServiceJni22", "OutputCB----nSize: %ld.", (jint)(nSize));
		if(m_pBuff == NULL) {
			LOGD("OutputCB m_pBuff is null");
		}
		__android_log_print(ANDROID_LOG_DEBUG, "TtsServiceJni11111", "OutputCB----nSize: %ld.", (jint)(nSize));
		__android_log_print(ANDROID_LOG_DEBUG, "TtsServiceJni22222", "OutputCB----nSize: %d.", (jint)(nSize));
		//LogCommRes("set def", nSize,nSize);
		if (NULL != m_pBuff){
			LOGD("OutputCB 3");
			(*m_JavaTtsClass.env)->SetByteArrayRegion(m_JavaTtsClass.env
				,m_pBuff, 0, (jint)(nSize), (jbyte*)pcData);
			LOGD("OutputCB 4");
			(*m_JavaTtsClass.env)->CallStaticVoidMethod(m_JavaTtsClass.env
				,m_JavaTtsClass.clazzTts,m_JavaTtsClass.onJniOutData,(jint)(nSize),m_pBuff);
			LOGD("OutputCB 5");
			(*m_JavaTtsClass.env)->DeleteLocalRef(m_JavaTtsClass.env
				,m_pBuff);
		}
	}else{
		LOGD("OutputCB but java method null");
	}
	LOGD("OutputCB 2");
	return ivTTS_ERR_OK;
}

//6. 合成进度回调
static ivTTSErrID ivCall ProgressCB(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivUInt32		iProcBegin,		/* [in] current processing position */
	ivUInt32		nProcLen )		/* [in] current processing length */
{
/*
	jbyte * m_pBuffTest = NULL;
	m_pBuffTest =  (*m_JavaTtsClass.env)->NewByteArray(m_JavaTtsClass.env,(jint)(100));
	if(m_pBuffTest == NULL) {
		LOGD("OutputCB m_pBuffTest(100) is null");
	} else {
		LOGD("OutputCB m_pBuffTest(100) is not null");
	}
*/
	pthread_t hThreadId = pthread_self();
	if(!pthread_equal(hThreadId, g_JavaSpeakThreadId)) {
		LOGD("ProgressCB pthread not same id, this should never happend.");
	} else {
		LOGD("ProgressCB pthread same id.");
	}
	LOGD("ProgressCB 1");
	if ( 0 == nProcLen ){
		if (iProcBegin != m_JavaTtsClass.nProcBegin){
			m_JavaTtsClass.nProcBegin = iProcBegin;
			if ( m_JavaTtsClass.onJniWatchCB){
				(*m_JavaTtsClass.env)->CallStaticVoidMethod(m_JavaTtsClass.env
					,m_JavaTtsClass.clazzTts,m_JavaTtsClass.onJniWatchCB
					,m_JavaTtsClass.nProcBegin);

			}else{
				LOGD("WatchCB but java method null");
			}
		}
	}
	LOGD("ProgressCB 2");

	if (m_JavaTtsClass.env && m_JavaTtsClass.clazzTts
	&& m_JavaTtsClass.onJniOutData && b_StopAudioStatus){
		char buf[1024] = {0};
		int nSize = 16;
		jbyte * m_pBuff = NULL;
		m_pBuff =  (*m_JavaTtsClass.env)->NewByteArray(m_JavaTtsClass.env,(jint)(nSize));
		if(m_pBuff == NULL)
		{
			LOGD("OutputCB m_pBuff is null");

		}

		//LogCommRes("set def", nSize,nSize);

		if (NULL != m_pBuff){
			LOGD("OutputCB 3");
			(*m_JavaTtsClass.env)->SetByteArrayRegion(m_JavaTtsClass.env
				,m_pBuff, 0, nSize, (jbyte*)buf);
			LOGD("OutputCB 4");
			(*m_JavaTtsClass.env)->CallStaticVoidMethod(m_JavaTtsClass.env
				,m_JavaTtsClass.clazzTts,m_JavaTtsClass.onJniOutData,nSize,m_pBuff);

			LOGD("OutputCB 5");
			(*m_JavaTtsClass.env)->DeleteLocalRef(m_JavaTtsClass.env ,m_pBuff);
		}
	}

	return ivTTS_ERR_OK;
}

//7. 输出日志回调
static ivUInt16 ivCall TtsLogCB
(
	 ivPointer	pParameter,			/* [out] user callback parameter */
	 ivCPointer	pcData,				/* [in] output data buffer */
	 ivSize		nSize,				/* [in] output data size */
	 ivSize		nCount				/* [in] output data count */
 )
{
	FILE *fid = (FILE *)pParameter;
	if  (NULL == fid ){
		LOGD("TtsLogCB file is null");
		return ivTTS_ERR_OK;
	}
	ivSize nWriten = fwrite(pcData,1,nSize,fid);
	if (nWriten == nSize){
		LOGD("TtsLog write ok");
		return ivTTS_ERR_OK;
	}
	return ivTTS_ERR_FAILED;;
}

//JniInit JniDinit  因为JniTtsSpeak是另外的线程来做的
static void JniInit(JNIEnv *env)
{

	pthread_t hThreadId = pthread_self();
	if(!pthread_equal(hThreadId, g_JavaSpeakThreadId)) {
		LOGD("pthread not same id, this should never happend.");
	} else {
		LOGD("pthread same id.");
	}


	jclass classTmp =  (*env)->FindClass(env,"com/iflytek/tts/TtsService/AudioData");

	m_JavaTtsClass.clazzTts = (*env)->NewGlobalRef(env,classTmp);
	m_JavaTtsClass.env = env;

	if (NULL != m_JavaTtsClass.clazzTts ){
		m_JavaTtsClass.onJniOutData  = (*env)->GetStaticMethodID(env
			, m_JavaTtsClass.clazzTts,"onJniOutData","(I[B)V");
		m_JavaTtsClass.onJniWatchCB  = (*env)->GetStaticMethodID(env
			, m_JavaTtsClass.clazzTts,"onJniWatchCB","(I)V");
		LOGD("JniInit  ok");
	}
	m_JavaTtsClass.nProcBegin = 0;
}

static void JniDinit()
{

	if (NULL != m_JavaTtsClass.clazzTts){
		(*m_JavaTtsClass.env)->DeleteGlobalRef(m_JavaTtsClass.env,m_JavaTtsClass.clazzTts);
		m_JavaTtsClass.clazzTts = NULL;
	}
	m_JavaTtsClass.env = NULL;
	m_JavaTtsClass.onJniOutData = 0;
	m_JavaTtsClass.onJniWatchCB = 0;
	LOGD("JniDInit  ok");
}


// Free heap and ResPack
static void TtsDInit()
{

	if ((PLAY_STATUS_FINISH != m_TtsPlayStatus)&&(PLAY_STATUS_STOP != m_TtsPlayStatus)){
		LOGD("TtsDInit --but tts is runing--- ");
	}
	while (PLAY_STATUS_FINISH != m_TtsPlayStatus){
		usleep(50);
	}

	if (NULL != m_hTTS){
		m_lastErrId = ivTTS_Destroy(&m_hTTS);
		b_CreateStatus = ivFalse;
		m_hTTS = NULL;
	}
	if (NULL != m_pHeap){
		free(m_pHeap);
		m_pHeap = NULL;
	}
	if (NULL != m_pResPackDesc){
		free(m_pResPackDesc) ;
		m_pResPackDesc = NULL;
	}
	LOGD("TtsDInit ---OK-- ");
}


//Init  heap and ResPack
static void TtsInit(char *filename)
{
	//1. already create
	LOGD("TtsInit --1-- ");
	TtsDInit();
	LOGD("TtsInit --2-- ");
	//2. malloc memory
	int allSize = TTS_HEAP_SIZE +
	(TTS_CACHE_SIZE + 1 ) * (TTS_CACHE_COUNT + TTS_CACHE_EXT )  * sizeof (ivInt8);
	m_pHeap = (ivPByte)malloc (allSize);

	if (NULL == m_pHeap ){
		LOGD("TtsInit --1.1-- malloc m_pHeap null");
		return;
	}
	m_pResPackDesc = (ivPResPackDescExt)malloc(sizeof(ivTResPackDescExt) * TTS_RES_COUNT);
	if (NULL == m_pResPackDesc){
		LOGD("TtsInit --1.2-- malloc m_pResPackDesc null");
		free(m_pHeap);
		m_pHeap = NULL;
		return ;
	}

	memset(m_pHeap,0,allSize);
	memset(m_pResPackDesc,0,sizeof(ivTResPackDescExt) * TTS_RES_COUNT);
	LOGD("TtsInit --3-- ");
	//3. set recoure file
	FILE *fpRes = fopen(filename,"rb");
	LOGD(filename);
	if (NULL == fpRes){
		LOGD("open failed");
	}
	fseek(fpRes, 0, SEEK_END);
	LOGD("TtsInit --4-- ");
	ivUInt32 nSize = ftell(fpRes);
	ivPByte pBuffer = (ivPByte)malloc(nSize+2);
	fseek(fpRes, 0, SEEK_SET);
	fread(pBuffer, nSize, 1, fpRes);
	fclose(fpRes);
	LOGD("TtsInit --5-- ");

	if (NULL == fpRes){
		LOGD("TtsInit --1.3-- open res file failed");
		LOGD(filename);
	}
	m_pResPackDesc->pCBParam = (ivPointer)pBuffer;
	m_pResPackDesc->pfnRead = ReadResCB;
	m_pResPackDesc->pCacheBlockIndex = 0;//m_pHeap + TTS_HEAP_SIZE;
	m_pResPackDesc->pCacheBuffer = 0;//m_pHeap + TTS_HEAP_SIZE + TTS_CACHE_COUNT + TTS_CACHE_EXT;
	m_pResPackDesc->nCacheBlockSize = 0;//TTS_CACHE_SIZE;
	m_pResPackDesc->nCacheBlockCount = 0;//TTS_CACHE_COUNT;
	m_pResPackDesc->nCacheBlockExt = 0;//TTS_CACHE_EXT;

	//4. set log file
#if TTS_SAVE_LOG
	FILE *fpCBLog = fopen(FILE_TTS_LOG,"wb+");
	m_stUserInfo.pCBLogParameter = (ivPointer)fpCBLog;
	m_stUserInfo.pfnLog = TtsLogCB;
#endif
	LOGD("TtsInit --1.4- Ok");
}

static void LogCommRes(char *com,int set,int ret)
{
	char Message[256] = {0};
	sprintf(Message, "Command: %s set value=%d  ret=%d",com,set,ret);
	LOGD(Message);
}

static void TtsGetParams(StTtsParams* pStTtsParams)
{
	ivUInt32 param = 0;

	if( NULL == m_hTTS || NULL == pStTtsParams ){
		LOGD("NULL m_hTTS in TtsGetParam");
		return;
	}

	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_VOICE_PITCH, &param);
	LogCommRes("get pitch",param,m_lastErrId);
	if( ivTTS_ERR_OK == m_lastErrId)	{
		pStTtsParams->pitch = param;
	}
	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_VOICE_SPEED, &param);
	LogCommRes("get speed",param,m_lastErrId);
	if( ivTTS_ERR_OK == m_lastErrId)	{
		pStTtsParams->speed = param;
	}
	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_VOLUME,&param);
	LogCommRes("get volume",param,m_lastErrId);
	if( ivTTS_ERR_OK == m_lastErrId)	{
		pStTtsParams->volume= param;
	}
/*	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_ENGLISH_ROLE, &param);
	LogCommRes("get en role",param,m_lastErrId);

	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_CHINESE_ROLE, &param);
	LogCommRes("get cn role",param,m_lastErrId);
	if( ivTTS_ERR_OK == m_lastErrId)	{
		pStTtsParams->en_role = param;
	}
	m_lastErrId = ivTTS_GetParam(m_hTTS, ivTTS_PARAM_LANGUAGE, &param);
	LogCommRes("get language",param,m_lastErrId);
	if( ivTTS_ERR_OK == m_lastErrId)	{
		pStTtsParams->language= param;
	}*/
}

//设置Tts 默认参数
static void TtsSetDefParams()
{
	ivTTSErrID m_lastErrId = 0;

	if (NULL == m_hTTS){
		LOGD("TtsSetDefParams is null");
		return;
	}

	//1. pitch
	if (m_stDefParam.is_set_pitch){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOICE_PITCH	, m_stDefParam.pitch);
		m_stDefParam.is_set_pitch = 0;
		LogCommRes("set def pitch", m_stDefParam.pitch,m_lastErrId);
	}
	//2. speed
	if (m_stDefParam.is_set_speed){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOICE_SPEED, m_stDefParam.speed);
		m_stDefParam.is_set_speed = 0;
		LogCommRes("set  def speed", m_stDefParam.speed,m_lastErrId);
	}
	//3. effect
	if (m_stDefParam.is_set_volume){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOLUME, m_stDefParam.volume);
		m_stDefParam.is_set_volume = 0;
		LogCommRes("set def volume", m_stDefParam.volume,m_lastErrId);
	}
	//4. en_role
/*	if (m_stDefParam.is_set_en_role){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_ENGLISH_ROLE, m_stDefParam.en_role);
		LogCommRes("set def  en_role", m_stDefParam.en_role,m_lastErrId);
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_CHINESE_ROLE, m_stDefParam.en_role);
		LogCommRes("set def  cn_role", m_stDefParam.en_role,m_lastErrId);
		m_stDefParam.is_set_en_role = 0;

	}
*/
	//5. lang
/*	if (m_stDefParam.is_set_language){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_LANGUAGE, m_stDefParam.language);
		m_stDefParam.is_set_language = 0;
		LogCommRes("set  def language", m_stDefParam.language,m_lastErrId);
	}*/
}

//设置Tts 当前参数
static void TtsSetCurParams()
{
	ivTTSErrID m_lastErrId = 0;

	if (NULL == m_hTTS){
		LOGD("TtsSetCurParams is null");
		return;
	}
	//1. pitch
	if (m_stCurParam.is_set_pitch){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOICE_PITCH	, m_stCurParam.pitch);
		m_stCurParam.is_set_pitch = 0;
		m_stDefParam.is_set_pitch = 1;
		LogCommRes("set cur pitch", m_stCurParam.pitch,m_lastErrId);
	}
	//2. speed
	if (m_stCurParam.is_set_speed){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOICE_SPEED, m_stCurParam.speed);
		m_stCurParam.is_set_speed = 0;
		m_stDefParam.is_set_speed = 1;
		LogCommRes("set cur speed", m_stCurParam.speed,m_lastErrId);
	}
	//3. effect
	if (m_stCurParam.is_set_volume){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOLUME, m_stCurParam.volume);
		m_stCurParam.is_set_volume= 0;
		m_stDefParam.is_set_volume = 1;
		LogCommRes("set cur volume", m_stCurParam.volume,m_lastErrId);
	}

	//4. en_role
/*	if (m_stCurParam.is_set_en_role){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_ENGLISH_ROLE, m_stCurParam.en_role);
		LogCommRes("set cur en_role", m_stDefParam.en_role,m_lastErrId);
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_CHINESE_ROLE, m_stCurParam.en_role);
		m_stCurParam.is_set_en_role = 0;
		m_stDefParam.is_set_en_role = 1;
		LogCommRes("set cur cn_role", m_stCurParam.en_role,m_lastErrId);
	}*/
	//5. lang
/*	if (m_stCurParam.is_set_language){
		m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_LANGUAGE, m_stCurParam.language);
		m_stCurParam.is_set_language = 0;
		m_stDefParam.is_set_language = 1;
		LogCommRes("set cur language", m_stCurParam.language,m_lastErrId);
	}*/
}


static int TtsCreate(char *filename)
{
	//1. init resource and malloc heap
	LOGD("JniTtsCreate 1");
	TtsInit(filename);

	LOGD("JniTtsCreate 2 init ok");
	if (NULL == m_pHeap || NULL == m_pResPackDesc){
		m_lastErrId = ivTTS_ERR_FAILED;
		LOGD("JniTtsCreate 2 m_pHeap null");
		return 0;
	}
	//2.  tts create
	m_lastErrId = ivTTS_Create(&m_hTTS, m_pHeap, TTS_HEAP_SIZE, 0, m_pResPackDesc,
		TTS_RES_COUNT,&m_stUserInfo);
	if (ivTTS_ERR_OK != m_lastErrId){
		LOGD("JniTtsCreate 3 ivTTS_Create error");
		LogCommRes("JniTtsCreate 3 ivTTS_Create error",m_lastErrId,0);
		return m_lastErrId;
	}
	LOGD("JniTtsCreate 3 ivTTS_Create ok");
	b_CreateStatus = ivTrue;

	//3. set default param
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_OUTPUT_CALLBACK, (ivSize)OutputCB);
//	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_WATCH_CALLBACK, (ivUInt32)WatchCB);
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_PROGRESS_CALLBACK, (ivSize)ProgressCB);
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_LANGUAGE, ivTTS_LANGUAGE_AUTO);
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_INPUT_CODEPAGE, ivTTS_CODEPAGE_UNICODE);
  	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_VOLUME, ivTTS_VOLUME_MAX);
	//4. save default param
	TtsGetParams(&m_stDefParam);
	LOGD("JniTtsCreate 5 ivTTS_Create finish");

	return m_lastErrId;

}
/*
* 修改默认语调
*/
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetDefPitch
	(JNIEnv *env, jobject thiz, jint v)
{
	m_stDefParam.pitch = v;
	m_stDefParam.is_set_pitch = 1;
	return ivTTS_ERR_OK;
}

//修改当前语调
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetCurPitch
	(JNIEnv *env, jobject thiz, jint v)
{
	m_stCurParam.pitch = v;
	m_stCurParam.is_set_pitch = 1;
	return ivTTS_ERR_OK;
}

//取默认pitch
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetPitch
	(JNIEnv *env, jobject thiz)
{
	return (jint)m_stDefParam.pitch;
}

/**
* 修改speed
*/
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetDefSpeed
	(JNIEnv *env, jobject thiz, jint v)
{
	m_stDefParam.speed = v;
	m_stDefParam.is_set_speed = 1;
	return ivTTS_ERR_OK;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetCurSpeed
	(JNIEnv *env, jobject thiz,jint v)
{
	m_stCurParam.speed = v;
	m_stCurParam.is_set_speed = 1;
	return ivTTS_ERR_OK;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetSpeed
	(JNIEnv *env, jobject thiz)
{
	return (jint)m_stDefParam.speed;
}

/**
* 修改 effect
*/
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetDefVolume
	(JNIEnv *env, jobject thiz, jint v)
{
	m_stDefParam.volume= v;
	m_stDefParam.is_set_volume= 1;
	return ivTTS_ERR_OK;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetCurVolume
	(JNIEnv *env, jobject thiz,jint v)
{
	m_stCurParam.volume= v;
	m_stCurParam.is_set_volume= 1;
	return ivTTS_ERR_OK;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetVolume
	(JNIEnv *env, jobject thiz)
{
	return (jint)m_stDefParam.volume;
}

/*
* 修改Role
*/

//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetDefRole
//	(JNIEnv *env, jobject thiz, jint v)
//{
	//要增加对Role的判断
	//....
//	m_stDefParam.en_role = v;
//	m_stDefParam.is_set_en_role = 1;
//	return ivTTS_ERR_OK;
//}

//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetCurRole
//	(JNIEnv *env, jobject thiz,jint v)
//{
	//要增加对Role的判断
	//....
//	m_stCurParam.en_role = v;
//	m_stCurParam.is_set_en_role = 1;
//	return ivTTS_ERR_OK;
//}

//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetRole
//	(JNIEnv *env, jobject thiz)
//{
//	return (jint) m_stDefParam.en_role;
//}

/*
* 修改语言类型
* 同时也会修改对应的Role
*/
//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetDefLang
//	(JNIEnv *env, jobject thiz, jint v)
//{
	//增加Language 的支持判断
	//...

//	m_stDefParam.language =  v;
//	m_stDefParam.is_set_language =  1;


	//增加对应Role 的修改
	//...
//	return ivTTS_ERR_OK;
//}

//jint  JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetCurLang
//	(JNIEnv *env, jobject thiz, jint v)
//{
	//增加Language 的支持判断
	//...

//	m_stCurParam.language =  v;
//	m_stCurParam.is_set_language =  1;


	//增加对应Role 的修改
	//...
//	return ivTTS_ERR_OK;
//}
//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetLanguage
//	(JNIEnv *env, jobject thiz)
//{
//	return (jint)m_stDefParam.language ;
//}


/**
* 注意函数的大小写问题
* C 里习惯以大写开头,Java 以小写开头
*/
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetVersion
  (JNIEnv *env, jobject thiz)
{
	jint man = 0;
	int sub = 0;
	int build = 0;
	LOGD("get version 1");
	ivTTS_GetVersion((ivPUInt8)&man,(ivPUInt8)&sub,(ivPUInt16)&build);
	LOGD("get version  2 ");

	char msg[256] = {0};

	sprintf(msg,"TtsInit env %x ,obj %x ",env,thiz );
	LOGD(msg);
	return man;
}



//创建TTS 实例
//jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniCreate
//  (JNIEnv *env, jobject thiz)
//{

//	return TtsCreate();

//}

//创建TTS 实例
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniCreate
  (JNIEnv *env, jobject thiz,jstring resFilename)
{
	jint ret = 0;

	char * pName = (char *)(*env)->GetStringUTFChars(env,resFilename,NULL);
	strcpy(m_sResFilename,pName);
	ret = TtsCreate(pName);
	(*env)->ReleaseStringUTFChars(env,resFilename,pName);
	return ret;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniStop
	(JNIEnv *env, jobject thiz)
{
	b_StopAudioStatus = ivFalse;
	m_TtsPlayStatus = PLAY_STATUS_STOP;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniIsPlaying
	(JNIEnv *env, jobject thiz)
{
	return	m_TtsPlayStatus;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniIsCreated
	(JNIEnv *env, jobject thiz)
{
	return	b_CreateStatus;
}

// Speak Text
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSpeak
	(JNIEnv *env, jobject thiz,jstring text)
{
	char msg[256] = {0};
	char *pBuf  = NULL;
	int nBufLen = 0;
	int nTextLen = 0;

	g_JavaSpeakThreadId = pthread_self();

	b_StopAudioStatus = ivTrue;

	//判断TTS 是否创建过成功过
	if (NULL == m_hTTS){
		TtsCreate(m_sResFilename);
	}
    LOGD(" speak  TTS null");
	if (NULL == m_hTTS){
		m_lastErrId = ivTTS_ERR_INVALID_HANDLE;
		LOGD(" speak  TTS null");
		return m_lastErrId;
	}

	//1. 必需每次在Run 前调用
	JniInit(env);
	LOGD("JniInit ok");

	nTextLen =  (*env)->GetStringLength(env, text);
	if (nTextLen <= 0){
		LOGD("speak Text  null");
		m_lastErrId = ivTTS_ERR_FAILED;
		return m_lastErrId;
	}
	sprintf(msg,"StringLength %d",nTextLen);
	LOGD(msg);

	nBufLen = nTextLen  * 2;
	pBuf = (char *)malloc(nBufLen);
	if (NULL == pBuf){
		LOGD(" speak  malloc  null");
		m_lastErrId = ivTTS_ERR_FAILED;
		return m_lastErrId;
	}
	memset(pBuf,0,nBufLen);

	(*env)->GetStringRegion(env, text, 0, nTextLen, (jchar *)pBuf);

	sprintf(msg,"speak --1--len %d",nTextLen);
	LOGD(msg);

	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_INPUT_MODE, ivTTS_INPUT_FIXED_BUFFER);
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_INPUT_TEXT_BUFFER, (unsigned long)pBuf);
	m_lastErrId = ivTTS_SetParam(m_hTTS, ivTTS_PARAM_INPUT_TEXT_SIZE, (ivSize)nBufLen);

	//2.1 设置默认参数
	TtsSetDefParams();
	//2.2 设置当前参数
	TtsSetCurParams();

	//3. 执行RUN
	LOGD("speak--3-- run ");
	m_TtsPlayStatus = PLAY_STATUS_SPEAK;
	m_lastErrId = ivTTS_Run(m_hTTS);
	m_TtsPlayStatus = PLAY_STATUS_FINISH;


	//4.  执行JniDinit,如果参数被修改要恢复默认
	LOGD("speak--4-- run finish");

	free(pBuf);
	pBuf = NULL;		
	return m_lastErrId; 

}
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniDestory
	(JNIEnv *env, jobject thiz)
{
	m_lastErrId = 0;
	TtsDInit();	
	return m_lastErrId;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniSetParam
	(JNIEnv *env, jobject thiz,jint paramId,jint value)
{
	m_lastErrId = ivTTS_SetParam(m_hTTS, (ivSize)paramId , (ivSize)value);
	LogCommRes("JniSetParam", paramId,value);
	return m_lastErrId;
}

jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniGetParam
	(JNIEnv *env, jobject thiz,jint paramId)
{
	ivUInt32 param = 0;
	m_lastErrId = ivTTS_GetParam(m_hTTS,(ivSize)paramId, &param);
	LogCommRes("JniGetParam",paramId,param);
	return param;
}

#if 0


#define gClassName "com/iflytek/tts/TtsService/TtsJni"
#define gMethodNum  3

static JNINativeMethod gMethods[gMethodNum] = {
/* name, signature, funcPtr */
{ "JNICALL Java_com_iflytek_tts_TtsService_TtsJni_GetVersion", "(Ljava/lang/String;I)V", JNICALL Java_com_iflytek_aisound_Data_prepareData },
{ "JNICALL Java_com_iflytek_aisound_Data_ContrlSyn", "(I)I", JNICALL Java_com_iflytek_aisound_Data_ContrlSyn },
{ "JNICALL Java_com_iflytek_aisound_Data_ContrlInit", "()V", JNICALL Java_com_iflytek_aisound_Data_ContrlInit },};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv *env;
	jclass cls;
	if ( (*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_4) ) 
		return JNI_ERR;
	
	cls = (*env)->FindClass(env, gClassName);
	(*env)->RegisterNatives(env, cls, gMethods, gMethodNum);
	return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
	JNIEnv *env;
	jclass cls;
	if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_4)) 
		return;
	cls = (*env)->FindClass(env, gClassName);
	(*env)->UnregisterNatives(env, cls);
}

#endif

