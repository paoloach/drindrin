package it.achdjian.paolo.drindrin

import android.content.Intent
import android.media.RingtoneManager
import android.os.Bundle
import android.preference.PreferenceManager
import android.support.v7.app.AppCompatActivity
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.widget.RadioButton
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity(), TextWatcher {
    override fun afterTextChanged(editable: Editable) {
        val edit =PreferenceManager.getDefaultSharedPreferences(this).edit()
        try {
            val newDuration = editable.toString().toLong()*1000
            edit.putLong(Constants.SHARED_PREFERENCE_TONE_DURATION, newDuration)
            edit.apply()
        } catch (e:Exception){
            Log.i("setup","Unable to save $editable as duration")
        }
    }

    override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
    }

    override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        duration.addTextChangedListener(this)

        val service = Intent(this, TcpServer::class.java)

        val preference =PreferenceManager.getDefaultSharedPreferences(this)
        val preferedTone = preference.getString(Constants.SHARED_PREFERENCE_TONE_ID, "")
        val preferedDuration = preference.getLong(Constants.SHARED_PREFERENCE_TONE_DURATION, 10000)
        duration.setText((preferedDuration/1000).toString())
        val ringtoneManager = RingtoneManager(this)
        val cursor = ringtoneManager.cursor
        while (!cursor.isAfterLast) {
            val radioButton = RadioButton(this)
            radioButton.text = cursor.getString(RingtoneManager.TITLE_COLUMN_INDEX)
            val id =cursor.getInt(RingtoneManager.ID_COLUMN_INDEX)
            radioButton.tag = cursor.getString(RingtoneManager.URI_COLUMN_INDEX)+ "/$id"
            radioButton.setOnClickListener {
                val edit = preference.edit()
                Log.i("setup", "set toneId: ${it.tag}")
                edit.putString(Constants.SHARED_PREFERENCE_TONE_ID, it.tag as String)
                edit.apply()
            }
            radiouGroup.addView(radioButton)

            cursor.moveToNext()
        }
        val prefered = radiouGroup.findViewWithTag<RadioButton>(preferedTone);
        prefered?.isChecked=true
        startService(service)

    }

}
