package com.fmone.utils;

import android.content.Context;

import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.NetworkRequest;
import android.net.NetworkCapabilities;
import android.net.Network;

import android.util.Log;

public class NetUtils {

    public static void bindToWiFi(Context context){
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        // Network[] network = connectivityManager.getAllNetworks();
        // if(network != null && network.length >0 ){
        //      for(int i = 0 ; i < network.length ; i++){
        //           NetworkInfo networkInfo = connectivityManager.getNetworkInfo(network[i]);
        //           int networkType = networkInfo.getType();
        //           if(ConnectivityManager.TYPE_MOBILE == networkType ){
        //                connectivityManager.bindProcessToNetwork(network[i]);
        //           }
        //      }

        NetworkRequest.Builder req = new NetworkRequest.Builder();

        //req.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        req.addTransportType(NetworkCapabilities.TRANSPORT_WIFI); //TRANSPORT_ETHERNET

        connectivityManager.requestNetwork(req.build(), new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(Network network) {

                 Log.d("Network finded", network.toString());

                 connectivityManager.bindProcessToNetwork(network);
            }
        });
    }

}
