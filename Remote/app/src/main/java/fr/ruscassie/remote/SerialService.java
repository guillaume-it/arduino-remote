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

import static android.webkit.ConsoleMessage.MessageLevel.LOG;

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
                    write("{INIT}");

                } catch (IOException e) {
                    Log.e(TAG,e.getMessage());
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

    public void write(String data) throws IOException {
        if (!connected)
            throw new IOException("not connected");
        byte[] bytes = new String("{" + data + "}").getBytes();
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

        int start = dataRead.indexOf("{")+1;
        int end = dataRead.indexOf("}");

        if (start > 0 && end > 0) {
           String message = dataRead.substring(start, end);
           String [] split = message.split(":");

            stringBuffer.delete(start, end);
            
            Log.i(TAG,message);
            switch (split[0]){
               case "INIT":
                   value = split[1];
                break;
                case "START":
                    connected = true;
                    value = split[1];
                    break;
                case "LOG":
                    value = split[1];
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + split[0]);
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
