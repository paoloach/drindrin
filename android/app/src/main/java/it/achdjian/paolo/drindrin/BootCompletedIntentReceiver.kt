package it.achdjian.paolo.drindrin

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

/**
 * Created by Paolo Achdjian on 7/10/18.
 */
class BootCompletedIntentReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent?) {
        intent?.let {
            if (Intent.ACTION_BOOT_COMPLETED == intent.action) {
                val pushIntent = Intent(context, TcpServer::class.java)
                context.startService(pushIntent)
            }
        }
    }
}