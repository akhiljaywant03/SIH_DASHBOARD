#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN     22         //10 // Configurable, see typical pin layout above
#define SS_PIN      21         //9// Configurable, see typical pin layout above


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
//connecting to the AWS iot
void connectAWSIoT();
void mqttCallback (char* topic, byte* payload, unsigned int length);

//connecting to the local wifi
char *ssid = "only";
char *password = "77777777";

//Assigning the REST API endopoint
//const char *endpoint = "avawwsnryj61m-ats.iot.ap-south-1.amazonaws.com"; //utkarsh
const char *endpoint = "a2ejfun9oxx8np-ats.iot.ap-south-1.amazonaws.com"; //annam
//Data That needs to be sent;
const String gps = "1112 6545";
const String tim = "14:15";
const String ID = "123456";
//assigning the AWS communication port *NEVER use port 1883
const int port = 8883;

//Assigning the topic to where the data is to be published and subscribed
char *pubTopic = "rfid/data";
char *subTopic = "rfid/data1";
//$aws/things/coffee12/shadow/update/delta subscribe to this topic with your custom topic name in the blank on your AWS test window


//Uploading the ROOTca, PRIVATE and CERTIFICATE.

//  Pranav Annam
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
                     "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
                     "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
                     "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
                     "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
                     "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
                     "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
                     "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
                     "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
                     "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
                     "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
                     "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
                     "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
                     "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
                     "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
                     "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
                     "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
                     "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
                     "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
                     "-----END CERTIFICATE-----\n";


const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
                          "MIIDWTCCAkGgAwIBAgIUIxsZ7e3IVUfEkjjwL9/lvdGPRDowDQYJKoZIhvcNAQEL\n"
                          "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
                          "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMDUxNTA5MDI0\n"
                          "NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
                          "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALKvgNsqgzTaJCOjwCMM\n"
                          "EUYi9xmvc9P97KynMJxOkp9jM+ZP8W9Cr/IqgszXs0CTwkVgAi6RhNHhJ7QudOgD\n"
                          "PofcO8czPc3mVTajg1ISvZo3Me2mGjZFF2X/y8mp9eNPZp2qz2eMxT553cRAngbl\n"
                          "h/CLLcAtPdBH00+r6k7m5FEAuxbZ6jJ/KE5MIMspchNOuf+0dmSKybAl03g/+LYr\n"
                          "dtMdgGHj6MY9/kXJEmEjI1AR6tXTsEmv3PrDExx0iuXX4pxJM0F6/+XWCN4uNCxU\n"
                          "KKHA/9PQgi5vYrAjulYQTNiP5AmXAWmHmdIQvmCfvGnb/QtaDnQHYG/oCmzmmeC8\n"
                          "t4ECAwEAAaNgMF4wHwYDVR0jBBgwFoAU65ZxPLFbH/+onXO+QVvZ/pD1h6wwHQYD\n"
                          "VR0OBBYEFOjVcwJUKOljmXT/snpHLMTFcsiiMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
                          "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAlBpLg4Kz2BZyllryE5Qjz5wkL\n"
                          "xt2kytVazd61gPAHnHzdwvdq4yw5/8k5Ohc3TtOGL6pVV1ude0kLNEdxCQrwNOHW\n"
                          "rfNZk7oPDN84k2T0AyoZL9zVRNfse/PX+Ba0EKePpRVOjZbuuqdDzYhKFAFf9QGH\n"
                          "uY+Wf96HU92i3Oq6oBkUF1Ep3sqgrhJiHFnfItg98UpBASdhYQiDckE/Vg3SZ2jc\n"
                          "wC4SEeHI++H2RnKyaACKNpnABzOYDDQkL1o8PQFSIVdIn3SjDoyZoGHCRb73PO4Q\n"
                          "hkZtS2kbwxtXBbsBcC6HMHJCsLqQNu676kKt5C+OhjDGCp0IXJrilAKz7vaJ\n"
                          "-----END CERTIFICATE-----\n";


const char* privateKey =  "-----BEGIN RSA PRIVATE KEY-----\n" \
                          "MIIEowIBAAKCAQEAsq+A2yqDNNokI6PAIwwRRiL3Ga9z0/3srKcwnE6Sn2Mz5k/x\n"
                          "b0Kv8iqCzNezQJPCRWACLpGE0eEntC506AM+h9w7xzM9zeZVNqODUhK9mjcx7aYa\n"
                          "NkUXZf/Lyan1409mnarPZ4zFPnndxECeBuWH8IstwC090EfTT6vqTubkUQC7Ftnq\n"
                          "Mn8oTkwgyylyE065/7R2ZIrJsCXTeD/4tit20x2AYePoxj3+RckSYSMjUBHq1dOw\n"
                          "Sa/c+sMTHHSK5dfinEkzQXr/5dYI3i40LFQoocD/09CCLm9isCO6VhBM2I/kCZcB\n"
                          "aYeZ0hC+YJ+8adv9C1oOdAdgb+gKbOaZ4Ly3gQIDAQABAoIBAF3rT5BjHQnXqu0s\n"
                          "sd36Bs5VrInMpdYEfrUVP8C4r+qlCu+Aj4eKhs6Dt7KRob9Mqu0qFJuDk0t5zBOx\n"
                          "43Lb2a8xfB4s+cxSx/FrKqgl/5CieFTQXLCvyiertuKtufebkXVIBbII1NgM2BrC\n"
                          "WA2J8dTmYsrDPN1QE2qqEwMb62ADX5YjxE0p8PnHgkZI0+Qs0zQHpcOvKZJNvJ5H\n"
                          "bAv0pxnSYnw5nL6WJjpuYdxuPcg9Xg/rtopOp2HS8bo3CXsLYbKtk8j/FxsQmtES\n"
                          "neiVWxMZAN9FdmmmLR+HHn3GsTGD1vhYGhFOi4Jhtd2cm029SUcC3Bhgl2aj7aLA\n"
                          "Nezba+0CgYEA6elpyE+0FE7ZytWiyiz9uh42+fsFCu29hgP+VXccFJDAj5/IpeX0\n"
                          "CPvyzWKyGRqoz+5b1/ffh3gRqTtBcE8weVf0FA4mEd3bSxoH9k+EjGpERFU4B/qM\n"
                          "Xw7SMjiED3lvIgMOrUDNkLoBGLXF3y0WeaC9QheiWsm7Dq1zfUEyWssCgYEAw48N\n"
                          "BRTFCuN+DDkldnL2q/hypQgMeQB/Pqwex9KHmUPj72TJK7t5F2cwDL6lo9GFFzVe\n"
                          "dHHZtxPtrj4mVL2xmDSECs+i5wtRha53xmlGlFuqCUuFvo8GXG8OWVOAKdbrXWvP\n"
                          "sT7gReSxCEEHBv5bDdRskrnsyycGMfyg7FqQcWMCgYBwJX2XEUeYWPmoeubSoeGA\n"
                          "I05eadBQHg24vB9rYHDxFzmLYqMzjt2ClTEi8YfL63E2R+R21huweouVJJjKPN1T\n"
                          "wCyAlQwBkum5xyA8IoG9RJG4ZVc41BIK3TM/6sK+HI2AtBCMVj54fgVakIPu7VIS\n"
                          "PFOg5W3ssz1g2x481k5LIwKBgQCVaX5xG2VQ5G8xt5e/4CZHThOJs5ST/qU5pRf/\n"
                          "ZALmPtCv3zk0iJAEKXq0whScDUC6crQwE6xcDIXUUvfGVwiRWc5o1CuKi0NF9/7b\n"
                          "QSQDxeUsJN+SsIQTq2YzKXEKhoJdZjnRGlEQFaKMw+IynbPD5UDu+c7JiI9bMcFh\n"
                          "0+/4MQKBgHjTDcaUq2Y2F2CzIQDGfGZmtucPDEaKnYsfLyQ2eDWzyZ3M5l3EuRxh\n"
                          "G6udIXGVleRunHotDtIRnYThcDnOR3d+H03q/8EbScKnL3b3RxzEdeJ54UmXez0G\n"
                          "Vs5JWKCoHMk2CN9YBEHTI57ir+bnrrqHSi2CqQ6vXXr2WGtSyDVb\n"
                          "-----END RSA PRIVATE KEY-----\n";



WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

void setup() {
  delay(1000);
  Serial.begin(115200);

  // Start WiFi
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Configuring MQTT Client
  httpsClient.setCACert(rootCA);
  httpsClient.setCertificate(certificate);
  httpsClient.setPrivateKey(privateKey);
  mqttClient.setServer(endpoint, port);
  mqttClient.setCallback(mqttCallback);

  connectAWSIoT();

  Serial.println("\nConnected.");
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  Serial.println("All Set");

}

void connectAWSIoT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32_device")) {
      Serial.println("Connected.");
      int qos = 0; //Make sure the qos is zero in the MQTT broker of AWS
      mqttClient.subscribe(subTopic, qos);
      Serial.println("Subscribed.");
    } else {
      Serial.print("Failed. Error state=");
      Serial.print(mqttClient.state());

      delay(100); //change the delay here to manipulate the device shadow updation speed
    }
  }
}

long messageSentAt = 0;
int dummyValue = 0;
char pubMessage[128];
char subMessage[128];
String pubMes;

void mqttCallback (char* topic, byte* payload, unsigned int length) {
  Serial.print("Received. topic=");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("\n");
}

void mqttLoop(String tag) {
  if (!mqttClient.connected()) {
    connectAWSIoT();
  }
  mqttClient.loop();

  long now = millis();
  if (now - messageSentAt > 100) { //change the delay here to manipulate the device shadow updation speed
    messageSentAt = now;
    //sprintf(pubMessage, "{\"state\": {\"desired\":{\"DUMMY VALUE\":\"%d\"}}}", a);
    //pubMes = "{\"state\": {\"desired\":{ \"Time\":\"" + tim + "\",\"Tag\":\""+ tag + "\", \"GPS\":\""+ gps + "\",}}}";
    pubMes = "{ \"ID\":\"" + ID + "\", \"Time\":\"" + tim + "\",\"Tag\":\"" + tag + "\", \"GPS\":\"" + gps + "\"}";
    //pubMes = "{\"state\": {\"desired\":{\"Tag\":\"" + tag + "\"}}}";
    pubMes.toCharArray(pubMessage, 128);
    Serial.print("Publishing message to topic ");
    //Serial.println(pubTopic);
    Serial.println(pubMessage);
    mqttClient.publish(pubTopic, pubMessage);
    Serial.println("Published.");


  }
  delay(100);
}

void loop() {
  if (!mqttClient.connected()) {
    connectAWSIoT();
  }
  mqttClient.loop();

  // Look for new cards
  
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
    return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
    return;
    }
    //Show UID on serial monitor
    Serial.println();
    Serial.print(" UID tag :");
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    mqttLoop(content.substring(1));
  
}

/* 
//  Utkarsh Jakate
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
                     "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
                     "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
                     "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
                     "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
                     "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
                     "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
                     "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
                     "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
                     "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
                     "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
                     "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
                     "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
                     "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
                     "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
                     "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
                     "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
                     "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
                     "rqXRfboQnoZsG4q5WTP468SQvvG5"
                     "-----END CERTIFICATE-----\n";


const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
                          "MIIDWjCCAkKgAwIBAgIVAJcZLCbh+UbGJHypvcdfPz515YMNMA0GCSqGSIb3DQEB\n"
                          "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"
                          "IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA0MjEwMTIx\n"
                          "NDdaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"
                          "dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQD1Xbk3I+kSA+bxXNeb\n"
                          "WK/rIugb6wBNR+x7YOTNDh+jLeu2dLk/b2EDvRTKKuwihJcPiEF6HkbVo+O2uMBB\n"
                          "sY80VqXPgs9rbNtUkkYg9T+cU7GPEj0tdWHn+jc0Died9AaGscgYRQkBzk3IeD28\n"
                          "vAYbMOz9FeSSzmlnndp5wn6labiO52o+HJrwnnOkN1udEViyqzVu4dL5EsFCxKFI\n"
                          "LF8SpMTXS9qrYB8+V8dKc7y9KbEgAgGqlfRqbjgmhUpaQdJyB4JAd4HAUlvS+JRV\n"
                          "dhkPBrZvDPNcjl9JIjm2tt81lAm7rjx2APEQ2awbr+t3B9IlbbP4MrG9YRvlzprZ\n"
                          "yZZTAgMBAAGjYDBeMB8GA1UdIwQYMBaAFNaPMCTQokH2TH6Qdt1khLUUp4GMMB0G\n"
                          "A1UdDgQWBBTqxzInwwKhIL0sErZrwLwGVEipczAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"
                          "DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAjCIKr5Xz4EEW7KFSHIUzPesp\n"
                          "QRs4liQIbNUdOdMo2BH36CovMfA8rtaZhG0AU6nkJbUDFFe0j/7WGWpdzVxdW4nX\n"
                          "FTe1vPINQMohhS5cIvzBqUxGHMWH3WG/upldFdnlWwyMuXrMnGBgSlSXhFFB6mZ/\n"
                          "2mIpqpDUBzWOES+aYlWvHGSWDaviw6u76VUP3iOCIuQOkeNiaYzad/YvegP3AsOD\n"
                          "nUXfBiYchOCuCdtH7Yp29O2FdLEaqh/ts/m1B2LmFP/Vm8VrGBu2p29RX9fEe5UT\n"
                          "S5YWauNMqdNc4S0CoFXYqjltcAQ3RTggfBthtqdaAEe8L7lFBRqjPHTO6Fvigg==\n"
                          "-----END CERTIFICATE-----\n";


const char* privateKey =  "-----BEGIN RSA PRIVATE KEY-----\n" \
                          "MIIEpAIBAAKCAQEA9V25NyPpEgPm8VzXm1iv6yLoG+sATUfse2DkzQ4foy3rtnS5\n"
                          "P29hA70UyirsIoSXD4hBeh5G1aPjtrjAQbGPNFalz4LPa2zbVJJGIPU/nFOxjxI9\n"
                          "LXVh5/o3NA4nnfQGhrHIGEUJAc5NyHg9vLwGGzDs/RXkks5pZ53aecJ+pWm4judq\n"
                          "Phya8J5zpDdbnRFYsqs1buHS+RLBQsShSCxfEqTE10vaq2AfPlfHSnO8vSmxIAIB\n"
                          "qpX0am44JoVKWkHScgeCQHeBwFJb0viUVXYZDwa2bwzzXI5fSSI5trbfNZQJu648\n"
                          "dgDxENmsG6/rdwfSJW2z+DKxvWEb5c6a2cmWUwIDAQABAoIBAG7bm5UoKmOzU6J1\n"
                          "Wgyb809/g/MpKAfCmTvdOULUQMMPOlJDRnc8sU5pHX2CCxJFaaLg4wRzrXlVBMtH\n"
                          "icWknZj+FQg+h8x3cFkzhW/anpTY035atT1WrZeL22kvhAK/gDtXE5qzhjBEr6wF\n"
                          "d49QPEoIvupR++XAUKs6DL26fFHrIbbFaxdMX9LDjMWdfFG8fMQVrwSCJxit8MIB\n"
                          "quYqJpvbO1DROZZZ7j/F8x+Sd1KGO85CLHXqywdaDOz6BnkxBFcLMgaBCjv3FX/l\n"
                          "OdDXRw5+GxTaDIsUg89DbM9GjR6cybXaIgrjxgL5PvENnDmJ/SzanCvBDD2i2pjw\n"
                          "eo4mwhECgYEA/KOyjfR/b9NwqUHc5wHVhSDM2oKZ2nMF7XHt3qCKdxA5iNeNUgTV\n"
                          "o6DxDExEc2ppsImNLZ8KaQOnLeSKWZER+FD3W4wARVTIUU6NMU/oBXQun3Yy4FXX\n"
                          "nNwyaY24+ewHQqSGf0UNPm9RrD8t/oBIXi5K0Xz2AvklSpPD2+ln0x8CgYEA+KFC\n"
                          "KKhgMGU0BP3902M4PiY+H32Wog8lJODqkYxshVgmu5ecRqKr/Q+txfW3J6peoZn7\n"
                          "5ybAQgzEIfkE9QMDkpx1fLcaJlSHuokrGYPgfRFDYwvT/jKSu9J2Jloa49eQwH8z\n"
                          "A5NtqfYlDvi0gmdqybBqg6tQZR3G0lercJGUKk0CgYEAo6AqLwKh3v1kxFriBcQM\n"
                          "VJLv2r4ng3Jn7meRllUIGSErAlQJNYvoCnhcZoim9NGA7lPIWjnddjJlfrW8XVI1\n"
                          "/0wBSQudyUmmC/MCQ2PrdEzRPXpCbUVrxi5fgDnICiYydZk1ewxwDXXvwc2RrnnL\n"
                          "ZSQ6VP/IHhjbbDfT7IR5OCUCgYEA1KZwBIBGjP3yKRboIwWo1Bc3aQ2saX2Uol8w\n"
                          "oT4+jyofLrF0e0Gk4nOniDop5eYkvAkM195f794lXn0zDS//vUTOg9XDqub/k77A\n"
                          "Ozl9Jts+DJfQQIkW0Ugnv5OvjhIQf2QLyNIuqICbn5t+IE69mjWmVGmRAHsgU0Wi\n"
                          "n0wUMr0CgYAXDGWwcTJ9CJNUZVd0zKUWpu6q2JgatTZ0Nl5ovs+sh6HNq0UtU/GX\n"
                          "7LU3s123wG7X4UqcU5G3ebREf6CiP+HTIxKpN1HpMrsby7IhIXw5FmXbbJ4QnnsU\n"
                          "+TD86UMSK+yZltYNDeVYbolkc32JVK48sjl4wwe/4+rW67cgwdJY3A==\n"
                          "-----END RSA PRIVATE KEY-----\n";
*/
