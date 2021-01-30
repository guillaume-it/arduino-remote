package fr.ruscassie.remote;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;


/**
 * Create notification and queue serial data while activity is not in the foreground
 * use listener chain: SerialSocket -> SerialService -> UI fragment
 */
public class SerialService extends Service implements SerialListener {

    private String TAG = "[SerialService]";

    class SerialBinder extends Binder {
        SerialService getService() {
            return SerialService.this;
        }
    }

    private final IBinder binder = new SerialBinder();
    private final Queue<String> queue = new LinkedList<>();
    private final StringBuffer stringBuffer = new StringBuffer();
    private BluetoothGattCallbackImpl socket;
    private boolean connected;
    private String value;

    public StringBuffer getStringBuffer() {
        return stringBuffer;
    }

    public String getValue() {
        return value;
    }

    /**
     * Lifecycle
     */
    public SerialService() {

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
                    write(BluetoothConstant.INITIALISATION, "");

                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }, 3000);
    }

    public void disconnect() {
        connected = false; // ignore data,errors while disconnecting
        if (socket != null) {
            socket.disconnect();
            socket = null;
        }
    }

    public void write(String key, String value) throws IOException {
        if (!connected)
            throw new IOException("not connected");
        byte[] bytes = new String("{" + key + ":" + value + "}").getBytes();
        socket.write(bytes);
    }

    /**
     * SerialListener
     */
    @Override
    public void onSerialConnect() {
        if (connected) {
            synchronized (this) {

            }
        }
    }

    @Override
    public void onSerialConnectError(Exception e) {
        if (connected) {
            synchronized (this) {

            }
        }
    }

    //text\r\nLOG: Fin trans
    @Override
    public void onSerialRead(byte[] data) {
        //if(connected) {
        synchronized (this) {
            final String dataRead = new String(data);
            stringBuffer.append(dataRead);
            manageMessage(stringBuffer);


        }
        //}
    }

    private void manageMessage(StringBuffer stringBuffer) {
        //message.split(":");
        String dataRead = stringBuffer.toString();

        int start = dataRead.indexOf("{") + 1;
        int end = dataRead.indexOf("}");
        if (end < start) {
            end = dataRead.indexOf("}", start);
        }
        if (start >= 0 && end >= 0 && start < end) {
            String message = dataRead.substring(start, end);
            String[] split = message.split(":");

            // stringBuffer.delete(start, end);
            stringBuffer.delete(0, stringBuffer.length());
            Log.i(TAG, message);
            try {
                String state = split[0];
                switch (state) {
                    case "I":
                        value = split[1];
                        break;
                    case "C":
                        connected = true;
                        if (split.length > 1) {
                            value = split[1];
                        }
                        break;
                    case "L":
                        value = split[1];
                        break;
                    default:
                        Log.i(TAG, "message: " + message + " value: " + value);
                }
            }catch (IllegalArgumentException e){
                Log.e(TAG,e.getMessage());
            }
        }
    }

    @Override
    public void onSerialIoError(Exception e) {
        if (connected) {
            synchronized (this) {

            }
        }
    }

}
