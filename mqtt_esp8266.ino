
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

unsigned long update_interval = 3000;

const char* ssid = "IOT";
const char* wifi_password = "IOT12345";

const char* mqtt_server = "mqtt3.thingspeak.com";
uint16_t mqtt_server_port = 1883;

const char* client_id = "DhMXPCQUOyg4NAYJITIHBQ4";
const char* username = "DhMXPCQUOyg4NAYJITIHBQ4";
const char* password = "w/IM7E7yul8l1gH9W2RpUeTw";

const char* out_topic = "channels/1013452/publish";
const char* in_topic = "channels/1013452/publish";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // const char* client_id += String(random(0xffff), HEX).c_str();
    // Attempt to connect
    if (client.connect(client_id, username, password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish(out_topic, "field1=0&field2=0");
      // ... and resubscribe
      client.subscribe(in_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_server_port);
  client.setCallback(callback);
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > update_interval) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "field1=%ld&field2=%f", value, value/10.0);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(out_topic, msg);
  }
}
