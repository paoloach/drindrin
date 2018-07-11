package it.achdjian.paolo.drindrin

import android.app.AlarmManager
import android.app.PendingIntent
import android.app.Service
import android.content.Intent
import android.os.IBinder
import android.util.Log


class TcpServer : Service() {
    private lateinit var player: PlayingThread
    private lateinit var tcpThread: TcpThread

    override fun onBind(p0: Intent?): IBinder? {
        return null
    }

    override fun onCreate() {
        player = PlayingThread(applicationContext)
        tcpThread = TcpThread(player)
        Log.i("server", "OnCreate")
        player.start()
        tcpThread.start()
    }


    override fun onDestroy() {
        Log.i("server", "OnDestroy")
        tcpThread.join()
        val alarmMgr = getSystemService(ALARM_SERVICE) as AlarmManager
        val i = Intent(this, TcpServer::class.java)
        val pendingIntent = PendingIntent.getService(this, 0, i, 0)
        alarmMgr.set(AlarmManager.RTC_WAKEUP, System.currentTimeMillis() + 1000, pendingIntent)
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        return START_STICKY
    }
}
