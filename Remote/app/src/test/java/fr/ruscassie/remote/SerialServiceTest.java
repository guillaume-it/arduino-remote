package fr.ruscassie.remote;

import org.junit.Test;

import static org.junit.Assert.*;

public class SerialServiceTest {

    @Test
    public void testOnRead() {
        SerialService serialService = new SerialService();
        serialService.onSerialRead("{START:TEST}".getBytes());
        assertEquals("TEST",serialService.getValue());
    }
}
