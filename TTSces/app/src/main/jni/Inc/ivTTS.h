/*--------------------------------------------------------------+
 |																|
 |	ivTTS.h - AiSound 5 Kernel API	            				|
 |																|
 |		Copyright (c) 1999-2011, ANHUI USTC iFLYTEK CO.,LTD.	|
 |		All rights reserved.									|
 |																|
 +--------------------------------------------------------------*/

#ifndef IFLYTEK_VOICE__TTS__H
#define IFLYTEK_VOICE__TTS__H


#include "ivDefine.h"
#include "ivTTSSDKID.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 *	COMMON DEFINES
 */

typedef ivUInt32 ivTTSErrID;
typedef ivPointer ivHTTS;


/*
* TTS User Info 
*/
typedef struct tagTTSUserInfo ivTTTsUserInfo, ivPtr ivPTTSUserInfo;
struct tagTTSUserInfo
{
	ivPointer		pCBLogParameter;	/* log callback parameter */
	ivCBLogExt		pfnLog;				/* write log callback entry */
};

/*
 *	FUNCTION ENTRIES
 */

/* get SDK version */
ivTTSErrID ivCall ivTTS_GetVersion(
	ivPUInt8		piMajor,			/* [out] major version number */
	ivPUInt8		piMinor,			/* [out] minor version number */
	ivPUInt16		piRevision);		/* [out] revision number */

/* create an instance */
#define ivTTS_Create(phTTS, pHeap, nHeapSize, pCBParam, pResPackDesc, nResPackCount,pUserInfo) \
ivTTS_CreateG(phTTS, pHeap, nHeapSize, pCBParam, pResPackDesc, nResPackCount,pUserInfo, AISOUND_SDK_USERID)

ivTTSErrID ivCall ivTTS_CreateG(
	ivHTTS ivPtr	phTTS,				/* [out] handle to an instance */
	ivPointer		pHeap,				/* [in] heap for instance */
	ivSize			nHeapSize,			/* [in] size of the heap */
	ivPointer		pCBParam,			/* [in] user callback parameter */
	ivPResPackDescExt	pResPackDesc,		/* [in] resource pack description array */
	ivSize			nResPackCount,		/* [in] resource pack count */
	ivPTTSUserInfo pUserInfo,			/* [in] TTS User Info */
    ivCStrA        pUserID);            /* [in] SDK ID */


/* destroy an instance */
ivTTSErrID ivCall ivTTS_Destroy(
	ivHTTS			hTTS );				/* [in] handle to an instance */

/* get a parameter associated with an instance */
ivTTSErrID ivCall ivTTS_GetParam(
	ivHTTS			hTTS,				/* [in] handle to an instance */
	ivUInt32		nParamID,			/* [in] parameter ID */
	ivPAddress		pnParamValue );		/* [out] buffer to receive the parameter value */

/* set a parameter associated with an instance */
ivTTSErrID ivCall ivTTS_SetParam(
	ivHTTS			hTTS,				/* [in] handle to an instance */
	ivUInt32		nParamID,			/* [in] parameter ID */
	ivSize			nParamValue );		/* [in] parameter value */

/* run an instance and hold current thread's control */
ivTTSErrID ivCall ivTTS_Run(
	ivHTTS			hTTS );				/* [in] handle to an instance */

/* exit running of an instance and leave current thread's control */
ivTTSErrID ivCall ivTTS_Exit(
	ivHTTS			hTTS );				/* [in] handle to an instance */

/* synthesize a buffer of text on an instance */
ivTTSErrID ivCall ivTTS_SynthText(
	ivHTTS			hTTS,				/* [in] handle to an instance */
	ivCPointer		pcData,				/* [in] pointer of text buffer data to be synthesized */
	ivSize			nSize );			/* [in] size of text buffer data to be synthesized */

/* begin to synthesize from callback on an instance */
ivTTSErrID ivCall ivTTS_SynthStart(
	ivHTTS			hTTS );				/* [in] handle to an instance */

ivTTSErrID ivCall ivTTS_Cssml2SimpleTag(
                                        ivHTTS      hTTS,       /* [in] handle to an instance */
                                        ivCPointer  pText,      /* [in] pointer of the text buffer */
                                        ivSize   pTextLen,   /* [in] size of the text buffer */
                                        ivPointer   pInputBuf,  /* [in] pointer of the input buffer */
                                        ivPInt32      nInputBufSize   /* [in/out] size of the input buffer */
                                        );
/*
 *	ERROR CODES
 */

#define ERRCODE(x)						((x)&0x0000FFFF)		/* macro definition of error codes */ 

#define ivTTS_ERR_OK					0x00000000				/* success */
#define ivTTS_ERR_FAILED				0xFFFFFFFF				/* failed */

#define ivTTS_ERR_END_OF_INPUT			0x00000001				/* end of input stream */
#define ivTTS_ERR_EXIT					0x00000002				/* exit TTS */
#define ivTTS_STATE_BASE				0x00000100				/* state base */
#define ivTTS_STATE_TTS_RUNNING			ivTTS_STATE_BASE + 0	/* Indicates the TTS in running state */
#define ivTTS_STATE_TTS_STOPPED			ivTTS_STATE_BASE + 1	/* Indicates the TTS has stopped */
#define ivTTS_STATE_INVALID_DATA		ivTTS_STATE_BASE + 2	/* invalid data */
#define ivTTS_STATE_TTS_STOP			ivTTS_STATE_TTS_STOPPED	/* TTS stop (deprecated)*/

#define ivTTS_ERR_BASE					0x00008000				/* error number base */

#define ivTTS_ERR_UNIMPEMENTED			ivTTS_ERR_BASE + 0		/* unimplemented function */
#define ivTTS_ERR_UNSUPPORTED			ivTTS_ERR_BASE + 1		/* unsupported on this platform */
#define ivTTS_ERR_INVALID_HANDLE		ivTTS_ERR_BASE + 2		/* invalid handle */
#define ivTTS_ERR_INVALID_PARAMETER		ivTTS_ERR_BASE + 3		/* invalid parameter(s) */
#define ivTTS_ERR_INSUFFICIENT_HEAP		ivTTS_ERR_BASE + 4		/* insufficient heap size  */
#define ivTTS_ERR_STATE_REFUSE			ivTTS_ERR_BASE + 5		/* refuse to do in current state  */
#define ivTTS_ERR_INVALID_PARAM_ID		ivTTS_ERR_BASE + 6		/* invalid parameter ID */
#define ivTTS_ERR_INVALID_PARAM_VALUE	ivTTS_ERR_BASE + 7		/* invalid parameter value */
#define ivTTS_ERR_RESOURCE				ivTTS_ERR_BASE + 8		/* Resource is error */
#define ivTTS_ERR_RESOURCE_READ			ivTTS_ERR_BASE + 9		/* read resource error */
#define ivTTS_ERR_LBENDIAN				ivTTS_ERR_BASE + 10		/* the Endian of SDK  is error or ReadResCB function is error */
#define ivTTS_ERR_HEADFILE				ivTTS_ERR_BASE + 11		/* the HeadFile is different of the SDK */
#define ivTTS_ERR_SIZE_EXCEED_BUFFER	ivTTS_ERR_BASE + 12		/* get data size exceed the data buffer */
#define ivTTS_ERR_INVALID_ISAMPA		ivTTS_ERR_BASE + 13		/*!Invalid iSampa format or input iSampa text contain invalid alphabet*/
#define ivTTS_ERR_HEAP_ALLOC			ivTTS_ERR_BASE + 14     /* heap alloc failed */
#define ivTTS_ERR_INVALID_CSSML			ivTTS_ERR_BASE + 15		/*!Invalid cssml format */

/*
 *	INSTANCE PARAMETERS
 */

/* constants for values of field nParamID */
#define ivTTS_PARAM_PARAMCH_CALLBACK	0x00000000	/* parameter change callback entry */
#define ivTTS_PARAM_LANGUAGE			0x00000100	/* language, e.g. Chinese */
#define ivTTS_PARAM_INPUT_CODEPAGE		0x00000101	/* input code page, e.g. GBK */
#define ivTTS_PARAM_TEXT_MARK			0x00000102	/* text mark, e.g. CSSML */
#define ivTTS_PARAM_TEXT_SCOPE			0x00000103	/* text scope, e.g. number */
#define ivTTS_PARAM_USE_PROMPTS			0x00000104	/* whether use prompts */
#define ivTTS_PARAM_USE_BGSOUND			0x00000106	/* whether use bgsound */
#define ivTTS_PARAM_RECOGNIZE_PHONEME	0x00000105	/* how to recognize phoneme input */
#define ivTTS_PARAM_INPUT_MODE			0x00000200	/* input mode, e.g. from fixed buffer, from callback */
#define ivTTS_PARAM_INPUT_TEXT_BUFFER	0x00000201	/* input text buffer */
#define ivTTS_PARAM_INPUT_TEXT_SIZE		0x00000202	/* input text size */
#define ivTTS_PARAM_INPUT_CALLBACK		0x00000203	/* input callback entry */
#define ivTTS_PARAM_PROGRESS_BEGIN		0x00000204	/* current processing position */
#define ivTTS_PARAM_PROGRESS_LENGTH		0x00000205	/* current processing length */
#define ivTTS_PARAM_PROGRESS_CALLBACK	0x00000206	/* progress callback entry */
#define ivTTS_PARAM_PROGRESS_CALLBACK_EXT	0x00000207	/* new progress callback entry */
#define ivTTS_PARAM_PROGRESS_CALLBACK_TYPE  0x00000208	/* progress callback entry type */

#define ivTTS_PARAM_READ_AS_NAME		0x00000301	/* whether read as name */
#define ivTTS_PARAM_READ_DIGIT			0x00000302	/* how to read digit, e.g. read as number, read as value  */
#define ivTTS_PARAM_CHINESE_NUMBER_1	0x00000303	/* how to read number "1" in Chinese */
#define ivTTS_PARAM_MANUAL_PROSODY		0x00000304	/* whether use manual prosody */
#define ivTTS_PARAM_ENGLISH_NUMBER_0	0x00000305	/* how to read number "0" in Englsih */
#define ivTTS_PARAM_READ_WORD           0x00000306	/* how to read word in Englsih,  e.g. read by word, read as alpha  */
#define ivTTS_PARAM_OUTPUT_CALLBACK		0x00000401	/* output callback entry */
#define ivTTS_PARAM_ROLE				0x00000500	/* speaker role */
#define ivTTS_PARAM_SPEAK_STYLE			0x00000501	/* speak style */
#define ivTTS_PARAM_VOICE_SPEED			0x00000502	/* voice speed */
#define ivTTS_PARAM_VOICE_PITCH			0x00000503	/* voice tone */
#define ivTTS_PARAM_VOLUME				0x00000504	/* volume value */
#define ivTTS_PARAM_CHINESE_ROLE        0x00000510	/* Chinese speaker role */
#define ivTTS_PARAM_ENGLISH_ROLE        0x00000511	/* English speaker role */
#define ivTTS_PARAM_VEMODE				0x00000600	/* voice effect - predefined mode */
#define ivTTS_PARAM_USERMODE			0x00000701	/* user's mode */
#define ivTTS_PARAM_NAVIGATION_MODE		0x00000701	/* Navigation Version */
#define ivTTS_PARAM_POST_PROCESS		0x00000800	/* Post Process */
#define ivTTS_PARAM_SYN_MODEL   		0x00000801	/* Synthesis Model */
#define ivTTS_PARAM_METHOD_SPACE		0x00000802	/*method of dealing space*/

#define ivTTS_PARAM_ALL_DEFAULT  		0x00001000	/* default param  */
#define ivTTS_PARAM_EVENT_CALLBACK		0x00001001	/* sleep callback entry */
#define ivTTS_PARAM_OUTPUT_BUF			0x00001002	/* output buffer */
#define ivTTS_PARAM_OUTPUT_BUFSIZE		0x00001003	/* output buffer size */
#define ivTTS_PARAM_DELAYTIME			0x00001004	/* delay time */
#define ivTTS_PARAM_TTS_STATE			0x00001005	/* Current state of the TTS */

/* BGSound audio params */
#define ivTTS_PARAM_BGSOUND_POSITION    0x00001007  /* BGSound audio read pos when cycle */

/* constants for values of parameter ivTTS_PARAM_ALL_NORMAL */
#define ivTTS_PARAM_DEFAULT				0			/* default param */

#define ivTTS_PARAM_PCM_FORMAT			0x00003001  /* output pcm sample rate */

#define ivTTS_PARAM_INPUT_STYLE				0x00001101  /*set the input  style*/
#define ivTTS_PARAM_OUTPUT_STYLE			0x00001102  /*set the output style*/
#define ivTTS_PARAM_G2P_CALLBACK			0x00001103  /*WatchPinyin callback entry*/
#define ivTTS_PARAM_ISAMPA_CALLBACK			0x00001104  /*WatchISampa callback entry*/

/* constants for values of parameter ivTTS_PARAM_INPUT_STYLE */
#define ivTTS_INPUT_STYLE_NORMAL			0x00000001  /*set the input style as normal text*/
#define ivTTS_INPUT_STYLE_PHONEME			0x00000002  /*set the input style as phoneme text*/
#define ivTTS_INPUT_STYLE_RHYTHM			0x00000004  /*set the input style as rhythm text*/
#define ivTTS_INPUT_STYLE_ISAMPA			0x00000008  /*set the input style as I-SAMPA*/

/* constants for values of parameter ivTTS_PARAM_OUTPUT_STYLE */
#define ivTTS_OUTPUT_STYLE_PCM				0x00000001  /*set the output style as PCM*/
#define ivTTS_OUTPUT_STYLE_ISAMPA			0x00000004  /*set the output style as I-SAMPA file*/

/* constants for values of parameter ivTTS_PARAM_LANGUAGE */
#define ivTTS_LANGUAGE_AUTO             0           /* Detect language automatically */
#define ivTTS_LANGUAGE_CHINESE			1			/* Chinese (with English) */
#define ivTTS_LANGUAGE_ENGLISH			2			/* English */

/* constants for values of parameter ivTTS_PARAM_INPUT_CODEPAGE */
#define ivTTS_CODEPAGE_ASCII			437			/* ASCII */
#define ivTTS_CODEPAGE_GBK				936			/* GBK (default) */
#define ivTTS_CODEPAGE_BIG5				950			/* Big5 */
#define ivTTS_CODEPAGE_UTF16LE			1200		/* UTF-16 little-endian */
#define ivTTS_CODEPAGE_UTF16BE			1201		/* UTF-16 big-endian */
#define ivTTS_CODEPAGE_UTF8				65001		/* UTF-8 */
#define ivTTS_CODEPAGE_GB2312			ivTTS_CODEPAGE_GBK
#define ivTTS_CODEPAGE_GB18030			ivTTS_CODEPAGE_GBK
#if IV_BIG_ENDIAN
#define ivTTS_CODEPAGE_UTF16			ivTTS_CODEPAGE_UTF16BE
#else
#define ivTTS_CODEPAGE_UTF16			ivTTS_CODEPAGE_UTF16LE
#endif
#define ivTTS_CODEPAGE_UNICODE			ivTTS_CODEPAGE_UTF16
#define ivTTS_CODEPAGE_PHONETIC_PLAIN	23456		/* Kingsoft Phonetic Plain */

/* constants for values of parameter ivTTS_PARAM_TEXT_MARK */
#define ivTTS_TEXTMARK_NONE				0			/* none */
#define ivTTS_TEXTMARK_SIMPLE_TAGS		1			/* simple tags (default) */

/* constants for values of parameter ivTTS_PARAM_TEXT_SCOPE */
#define ivTTS_SCOPE_UNLIMITED			0			/* unlimited scope (default) */
#define ivTTS_SCOPE_CHINESE_NUMBER		1			/* Chinese number */
#define ivTTS_SCOPE_CHINESE_VALUE		2			/* Chinese value */
#define ivTTS_SCOPE_CHINESE_DATETIME	3			/* Chinese date or time (No) */
#define ivTTS_SCOPE_CHINESE_POI         4           /* Chinese POI */
#define ivTTS_SCOPE_CHINESE_EPG         5           /* Chinese EPG */
#define ivTTS_SCOPE_MAX_SCOPE           6           /* text max scope */

/* constants for values of parameter ivTTS_PARAM_INPUT_MODE */
#define ivTTS_INPUT_FIXED_BUFFER		0			/* from fixed buffer */
#define ivTTS_INPUT_CALLBACK			1			/* from callback */

/* constants for values of parameter ivTTS_PARAM_READ_DIGIT */
#define ivTTS_READDIGIT_AUTO			0			/* decide automatically (default) */
#define ivTTS_READDIGIT_AS_NUMBER		1			/* say digit as number */
#define ivTTS_READDIGIT_AS_VALUE		2			/* say digit as value */
#define ivTTS_READDIGIT_AS_CUSTOM		3			/* say digit as custom */

/* constants for values of parameter ivTTS_PARAM_CHINESE_NUMBER_1 */
#define ivTTS_CHNUM1_READ_YAO			0			/* read number "1" [yao1] in chinese (default) */
#define ivTTS_CHNUM1_READ_YI			1			/* read number "1" [yi1] in chinese */

/* constants for values of parameter ivTTS_PARAM_ENGLISH_NUMBER_0 */
#define ivTTS_ENNUM0_READ_ZERO			0			/* read number "0" [zero] in english (default) */
#define ivTTS_ENNUM0_READ_O				1			/* read number "0" [o] in englsih */

/* constants for values of parameter ivTTS_PARAM_CHINESE_CRUTE_SPLIT */
//#define ivTTS_NO_CRUTE_SPLIT			0			/* no using crute split words(default) */
//#define ivTTS_CRUTE_SPLIT				1			/* using crute split words */

/* constants for values of parameter ivTTS_PARAM_SPEAKER */
#define ivTTS_ROLE_TIANCHANG			1			/* Tianchang (female, Chinese) */
#define ivTTS_ROLE_WENJING				2			/* Wenjing (female, Chinese) */
#define ivTTS_ROLE_XIAOYAN				3			/* Xiaoyan (female, Chinese) */
#define ivTTS_ROLE_YANPING				3			/* Xiaoyan (female, Chinese) */
#define ivTTS_ROLE_XIAOFENG				4			/* Xiaofeng (male, Chinese) */
#define ivTTS_ROLE_YUFENG				4			/* Xiaofeng (male, Chinese) */
#define ivTTS_ROLE_SHERRI				5			/* Sherri (female, US English) */
#define ivTTS_ROLE_XIAOJIN				6			/* Xiaojin (female, Chinese) */
#define ivTTS_ROLE_NANNAN				7			/* Nannan (child, Chinese) */
#define ivTTS_ROLE_JINGER				8			/* Jinger (female, Chinese) */
#define ivTTS_ROLE_JIAJIA				9			/* Jiajia (girl, Chinese) */
#define ivTTS_ROLE_YUER					10			/* Yuer (female, Chinese) */
#define ivTTS_ROLE_XIAOQIAN				11			/* Xiaoqian (female, Chinese Northeast) */
#define ivTTS_ROLE_LAOMA				12			/* Laoma (male, Chinese) */
#define ivTTS_ROLE_BUSH					13			/* Bush (male, US English) */
#define ivTTS_ROLE_XIAORONG				14			/* Xiaorong (female, Chinese Szechwan) */
#define ivTTS_ROLE_XIAOMEI				15			/* Xiaomei (female, Cantonese) */
#define ivTTS_ROLE_ANNI					16			/* Anni (female, Chinese) */
#define ivTTS_ROLE_JOHN					17			/* John (male, US English) */
#define ivTTS_ROLE_ANITA				18			/* Anita (female, British English) */
#define ivTTS_ROLE_TERRY				19			/* Terry (female, US English) */
#define ivTTS_ROLE_CATHERINE			20			/* Catherine (female, US English) */
#define ivTTS_ROLE_TERRYW				21			/* Terry (female, US English Word) */
#define ivTTS_ROLE_XIAOLIN				22			/* Xiaolin (female, Chinese) */
#define ivTTS_ROLE_XIAOMENG				23			/* Xiaomeng (female, Chinese) */
#define ivTTS_ROLE_XIAOQIANG			24			/* Xiaoqiang (male, Chinese) */
#define ivTTS_ROLE_XIAOKUN				25			/* XiaoKun (male, Chinese) */
#define ivTTS_ROLE_JIUXU				51			/* Jiu Xu (male, Chinese) */
#define ivTTS_ROLE_DUOXU				52			/* Duo Xu (male, Chinese) */
#define ivTTS_ROLE_XIAOPING				53			/* Xiaoping (female, Chinese) */
#define ivTTS_ROLE_DONALDDUCK			54			/* Donald Duck (male, Chinese) */
#define ivTTS_ROLE_BABYXU				55			/* Baby Xu (child, Chinese) */
#define ivTTS_ROLE_DALONG				56			/* Dalong (male, Cantonese) */
#define ivTTS_ROLE_TOM					57			/* Tom (male, US English) */
#define ivTTS_ROLE_MINGMA				58			/* MingMa (male, Chinese) */
#define ivTTS_ROLE_BAOMA				59			/* BaoMa (child, Chinese) */
#define ivTTS_ROLE_BELLA				60			/* Bella (child, US English) */
#define ivTTS_ROLE_USER					99			/* user defined */
/* 自定义发音人VID,范围区间为50,000~100,000 */
#define ivTTS_ROLE_XIAORUI              50001       /* Xiaorui   (female, Chinese) */
#define ivTTS_ROLE_XIAOSHI              50002       /* Xiaoshi   (female, Chinese) */
#define ivTTS_ROLE_XIAOYAO              50003       /* Xiaoyao   (female, Chinese) */
#define ivTTS_ROLE_XIAOYANG             50004       /* Xiaoyang  (male, Chinese) */
#define ivTTS_ROLE_XIAOHONG             50005       /* Xiaohong  (male, Chinese) */
#define ivTTS_ROLE_XIAOHUI              50006       /* Xiaohui   (male, Chinese) */
#define ivTTS_ROLE_XIAOBO               50007       /* Xiaobo    (male, Chinese) */
#define ivTTS_ROLE_XIAOWANZI            50008       /* Xiaowanzi (female, Chinese) */
#define ivTTS_ROLE_XIAOXIN              50009       /* Xiaoxin   (male, Chinese) */
#define ivTTS_ROLE_XIAOMA               50010      /* Xiaoma    (male, Chinese) */
#define ivTTS_ROLE_XIAOLING             50011      /* Xiaoling  (female, Chinese) */

/* constants for values of parameter ivTTS_PARAM_SPEAK_STYLE */
#define ivTTS_STYLE_PLAIN				0			/* plain speak style */
#define ivTTS_STYLE_NORMAL				1			/* normal speak style (default) */

/* constants for values of parameter ivTTS_PARAM_VOICE_SPEED */
/* the range of voice speed value is from -32768 to +32767 */
#define ivTTS_SPEED_MIN					-32768		/* slowest voice speed */
#define ivTTS_SPEED_NORMAL				0			/* normal voice speed (default) */
#define ivTTS_SPEED_MAX					+32767		/* fastest voice speed */

/* constants for values of parameter ivTTS_PARAM_VOICE_PITCH */
/* the range of voice tone value is from -32768 to +32767 */
#define ivTTS_PITCH_MIN					-32768		/* lowest voice tone */
#define ivTTS_PITCH_NORMAL				0			/* normal voice tone (default) */
#define ivTTS_PITCH_MAX					+32767		/* highest voice tone */

/* constants for values of parameter ivTTS_PARAM_VOLUME */
/* the range of volume value is from -32768 to +32767 */
#define ivTTS_VOLUME_MIN				-32768		/* minimized volume */
#define ivTTS_VOLUME_NORMAL				0			/* normal volume */
#define ivTTS_VOLUME_MAX				+32767		/* maximized volume (default) */

/* constants for values of parameter ivTTS_PARAM_VEMODE */
#define ivTTS_VEMODE_NONE				0			/* none */
#define ivTTS_VEMODE_WANDER				1			/* wander */
#define ivTTS_VEMODE_ECHO				2			/* echo */
#define ivTTS_VEMODE_ROBERT				3			/* robert */
#define ivTTS_VEMODE_CHORUS				4			/* chorus */
#define ivTTS_VEMODE_UNDERWATER			5			/* underwater */
#define ivTTS_VEMODE_REVERB				6			/* reverb */
#define ivTTS_VEMODE_ECCENTRIC			7			/* eccentric */

/* constants for values of parameter ivTTS_PARAM_USERMODE(ivTTS_PARAM_NAVIGATION_MODE) */
#define ivTTS_USE_NORMAL				0			/* synthesize in the Mode of Normal */
#define ivTTS_USE_NAVIGATION			1			/* synthesize in the Mode of Navigation */
#define ivTTS_USE_MOBILE				2			/* synthesize in the Mode of Mobile */
#define ivTTS_USE_EDUCATION				3			/* synthesize in the Mode of Education */
#define ivTTS_USE_TV				    4			/* synthesize in the Mode of TV */

/* constants for values of parameter ivTTS_PARAM_READ_WORD */
#define ivTTS_READWORD_BY_WORD			2			/* say words by the way of word */
#define ivTTS_READWORD_BY_ALPHA			1			/* say words by the way of alpha */
#define ivTTS_READWORD_BY_AUTO			0			/* say words by the way of auto */

/* constants for values of parameter ivTTS_PARAM_POST_PROCESS */
#define ivTTS_USE_POST_PROCESS			1			/* synthesize with Post Process */
#define ivTTS_UNUSE_POST_PROCESS		0			/* synthesize without Post Process */

/* constants for values of parameter ivTTS_PARAM_SYN_MODEL */
#define ivTTS_USE_SynModel_AUTO			0			/* synthesize by the Model of Normal(default) */
#define ivTTS_USE_SynModel_Small		1			/* synthesize by the Model of Small */
#define ivTTS_USE_SynModel_Big		    2			/* synthesize by the Model of Big */



/* constants for values of parameter ivTTS_PARAM_METHOD_SPACE */
#define  ivTTS_USE_SPACE_NOPAUSE	1				/*space dealed as no space*/
#define  ivTTS_USE_SPACE_ASL3		0				/*space dealed as L3*/

/* constants for values of parameter ivTTS_PARAM_PROGRESS_CALLBACK_TYPE */
#define ivTTS_PROGRESS_CALLBACK_NORMAL              0       /* callback all input */
#define ivTTS_PROGRESS_CALLBACK_NO_LABEL            1       /* callback no Simple Tag input */
#define ivTTS_PROGRESS_CALLBACK_WORD                2       /* callback input word */
#define ivTTS_PROGRESS_CALLBACK_WORD_NO_LABEL       3       /* callback input word */

/* parameter change callback type */
typedef ivTTSErrID (ivProc ivTTSCB_ParamChange)(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivUInt32		nParamID,		/* [in] parameter ID */
	ivUInt32		nParamValue );	/* [in] parameter value */

/* progress callback type */
typedef ivTTSErrID (ivProc ivTTSCB_Progress)(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivUInt32		iProcBegin,		/* [in] current processing position */
	ivUInt32		nProcLen );		/* [in] current processing length */

/* input callback type */
typedef ivTTSErrID (ivProc ivTTSCB_Input)(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivPointer		pText,			/* [out] input text buffer */
	ivSize ivPtr	pnSize );		/* [in/out] input text size */

/* output callback type */
typedef ivTTSErrID (ivProc ivTTSCB_Output)(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivUInt16		nCode,			/* [in] output data code */
	ivCPointer		pcData,			/* [in] output data buffer */
	ivSize			nSize );		/* [in] output data size */

/* parameter change callback type */
typedef ivTTSErrID (ivProc ivTTSCB_Event)(
	ivPointer		pParameter,		/* [in] user callback parameter */
	ivUInt32		nEventID,		/* [in] parameter ID */
	ivUInt32		nValue );	/* [in] parameter value */


/* constants for values of parameter nEventID */
#define ivTTS_EVENT_SLEEP				0x0100		/* sleep */
#define ivTTS_EVENT_PLAYSTART			0x0101		/* start playing */
#define ivTTS_EVENT_SWITCHCONTEXT		0x0102		/* context switch */

/* constants for values of parameter wCode */
#define ivTTS_CODE_PCM6K16B				0x0206		/* PCM 6K 16bit */
#define ivTTS_CODE_PCM8K16B				0x0208		/* PCM 8K 16bit */
#define ivTTS_CODE_PCM11K16B			0x020B		/* PCM 11K 16bit */
#define ivTTS_CODE_PCM12K16B			0x020C		/* PCM 11K 16bit */
#define ivTTS_CODE_PCM16K16B			0x0210		/* PCM 16K 16bit */
#define ivTTS_CODE_PCM22K16B			0x0216		/* PCM 22K 16bit */
#define ivTTS_CODE_PCM24K16B			0x0218		/* PCM 24K 16bit */
#define ivTTS_CODE_PCM32K16B			0x0220		/* PCM 32K 16bit */
#define ivTTS_CODE_PCM44K16B			0x022C		/* PCM 44K 16bit */
#define ivTTS_CODE_PCM48K16B			0x0230		/* PCM 48K 16bit */

/* constants for values of parameter ivTTS_PARAM_PCM_FORMAT */
#define ivTTS_PCM_FORMAT_DEFAULT	0x0000						/* PCM default */
#define ivTTS_PCM_FORMAT_6K16B		ivTTS_CODE_PCM6K16B			/* PCM 6K 16bit */
#define ivTTS_PCM_FORMAT_8K16B		ivTTS_CODE_PCM8K16B			/* PCM 8K 16bit */
#define ivTTS_PCM_FORMAT_11K16B		ivTTS_CODE_PCM11K16B		/* PCM 11K 16bit */
#define ivTTS_PCM_FORMAT_12K16B		ivTTS_CODE_PCM12K16B		/* PCM 11K 16bit */
#define ivTTS_PCM_FORMAT_16K16B		ivTTS_CODE_PCM16K16B		/* PCM 16K 16bit */
#define ivTTS_PCM_FORMAT_22K16B		ivTTS_CODE_PCM22K16B		/* PCM 22K 16bit */
#define ivTTS_PCM_FORMAT_24K16B		ivTTS_CODE_PCM24K16B		/* PCM 24K 16bit */
#define ivTTS_PCM_FORMAT_32K16B		ivTTS_CODE_PCM32K16B		/* PCM 32K 16bit */
#define ivTTS_PCM_FORMAT_44K16B		ivTTS_CODE_PCM44K16B		/* PCM 44K 16bit */
#define ivTTS_PCM_FORMAT_48K16B		ivTTS_CODE_PCM48K16B		/* PCM 48K 16bit */
/*
 *	PARAMETER STRUCTURES
 */

/* 内核发音人信息数组参数长度 */
#define SEM_ENGINENAME_LEN_MAX              64
#define SEM_VOICENAME_LEN_MAX               32
#define SEM_VOICE_LANG_MAX                  16
#define SEM_SAMPLERATE_MAX                   8

typedef struct tagTTSVoiceInfo TTSVoiceInfo, ivPtr PTTSVoiceInfo;

/* 发音人信息结构体 */
struct tagTTSVoiceInfo
{
    unsigned long        cbSize;                     /* [in] Size of structure */
    unsigned long        dwVoiceID;                  /* [out] Voice ID unique in the scope of engine */
    unsigned long        dwGender;                   /* [out] Gender of speaker */
    unsigned long        dwAge;                      /* [out] Age of speaker */
    ivInt8          szVoiceName[SEM_VOICENAME_LEN_MAX];     /* [out] Voice description string */
    unsigned long        dwLangCount;                /* [out] Count of languages */
    unsigned long        arrLanguage[SEM_VOICE_LANG_MAX];        /* [out] Language identifier */
    unsigned long        arrLangScore[SEM_VOICE_LANG_MAX];       /* [out] Language scores */
    unsigned long        dwSRCount;                  /* [out] Count of sample rates */
    unsigned long        arrSampleRate[SEM_SAMPLERATE_MAX];      /* [out] Voice Sample rate array: 6k, 8k, 11k, 16k, ... */
    unsigned long        dwEngineID;                 /* [out] ID of the engine which this voice belong to */
    unsigned long        dwReserved;                 /* [in/out] Reserved: must set to zero */
};


/* parameters for voice effect amplify */
typedef struct tagIsTTSVEAmplifyParam TIsTTSVEAmplifyParam;

struct tagIsTTSVEAmplifyParam
{
	ivUInt16	m_nPeriod;			/* 1-2000ms */
	ivUInt8		m_fAmpMin;			/* 0-100% */
	ivUInt8		m_fAmpMax;			/* 0-100% */
};


/* parameters for voice effect echo */
typedef struct tagIsTTSVEEchoParam TIsTTSVEEchoParam;

struct tagIsTTSVEEchoParam
{
	ivUInt8		m_fInitDecay;		/* 0-100% */
	ivUInt8		m_fDecay;			/* 0-100% */
	ivUInt16	m_nDelay;			/* 1-2000ms */
};


/* parameters for voice effectVE reverb */
typedef struct tagIsTTSVEReverbParam TIsTTSVEReverbParam;

struct tagIsTTSVEReverbParam
{
	ivUInt8		m_fInitDecay;		/* 0-100% */
	ivUInt8		m_nFilters;			/* 1-16 */
	ivUInt8		m_fDecay[16];		/* 0-100% */
	ivUInt8		m_nDelay[16];		/* 1-100ms */
};


/* parameters for voice effect chrous */
typedef struct tagIsTTSVEChrousParam TIsTTSVEChrousParam;

struct tagIsTTSVEChrousParam
{
	ivUInt8		m_fInitDecay;		/* 0-100% */
	ivUInt8		m_fInitRatio;		/* 0-100% */
	ivUInt8		m_nDelayMin;		/* 1-40ms */
	ivUInt8		m_nDelayMax;		/* 1-40ms */
	ivUInt8		m_nFilters;			/* 1-16 */
	ivUInt8		m_nFilterFreq;		/* 1-50Hz */
	ivUInt8		m_nFilterFreqDelta;	/* 1-50Hz */
};


/* parameters for voice effect pitch */
typedef struct tagIsTTSVEPitchParam TIsTTSVEPitchParam;

struct tagIsTTSVEPitchParam
{
	ivUInt8		m_fDeltaPitch;		/* 0-90% */
	ivUInt16	m_nPeriod;			/* 1-20000ms */
};


/* get pcm data */
ivTTSErrID ivCall ivTTS_GetData(
	ivHTTS			hTTS,				/* [in] handle to an instance */
	ivPointer		pData,				/* [in] pointer of pcm data buffer */
	ivSize ivPtr	pSize );			/* [in/out] data size */

/* Label text with symbol */
ivTTSErrID ivCall ivTTS_SymbolLabel(
		ivHTTS hTTS,	/* [in] handle to an instance */
		ivCPointer pText, /* [in] pointer of the text buffer */
		ivSize nTextLen, /* [in] size of the text buffer */
		ivCPointer pSymbol, /* [in] pointer of the symbol buffer */
		ivSize nSymbolLen, /* [in] size of the symbol buffer */
		ivPointer pOut, /* [out] pointer of the output buffer */
		ivPUInt32 pnOutLen, /* [in], max size of the output buffer, [out] actually used size of the output buffer */
		ivBool bTone /* [in], if the symbols have tone */
	);

/* I-SAMPA Watch CallBack */
typedef ivTTSErrID (ivProc ivTTSCB_WatchISampa)(
	ivPointer	pParameter,   /* [in] pParameter  user callback parameter */
	ivUInt16	nISampaType,  /* [in] nISampaType ISampa component type   */
	ivCPointer	pcISampaBuf,  /* [in] pcISampaBuf ISampa buffer           */
	ivSize		nISampaLen);  /* [in] nISampaLen  ISampa length           */


#ifdef __cplusplus
}
#endif


#endif /* !IFLYTEK_VOICE__TTS__H */

