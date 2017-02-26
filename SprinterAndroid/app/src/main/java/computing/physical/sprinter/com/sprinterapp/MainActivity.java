package computing.physical.sprinter.com.sprinterapp;

import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;

import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.concurrent.ThreadFactory;



public class MainActivity extends ActionBarActivity {

    ImageButton Discnt, Abt,read;
    Button play1,play2,play,bwin,rwin;
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    volatile boolean stopWorker;
    int readBufferPosition;
    byte[] readBuffer;
    Thread workerThread,guiThread;
    InputStream mmInputStream;
    boolean isGetInput;
    long time_stgame;
    long time_engame;
    boolean isGamePlay;
    boolean isFirstTime;
    Timer timer;
    int p1score,p2score,wincount = 0;


    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Intent newint = getIntent();
        address = newint.getStringExtra(DeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device

        isGetInput = true;
        isGamePlay = false;
        //view of the ledControl
        timer = new Timer();
        setContentView(R.layout.activity_main);
        play = (Button) findViewById(R.id.play);
        play1 = (Button) findViewById(R.id.player1);
        play2 = (Button) findViewById(R.id.player2);
        bwin = (Button) findViewById(R.id.bluewin);
        rwin = (Button) findViewById(R.id.redwin);

        p1score = 0;
        p2score = 0;
        isFirstTime = true;
        //call the widgets
        //On = (ImageButton)findViewById(R.id.on);
        //Off = (ImageButton)findViewById(R.id.off);
        //Discnt = (ImageButton)findViewById(R.id.discnt);
        //Abt = (ImageButton)findViewById(R.id.abt);
        //read = (ImageButton)findViewById(R.id.read);
        beginListenForData();
        controlGUI(MainActivity.this);
        new ConnectBT().execute(); //Call the class to connect

        play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (btSocket!=null)
                {
                    try
                    {
                        btSocket.getOutputStream().write("start".toString().getBytes());
                    }
                    catch (IOException e)
                    {
                        msg("Error");
                    }
                }
            }
        });

    }

    private void Disconnect()
    {
        if (btSocket!=null) //If the btSocket is busy
        {
            try
            {
                btSocket.close(); //close connection
            }
            catch (IOException e)
            { msg("Error");}
        }
        finish(); //return to the first layout

    }
    private void doHandShake()
    {
        if(btSocket!=null)
        {
            try{
                btSocket.getOutputStream().write("ready".toString().getBytes());

            }catch (IOException e)
            {
                msg("Handshae Send Fail");
            }
        }
    }

    void mgetInputStream()
    {
        if(btSocket != null) {
            try {
                mmInputStream = btSocket.getInputStream();
                doHandShake();
                isGetInput = false;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    void controlGUI(final ActionBarActivity act)
    {
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                // Your logic here...

                // When you need to modify a UI element, do so on the UI thread.
                // 'getActivity()' is required as this is being ran from a Fragment.
                act.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(isFirstTime)
                        {
                            bwin.setVisibility(View.GONE);
                            rwin.setVisibility(View.GONE);
                            isFirstTime = false;
                        }
                        Log.i("SCORE"," : "+p1score);
                        Log.i("SCORE"," : "+p2score);
                        // This code will always run on the UI thread, therefore is safe to modify UI elements.
                        if(isGamePlay)
                        {play.setVisibility(View.GONE);}
                        else if(!isGamePlay){

                            if(p1score == 0 && p2score == 0)
                            {
                                play.setVisibility(View.VISIBLE);
                            }else if(p1score > p2score)
                            {
                                bwin.setVisibility(View.VISIBLE);
                                wincount ++;
                                if(wincount > 500)
                                {
                                    bwin.setVisibility(View.GONE);
                                    play.setVisibility(View.VISIBLE);
                                    p1score = 0;
                                    p2score = 0;
                                    wincount = 0;
                                    play1.setText("00");
                                    play2.setText("00");

                                }
                            }else if(p2score > p1score){
                                rwin.setVisibility(View.VISIBLE);
                                wincount ++;
                                if(wincount > 500)
                                {
                                    rwin.setVisibility(View.GONE);
                                    play.setVisibility(View.VISIBLE);
                                    p1score = 0;
                                    p2score = 0;
                                    wincount = 0;
                                    play1.setText("00");
                                    play2.setText("00");
                                }
                            }
                        }

                    }
                });
            }
        }, 0, 10);
    }
    void beginListenForData()
    {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[2048];
        workerThread = new Thread(new Runnable()
        {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    if (isBtConnected && btSocket != null) {
                        if(isGetInput)
                        {
                            mgetInputStream();
                        }
                        try {
                            int bytesAvailable = mmInputStream.available();
                            if (bytesAvailable > 0) {
                                byte[] packetBytes = new byte[bytesAvailable];
                                mmInputStream.read(packetBytes);
                                for (int i = 0; i < bytesAvailable; i++) {
                                    byte b = packetBytes[i];
                                    if (b == delimiter) {
                                        byte[] encodedBytes = new byte[readBufferPosition];
                                        System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                        final String data = new String(encodedBytes, "US-ASCII");
                                        readBufferPosition = 0;

                                        handler.post(new Runnable() {
                                            public void run() {
                                                Log.i("InputVal", data);

                                                if(data.contains("start"))
                                                {
                                                    Log.i("incompare","Result is equal");
                                                    //play.setVisibility(View.GONE);
                                                    //setPlayButtonVisible(false);
                                                    //play1.setVisibility(View.VISIBLE);
                                                    //play2.setVisibility(View.VISIBLE);
                                                    isGamePlay = true;
                                                    time_stgame = System.currentTimeMillis();
                                                }

                                                if(isGamePlay) {

                                                    if (data.toLowerCase().contains("p1:".toLowerCase())) {
                                                        int start = data.indexOf(":");
                                                        String suffix = data.substring(start + 1);
                                                        p1score = Integer.parseInt(suffix);
                                                        play1.setText(suffix);
                                                    }
                                                    if (data.toLowerCase().contains("p2:".toLowerCase())) {
                                                        int start = data.indexOf(":");
                                                        String suffix = data.substring(start + 1);
                                                        p2score = Integer.parseInt(suffix);
                                                        play2.setText(suffix);
                                                    }
                                                }


                                            }
                                        });
                                    } else {
                                        readBuffer[readBufferPosition++] = b;
                                    }
                                }
                            }

                            if(isGamePlay)
                            {
                                time_engame =System.currentTimeMillis();
                                long tDelta = time_engame - time_stgame;
                                double elapsed_seconds= tDelta/1000.0;
                                // Log.i("Elapsedime: ",""+elapsed_seconds);
                                if(elapsed_seconds >= 60)
                                {
                                    Log.i("Elapsedime: ","yo after ime");
                                    stopGame();
                                    isGamePlay = false;
                                }
                            }
                        } catch (IOException ex) {
                            stopWorker = true;
                        }
                    }
                }
            }
        });

        workerThread.start();
    }
    private void setPlayButtonVisible(boolean isV)
    {
        if(isV)
            play.setVisibility(View.VISIBLE);
        else
            play.setVisibility(View.INVISIBLE);

    }
    private void stopGame()
    {
        if (btSocket!=null)
        {
            try
            {
                btSocket.getOutputStream().write("stop".toString().getBytes());
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }
    }
    private void turnOnLed()
    {
        if (btSocket!=null)
        {
            try
            {
                btSocket.getOutputStream().write("1".toString().getBytes());
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }
    }

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

    public  void about(View v)
    {

    }




    private class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute()
        {
            progress = ProgressDialog.show(MainActivity.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            try
            {
                if (btSocket == null || !isBtConnected)
                {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();//start connection
                }
            }
            catch (IOException e)
            {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }
        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);

            if (!ConnectSuccess)
            {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            }
            else
            {
                msg("Connected.");
                isBtConnected = true;

            }
            progress.dismiss();
        }
    }
}
