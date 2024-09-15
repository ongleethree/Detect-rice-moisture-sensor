#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Ticker.h>
#include <WiFi.h>
#include <PubSubClient.h>
int sensorPin =33;  //ต้องแก้เลขpinในArduinoเป็นpinEsp32kit
int sensorValue = 0;
const int red=2;
const int green=4;
const int yellow=5;
int BUZZER_PIN=16;
Ticker read_sensor;
Ticker read_tone;
int start_tone=0;  // if start tone =1 then drive Buzzer
int Tone_seq=0;
char msg[100];
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(OLED_RESET);
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//...........Wifi setup................//
const char* myssid = "your ssid";
const char* mypassword = "your password ssid";
//.............Mqtt......................//
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "client id";
const char* mqtt_username = "client token";
const char* mqtt_password = "client secret";

// ...anoucement the mqtt function...//
WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
  Serial.begin(115200);
  //.....connect wifi.....//
  Serial.print("Connecting to "); 
  Serial.println(myssid);
  WiFi.begin(myssid, mypassword);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);//ให้ทำการสร้างพอร์ต mqtt
//.....connect wifi.....//



  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize I2C addr 0x3c
  display.clearDisplay(); // clears the screen and buffer
  //if (!display.begin(0x78, OLED_RESET)) {
  //  Serial.println(F("SSD1306 allocation failed"));
  //  for (;;); // Don't proceed, loop forever
  //}
  display.clearDisplay(); // clears the screen and buffer
  display.drawPixel(128, 64, WHITE);
  display.setTextSize(1.2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  // Display an image
  display.display();
  read_sensor.attach(0.5,read_sensor_soil);
  read_tone.attach(1,buzzer_ring_tone2); 
}

void Public_sensor()
{
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  String data = "{\"data\": {\"read_sensor_soil\":" + String(sensorValue)+"}}";
  Serial.println(data);
  data.toCharArray(msg, (data.length() + 1));
  client.publish("@shadow/data/update", msg);
  delay(5000);

}

void loop()
{
 Public_sensor();
}

void read_sensor_soil()
{
  sensorValue = analogRead(sensorPin);
  Serial.print("sensor = " );
  Serial.println(sensorValue);
  display.clearDisplay(); // clears the screen and buffer
  display.setCursor(10,3);
  display.print("sensorValue=");
  display.println(sensorValue,0);
  display.display();

  if(sensorValue<=1400)
  {
    //display.clearDisplay();
    //display.setCursor(20,20);
    Serial.println("rice wet");
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(yellow,LOW);
    //buzzer_ring_tone();

      start_tone=1;
  }
  else if(sensorValue>=1800&&sensorValue<=2000)
  {
    //display.clearDisplay();
    //display.setCursor(20,20);
    Serial.println("dry rice");
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(yellow,LOW);
    start_tone=0;
  }
  
  else
  {
    //display.clearDisplay();
    //display.setCursor(20,20);
    Serial.println("Low_wet rice");
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);
    digitalWrite(yellow,HIGH);
    start_tone=0;
  }
}
void buzzer_ring_tone()
{
  if(start_tone==1)
  {
    tone(BUZZER_PIN,440); //Buzzer will work.when water Too much than we Define
    delay(1000);
    tone(BUZZER_PIN,494);
    delay(1000);
    tone(BUZZER_PIN,523);
    delay(1000);
    tone(BUZZER_PIN,587);
    delay(1000);
    tone(BUZZER_PIN,659);
    delay(1000);
    tone(BUZZER_PIN,698);
    delay(1000);
    tone(BUZZER_PIN,784);
    delay(1000);
    noTone(BUZZER_PIN);
  }
}

void buzzer_ring_tone2()
{
  if(start_tone==1)
  {
    
    switch(Tone_seq)
    {
      case 0:
        tone(BUZZER_PIN,440); //Buzzer will work.when water Too much than we Define
        break;
      case 1:
        tone(BUZZER_PIN,494);
        break;
      case 2:
        tone(BUZZER_PIN,523);
        break;
      case 3:
        tone(BUZZER_PIN,587);
        break;
      case 4:
        tone(BUZZER_PIN,659);
        break;
      case 5:
        tone(BUZZER_PIN,698);
        break;
      case 6:
        tone(BUZZER_PIN,784);
        break;
      case 7:
        noTone(BUZZER_PIN);
        break;
      default:
       Tone_seq=0;
    }
    Tone_seq++;
  }
  else
  {
    Tone_seq=0;
    tone(BUZZER_PIN,78);
    noTone(BUZZER_PIN);
  }
}
void reconnect() 
{
  while (!client.connected()) //! mean not
  {
    Serial.print("Sensor MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username,mqtt_password)) 
    {
      Serial.println("connected");
      client.subscribe("@msg/OUT1");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
