package fr.ruscassie.remote;

import android.util.Log;

import java.io.IOException;
import java.text.DecimalFormat;

public class JoystickListener implements JoystickView.JoystickListener{
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
                double positive = (xPercent + 1)*67+25;
                DecimalFormat df = new DecimalFormat("#");
                try {

                    serialService.write( "SERVO:"+ df.format( positive));
                } catch (IOException e) {
                  Log.e("[RIGHT]",e.getMessage());
                }

                break;
            case R.id.joystickLeft:
                Log.d("Left Joystick", "X percent: " + xPercent + " Y percent: " + yPercent);
                double motor = xPercent*140+110;

                DecimalFormat dfm= new DecimalFormat("#");
                try {

                    serialService.write( "MOTOR:"+ dfm.format( motor));
                } catch (IOException e) {
                    Log.e("[RIGHT]",e.getMessage());
                }
                break;
        }
    }

    public void setSerialService(SerialService serialService) {
        this.serialService = serialService;
    }
}
