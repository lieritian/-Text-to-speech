package com.iflytek.tts;

import com.iflytek.tts.TtsService.Tts;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;

public class TtsJniDemo extends Activity implements OnClickListener{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);        
        this.findViewById(R.id.btn_start).setOnClickListener(this);
        this.findViewById(R.id.btn_stop).setOnClickListener(this);
        
        
     
		Tts.JniCreate("/sdcard/Resource.irf");
        Tts.JniSetParam(256, 1); 
        Tts.JniSetParam(1280, 3); 
//        Tts.JniSetParam(1297, 20);
//        Tts.JniSetParam(1282, 20000);
    }
    
    //按键控制    
    @Override
    public void onClick(View v) {
    	switch (v.getId()){
    	case R.id.btn_start:
    		onClickStart();
    		break;
    	case R.id.btn_stop:
    		onClickStop();
    		break;		
    	}		
    }

    private void onClickStop() {
    	//停止并退出
        Tts.JniStop();
        Tts.JniDestory();
    }


    private void onClickStart() {
    	//开启TTS播报线程
    	Tts.startReadThread(this);
    }
}

