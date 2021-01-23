package fr.ruscassie.remote;


import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.util.Log;

public class ConnectThread extends Thread {
    public static final String INITIALISATION_BLUETOOTH = "Initialisation Bluetooth";
    public static String TAG = "[CONNECT THREAD]";
    private Context context;
    private String deviceAddress;

    public ConnectThread(Context context) {
        this.context = context;
    }

    public void run() {

    }

}
