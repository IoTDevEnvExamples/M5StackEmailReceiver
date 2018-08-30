/**
 * A simple Azure IoT example for sending telemetry to Iot Hub.
 */

#include <WiFi.h>
#include <M5Stack.h>
#include "Esp32MQTTClient.h"
#include "parson.h"

#define DEVICE_ID "Esp32Device"
#define TFTW            320     // screen width
#define TFTH            240     // screen height
#define TFTW2           160     // half screen width
#define TFTH2           120     // half screen height
// Please input the SSID and password of WiFi
const char* ssid     = "bbbb";
const char* password = "chenyiwen123";

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "HostName=t-yiwche-test-2-iothub.azure-devices.net;DeviceId=t-yiwhce-test-2-device;SharedAccessKey=0FLOTU4rl2hfkz2F7DKAnkMNfn5coHfGRkX16jmbosc=";
static bool hasIoTHub = false;
static bool hasWifi = false;
static unsigned int current_choice = 0;




static bool test_bool = true;
void display_brief(const char * sender, const char * subject, unsigned int choice){  
  char send_to_display[20];
  char subject_to_display[40];
  if (strlen(sender) > 20){
    memcpy(send_to_display, &sender[0], 20);
    send_to_display[19] = '\0';
    send_to_display[18] = '.';
    send_to_display[17] = '.';
    send_to_display[16] = '.';
  }else{
    memcpy(send_to_display, &sender[0], strlen(sender));
    send_to_display[strlen(sender)] = '\0';
  }
  if (strlen(subject) >= 40){
    memcpy(subject_to_display, &subject[0], 40); 
    subject_to_display[39] = '\0';
    subject_to_display[38] = '.';
    subject_to_display[37] = '.';
    subject_to_display[36] = '.';
  }else{
    memcpy(subject_to_display, &subject[0], strlen(subject)); 
    subject_to_display[strlen(subject)] = '\0';
  }
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, TFTH * choice / 4 + 5);
  M5.Lcd.print("FROM: ");
  M5.Lcd.print(send_to_display);
  M5.Lcd.setCursor(5, TFTH * choice / 4 + 23);
  M5.Lcd.print("SUBJECT: ");
  M5.Lcd.print(subject_to_display);
}

void display_start_page(){
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.drawRect(0 , TFTH  * 0 / 4 , TFTW , TFTH , TFT_WHITE);
}

static void MessageCallback(const char* payLoad, int size)
{
  JSON_Value *root_value;
  root_value = json_parse_string(payLoad);
  JSON_Object *root_object = json_value_get_object(root_value);
  const char* from = json_object_get_string(root_object, "From");
  const char* subject = json_object_get_string(root_object, "Subject");
  const char* body = json_object_get_string(root_object, "Body");
  display_start_page();
  display_brief(from, subject, 0);
  M5.Lcd.setCursor(5, 65);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(body);
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("ESP32 Device\n");
  M5.Lcd.printf("Initializing...\n");
  M5.Lcd.printf(" > WiFi\n");
  M5.Lcd.printf("Starting connecting WiFi.\n");

  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.printf(".");
    hasWifi = false;
  }
  hasWifi = true;
  M5.Lcd.printf("\n");
  M5.Lcd.printf("WiFi connected\n");
  M5.Lcd.printf("SSID: ");
  M5.Lcd.println(WiFi.SSID());
  M5.Lcd.printf(" > IoT Hub\n");
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString, true))
  {
    hasIoTHub = false;
    M5.Lcd.printf("Initializing IoT hub failed.\n");
    return;
  }
  hasIoTHub = true;
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  display_start_page();
}

void loop() {
  if (hasWifi && hasIoTHub){
    Esp32MQTTClient_Check();
    M5.update();
    delay(10);
  }
}
