package it.achdjian.paolo.drindrin

import android.content.Context
import android.media.AudioManager
import android.media.MediaPlayer
import android.media.RingtoneManager
import android.net.Uri
import android.util.Log
import java.io.BufferedReader
import java.io.DataOutputStream
import java.io.InputStreamReader
import java.net.ServerSocket


/**
 * Created by Paolo Achdjian on 5/27/18.
 */
class TcpThread(val context: Context) : Thread() {
    val port = 6789;
    var stop = false;
    override fun run() {
        val alert = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_RINGTONE)
        Log.i("server", "ringtone: $alert")
        var serverSocket: ServerSocket? = null
        try {
            Log.i("server", "Start server at port $port")
            serverSocket = ServerSocket(port)
            while (!stop) {
                val connectionSocket = serverSocket.accept()
                val inFromClient = BufferedReader(InputStreamReader(connectionSocket.getInputStream()))
                val outToClient = DataOutputStream(connectionSocket.getOutputStream())
                val clientSentence = inFromClient.readLine()
                Log.i("server", "code: $clientSentence")
                if ("DrinDrin" == clientSentence) {
                    outToClient.writeBytes("Playing")
                    var mediaPlayer: MediaPlayer? = null
                    try {
                        mediaPlayer = MediaPlayer.create(context, alert)
                        //mMediaPlayer.setDataSource(context, alert);
                        mediaPlayer?.setLooping(true);
                        mediaPlayer?.start();
                    } finally {
                        mediaPlayer?.release()
                    }
                }
            }
        } finally {
            serverSocket?.close()
        }
    }
}