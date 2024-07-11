#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Include the ArduinoJson library
#include "time.h"
#include "sntp.h"

const char* ssid = "WIFIUSERNAME";
const char* password = "WIFIPASSWORD";

// Set up MQTT Broker
const char* mqtt_server = "YoursMQTTBroker";
const char* mqtt_user = "YourUsername";
const char* mqtt_password = "YourPassword";


// Setting NTP - Untuk waktu lokal (WIB)
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 7 * 3600; // WIB (UTC + 7 hours)
const int   daylightOffset_sec = 0;

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)


// Topik mesin - ADD YOUR TOPIC TO VARIABLE
const char* MQTT_topic = "MQTT/status";
const char* control_topic = "Motor/control";
const char* resp_topic = "Motor/resp";
const char* status_topic = "Motor/status";
const char* counter_topic = "counter/value";

// Sensor Proximity Pin
const int irSensorPin = 13;  // Pin ADC yang terhubung ke kaki Vo sensor IR.

bool commandReceived = false; // Pemeriksa untuk melacak penerimaan perintah

WiFiClient espClient;
PubSubClient client(espClient);

char timeString[40];

int counter = 0;
unsigned long previousMillis = 0;
const long interval = 7000;

// Konfigurasi pin motor stepper
const int PUL_PIN = 16;  // GPIO 26 pada ESP32
const int DIR_PIN = 17;  // GPIO 27 pada ESP32

// Fungsi print waktu lokal
void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  strftime(timeString, sizeof(timeString), "%Y - %B - %d . %H:%M:%S ", &timeinfo);

  //When : Year/month/day Time
  Serial.println(timeString);

  if (!client.connected()) {
    reconnect();
  }

}

void publish_HIGH_Message() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Status"] = "HIGH";
  jsonDoc["Open"] = "True";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer

  // Add a new line character at the end of the JSON payload
  strcat(jsonBuffer, "\n");

  // Publish the JSON message to the MQTT topic
  client.publish(resp_topic, jsonBuffer);
}

void publish_LOW_Message() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Status"] = "LOW";
  jsonDoc["Open"] = "False";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer

  // Add a new line character at the end of the JSON payload
  strcat(jsonBuffer, "\n");

  // Publish the JSON message to the MQTT topic
  client.publish(resp_topic, jsonBuffer);
}



void publish_Falsestatus() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Open"] = "False";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer

  // Add a new line character at the end of the JSON payload
  strcat(jsonBuffer, "\n");

  // Publish the JSON message to the MQTT topic
  client.publish(resp_topic, jsonBuffer);
}

void publish_Truestatus() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Open"] = "True";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer

  // Add a new line character at the end of the JSON payload
  strcat(jsonBuffer, "\n");

  // Publish the JSON message to the MQTT topic
  client.publish(resp_topic, jsonBuffer);
}




void sendESPStatus() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string
  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Connection"] = "ONLINE";

  if (!client.connected()) {
    reconnect();
  }
  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  // Add a new line character at the end of the JSON payload
  strcat(jsonBuffer, "\n");
  // Publish the JSON message to the MQTT topic
  client.publish(resp_topic, jsonBuffer);
}




void Publish_Door_Open() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["DOOR"] = "OPEN / TERBUKA";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  strcat(jsonBuffer, "\n"); // Add a new line character at the end of the JSON payload
  client.publish(status_topic, jsonBuffer);
}

void Publish_Door_Close() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["DOOR"] = "CLOSED / TERTUTUP";
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  strcat(jsonBuffer, "\n"); // Add a new line character at the end of the JSON payload
  client.publish(status_topic, jsonBuffer);
}

void Sensor_status() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string

  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Object"] = "OBJECT DETECTED";
  jsonDoc["When"] = timeString;
  if (!client.connected()) {
    reconnect();
  }

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  strcat(jsonBuffer, "\n"); // Add a new line character at the end of the JSON payload
  client.publish(status_topic, jsonBuffer);
}


void Counter_Message() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string
  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Jumlah"] = counter;
  jsonDoc["When"] = timeString;

  if (!client.connected()) {
    reconnect();
  }
  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  strcat(jsonBuffer, "\n");
  client.publish(counter_topic, jsonBuffer);
}


void ResetCounter_() {
  printLocalTime(); // Call the printLocalTime function to get the formatted time string
  DynamicJsonDocument jsonDoc(256); // Create a JSON document with a capacity of 256 bytes

  jsonDoc["ID"] = "MSB-001-T";
  jsonDoc["Jumlah"] = counter;
  jsonDoc["Info"] = "Counter reset to 0";

  if (!client.connected()) {
    reconnect();
  }
  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer); // Serialize the JSON document to a buffer
  strcat(jsonBuffer, "\n");
  client.publish(counter_topic, jsonBuffer);
}

//////////        //////////////////




void setup() {
  Serial.begin(115200);

  pinMode(irSensorPin, INPUT);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Wifi Setup
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // NTP - local time setup -  set notification call-back function
  sntp_servermode_dhcp(1);    // (optional)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  //configTzTime(time_zone, ntpServer1, ntpServer2);

}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message : " + message);
  Serial.println();
  Serial.println("MSB-001-T Online");
  Serial.println("-----------------------");

  // Assuming message is a JSON string like {"ID": "MSB-001-T"}
  DynamicJsonDocument jsonDoc(256);
  DeserializationError error = deserializeJson(jsonDoc, message);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  } 
  else {
    const char* device = jsonDoc["ID"];
    const char* check_status = jsonDoc["Code"];

    if (strcmp(device, "MSB-001-T") == 0) {
      commandReceived = true;
      digitalWrite (DIR_PIN, HIGH);
      for (int i = 0; i < 215; i++) { // 215 langkah
        digitalWrite (PUL_PIN, HIGH);
        delayMicroseconds(1200); // Kecepatan bisa disesuaikan
        digitalWrite (PUL_PIN, LOW);
        delayMicroseconds(1200);
      }
      delay(1000);
      publish_HIGH_Message();
      Serial.println("-----------------------");
      Serial.println("MSB-001-T - Terbuka");
      Publish_Door_Open();
      Serial.println("-----------------------");
    }
    else if (strcmp(device, "MSB-001-T") == 0 && strcmp(check_status, "3") == 0) {
      if (commandReceived = true) {
        publish_Truestatus();
      }
      else {
        publish_Falsestatus();
      }
    }
    else if (strcmp(device, "MSB-001-T") == 0 && strcmp(check_status, "reset") == 0) {
      commandReceived = false;
      counter = 0;

      // Create reset message
      String resetMessage = "Counter reset to 0";
      Serial.println(resetMessage);
      ResetCounter_();
    }
   }

    if (message == "MSB-001-T") {
      commandReceived = true;
      publish_HIGH_Message();
      digitalWrite (DIR_PIN, HIGH);
      for (int i = 0; i < 215; i++) { // 215 langkah motor
        digitalWrite (PUL_PIN, HIGH);
        delayMicroseconds(1200); // Kecepatan bisa disesuaikan
        digitalWrite (PUL_PIN, LOW);
        delayMicroseconds(1200);
      }
      // Delay untuk menunggu sepenuhnya
      delay(1000);
      publish_HIGH_Message();
      Serial.println("-----------------------");
      Publish_Door_Open();
      Serial.println("MSB-001-T - Terbuka");
      Serial.println("-----------------------");
    }

    else if (message == "MSB-001-T - Reset") {
      commandReceived = false;
      counter = 0;

      // Create reset message
      String resetMessage = "Counter reset to 0";
      Serial.println(resetMessage);
      ResetCounter_();
    }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("MSB-001-T", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.publish(MQTT_topic, "Hi MSB-001-T ,we're connected");
      client.subscribe(control_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

  void loop() {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      sendESPStatus();
      delay(2000);
    }
    delay(1000);


    int irSensorValue = digitalRead(irSensorPin);

    if (commandReceived && irSensorValue == LOW) {
      Serial.println("Objek terdeteksi!");
      // Publish status jika objek terdeteksi
      Sensor_status();
      // Jalankan logika objek terdeteksi
      Serial.println("MSB-001-L1 LOW");
      digitalWrite (DIR_PIN, LOW);
      for (int i = 0; i < 215; i++) { // 215 langkah menutup lengan
        digitalWrite (PUL_PIN, HIGH);
        delayMicroseconds(1600); // kecepatan bisa disesuaikan
        digitalWrite (PUL_PIN, LOW);
        delayMicroseconds(1600);
      }
      // Delay untuk menunggu sepenuhnya
      delay(1000);
      publish_LOW_Message();
      Serial.println("-----------------------");
      Publish_Door_Close();
      Serial.println("Tertutup");
      delay(800);
      commandReceived = false;
      counter++;


      // Create message counter +
      String counterMessage = "Buah Manis (Cat-A): " + String(counter);
      Serial.println(counterMessage);

      Counter_Message();
      Serial.println("-----------------------");

    }
    else {
      Serial.println("Tidak ada objek terdeteksi.");
      delay(800);
      // Tambahkan logika kontrol atau respons sesuai kebutuhan
    }
    delay(1000);  // Tambahkan penundaan agar tidak membaca data terlalu cepat

  }
