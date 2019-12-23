//package com.mapbar.wedrive;
package com.iflytek.tts.TtsService;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import com.iflytek.tts.TtsJniDemo;
public class Tts{
	static {
		System.loadLibrary("Aisound");
	}
	private static AudioTrack mAudio = null;
	private static final String TAG = "TtsService(audio)";
	private static int mStreamType = AudioManager.STREAM_MUSIC;
	private static int mSampleRate = 16000;
	private static int mBuffSize = 20480;


	static {
		mAudio = new AudioTrack(mStreamType
				,mSampleRate, AudioFormat.CHANNEL_CONFIGURATION_MONO
				,AudioFormat.ENCODING_PCM_16BIT
				,mBuffSize,AudioTrack.MODE_STREAM );
		Log.d(TAG,"mBuffSize = " + mBuffSize);
		Log.d(TAG," AudioTrack create ok");
	}

	/**
	 * For C call
	 */
	public static  void onJniOutData(int len,byte [] data){

		Log.e(TAG," gxd onJniOutData  "+len);
		if (null == mAudio){
			Log.e(TAG," mAudio null");
			return;
		}
		if (mAudio.getState() != AudioTrack.STATE_INITIALIZED ){
			Log.e(TAG," mAudio STATE_INITIALIZED");
			return;
		}

		try{
			mAudio.write(data, 0, len);
			mAudio.play();
		}catch (Exception e){
			Log.e(TAG,e.toString());
		}
	}

	/**
	 * For C Watch Call back
	 * @param nProcBegin
	 */
	public static void onJniWatchCB(int nProcBegin){
		Log.d(TAG,"onJniWatchCB  process begin = " + nProcBegin);

	}

	public  synchronized static void startReadThread(TtsJniDemo ctx){
		class TtsRunThread implements Runnable{
			@Override
			public void run() {
						JniSpeak("您好，欢迎使用科大讯飞语音合成系统122222222.，");
				//ivTTS_speak("您好，欢迎使用科大讯飞语音合成系统");
			}
		}
		Thread ttsRun = (new Thread(new TtsRunThread()));
		ttsRun.setPriority(Thread.MAX_PRIORITY);
		ttsRun.start();
	}
//	public static native  boolean ivTTS_init(String filename);
//	public static native  int ivTTS_cleanup();
//	public static native  boolean ivTTS_reInit(String fileName);
//	public static native  int ivTTS_speak(String text);
//	public static native  void ivTTS_stop();
//	public static native  int ivTTS_setParameter(int param, int value);
//	public static native  int ivTTS_getParameter(int param);
	public static native int JniGetVersion();
	public static native int JniCreate(String resFilename);
	public static native int JniDestory();
	public static native int JniStop();
	public static native int JniSpeak(String text);
	public static native int JniSetParam(int paramId,int value);
	public static native int JniGetParam(int paramId);
	public static native int JniIsPlaying();
	public static native boolean JniIsCreated();
	//public static void onJniOutData(int TotalLength, byte[] Data)
//	{
	//	Log.e("gxd","55555"+TotalLength);
	//}
}
