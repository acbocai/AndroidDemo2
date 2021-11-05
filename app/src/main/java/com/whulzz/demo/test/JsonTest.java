package com.whulzz.demo.test;

import android.content.Context;

import com.whulzz.demo.DemoApplication;
import com.whulzz.demo.utils.FileUtil;

public class JsonTest {
    public static void main(Context appContext) {
        final String assetPath = "test.json";
        final String dstDir = appContext.getApplicationInfo().dataDir;
        FileUtil.tryExtractFromAsset(appContext, assetPath, dstDir, assetPath, false);
        DemoApplication.testJson(dstDir + "/" + assetPath);
    }
}
