package rug.ucProject;

import android.app.Activity;
import android.os.Bundle;


import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import rug.ucProject.mobileProtocol.ucMsg;

import com.google.protobuf.InvalidProtocolBufferException;

import android.content.Context;
import android.os.Vibrator;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class UbiquitousComputing extends Activity {
    /** Called when the activity is first created. */
	public comServer comThread = null;
	public String phoneIP;
	DatagramSocket socket = null;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        final TextView txtInfo = (TextView)findViewById(R.id.TextViewIPInfo);
        txtInfo.setText("This application can make the phone vibration according to the instruction from PC.\n\n"
        		        +"Please make sure turn on the Wifi and startup application in PC.\n\n"
        				+"This application is created by Zhe Sun and Divya. It follows GNU General Public License.");
        
        final Button buttonExit = (Button) findViewById(R.id.ButtonExit);
        buttonExit.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
            	socket.close();
            	UbiquitousComputing.this.finish();
            }
        });

        
        phoneIP = getWifiIP();

        /* Retrieve the ServerName */
        InetAddress serverAddr = null;
		try {
			serverAddr = InetAddress.getByName(phoneIP);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        /* Create new UDP-Socket */
	    final int SERVERPORT = 9876;
        
	    
		try {
			socket = new DatagramSocket(SERVERPORT, serverAddr);
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        comThread = new comServer(socket);
        comThread.start();
    }
    
	public class comServer extends Thread implements Runnable {
		
 		public DatagramSocket sock; // 'Within' the emulator!
  	    public comServer(DatagramSocket s) {
	    	sock = s;
		}    

		@Override
	    public void run() {
            /* By magic we know, how much data will be waiting for us */
            byte[] buf = new byte[1000];
            /* Prepare a UDP-Packet that can
             * contain the data we want to receive */
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            Vibrator v = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
            
            for (;;){
        	    /* Receive the UDP-Packet */
            	try {
					sock.receive(packet);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
                //if (!threadEndFlag)
                {
        			byte[] bufProto = packet.getData();
        			int len = packet.getLength();
        			byte[] bufTemp = new byte[len];
        			for (int i = 0; i < len; i++) {
        				bufTemp[i] = bufProto[i];
        			}
        			        			
        			ucMsg msg = null;
					try {
						msg = ucMsg.parseFrom(bufTemp);
					} catch (InvalidProtocolBufferException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
        			        			
        			if (msg != null && msg.getType() == mobileProtocol.CmdType.VIBRATION){
        				int num = msg.getVPatternCount();
        				long[] vPattern = new long[num];
						for (int i=0; i<num; i++){
							long pattern = msg.getVPattern(i); 
							vPattern[i] = pattern;
						}
    			
				        v.vibrate(vPattern, -1);
        			} 
            	}
            }    
		}
  }

  	public String getWifiIP(){
		WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		int ipAddress = wifiInfo.getIpAddress();
		return String.format("%d.%d.%d.%d",
				(ipAddress & 0xff),
				(ipAddress >> 8 & 0xff),
				(ipAddress >> 16 & 0xff),
				(ipAddress >> 24 & 0xff));
	}
}


