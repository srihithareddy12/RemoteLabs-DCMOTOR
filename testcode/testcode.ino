#include "Variable.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ThingSpeak.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <time.h>
#include <HTTPClient.h>
WebServer server(80);
WiFiClient client;
PubSubClient mqttClient(client);

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime( & timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time( & now);
  return now;
}

void createCI (sensor_data argdata)
{
  HTTPClient http;
  if (millis() - prev_millis >= INTERVAL) {
    epochTime = getTime();
    String data;
    String server = "https://" + String() + CSE_IP + ":" + String() + CSE_PORT + String() + OM2M_MN;

    int httpcode ;
    httpcode = http.begin(server + String() + OM2M_AE + "/" + OM2M_DATA_CONT + "/");
    Serial.print("HTTPcode = ");
    Serial.println(httpcode);

    http.addHeader("X-M2M-Origin", OM2M_ORGIN);
    http.addHeader("Content-Type", "application/json;ty=4");
    http.addHeader("Content-Length", "100");

    data = "[" + String(epochTime) + ", " + String(argdata.v1) + ", "
           + String(argdata.r1) +   + "]";
    String req_data = String() + "{\"m2m:cin\": {"

                      +
                      "\"con\": \"" + data + "\","

                      +
                      "\"lbl\": \"" + "V1.0.0" + "\","

                      //+ "\"rn\": \"" + "cin_"+String(i++) + "\","

                      +
                      "\"cnf\": \"text\""

                      +
                      "}}";
    int code = http.POST(req_data);
    http.end();
    Serial.print("HTTP return code = ");
    Serial.println(code);
    prev_millis = millis();
  }
}

void updateOm2m(int val[]) {
  sensor_data argdata;
  argdata.v1 = val[0];
  argdata.r1 = val[1];
  createCI(argdata);

}


void mqttConnect()
{
  while ( !mqttClient.connected() )
  {
    // Connect to the MQTT broker.
    if ( mqttClient.connect(clientID, mqttUserName, mqttPass) ) {
      Serial.print( "MQTT to " );
      Serial.print( servers );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    } else {
      Serial.print( "MQTT connection failed, rc = " );
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay( 100 );
    }
  }
}
void publish(float v, float r)
{
  float val[2];
  val[0] = v;
  val[1] = r;
  ThingSpeak.setField(3, v);
  ThingSpeak.setField(2, r);
  ThingSpeak.writeFields(writeChannelID, writeAPIKey);
  updateOm2m(val);
}

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi.");
  Serial.println("IP address is : ");
  Serial.println(WiFi.localIP());
  //configTime(0, 0, ntpServer);
  mqttClient.setServer( servers, mqttPort);
  ThingSpeak.begin(client);

  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(HALLSEN_A, INPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  // testing
  Serial.print("Testing DC Motor...");

}

float get() {

  /// this function returns the rpm value
  // motor will run for 25 seconds as in a loop of index 5 with 5 seconds each

  float rpm2 = 0;
  for (int i = 0; i < 5; i++)
  {
    unsigned long stTime = millis();
    float Rota = 0;
    unsigned long enTime = stTime + 5000;
    while (millis() < enTime) {
      long s1 = millis();
      if (digitalRead( HALLSEN_A))  {
        Rota += 1;
        while (digitalRead( HALLSEN_A) == 1 && millis() < s1 + 1000) {}

      }
    }

    // rpm calculation
    /// rpm sensor has an encoder that has 20
    /// for each sensor will sense once so, if the sensor detects 20 such high signals,motor will complete a full rotation
    /// since e are running the loop for 5 sec, rpm calcluation is as follows,
    //////             rpm=(((Rota)*60)/20)/5 = Rota*0.6
    Rota *= 0.6;
    rpm2 += Rota;
  }
  rpm2 *= 0.2;
  return rpm2;

}
void loop()
{
  if ((millis() - lastTime) > timerDelay) {

    if (!mqttClient.connected())
      mqttConnect();
    mqttClient.loop();

    Serial.print("enter the voltage: ");

    level = ThingSpeak.readFloatField(writeChannelID, 1, "RM7XJZI5B5QF4MM8");
    if (prevlevel == level)
    {
      for (int h = 0; h < 5; h++)
      {
        delay(500);
        level = ThingSpeak.readFloatField(writeChannelID, 1, "RM7XJZI5B5QF4MM8");
      }
    }
    Serial.println(level);
    if (level < 13 && level > 0) {


      //////////////
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      unsigned long stTime1 = millis();
      unsigned long enTime1 = stTime1 + 100;
      while (millis() < enTime1) {
      }

      //////////////


      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);

      dutyCycle = (vol[level - 1] * 256) / 12;
      ledcWrite(pwmChannel, dutyCycle);
      float voltage = v[level - 1];
      rpm = get();
      prevlevel = level;

      // Only update display when there have readings
      if ( rpm >= 0) {
        Serial.print(rpm);
        Serial.println(" RPM");
        Serial.println(voltage);
        if (level < 6)
          publish(voltage, 0);
        else if (level == 6)
        {
          if (rpm > 60)
            publish(voltage, 40);
          else
            publish(voltage, 0);
        }
        else
          publish(voltage, rpm);
      }
    }
    else
      Serial.println("voltage out of bounds: ");
    Serial.print("exiting ");
  }
}
