package com.whulzz.demo.utils;

import android.content.Context;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class FileUtil {
    public static boolean tryExtractFromAsset(Context context, String assetPath, String dstDir, String dstPath, boolean forceUpdate) {
        boolean ret = false;
        if (context != null) {
            InputStream is = null;
            OutputStream os = null;
            try {
                File dstFile = new File(dstDir, dstPath);
                boolean exists = dstFile.exists();
                if (!exists || forceUpdate) {
                    is = context.getAssets().open(assetPath);
                    if (exists) dstFile.delete();//open(assetPath)成功，才需要删除dstFile
                    os = new FileOutputStream(dstFile);

                    byte[] buf = new byte[1024];
                    int size = 0;
                    while ((size = is.read(buf)) != -1) {
                        os.write(buf, 0, size);
                    }

                    os.flush();
                }

                if (dstFile.exists() && dstFile.length() > 0) {
                    ret = true;
                }

            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (os != null) {
                    try {
                        os.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if (is != null) {
                    try {
                        is.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return ret;
    }

    public static boolean checkJsonFile(File file, final String tag) {
        BufferedReader br = null;
        StringBuffer jsonBuffer = new StringBuffer();
        try {
            br = new BufferedReader(new FileReader(file));

            String line = null;

            while ((line = br.readLine()) != null) {
                jsonBuffer.append(line);
            }
        } catch (Exception e) {

        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        if (jsonBuffer.length() > 0) {
            try {
                JSONObject jsonObject = new JSONObject(jsonBuffer.toString());
                if (jsonObject.has(tag)) {
                    return true;
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        return false;
    }
}
