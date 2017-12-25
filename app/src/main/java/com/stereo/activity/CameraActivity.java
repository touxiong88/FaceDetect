package com.stereo.activity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.ViewGroup.LayoutParams;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.demo.playcamera_v3.R;
import com.stereo.camera.CameraGLSurfaceView;
import com.stereo.util.HoloLister;


public class CameraActivity extends Activity implements HoloLister {
	private static final String TAG = "Holography";
	CameraGLSurfaceView mCamGLSurfaceView = null;
	float previewRate = -1f;
	private static WebView webView;
	static JSHook mJSHook ;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);//
		//		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		//		requestWindowFeature(Window.FEATURE_NO_TITLE);//
		//		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);//
		setContentView(R.layout.activity_camera);
		initUI();
		initViewParams();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.camera, menu);
		return true;
	}

	@SuppressLint("JavascriptInterface")
	private void initUI(){
		mCamGLSurfaceView = (CameraGLSurfaceView)findViewById(R.id.camera_textureview);
		webView = (WebView) findViewById(R.id.web_view);//获取WebView的实例
		webView.getSettings().setJavaScriptEnabled(true); //让webview支持JavaScript脚本
		webView.setWebViewClient(new WebViewClient());//调用setwebviewclient方法并传入一个webviewclient实例
		webView.loadUrl("file:///android_asset/eye.html");//调用loadURL方法并传入网址 即可展示网页内容
		mJSHook = new JSHook();//获取JSHook实例
		webView.addJavascriptInterface(mJSHook, "test");//添加JS反射接口并传入mJSHook实例

/*
		webView = (WebView) findViewById(R.id.web_view);
		webView.getSettings().setJavaScriptEnabled(true);
		webView.setWebViewClient(new WebViewClient());
		webView.loadUrl("http://10.10.31.9/test.html");
		mJSHook = new JSHook();
		webView.addJavascriptInterface(mJSHook, "test");*/
	}
	private void initViewParams(){
		LayoutParams params = mCamGLSurfaceView.getLayoutParams();
		Point p = com.stereo.util.DisplayUtil.getScreenMetrics(this);
		params.width = p.x;
		params.height = p.y;
		previewRate = com.stereo.util.DisplayUtil.getScreenRate(this); //默认全屏的比例预览
		mCamGLSurfaceView.setLayoutParams(params);
		mCamGLSurfaceView.passHoloLister(this);
	}


	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		mCamGLSurfaceView.bringToFront();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		mCamGLSurfaceView.onPause();
	}

	public static Handler mInfoHandler = new Handler(){
		@Override
		public void handleMessage(Message msg) {

			if(msg.what == 1){
				float leftEyeX = msg.getData().getFloat("eyeLeftX");//接受msg传递过来的参数
				float eyeLeftY = msg.getData().getFloat("eyeLeftY");//接受msg传递过来的参数
				float eyeRightX = msg.getData().getFloat("eyeRightX");//接受msg传递过来的参数
				float eyeRightY = msg.getData().getFloat("eyeRightY");//接受msg传递过来的参数

				mJSHook.sendEyeXY(leftEyeX,eyeLeftY,eyeRightX,eyeRightY);
				Log.d(TAG,"eyeXY: "+leftEyeX+"  "+eyeLeftY+" "+eyeRightX+"  "+eyeRightY);


		}}
	};

	String CHANGE_BODY = "javascript:changebody(%s)";

	@Override
	public void onMessage(Message msg) {
		mInfoHandler.sendMessage(msg);
	}

	public class JSHook{

		public void sendEyeXY(float x, float y, float m, float n){
			Log.d(TAG , "JSHook.sendEyeXY() called! + ");
			String arg = x + ","+ y+ ","+ m+ ","+ n;
			//changebody(x,y,m,n)

			webView.loadUrl(String.format(CHANGE_BODY , arg));
		}
	}
}
