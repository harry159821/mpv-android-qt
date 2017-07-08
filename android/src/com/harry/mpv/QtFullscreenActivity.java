package com.harry.mpv;

import android.app.Activity;
import android.os.Bundle;  
import android.view.Window;
import android.view.WindowManager;  
  
public class QtFullscreenActivity extends org.qtproject.qt5.android.bindings.QtActivity
{  
    private final static String TAG = "QtFullscreen";  
    @Override  
    public void onCreate(Bundle savedInstanceState) {                   
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
    }
}
