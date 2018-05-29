package it.achdjian.paolo.drindrin

import android.content.Intent
import android.media.RingtoneManager
import android.net.Uri
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)

        val service = Intent(this, TcpServer::class.java);

        val ringtoneManager = RingtoneManager(this);
        val cursor = ringtoneManager.cursor
        val allarms = ArrayList<Uri>()
        while( !cursor.isAfterLast){
            cursor.moveToNext()
            Log.i("ringtone","Title: ${cursor.getString(1)}")
            val ringtone =ringtoneManager.getRingtoneUri(cursor.position)
            if (ringtone != null)
                allarms.add(ringtone)
        }
        cursor.close()

        allarms.forEach { Log.i("ringtone", it.toString())  }
        startService(service)

    }

}
