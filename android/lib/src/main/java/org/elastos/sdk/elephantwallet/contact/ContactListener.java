package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
abstract class ContactListener extends CrossBase {
    public class AcquireArgs extends org.elastos.sdk.elephantwallet.contact.internal.AcquireArgs {
        private AcquireArgs(int type, String pubKey, byte[] data) {
            super(type, pubKey, data);
        }
    }

    public class EventArgs extends org.elastos.sdk.elephantwallet.contact.internal.EventArgs {
        public EventArgs(int type, String humanCode, int channelType, byte[] data) {
            super(type, humanCode, channelType, data);
        }
    }

    public class StatusEvent extends EventArgs {
        public StatusEvent(int type, String humanCode, int channelType, byte[] data) {
            super(type, humanCode, channelType, data);
            status = ContactStatus.valueOf(data[0]);
        }
        @Override
        public String toString() {
            return "StatusEvent" + "[type=" + type
                    + ",humanCode=" + humanCode + ",channelType=" + channelType
                    + ",status=" + status +"]";
        }

        public ContactStatus status;
    }

    public ContactListener() {
        super(ContactListener.class.getName(), 0);
    }

    public abstract byte[] onAcquire(AcquireArgs request);
    public abstract void onEvent(EventArgs event);
    public abstract void onError(int errCode, String errStr);

    @CrossInterface
    private byte[] onAcquire(int reqType, String pubKey, byte[] data) {
        Log.i(Contact.TAG, "ContactListener.onAcquire()");

        AcquireArgs args = new AcquireArgs(reqType, pubKey, data);
        byte[] ret = onAcquire(args);

        return ret;
    }

    @CrossInterface
    private void onEvent(int eventType, String humanCode, int channelType, byte[] data) {
        Log.i(Contact.TAG, "ContactListener.onEvent()");

        EventArgs args = null;

        EventArgs.Type type = EventArgs.Type.valueOf(eventType);
        switch (type) {
            case StatusChanged:
            case FriendStatusChanged:
                args = new StatusEvent(eventType, humanCode, channelType, data);
                break;
            case ReceivedMessage:
                break;
            case SentMessage:
                break;
            case FriendRequest:
                break;
            default:
                throw new RuntimeException("Unimplemented type: " + type);
        }

        onEvent(args);
        return;
    }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
