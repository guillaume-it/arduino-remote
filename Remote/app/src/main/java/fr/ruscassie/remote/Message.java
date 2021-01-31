package fr.ruscassie.remote;

public class Message {
    private String key;
    private  Integer value;

    public Message(String key, Integer value) {
        this.key = key;
        this.value = value;
    }

    public Message() {

    }

    public Message(String key) {
        this.key = key;
    }

    public String getKey() {
        return key;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public Integer getValue() {
        return value;
    }

    public void setValue(Integer value) {
        this.value = value;
    }
}
