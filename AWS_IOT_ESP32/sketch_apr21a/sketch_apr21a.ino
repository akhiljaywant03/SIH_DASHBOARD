#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

//connecting to the AWS iot
void connectAWSIoT();
void mqttCallback (char* topic, byte* payload, unsigned int length);

//connecting to the local wifi
char *ssid = "only";
char *password = "77777777";

//Assigning the REST API endopoint 
const char *endpoint = "avawwsnryj61m-ats.iot.ap-south-1.amazonaws.com";


//assigning the AWS communication port *NEVER use port 1883 
const int port = 8883;

//Assigning the topic to where the data is to be published and subscribed
char *pubTopic = "$aws/things/ESP_RFID/shadow/update";
char *subTopic = "$aws/things/ESP_RFID/shadow/update/delta";   
//$aws/things/coffee12/shadow/update/delta subscribe to this topic with your custom topic name in the blank on your AWS test window


//Uploading the ROOTca, PRIVATE and CERTIFICATE.
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
    Serial.println("\nConnected.");

    // Configuring MQTT Client
    httpsClient.setCACert(rootCA);
    httpsClient.setCertificate(certificate);
    httpsClient.setPrivateKey(privateKey);
    mqttClient.setServer(endpoint, port);
    mqttClient.setCallback(mqttCallback);

    connectAWSIoT();
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
String pubMes;

void mqttCallback (char* topic, byte* payload, unsigned int length) {
    Serial.print("Received. topic=");
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.print("\n");
}

void mqttLoop() {
    if (!mqttClient.connected()) {
        connectAWSIoT();
    }
    mqttClient.loop();
    
    char pub[40];
    a.toCharArray(pub,40);
    long now = millis();
    if (now - messageSentAt > 100) { //change the delay here to manipulate the device shadow updation speed
        messageSentAt = now;
        //sprintf(pubMessage, "{\"state\": {\"desired\":{\"DUMMY VALUE\":\"%d\"}}}", a);
        pubMes = "{\"state\": {\"desired\":{\"DUMMY VALUE\":\""+ tag + "\"}}}";
        pubMes.toCharArray(pubMessage,128);
        Serial.print("Publishing message to topic "); 
        //Serial.println(pubTopic);
        Serial.println(pubMessage);
        mqttClient.publish(pubTopic, pubMessage);
        Serial.println("Published.");
    }
    delay(100);
}

void loop() {
  mqttLoop();
}
