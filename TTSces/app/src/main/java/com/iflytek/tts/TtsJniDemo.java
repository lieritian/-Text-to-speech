package com.iflytek.tts;

import com.example.tts.R;
import com.iflytek.tts.TtsService.Tts;

import android.Manifest;
import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;

import androidx.core.app.ActivityCompat;

public class TtsJniDemo extends Activity implements OnClickListener{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
       this.findViewById(R.id.btn_start).setOnClickListener(this);
        this.findViewById(R.id.btn_stop).setOnClickListener(this);
        this.findViewById(R.id.btn_init).setOnClickListener(this);
        String path= Environment.getExternalStorageDirectory().getAbsolutePath();
//        //  Tts.JniCreate("/sdcard/Resource.irf");
//        Tts.JniCreate(path + "/Resource.irf");
//
////        Tts.ivTTS_init(path + "/Resource.irf");
////        Tts.ivTTS_setParameter(256, 1);
////        Tts.ivTTS_setParameter(1280, 3);
////        Tts.ivTTS_setParameter(1297, 20);
////        Tts.ivTTS_setParameter(1282, 20000);
////        Tts.ivTTS_setParameter(0x00003001, 0x0210);
//        Tts.JniSetParam(256, 1);
//        Tts.JniSetParam(1280, 3);
//        Tts.JniSetParam(1297, 20);
//        Tts.JniSetParam(1282, 20000);
//        Tts.JniSetParam(0x00003001, 0x0210);
//
//		//Tts.JniCreate("/sdcard/Resource.irf");
//     //   Tts.JniSetParam(256, 1);
//      //  Tts.JniSetParam(1280, 3);
//      //  Tts.JniSetParam(1297, 20);
//       // Tts.JniSetParam(1282, 20000);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.INTERNET,
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0x1235);
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
        case R.id.btn_init: {
            String path= Environment.getExternalStorageDirectory().getAbsolutePath();
            //  Tts.JniCreate("/sdcard/Resource.irf");
            Tts.JniCreate(path + "/Resource.irf");
            //设置语速啥的
//        Tts.ivTTS_init(path + "/Resource.irf");
//        Tts.ivTTS_setParameter(256, 1);
//        Tts.ivTTS_setParameter(1280, 3);
//        Tts.ivTTS_setParameter(1297, 20);
//        Tts.ivTTS_setParameter(1282, 20000);
//        Tts.ivTTS_setParameter(0x00003001, 0x0210);
            Tts.JniSetParam(256, 1);
            Tts.JniSetParam(1280, 3);
            Tts.JniSetParam(1297, 20);
            Tts.JniSetParam(1282, 20000);
            Tts.JniSetParam(0x00003001, 0x0210);

//		Tts.JniCreate("/sdcard/Resource.irf");
//        Tts.JniSetParam(256, 1);
//        Tts.JniSetParam(1280, 3);
//        Tts.JniSetParam(1297, 20);
//        Tts.JniSetParam(1282, 20000);
            break;
        }
    	}
    }

    private void onClickStop() {
    	//停止并退出
//        Tts.ivTTS_stop();
//        Tts.ivTTS_cleanup();
        Tts.JniDestory();
    }


    private void onClickStart() {
    	//开启TTS播报线程
    	Tts.startReadThread(this);
    }
}

