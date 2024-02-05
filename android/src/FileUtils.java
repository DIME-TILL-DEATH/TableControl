package com.fmone.utils;

import android.content.Context;
import android.app.Activity;

import android.database.Cursor;
import android.net.Uri;
import android.content.Intent;
import android.os.Build;
import android.content.ContentUris;
//import android.support.v4.content.FileProvider; // change to androidx?
import androidx.core.content.FileProvider;

import android.os.Environment;
import android.content.ContentResolver;

import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.provider.OpenableColumns;

import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;
import android.util.Log;
import java.lang.NumberFormatException;

import 	java.lang.SecurityException;

public class FileUtils {
    public static void takeReadUriPermission(Uri uri, Context context) {
        try{
            ContentResolver contentResolver = context.getContentResolver();
            contentResolver.takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }
        catch(Exception e){
            Log.d("takeUriPermissions. Caught exception:", e.getMessage());
        }
    }

    public static void takeWriteUriPermission(Uri uri, Context context) {
        try{
            ContentResolver contentResolver = context.getContentResolver();
            contentResolver.takePersistableUriPermission(uri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        }
        catch(Exception e){
            Log.d("takeUriPermissions. Caught exception:", e.getMessage());
        }
    }

    public static String getFileName(Uri uri, Context context) {
        String displayName = "";
        Cursor cursor;

        try{
            ContentResolver contentResolver = context.getContentResolver();

            String[] proj = {OpenableColumns.DISPLAY_NAME};
            cursor = contentResolver.query(uri, proj, null, null, null, null);

            if (cursor != null && cursor.moveToFirst()) {

                int column_index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if(column_index != -1){
                    displayName = cursor.getString(column_index);
                }
                cursor.close();
            }
        }
        catch(Exception e){
            Log.d("getFileName. Caught exception:", e.getMessage());
        }

        return displayName;
    }

    public static void openFileExternally(String fileName, Context context) {
        File filesPath = new File(context.getFilesDir(), "");
        File newFile = new File(filesPath, fileName);

        Log.d("pangaea mobile, file path", newFile.getPath());
        Log.d("pangaea mobile, file exist:", String.valueOf(newFile.exists()));

        Uri contentUri = FileProvider.getUriForFile(context, "com.amtelectronics.pangaea.fileprovider", newFile);

        context.grantUriPermission(context.getPackageName(), contentUri, Intent.FLAG_GRANT_READ_URI_PERMISSION);

        Intent intent = new Intent(Intent.ACTION_VIEW);

        intent.setDataAndType(contentUri, "application/pdf");
        intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        context.startActivity(intent);
    }
}
