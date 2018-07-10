package it.achdjian.paolo.drindrin

import android.content.Context
import android.media.AudioManager
import android.media.MediaPlayer
import android.media.Ringtone
import android.media.RingtoneManager
import android.net.Uri
import android.os.CountDownTimer
import android.os.Handler
import android.os.Looper
import android.util.Log
import java.io.BufferedReader
import java.io.DataOutputStream
import java.io.InputStreamReader
import java.net.ServerSocket


/**
 * Created by Paolo Achdjian on 5/27/18.
 */
class TcpThread(val player: PlayingThread) : Thread() {
    val port = 6789;
    var stop = false;
    override fun run() {
        Looper.prepare()

        var serverSocket: ServerSocket? = null
        try {
            Log.i("server", "Start server at port $port")
            serverSocket = ServerSocket(port)
            while (!stop) {
                val connectionSocket = serverSocket.accept()
                val inFromClient = BufferedReader(InputStreamReader(connectionSocket.getInputStream()))
                val clientSentence = inFromClient.readLine()
                Log.i("server", "code: $clientSentence")
                if ("DrinDrin" == clientSentence) {
                    player.startPlay()
                }
            }
        } finally {
            Log.i("server", "Closing")
            serverSocket?.close()
        }
    }
}
