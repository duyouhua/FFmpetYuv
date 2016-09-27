package com.lichao.ffmpetyuv;

import java.io.File;

import com.lichao.ffmpetyuv.view.VideoView;
import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.Surface;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

/**
 * FFmpeg视频转码成YUV格式
 * 自己用Ubntu编译成的FFmpeg
 * @author LiChao
 *
 */
public class MainActivity extends Activity {

	private MyPlay play;
	private VideoView videoView;
	private Spinner sp_video;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        videoView = (VideoView)findViewById(R.id.video_view);
        sp_video = (Spinner) findViewById(R.id.sp_video);
        play = new MyPlay();
        //多种格式的视频列表
  		String[] videoArray = getResources().getStringArray(R.array.video_list);
  		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, 
  				android.R.layout.simple_list_item_1, 
  				android.R.id.text1, videoArray);
  		sp_video.setAdapter(adapter);
    }
    
    public void mPlay(View btn){
//    	String video = sp_video.getSelectedItem().toString();
//		String input = new File(Environment.getExternalStorageDirectory(),video).getAbsolutePath();
//		//surface传入到native中
//		Surface surface = videoView.getHolder().getSurface();
//		play.render(input, surface);
    	
    	String input = new File(Environment.getExternalStorageDirectory(),"hehuoren.flv").getAbsolutePath();
		String output = new File(Environment.getExternalStorageDirectory(),"lichao.pcm").getAbsolutePath();
		play.sound(input, output);    	
	}
}
