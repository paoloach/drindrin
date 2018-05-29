package it.achdjian.paolo.drindrin

import android.app.Service
import android.content.Intent
import android.os.IBinder

class TcpServer : Service() {
    private var tcpThread = TcpThread(this);

    override fun onBind(p0: Intent?): IBinder? {
        return null;
    }

    override fun onCreate() {
        tcpThread.start()
    }

    override fun onDestroy() {
        tcpThread.join();
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {

        return START_STICKY;
    }
}
