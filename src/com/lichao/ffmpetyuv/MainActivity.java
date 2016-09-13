package com.lichao.ffmpetyuv;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;

/**
 * FFmpeg视频转码成YUV格式
 * 自己用Ubntu编译成的FFmpeg
 * @author LiChao
 *
 */
public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    
    public void mDecode(View btn){
		String input = new File(Environment.getExternalStorageDirectory(),"sintel.mp4").getAbsolutePath();
		String output = new File(Environment.getExternalStorageDirectory(),"sintel_1280x720_yuv420p.yuv").getAbsolutePath();
		VideoUtils.decode(input, output);
	}
}
