#include "com_lichao_ffmpetyuv_MyPlay.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <jni.h>
//编码
#include "include/libavcodec/avcodec.h"
//封装格式处理
#include "include/libavformat/avformat.h"
//像素处理
#include "include/libswscale/swscale.h"
//YUV
#include "include/libyuv/libyuv.h"
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"lichao",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"lichao",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL Java_com_lichao_ffmpetyuv_MyPlay_render
  (JNIEnv *env, jclass jcls, jstring input_jstr, jobject surface){
	//需要转码的视频文件(输入的视频文件)
	const char* input_cstr = (*env)->GetStringUTFChars(env,input_jstr,NULL);

	//1.注册所有组件
	av_register_all();

	//封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
	AVFormatContext *pFormatCtx = avformat_alloc_context();

	//2.打开输入视频文件
	if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) != 0)
	{
		LOGE("%s","无法打开输入视频文件");
		return;
	}

	//3.获取视频文件信息
	if (avformat_find_stream_info(pFormatCtx,NULL) < 0)
	{
		LOGE("%s","无法获取视频文件信息");
		return;
	}

	//获取视频流的索引位置
	//遍历所有类型的流（音频流、视频流、字幕流），找到视频流
	int v_stream_idx = -1;
	int i = 0;
	//number of streams
	for (; i < pFormatCtx->nb_streams; i++)
	{
		//根据类型判断，是否是视频流
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			v_stream_idx = i;
			break;
		}
	}

	if (v_stream_idx == -1){
		LOGE("%s","找不到视频流\n");
		return;
	}

	//只有知道视频的编码方式，才能够根据编码方式去找到解码器
	//获取视频流中的编解码上下文
	AVCodecContext *pCodeCtx = pFormatCtx->streams[v_stream_idx]->codec;
	//4.根据编解码上下文中的编码id查找对应的解码
	AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
	//（迅雷看看，找不到解码器，临时下载一个解码器）
	if (pCodec == NULL){
		LOGE("%s","找不到解码器\n");
		return;
	}

	//5.打开解码器
	if (avcodec_open2(pCodeCtx,pCodec,NULL)<0){
		LOGE("%s","解码器无法打开\n");
		return;
	}

	//输出视频信息
	LOGI("视频的文件格式：%s",pFormatCtx->iformat->name);
	LOGI("视频时长：%d", (pFormatCtx->duration)/1000000);
	LOGI("视频的宽高：%d,%d",pCodeCtx->width,pCodeCtx->height);
	LOGI("解码器的名称：%s",pCodec->name);

	//准备读取
	//AVPacket用于存储一帧一帧的压缩数据（H264）
	//缓冲区，开辟空间
	AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));

	//AVFrame用于存储解码后的像素数据(YUV)
	//内存分配，像素数据（解码数据）
	AVFrame *yuv_frame = av_frame_alloc();
	AVFrame *rgb_frame = av_frame_alloc();

	//native绘制
	//窗体
	ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);
	//绘制时的缓冲区
	ANativeWindow_Buffer outBuffer;

	int got_picture, ret;
	int frame_count = 0;
	//6.一帧一帧的读取压缩数据
	while (av_read_frame(pFormatCtx, packet) >= 0){
		//只要视频压缩数据（根据流的索引位置判断）
		if (packet->stream_index == v_stream_idx){
			//7.解码一帧视频压缩数据，得到视频像素数据
			ret = avcodec_decode_video2(pCodeCtx, yuv_frame, &got_picture, packet);
			if (ret < 0){
				LOGE("%s","解码错误");
				return;
			}

			//为0说明解码完成，非0正在解码
			if (got_picture){
				LOGI("解码第%d帧",frame_count++);
				//1.lock
				//设置缓冲区的属性（宽、高、像素格式）
				ANativeWindow_setBuffersGeometry(nativeWindow, pCodeCtx->width, pCodeCtx->height,WINDOW_FORMAT_RGBA_8888);
				ANativeWindow_lock(nativeWindow,&outBuffer,NULL);

				//2.fix buffer
				//设置rgb_frame的属性（像素格式、宽高）和缓冲区
				//rgb_frame缓冲区与outBuffer.bits是同一块内存
				avpicture_fill((AVPicture *)rgb_frame, outBuffer.bits, PIX_FMT_RGBA, pCodeCtx->width, pCodeCtx->height);

				//YUV->RGBA_8888
				I420ToARGB(yuv_frame->data[0],yuv_frame->linesize[0],
						yuv_frame->data[2],yuv_frame->linesize[2],
						yuv_frame->data[1],yuv_frame->linesize[1],
						rgb_frame->data[0], rgb_frame->linesize[0],
						pCodeCtx->width,pCodeCtx->height);

				//3.unlock
				ANativeWindow_unlockAndPost(nativeWindow);
				usleep(1000 * 16);
			}
		}
		//释放资源
		av_free_packet(packet);
	}

	ANativeWindow_release(nativeWindow);
	av_frame_free(&yuv_frame);
	avcodec_close(pCodeCtx);
	avformat_free_context(pFormatCtx);

    (*env)->ReleaseStringUTFChars(env,input_jstr,input_cstr);
}
