#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1


// Credencial GPRS
const char apn[]  = "broadband.tigo.gt";
const char user[] = "";
const char pass[] = "";

//Credencial 2T
const char* broker = "m13.cloudmqtt.com";
const char* userBroker = "---";
const char* passwordBroker = "---";
const char* idClient = "-----";
const int port = 1883;

const char* topicLed = "GPRS/led";
const char* topicInit = "GPRS/init";
const char* topicLedStatus = "GPRS/ledStatus";

TinyGsm modem(Serial1);
TinyGsmClient client(modem);
PubSubClient mqtt(client);



byte PWR_KEY=9;
bool SIM_ON = false;

unsigned long tm=0;
unsigned long tm1=0;
long lastReconnectAttempt = 0;

void setup() {
  pinMode(PWR_KEY,OUTPUT);
//  SIM_ON = statusGPRS();
//  if(!SIM_ON){
//    onGPRS();
//    }
     digitalWrite(PWR_KEY,HIGH);
    delay(2000);
    digitalWrite(PWR_KEY,LOW); 

  //PLOT SERIAL
  SerialUSB.begin(115200);
  //GPRS SERIAL
  Serial1.begin(115200);
  delay(3000);
  SerialUSB.println("Iniciado");
  


  SerialUSB.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialUSB.print("Modem: ");
  SerialUSB.println(modemInfo);

  SerialUSB.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialUSB.println(" fail");
    while (true);
  }
  SerialUSB.println(" OK");

  SerialUSB.print("Connecting to ");
  SerialUSB.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialUSB.println(" fail");
    while (true);
  }
  SerialUSB.println(" OK");

  // MQTT Broker setup
  mqtt.setServer(broker, port);
  
  mqtt.setCallback(mqttCallback);

  tm = millis();
}

boolean mqttConnect() {
  SerialUSB.print("Connecting to ");
  SerialUSB.print("server");
  if (!mqtt.connect(idClient,userBroker,passwordBroker)) {
    SerialUSB.println(" fail");
    return false;
  }
  SerialUSB.println(" OK");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void loop() {


  if (mqtt.connected()) {
    mqtt.loop();
  } else {
    // Reconnect every 10 seconds
    unsigned long t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }

  tm1=millis();
  if(tm1>(tm+1000)){
    tm=millis();
    SerialUSB.println("Send data");
    mqtt.publish("GPRS/Cafetal", "Msa");
    }

}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialUSB.print("Message arrived [");
  SerialUSB.print(topic);
  SerialUSB.print("]: ");
  SerialUSB.write(payload, len);
  SerialUSB.println();

  // Only proceed if incoming message's topic matches
//  if (String(topic) == topicLed) {
//    ledStatus = !ledStatus;
//    mqtt.publish("GPRS/Cafetal", "hola");
//  }
}

 bool statusGPRS(){
   Serial1.println("AT");
   delay(5);
//   yield();
   if(Serial1.available()>0){
    return true;
    }else{
      return false;      
      }
  }

void onGPRS(){
    digitalWrite(PWR_KEY,HIGH);
    delay(2000);
    digitalWrite(PWR_KEY,LOW); 
  }
