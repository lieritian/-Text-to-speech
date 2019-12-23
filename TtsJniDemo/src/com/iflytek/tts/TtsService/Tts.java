package com.iflytek.tts.TtsService;

import com.iflytek.tts.TtsJniDemo;

public final class Tts{
	static {
		System.loadLibrary("Aisound");	
	}
	
	/**
	 * 开始合成任务线程
	 */
	public  synchronized static void startReadThread(TtsJniDemo ctx){
		class TtsRunThread implements Runnable{			
			@Override
			public void run() {   
				JniSpeak("您好，欢迎使用科大讯飞语音合成系统，");	
			}			
		}		
		Thread ttsRun = (new Thread(new TtsRunThread()));
		ttsRun.setPriority(Thread.MAX_PRIORITY);
		ttsRun.start();
	}
	
	public static native int JniGetVersion();
	public static native int JniCreate(String resFilename);	
	public static native int JniDestory();	
	public static native int JniStop(); 
	public static native int JniSpeak(String text); 	
	public static native int JniSetParam(int paramId,int value);
	public static native int JniGetParam(int paramId);
	public static native int JniIsPlaying();
	public static native boolean JniIsCreated();	
}
