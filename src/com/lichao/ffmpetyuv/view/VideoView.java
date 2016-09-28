package com.lichao.ffmpetyuv.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * 视频绘制的画布
 * @author dell
 *
 */
public class VideoView extends SurfaceView{

	public VideoView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public VideoView(Context context, AttributeSet attrs) {
		super(context, attrs);
		
	}

	public VideoView(Context context) {
		super(context);
		
	}

	private void init(){
		//初始化,SurfaceView绘制的像素格式
		SurfaceHolder holder = getHolder();
		holder.setFormat(PixelFormat.RGBA_8888);
	}
}
