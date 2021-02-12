package fr.ruscassie.remote;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Timer;
import java.util.TimerTask;


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
    LocalDateTime now;
    private LinkedList<Message> messageLinkedList = new LinkedList<>();

    public StringBuffer getStringBuffer() {
        return stringBuffer;
    }

    public String getValue() {
        return value;
    }

    public LinkedList<Message> getMessageLinkedList() {
        return messageLinkedList;
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
        // connected = true;
        Handler handler = new Handler();
        SerialService serialService = this;
        handler.postDelayed(new Runnable() {
            public void run() {
                try {
                    write("{I:}");
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
//        if (!connected)
//            throw new IOException("not connected");
        byte[] bytes = data.getBytes();
        socket.write(bytes);
    }

    public void writeBuffer(String key, Integer value) {
        messageLinkedList.add(new Message(key, value));
        send();
    }
    public void writeBuffer(String key) {
        messageLinkedList.add(new Message(key));
        send();
    }
    public void send() {
        if(now == null) {
            now = LocalDateTime.now();
        }else{
            if(LocalDateTime.now().isAfter(now.plusNanos(50000000))) {
                now = LocalDateTime.now();

                List<Message> list = new ArrayList();
                while (!messageLinkedList.isEmpty()) {
                    list.add(messageLinkedList.removeFirst());
                }
                Message messageW = new Message("W", 0);
                Message messageM = new Message("M", 0);
                int idM = 0, idW = 0;
                for (Message message1 : list) {
                    switch (message1.getKey()) {
                        case "M":
                            messageM.setValue(messageM.getValue() + message1.getValue());
                            idM++;
                            break;
                        case "W":
                            messageW.setValue(messageW.getValue() + message1.getValue());
                            idW++;
                            break;
                    }
                }
                if (messageM.getValue() > 0) {
                    messageM.setValue(messageM.getValue() / idM);
                }
                if (messageW.getValue() > 0) {
                    messageW.setValue(messageW.getValue() / idW);
                }
                try {
                    Log.i(TAG,"{" + messageW.getKey() + ":" + messageW.getValue() + "}{" + messageM.getKey() + ":" + messageM.getValue() + "}");
                    write("{" + messageW.getKey() + ":" + messageW.getValue() + "}");
                            write("{" + messageM.getKey() + ":" + messageM.getValue() + "}");
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
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
            } catch (IllegalArgumentException e) {
                Log.e(TAG, e.getMessage());
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
