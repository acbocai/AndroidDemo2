package com.whulzz.demo;

import android.app.Application;
import android.content.Context;

public class DemoApplication extends Application {
    static {
        System.loadLibrary("demo");
    }
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    public static native void testJson(final String path);
    public static native void testUnw();
}
