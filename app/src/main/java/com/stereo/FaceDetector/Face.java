package com.stereo.FaceDetector;

import android.graphics.PointF;

public class Face {
        /** The minimum confidence factor of good face recognition */
        public static final float  CONFIDENCE_THRESHOLD = 0.4f;
    	public              String TAG                  = "camera";
    	
    	private float   mConfidence;
        private float   mMidPointX;
        private float   mMidPointY;
        private float   mEyesDist;
    	
        public Face() {
        }
        
        public float confidence() {
            return mConfidence;
        }

        public void getMidPoint(PointF point) {
            point.set(mMidPointX, mMidPointY);
        }
        public float pointx(){
        	
        	return mMidPointX;
        }
        public float pointy(){
        	
        	return mMidPointY;
        }
        public float eyesDistance() {
            return mEyesDist;
        }
		public void setValue(float x,float y,float dis,float c){
			mConfidence = c;
			mMidPointX = x;
			mMidPointY = y;
			mEyesDist = dis;
		}
    
}
