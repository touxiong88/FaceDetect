package com.stereo.FaceDetector;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import com.stereo.activity.CameraActivity;
import com.stereo.util.HoloLister;
public class FaceDetector {
	
	public String TAG = "Holography";
	
	static{
		System.loadLibrary("FFTEmTest");
	}
	Context mContext;
	public static HoloLister mHoloLister;
	public native void nativeSetEyeDistance(int minDist, int maxDist);
    public native int   nativeConfigAlg(AssetManager ass, String filename);
    public native static int   nativeFaceInit(int width, int height);
    public native static int   nativeDetect(byte[] data);
    public native static void  nativeGetFace(Face[] face, int index);
	public native static float nativeGetConfidence();
	public native static float nativeGetMidpointx();
	public native static float nativeGetMidpointy();
	public native static float nativeGetEyedist();

	public native static float  GetEyeLeftX();
	public native static float  GetEyeRightX();
	public native static float  GetEyeLeftY();
	public native static float  GetEyeRightY();
	
	public static native  void nativeRelease();
	
	public native static int nativeFaceDetectInit(int width, int height, AssetManager ass, String filename , int minDist, int maxDist);
	public native int   nativeFaceRelease();//重置人脸检测对象
	
	public FaceDetector(Context context){
		mContext = context;
	//	nativeConfigAlg(mContext.getAssets(), "RFFprec_501.bmd");//Rffprec可信度值最低、Rffstd其次、Rffspeed最高
	//	setEyeDis(100,200);
	}
	
	public void setEyeDis(int min,int max){
	//	nativeSetEyeDistance(min, max);
	}
	
	public void init(int width,int height, int fdMidDis, int fdMaxDis, int faces){
		Log.v("Test", "fdMidDis = " + fdMidDis + "    fdMaxDis = " + fdMaxDis);
		int re = nativeFaceDetectInit(width,height,mContext.getAssets(), "RFFprec_501.bmd",fdMidDis,fdMaxDis);
		Log.v(TAG,"nativeFaceDetectInit = " + re);
	}

    public int findFaces(byte[] data, Face[] faces){
		// TODO Auto-generated method stub

		int number = nativeDetect(data);
		Log.d(TAG,"number: "+number);
    	if(number > 0){

			float eyeLeftX = GetEyeLeftX();
			float eyeRightX = GetEyeRightX();
			float eyeLeftY = GetEyeLeftY();
			float eyeRightY = GetEyeRightY();

			Log.d(TAG,"eyes_para: "+eyeLeftX+" "+eyeRightX+" "+eyeLeftY+" "+eyeRightY);

			Message msg = new Message();
			msg.what = 1;
			Bundle bundle = new Bundle();
			bundle.putFloat("eyeLeftX",eyeLeftX);  //往Bundle中存放数据
			bundle.putFloat("eyeLeftY",eyeLeftY);  //往Bundle中存放数据
			bundle.putFloat("eyeRightX",eyeRightX);  //往Bundle中存放数据
			bundle.putFloat("eyeRightY",eyeRightY);  //往Bundle中存放数据
			msg.setData(bundle);//mes利用Bundle传递数据
//			mHoloLister.onMessage(msg); 用监听接口方法传递消息 TODO 2017-12-8
			CameraActivity.mInfoHandler.sendMessage(msg);//用activity中的handler发送消息
    	}

    	
    	for(int i =0;i < number;i++){
  //  		Log.d(TAG,"I:"+i);
    	//	nativeGetFace(null,i);
  //  		Log.d(TAG," " + nativeGetMidpointx()+" " + nativeGetMidpointy()+ " "
 //   		+ nativeGetEyedist() + " " + nativeGetConfidence());
    	//	if(faces[i] == null){
    	//		faces[i] = new Face();
    	//	}
    	//	faces[i].setValue(nativeGetMidpointx(), nativeGetMidpointy(), nativeGetEyedist(), nativeGetConfidence());	
    	}
		return 0;
	}
	public static void passHoloLister(HoloLister ll){
		mHoloLister = ll;
	}
}
