将text转成语音或者pcm数据，利用了c，JNI,java

TtsJniDemo:是调用了科大讯飞的.a库生成了一个android可以直接调用的.so

TTSces：是一个android的项目，调用TtsJniDemo生成的.so库，将text字生成语音和pcm

Document：是科大讯飞的文档，其中Aisound6.0Pro用户指南是其中最重要的

Resource：.irf是要放到手机上的，其中Resource.irf是需要的

整个过程都是通过JNI来实现c跟java之间的通信的

1.要先定一个供java使用的接口java文件，如下：要想用jni，接口前面要添加native， Load动态库：System.loadLibrary("Aisound"); 说明要调用的库为Aisound.so,JniCreate接口要在库里面实现(TTS.java)

```java
package com.iflytek.tts.TtsService;
public class TTS{
  
    static {
        System.loadLibrary("Aisound");
    }
  public static native int JniCreate(String resFilename)
}
```

2.需要通过.java来生成c可以用的.h文件

```java
需要在他所在的目录下执行
javah  TTS.java
这样就会生成一个TTS.h文件
jint JNICALL Java_com_iflytek_tts_TtsService_Tts_JniCreate就是.h中的头文件，他后面的名字是根据.java中的包名类名，接口名称一起构成的，java调用JniCreate接口就是调用如上文件
```

3.在c中使用jni的一些东西

3.1 如何在ndk中加log

```java
两种加log的方式
#define LOG_TAG "TTS"
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__ )
printf(" pBuf=%s\n",pBuf);
#define LOGD(text) __android_log_write(ANDROID_LOG_DEBUG,"TtsServiceJni",text)
LOGD("ParamChCB");
```

3.2 jni通过回调传递信息给java

```java
m_pBuff =  (*m_JavaTtsClass.env)->NewByteArray(m_JavaTtsClass.env,(jint)(nSize));
	if (NULL != m_pBuff){
(*m_JavaTtsClass.env)->SetByteArrayRegion(m_JavaTtsClass.env
				,m_pBuff, 0, (jint)(nSize), (jbyte*)pcData);
				/*首先通过 NewByteArray 在堆上分配数组对象，然后通过SetByteArrayRegion
				把本地的数组数据拷贝到堆上分配的数组中去，然后通过返回值将分配的数组
				对象返回到Java层即可。对于回调的方式，这几步操作也是一样的，唯一的不同是，
				回调方式不是以返回值的方式将数组对象返回给Java层，而是在回调函数中，
				以回调函数参数的形式返回给Java层。*/
(*m_JavaTtsClass.env)->CallStaticVoidMethod(m_JavaTtsClass.env,m_JavaTtsClass.clazzTts,m_JavaTtsClass.onJniOutData,(jint)(nSize),m_pBuff);
/*onJniOutData java那边的回调接口，后面的(jint)(nSize),m_pBuff)就是接口的两个参数*/
(*m_JavaTtsClass.env)->DeleteLocalRef(m_JavaTtsClass.env
				,m_pBuff);
    }
/*要在刚开始初始化的时候设置如下，设置了下面的onJniOutData信息就可以发给java了*/
/*这句说明了在com/iflytek/tts/TtsService/Tts弄onJniOutData函数才可以*/
jclass classTmp =  (*env)->FindClass(env,"com/iflytek/tts/TtsService/Tts");
	m_JavaTtsClass.clazzTts = (*env)->NewGlobalRef(env,classTmp);
	m_JavaTtsClass.env = env;
	if (NULL != m_JavaTtsClass.clazzTts ){
m_JavaTtsClass.onJniOutData  = (*env)->GetStaticMethodID(env
	, m_JavaTtsClass.clazzTts,"onJniOutData","(I[B)V");
        /*一下就是java端的接口*/
     public static  void onJniOutData(int len,byte [] data)
```

3.3 在jni中将java传的string转成c可以用的

```c
/*text 是javad*/   
char* test = (char*)env->GetStringUTFChars(text, JNI_FALSE);
```

