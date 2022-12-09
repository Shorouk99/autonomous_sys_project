#include <WiFi.h>
#include <ros.h>
#include <std_msgs/Int16.h>

/****************************************************************************************
************************************** Definitions **************************************
****************************************************************************************/
const char* ssid     = "WE_A6BCA2";
const char* password = "25104945288";
// Set the rosserial socket server IP address
IPAddress server(192,168,8,133);
// Set the rosserial socket server port
const uint16_t serverPort = 11411;

// ROS Node and Topics
ros::NodeHandle nh;
std_msgs::Int16 encoder_msg;
ros::Publisher pub_enc_ticks("/wheel", &encoder_msg); //Topic name /wheel

// Encoder definitions
int encoder_ch_a = 2;
int encoder_ch_b = 15;
volatile long encoder_count = 0;


/*************************************************************************************
************************************** Handlers **************************************
 ************************************************************************************/
//Encoder ISR
void IRAM_ATTR encoder_isr(){
  if(digitalRead(encoder_ch_a) == HIGH){
    if(digitalRead(encoder_ch_b) == LOW){
      encoder_count--; 
    }
    else{
      encoder_count++;
    }
  }
  else{
    if(digitalRead(encoder_ch_b) == LOW){
      encoder_count++; 
    }
    else{
      encoder_count--;
    }
  }
}


/*******************************************************************************************
************************************** Initialization **************************************
*******************************************************************************************/
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect the ESP32 the the wifi AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();

  // Another way to get IP
  Serial.print("IP = ");
  Serial.println(nh.getHardware()->getLocalIP());

  // Initialize encoder data publisher
  nh.advertise(pub_enc_ticks);

  // Setup encoder pins
  pinMode(encoder_ch_a, INPUT);
  pinMode(encoder_ch_b, INPUT);

  attachInterrupt(encoder_ch_a, encoder_isr, RISING);
}


/*****************************************************************************************
************************************** Infinite Loop **************************************
 *****************************************************************************************/
void loop()
{

  if (nh.connected()) {
    Serial.println("Connected");
    encoder_msg.data = encoder_count;
    pub_enc_ticks.publish(&encoder_msg);
  } else {
    Serial.println("Not Connected");
  }
  nh.spinOnce();
  delay(3);
}
