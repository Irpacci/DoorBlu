package com.example.doorblu2;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseSettings;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.altbeacon.beacon.Beacon;
import org.altbeacon.beacon.BeaconParser;
import org.altbeacon.beacon.BeaconTransmitter;

import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    private Button MainButton;
    private Button ChangeButton;
    private TextView CurStatus;
    private EditText EditUUID;
    private EditText EditMajor;
    private EditText EditMinor;

    private String CurUUID = "2f234454-cf6d-4a0f-adf2-f4911ba9ffa6";
    private String CurMajor= "1";
    private String CurMinor = "2";
    private static final String TAG = "Something";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        MainButton = findViewById(R.id.main_button);
        CurStatus = findViewById(R.id.CurStatus);
        ChangeButton = findViewById(R.id.change_button);
        EditUUID = findViewById(R.id.InputUUID);
        EditMajor = findViewById(R.id.InputMajor);
        EditMinor = findViewById(R.id.InputMinor);

        MainButton.setOnClickListener(new View.OnClickListener() {


            @RequiresApi(api = Build.VERSION_CODES.S)
            @Override
            public void onClick(View view) {
                Log.d(TAG, "MAIN PRESSED");


                int permissionsCode = 42;
                String[] permissions = {android.Manifest.permission.BLUETOOTH_ADVERTISE};

                ActivityCompat.requestPermissions(MainActivity.this, permissions, permissionsCode);


                EditUUID.setText(CurUUID);
                EditMajor.setText(CurMajor);
                EditMinor.setText(CurMinor);

                Beacon beacon = new Beacon.Builder()
                        .setId1(CurUUID.toString())
                        .setId2(CurMajor.toString())
                        .setId3(CurMinor.toString())
                        .setManufacturer(0x0118) // Radius Networks.  Change this for other beacon layouts
                        .setTxPower(-59)
                        .setDataFields(Arrays.asList(new Long[] {0l}))
                        .build();

                BeaconParser beaconParser = new BeaconParser()
                        .setBeaconLayout("m:2-3=beac,i:4-19,i:20-21,i:22-23,p:24-24,d:25-25");
                BeaconTransmitter beaconTransmitter = new BeaconTransmitter(getApplicationContext(), beaconParser);
                beaconTransmitter.startAdvertising(beacon);

                CurStatus.setText("Активен");
                CurStatus.setTextColor(Color.GREEN);
            }
        });

        ChangeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "CHANGE PRESSED");

                if(EditUUID.getText().toString().length() == 36)
                    CurUUID = EditUUID.getText().toString();
                if(EditMajor.getText().toString().length() > 0)
                    CurMajor = EditMajor.getText().toString();
                if(EditMinor.getText().toString().length() > 0)
                    CurMinor = EditMinor.getText().toString();

                Log.d(TAG, CurUUID);
                Log.d(TAG, CurMajor);
                Log.d(TAG, CurMinor);
            }
        });
    }
}