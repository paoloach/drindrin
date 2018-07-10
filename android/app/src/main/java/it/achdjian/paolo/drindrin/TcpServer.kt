package it.achdjian.paolo.drindrin

import android.app.Service
import android.content.Intent
import android.os.IBinder
import android.util.Log

class TcpServer : Service() {
    private var player =  PlayingThread(this)
    private var tcpThread = TcpThread(player);

    override fun onBind(p0: Intent?): IBinder? {
        return null;
    }

    override fun onCreate() {
        Log.i("server", "OnCreate")
        player.start()
        tcpThread.start()
    }

    override fun onDestroy() {
        Log.i("server", "OnDestroy")
        tcpThread.join();
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {

        return START_STICKY;
    }
}
