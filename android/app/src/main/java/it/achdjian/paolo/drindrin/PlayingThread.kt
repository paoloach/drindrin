package it.achdjian.paolo.drindrin

import android.content.Context
import android.media.AudioManager
import android.media.MediaPlayer
import android.media.RingtoneManager
import android.net.Uri
import android.os.Handler
import android.os.Looper
import android.preference.PreferenceManager
import android.util.Log


/**
 * Created by Paolo Achdjian on 7/6/18.
 */
class PlayingThread(val context: Context) :Thread() {
    private lateinit var handler: Handler

    override fun run() {
        Looper.prepare()
        handler = Handler()
        Looper.loop()
    }

    fun startPlay() {
        val preferenceManager =  PreferenceManager.getDefaultSharedPreferences(context)
        val duration = preferenceManager.getLong(Constants.SHARED_PREFERENCE_TONE_DURATION, 10000)
        val toneId = preferenceManager.getString(Constants.SHARED_PREFERENCE_TONE_ID, "")
        Log.i("server", "toneId: $toneId")
        val alert =Uri.parse( toneId)// RingtoneManager(context).getRingtoneUri(toneId)
        Log.i("server", "Playing $alert")

        val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        val volume = audioManager.getStreamVolume(AudioManager.STREAM_MUSIC)
        val maxVolume = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVolume, 0)
        val player = MediaPlayer.create(context, alert)
        player.isLooping = true
        player.start()
        handler.postDelayed({
            Log.i("server", "Stopping player")
            player.stop()
            audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, volume, 0)
        }, duration)
    }
}