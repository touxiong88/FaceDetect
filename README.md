# FaceDetect
Android Native neven to Android app,

安卓原生人脸检测库neven移植到安卓APP中,JNI调用获取人脸位置,人眼坐标并通过webview显示出来,通过GLSURfaceView预览Camera显示


Android app 与网页交互 WebView 与JS(JavaScript)交互 安卓webview 传递数据到网页
转载请注明出处:http://blog.csdn.net/touxiong/article/details/78769966
本文包含如下要点：

WebView的基础用法;
执行JavaScriptInterface及其方式;
demo app的AndroidStudio源代码。

老大说做个demo把人脸检测中人眼坐标传递到网页上显示出来,第一个想到的事webview,
但书上支持说用GET POST,并不能实现这种效果,请教罗享同志后百度webview与JS交互就有了.

进入正题:
网页HTML 网页 JavaScript 代码

```

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>hello World !</title>
    <script type="text/javascript">
        function  changebody(leftX,leftY,rightX,rightY) {
            document.getElementById("textOne").innerText = "左眼 x:"+leftX+" y:"+leftY;
            document.getElementById("textTwo").innerText = "右眼 x:"+rightX+" y:"+rightY;
        }


    </script>


</head>
<body>
   <div id="textOne";width=80px;height=100px>左眼 x:  y:</div>
   <div id="textTwo";width=80px;height=100px>右眼 x:  y:</div>

</body>
</html>

```


网页服务器已经架设好了,运行的代码如上,现在Android需要通过webview将数据传递给changebody在网页中显示出来.

关键代码:

```
		WebView webView = (WebView) findViewById(R.id.web_view);//获取WebView的实例
		webView.getSettings().setJavaScriptEnabled(true); //让webview支持JavaScript脚本
		webView.setWebViewClient(new WebViewClient());//调用setwebviewclient方法并传入一个webviewclient实例
		webView.loadUrl("http://10.10.31.3:8080/index.jsp");//调用loadURL方法并传入网址 即可展示网页内容
		mJSHook = new JSHook();//获取JSHook实例
		webView.addJavascriptInterface(mJSHook, "test");//添加JS反射接口并传入mJSHook实例
```
webview提供addJavascriptInterface方式。该方式包含两项参数：

绑定到JavaScript的类实例。
用来显示JavaScript中的实例的名称。

	Class JSHook:

```
	String CHANGE_BODY = "javascript:changebody(%s)";

	public class JSHook{

		public void sendEyeXY(float x, float y, float m, float n){
			Log.d(TAG , "JSHook.sendEyeXY() called! + ");
			String arg = x + ","+ y+ ","+ m+ ","+ n;
			//changebody(x,y,m,n)

			webView.loadUrl(String.format(CHANGE_BODY , arg));
		}
	}
```

调用:

```
mJSHook.sendEyeXY(leftEyeX,eyeLeftY,eyeRightX,eyeRightY);
```




测试demo的时候发现代码都是按照webview与JS开发流程写的,但是数据就是没传递过去,后台服务器也没检测到访问,罗同志一直怀疑是参数传递有问题,故上面的参数单独拿出来了,然后灵光一现,发现是在onCreat中调用的JavascriptInterface中的sendEyeXY方法,建议加一个按键去调用这个js interface,果然马上就把数据显示出来了,但这是为什么呢???

最后懒人福利,附上源码参考:
[DEMO源码](http://download.csdn.net/download/touxiong/10153545)
