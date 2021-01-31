package fr.ruscassie.remote;

import android.util.Log;

import java.io.IOException;
import java.text.DecimalFormat;

public class JoystickListener implements JoystickView.JoystickListener{
    private final static String TAG = "[JoystickListener]";
    private SerialService serialService;
    @Override
    public void onJoystickMoved(float xPercent, float yPercent, int id) {
        switch (id)
        {
            case R.id.joystickRight:
                Log.d("Right Joystick", "X percent: " + xPercent + " Y percent: " + yPercent);
                // 0 90 180
                //-1 0 1
                //0 1 2
                if(serialService != null) {
                    
                    Float positive = (xPercent + 1) * 67 + 25;
                    Log.i(TAG,"Servo: "+positive);
                    serialService.writeBuffer(BluetoothConstant.SERVO,positive.intValue());
                }

                break;
            case R.id.joystickLeft:
                Log.d("Left Joystick", "X percent: " + xPercent + " Y percent: " + yPercent);
                if(serialService != null) {
                    Float motor = xPercent * 140 + 110;
                    serialService.writeBuffer(BluetoothConstant.MOTOR, motor.intValue());
                }

                break;
        }
    }

    public void setSerialService(SerialService serialService) {
        this.serialService = serialService;
    }
}
