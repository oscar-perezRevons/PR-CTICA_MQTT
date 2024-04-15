int DISTANCIA = 0;
int pinEco = 18;
int pinGatillo = 19;
#include <WiFi.h>
#include <PubSubClient.h>
const char * WIFI_SSID = "Network_/";
const char * WIFI_PASS = "Red$9474120";
const int pinLED = 0;
const int pinLED2 = 2;
const int pinLED3 = 4;

const char * MQTT_BROKER_HOST = "broker.hivemq.com";
const int MQTT_BROKER_PORT = 1883;

const char * MQTT_CLIENT_ID = "razor@azzure.iot.bo"; // UNIQUE ID FIRST SMARTPHONE
const char * PUBLISH_TOPIC = "azzure.iot.bo/T_out"; 
const char * PUBLISH_TOPIC_SENSOR = "azzure.iot.bo/SENSOR_out"; 
const char * SUBSCRIBE_TOPIC_LED1 = "azzure.iot.bo/LED1_in"; 
const char * SUBSCRIBE_TOPIC_LED2 = "azzure.iot.bo/LED2_in"; 
const char * SUBSCRIBE_TOPIC_LED3 = "azzure.iot.bo/LED3_in"; 

WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

class LED
{
  private:
    int numero;
  public:
    int getnumero() {
      return numero;
    }
    void setnumero(int n){
      numero=n;
    }
    LED(int n){
      numero=n;
    }
    void encender(){
      digitalWrite(numero, HIGH);
    }
    void apagar() {
      digitalWrite(numero, LOW);
    }
};

LED LDS[3] = {LED(pinLED), LED(pinLED2), LED(pinLED3)};

void callback(const char * topic, byte * payload, unsigned int length){
 String message;
 for (int i = 0; i<length; i++) message += String((char) payload[i]);
 if (String(topic) == SUBSCRIBE_TOPIC_LED1){
    Serial.println("Message from topic " + String(topic) + ":" + message);
    if (message == "LED_ON"){
      digitalWrite(pinLED, HIGH);  
    }else if (message == "LED_OFF")
      digitalWrite(pinLED, LOW);
  }
 if (String(topic) == SUBSCRIBE_TOPIC_LED2){
    Serial.println("Message from topic " + String(topic) + ":" + message);
    if (message == "LED_ON"){
      digitalWrite(pinLED2, HIGH);
    }else if (message == "LED_OFF")
      digitalWrite(pinLED2, LOW);
   }
   if (String(topic) == SUBSCRIBE_TOPIC_LED3){
    Serial.println("Message from topic " + String(topic) + ":" + message);
    if (message == "LED_ON"){
      digitalWrite(pinLED3, HIGH);
    }else if (message == "LED_OFF")
      digitalWrite(pinLED3, LOW);
   }
}

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinLED3, OUTPUT);
  Serial.begin(115200);
  Serial.print("Connecting to " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected!");
  mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);
  Serial.print("Connecting to " + String(MQTT_BROKER_HOST));
  if(mqttClient.connect(MQTT_CLIENT_ID)) { //IMPORTANTE
    Serial.println(" Connected!");
    mqttClient.subscribe(SUBSCRIBE_TOPIC_LED1);
    mqttClient.subscribe(SUBSCRIBE_TOPIC_LED2);
    mqttClient.subscribe(SUBSCRIBE_TOPIC_LED3);
  }
}

unsigned char counter = 0;
unsigned long previousPublishMillis = 0;

void loop() {
  DISTANCIA = 0.01723 * readUltrasonicDistance(pinGatillo, pinEco);
  if (DISTANCIA > 0 && DISTANCIA <= 10) { digitalWrite(pinLED, HIGH); digitalWrite(pinLED2, LOW); digitalWrite(pinLED3, LOW);}
        if (DISTANCIA > 10 && DISTANCIA <= 20) { digitalWrite(pinLED2, HIGH); digitalWrite(pinLED, LOW); digitalWrite(pinLED3, LOW);}
        if (DISTANCIA > 20 && DISTANCIA <= 30) { digitalWrite(pinLED3, HIGH); digitalWrite(pinLED2, LOW); digitalWrite(pinLED, LOW);}
        if(DISTANCIA < 0 || DISTANCIA > 30) {delay(1000);digitalWrite(pinLED, LOW); digitalWrite(pinLED2, LOW); digitalWrite(pinLED3, LOW);}
  if(mqttClient.connected()){
    unsigned long now = millis();
    if (now - previousPublishMillis >= 2000) {
      previousPublishMillis = now;
      String message = String(DISTANCIA);
      mqttClient.publish(PUBLISH_TOPIC, message.c_str());
    }
    mqttClient.loop();
  }
  else{
    Serial.println("MQTT broker not connected!");
    delay(2000);
  }
}
