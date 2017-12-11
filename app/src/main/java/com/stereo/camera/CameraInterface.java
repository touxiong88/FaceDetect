package com.stereo.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;

import com.stereo.FaceDetector.Face;
import com.stereo.FaceDetector.FaceDetector;
import com.stereo.util.CamParaUtil;
import com.stereo.util.HoloLister;

import java.io.IOException;
import java.util.List;

public class CameraInterface {
	private static String TAG = "Holography";
	private Context mContext;
	private Camera mCamera;
	private Camera.Parameters mParams;
	private boolean isPreviewing = false;

	private static CameraInterface mCameraInterface;
	private int frontCam = 1;
	HandlerThread mCheckMsgThread;
	Handler       mCheckMsgHandler;
	FaceDetector  faceDect;
	Face[]        faces;
	boolean  turn_on_face = true;
	private int previewWidth = 800;
	private int previewHeight = 480;
	protected int mFDMinDis = 100;   // 人眼追踪的识别的最小距离
	protected int mFDMaxDis = 200;   // 人眼追踪的识别的最大距离

	private byte[] data1;
	private byte[] data2;
	private byte[] data3;
	byte[] mLastBuff ;

	public interface CamOpenOverCallback{
		public void cameraHasOpened();
	}

	private CameraInterface(Context mContext){
		this.mContext = mContext;
		initHandler();
		Log.d(TAG,"faceDect.init");
	}
	public static CameraInterface getInstance(Context context){

		if(mCameraInterface == null){
			synchronized(CameraInterface.class){
				if(mCameraInterface == null){
					mCameraInterface = new CameraInterface(context);
				}
			}
		}
		return mCameraInterface;
	}
	/**打开Camera
	 * @param callback
	 */
	public void doOpenCamera(CamOpenOverCallback callback){
		Log.i(TAG, "Camera open....");
		if(mCamera == null){
			mCamera = Camera.open(frontCam);

			Log.i(TAG, "Camera open over....");

			if(callback != null){
				callback.cameraHasOpened();
			}
		}else{
			Log.i(TAG, "Camera open 异常!!!");
			doStopCamera();
		}
			
	
	}
	/**使用Surfaceview开启预览
	 * @param holder
	 * @param previewRate
	 */
	public void doStartPreview(SurfaceHolder holder, float previewRate){
		Log.i(TAG, "doStartPreview...");
		if(isPreviewing){
			mCamera.stopPreview();
			return;
		}
		if(mCamera != null){
			initCamera(previewRate);
			try {
				mCamera.setPreviewDisplay(holder);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}


	}
	/**使用TextureView预览Camera
	 * @param mSurfaceTexture
	 * @param previewRate
	 */
	public void doStartPreview(SurfaceTexture mSurfaceTexture, float previewRate){
		Log.i(TAG, "doStartPreview...");

		if(mCamera != null){
			initCamera(previewRate);
			if(null == faceDect) {
				faceDect = new FaceDetector(mContext);
				faceDect.init(previewWidth, previewHeight, mFDMinDis, mFDMaxDis, 5);
			}

			try {
				Log.d(TAG,"Camera number:"+Camera.getNumberOfCameras());
				mLastBuff = new byte[previewWidth*previewHeight*2];
				data1 = new byte[previewWidth*previewHeight*3/2];
				data2 = new byte[previewWidth*previewHeight*3/2];
				data3 = new byte[previewWidth*previewHeight*3/2];

				mCamera.setPreviewCallbackWithBuffer(null);
				mCamera.addCallbackBuffer(data1);
				mCamera.addCallbackBuffer(data2);
				mCamera.addCallbackBuffer(data3);
				mCamera.setPreviewCallbackWithBuffer(mPreviewCallback);
				mCamera.setPreviewTexture(mSurfaceTexture);
			}catch (IOException e){
				e.printStackTrace();
			}
		}
		
	}

	/**
	 * 停止预览，释放Camera
	 */
	public void doStopCamera(){
		if(null != mCamera)
		{
			mCamera.setPreviewCallback(null);
			mCamera.stopPreview(); 
			isPreviewing = false; 
			mCamera.release();
			mCamera = null;     
		}
	}

	public boolean isPreviewing(){
		return isPreviewing;
	}



	private void initCamera(float previewRate){
		if(mCamera != null){

			mParams = mCamera.getParameters();
//			mParams.setPictureFormat(PixelFormat.JPEG);//设置拍照后存储的图片格式
//			CamParaUtil.getInstance().printSupportPictureSize(mParams);
//			CamParaUtil.getInstance().printSupportPreviewSize(mParams);
			//设置PreviewSize和PictureSize
//			Size pictureSize = CamParaUtil.getInstance().getPropPictureSize(
//					mParams.getSupportedPictureSizes(),previewRate, 800);
//			mParams.setPictureSize(pictureSize.width, pictureSize.height);
			Size previewSize = CamParaUtil.getInstance().getPropPreviewSize(
					mParams.getSupportedPreviewSizes(), previewRate, 800);
			previewWidth = previewSize.width;
			previewHeight = previewSize.height;
			mParams.setPreviewSize(previewSize.width, previewSize.height);
//			mParams.setPreviewSize(previewWidth, previewHeight);

			mCamera.setDisplayOrientation(90);


//			CamParaUtil.getInstance().printSupportFocusMode(mParams);
			List<String> focusModes = mParams.getSupportedFocusModes();
			if(focusModes.contains("continuous-video")){
				mParams.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
			}
			mCamera.setParameters(mParams);	
			mCamera.startPreview();//开启预览

			mParams = mCamera.getParameters(); //重新get一次
			Log.i(TAG, "最终设置:PreviewSize--With = " + mParams.getPreviewSize().width
					+ "Height = " + mParams.getPreviewSize().height);
			Log.i(TAG, "最终设置:PictureSize--With = " + mParams.getPictureSize().width
					+ "Height = " + mParams.getPictureSize().height);
		}
	}

	private void initHandler() {

		mCheckMsgThread = new HandlerThread("FaceDetect");
		mCheckMsgThread.start();
		mCheckMsgHandler = new Handler(mCheckMsgThread.getLooper())
		{
			@Override
			public void handleMessage(Message msg)
			{
				if(msg.what == 0){

					byte[] data = mLastBuff.clone();

					faceDect.findFaces(data,faces);

					Log.d(TAG,"Data Size:"+data.length);

				}else if(msg.what == 1){
					faceDect.nativeRelease();
				}
			}
		};
	}

	private Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback(){
		public void onPreviewFrame(byte[] data, Camera camera){
			Log.v(TAG, "onPreviewFrame");
			//	Log.d(TAG,"data size:"+data.length);
			if(turn_on_face){
				mCheckMsgHandler.removeMessages(0);
				mCheckMsgHandler.sendEmptyMessage(0);
				mLastBuff = data.clone();

				Log.v(TAG, "onPreviewFrame turn_on_face");
			}
			if(mCamera != null) {
				mCamera.addCallbackBuffer(data);
			}

		}
	};



	public void passHoloLister(HoloLister ll){
		FaceDetector.passHoloLister(ll);
	}

}
