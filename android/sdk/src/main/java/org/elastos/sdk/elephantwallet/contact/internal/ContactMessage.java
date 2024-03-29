package org.elastos.sdk.elephantwallet.contact.internal;

import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.annotations.SerializedName;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

import java.io.File;
import java.io.StringReader;

@CrossClass
public class ContactMessage extends CrossBase {
    public enum Type {
        Empty(0x00000000),
        MsgText(0x00000001),
        MsgAudio(0x00000002),
        MsgTransfer(0x00000004),
        MsgImage(0x00000008),
        MsgFile(0x00000010),
        MsgBinary(0x00000020);

        public static Type valueOf(int id) {
            Type[] values = Type.values();
            for(int idx = 0; idx < values.length; idx++) {
                if(values[idx].id == id) {
                    return values[idx];
                }
            }
            return null;
        }

        private Type(int id){
            this.id = id;
        }
        private int id;
    }

    public static abstract class MsgData {
        public String toString() {
            String str = new Gson().toJson(this);
            return str;
        }

        public abstract byte[] toData();
        public abstract void fromData(byte[] data);
    }

    public static class TextData extends MsgData {
        public TextData() {
        }
        public TextData(String text) {
            this.text = text;
        }

        @Override
        public String toString() {
            return text;
        }
        @Override
        public byte[] toData() {
            return text.getBytes();
        }
        @Override
        public void fromData(byte[] data) {
            text = new String(data);
        }

        public String text;
    }

    public static class BinaryData extends MsgData {
        public BinaryData() {
        }
        public BinaryData(byte[] binary) {
            this.binary = binary;
        }

        @Override
        public String toString() {
            String str = new Gson().toJson(binary);
            return str;
        }
        @Override
        public byte[] toData() {
            return binary;
        }
        @Override
        public void fromData(byte[] data) {
            binary = data;
        }

        public byte[] binary;
    }

    public static class FileData extends MsgData {
        public FileData() {
        }
        public FileData(File file) {
            devId = UserInfo.GetCurrDevId();
            name = file.getName();
            size = file.length();
            md5 = Utils.getMD5Sum(file);
        }

        @Override
        public String toString() {
            byte[] data = toData();
            return new String(data);
        }
        @Override
        public byte[] toData() {
            JsonObject json = new JsonObject();
            json.addProperty(JsonKey.DeviceId, this.devId);
            json.addProperty(JsonKey.Name, this.name);
            json.addProperty(JsonKey.Size, this.size);
            json.addProperty(JsonKey.Md5, this.md5);

            return json.toString().getBytes();
        }
        @Override
        public void fromData(byte[] data) {
            JsonElement element = new JsonParser().parse(new String(data));
            JsonObject json = element.getAsJsonObject();
            this.devId = json.get(JsonKey.DeviceId).getAsString();
            this.name = json.get(JsonKey.Name).getAsString();
            this.size = json.get(JsonKey.Size).getAsLong();
            this.md5 = json.get(JsonKey.Md5).getAsString();
        }

        // fix json decode and encode different issue
        public static String ConvertId(String id) {
            FileData fileData = new Gson().fromJson(id, FileData.class);
            if (fileData == null) {
                Log.w(Contact.TAG, "FileData.ConvertId() 0 Failed to convert " + id);
            }

            return fileData.toString();
        }

        public String devId;
        public String name;
        public long size;
        public String md5;
    }

    public final Type type;
    public MsgData data;
    public final String cryptoAlgorithm;
    public long timestamp;

    public int syncMessageToNative() {
        byte[] msgData = data.toData();
        int ret = syncMessageToNative(type.id, msgData, cryptoAlgorithm, timestamp);
        return ret;
    }

    public ContactMessage(String text, String cryptoAlgorithm) {
        this(Type.MsgText, new TextData(text), cryptoAlgorithm);
    }

    public ContactMessage(byte[] binary, String cryptoAlgorithm) {
        this(Type.MsgBinary, new BinaryData(binary), cryptoAlgorithm);
    }


    public ContactMessage(File file, String cryptoAlgorithm) {
        this(Type.MsgFile, new FileData(file), cryptoAlgorithm);
    }

    public ContactMessage(Type type, byte[] data, String cryptoAlgorithm) {
        this(type, (MsgData) null, cryptoAlgorithm);
        if(data != null) {
            switch (type) {
                case MsgText:
                    this.data = new TextData();
                    break;
                case MsgBinary:
                    this.data = new BinaryData();
                    break;
                case MsgFile:
                    this.data = new FileData();
                    break;
                default:
                    Log.w(Contact.TAG, "Unknown Message Type: " + type);
                    break;
            }
        }
        if(data != null) {
            this.data.fromData(data);
        }
    }

    private ContactMessage(Type type, MsgData data, String cryptoAlgorithm) {
        super(ContactMessage.class.getName(), 0);

        this.type = type;
        this.data = data;
        this.cryptoAlgorithm = cryptoAlgorithm;
        this.timestamp = System.currentTimeMillis();
    }

    @CrossInterface
    private native int syncMessageToNative(int type, byte[] data, String cryptoAlgorithm, long timestamp);
} // class Factory
