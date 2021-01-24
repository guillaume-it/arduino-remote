package fr.ruscassie.remote;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.Nullable;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 * Create notification and queue serial data while activity is not in the foreground
 * use listener chain: SerialSocket -> SerialService -> UI fragment
 */
public class SerialService extends Service implements SerialListener {

    class SerialBinder extends Binder {
        SerialService getService() { return SerialService.this; }
    }

    private final IBinder binder;
    private final Queue<String> queue;

    private BluetoothGattCallbackImpl socket;
    private boolean connected;

    /**
     * Lifecycle
     */
    public SerialService() {
        binder = new SerialBinder();
        queue = new LinkedList<>();
    }

    @Override
    public void onDestroy() {
         disconnect();
        super.onDestroy();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    /**
     * Api
     */
    public void connect(BluetoothGattCallbackImpl socket) throws IOException {
        socket.connect(this);
        this.socket = socket;
        connected = true;
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            public void run() {
                try {
                    write("{BLUETOOTH_INITIALISATION}");

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }, 3000);
    }

    public void disconnect() {
        connected = false; // ignore data,errors while disconnecting
         if(socket != null) {
            socket.disconnect();
            socket = null;
        }
    }

    public void write(String data) throws IOException {
        if(!connected)
            throw new IOException("not connected");
        byte[] bytes = new String("{"+data+"}").getBytes();
        socket.write(bytes);
    }

    /**
     * SerialListener
     */
    @Override
    public void onSerialConnect() {
        if(connected) {
            synchronized (this) {

            }
        }
    }
    @Override
    public void onSerialConnectError(Exception e) {
        if(connected) {
            synchronized (this) {

            }
        }
    }
    //text\r\nLOG: Fin trans
    @Override
    public void onSerialRead(byte[] data) {
        //if(connected) {
            synchronized (this) {
                queue.add(new String(data));
            }
        //}
    }
    @Override
    public void onSerialIoError(Exception e) {
        if(connected) {
            synchronized (this) {

            }
        }
    }

}
