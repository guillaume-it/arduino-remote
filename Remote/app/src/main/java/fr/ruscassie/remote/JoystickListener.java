package fr.ruscassie.remote;

import android.util.Log;

public class JoystickListener implements JoystickView.JoystickListener{
    private final static String TAG = "[JoystickListener]";
    private SerialService serialService;
    @Override
    public void onJoystickMoved(float xPercent, float yPercent, int id) {
        switch (id)
        {
            case R.id.joystickRight:
               // Log.d("Right Joystick", "X percent: " + xPercent + " Y percent: " + yPercent);
                // 0 90 180
                //-1 0 1
                //0 1 2
                if(serialService != null) {

                    Float positive = (1+(xPercent*-1) )* 125;
                //    Log.i(TAG, "Joys X percent: " + xPercent +"Servo: "+positive);
                    serialService.writeBuffer(BluetoothConstant.WHEEL,positive.intValue());
                }

                break;
            case R.id.joystickLeft:
               // Log.d("Left Joystick", "X percent: " + xPercent + " Y percent: " + yPercent);
                if(serialService != null) {

                    Float motor = yPercent  * -255;
                 //   Log.i(TAG,"Y percent: " + yPercent +"Motor: "+motor);
                    serialService.writeBuffer(BluetoothConstant.MOTOR, motor.intValue());
                }

                break;
        }
    }

    public void setSerialService(SerialService serialService) {
        this.serialService = serialService;
    }
}
