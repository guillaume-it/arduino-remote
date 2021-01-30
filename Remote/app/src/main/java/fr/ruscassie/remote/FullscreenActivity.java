package fr.ruscassie.remote;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.IOException;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class FullscreenActivity extends AppCompatActivity  {
    public static final String INITIALISATION_BLUETOOTH = "Initialisation Bluetooth";
    private BluetoothAdapter bluetoothAdapter;
    private final String TAG = "[GRU]";
    private TextView textView;
    private SerialService serialService = null;
    private JoystickListener joystickListenerRight = new JoystickListener();
    private JoystickListener joystickListenerLeft = new JoystickListener();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "on Create");
        setContentView(R.layout.activity_fullscreen);
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothDevice.ACTION_UUID);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(receiver, filter);
        textView = findViewById(R.id.information);

        JoystickView joystickViewRight = findViewById(R.id.joystickRight);
        joystickViewRight.setJoystickCallback(joystickListenerRight);

        JoystickView joystickViewLeft = findViewById(R.id.joystickLeft);
        joystickViewLeft.setJoystickCallback(joystickListenerLeft);

    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
    }

    public void onClickButtonBluetootConnect(View view) {
        checkBluetoothPermission();
        Log.i(TAG, "test");

        if (!bluetoothAdapter.isDiscovering()) {
            bluetoothAdapter.startDiscovery();
            textView.setText("Start discovery");
        } else {
            bluetoothAdapter.cancelDiscovery();
            textView.setText("Cancel discovery");
        }
    }

    public void onClickTest(View view) {

    }

    // Create a BroadcastReceiver for ACTION_FOUND.
    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Discovery has found a device. Get the BluetoothDevice
                // object and its info from the Intent.

                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
                Log.i(TAG, deviceName + " " + deviceHardwareAddress);
                if (deviceName != null && deviceName.equals("ELEGOO BT16")) {
                    textView.setText("ELEGOO detected");
                    connect(deviceHardwareAddress);
                }
            }
        }
    };

    /*
     * Serial + UI
     */
    private void connect(String deviceAddress) {
        try {
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            BluetoothDevice device = bluetoothAdapter.getRemoteDevice(deviceAddress);

            BluetoothGattCallbackImpl socket = new BluetoothGattCallbackImpl(this.getApplicationContext(), device);
            serialService = new SerialService();
            textView.setText("Test socket");
            
            serialService.connect(socket);
            joystickListenerLeft.setSerialService(serialService);
            joystickListenerRight.setSerialService(serialService);

        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void checkBluetoothPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            if (ContextCompat.checkSelfPermission(this.getApplicationContext(),
                    Manifest.permission.ACCESS_BACKGROUND_LOCATION)
                    != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(
                        FullscreenActivity.this,
                        new String[]{Manifest.permission.ACCESS_BACKGROUND_LOCATION},
                        0);
            }
        }
    }
}