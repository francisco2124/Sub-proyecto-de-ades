//======================================== Including the libraries.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <DHT.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
//======================================== DHT sensor settings (DHT11).
#define DHTPIN 15 //--> Defines the Digital Pin connected to the DHT11 sensor.
#define SS_PIN 5 //lector rfd
#define RST_PIN 22//lector rfd
#define DHTTYPE DHT11 //--> Defines the type of DHT sensor used. Here used is the DHT11 sensor.
DHT dht11_sensor(DHTPIN, DHTTYPE); //--> Initialize DHT sensor.
// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2 

//RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
String DatoHex;

//******************************


//======================================== SSID and Password of your WiFi router.
//const char* ssid = "Coayoweb";
//const char* password = "Coayoweb-WiFi-0001";
const char* ssid = "HUAWEI.....44....M";
const char* password = "Paraque.";
//const char* ssid = "Aula2-2.4G";
//const char* password = "Entra.2019";
//======================================== Variables for HTTP POST request data.
String postData = ""; //--> Variables sent for HTTP POST request data.
String payload = "";  //--> Variable for receiving response from HTTP POST.//======================================== 
/*
//======================================== Variables for DHT11 sensor data.
float send_Temp;
int send_Humd;
//+++++++++++++++Declarar valor a enviar++++++++++
int valor;
String send_Status_Read_DHT11 = "";
//======================================== 
// 
*/
/*________________________________________________________________________________ Subroutine to read and get data from the DHT11 sensor.
void get_DHT11_sensor_data() {
  Serial.println();
  Serial.println("-------------get_DHT11_sensor_data()");
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  send_Temp = dht11_sensor.readTemperature();
  // Read Humidity
  send_Humd = dht11_sensor.readHumidity();
  
  // Check if any reads failed.
  if (isnan(send_Temp) || isnan(send_Humd)) {
    Serial.println("Failed to read from DHT sensor!");
    send_Temp = 0.00;
    send_Humd = 0;
    send_Status_Read_DHT11 = "FAILED";
  } else {
    send_Status_Read_DHT11 = "SUCCEED";
  }
  
  Serial.printf("Temperature : %.2f °C\n", send_Temp);
  Serial.printf("Humidity : %d %%\n", send_Humd);
  Serial.printf("Status Read DHT11 Sensor : %s\n", send_Status_Read_DHT11);
  Serial.println("-------------");
}
// ________________________________________________________________________________
*/
//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200); //--> Initialize serial communications with the PC.

  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  
  digitalWrite(ON_Board_LED, HIGH); //--> Turn on Led On Board.

  delay(2000);

  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led On Board.n 

  //---------------------------------------- Make WiFi on ESP32 in "STA/Station" mode and start connecting to WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //---------------------------------------- 
  
  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");


  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  } 
  DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.

  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //........................................ Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    //........................................ 

    //........................................ Countdown "connecting_process_timed_out".
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
    //........................................ 
  }
  //---------------------------------------- 
  
  digitalWrite(ON_Board_LED, LOW); //--> Turn off the On Board LED when it is connected to the wifi router.
  
  //---------------------------------------- If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.println("-------------");
  //---------------------------------------- 

  // Setting up the DHT sensor (DHT11).
  dht11_sensor.begin();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP()
void loop() {

  //---------------------------------------------------------------------------------------------------

   // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
     if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verify if the NUID has been readed
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     
     Serial.print(F("PICC type: "));
     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
     Serial.println(rfid.PICC_GetTypeName(piccType));
     // Check is the PICC of Classic MIFARE type
     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
     {
       Serial.println("Su Tarjeta no es del tipo MIFARE Classic.");
       return;
     }
     
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
       Serial.println("Se ha detectado una nueva tarjeta.");
       
       // Store NUID into nuidPICC array
       for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       Serial.print("Codigo Tarjeta: "); Serial.println(DatoHex);
      
     }
     else 
     {
      Serial.println("Tarjeta leida previamente");
     }
     
     // Halt PICC
     rfid.PICC_HaltA();
     // Stop encryption on PCD
     rfid.PCD_StopCrypto1(); 
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------

  //---------------------------------------- Check WiFi connection status.
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    
    //........................................ The process of sending the DHT11 sensor data to the database.
    postData = "id=esp32_01";
    postData += "&DatoHex=" + String(DatoHex);

    /*
    valor = 1;
    postData += "&val=" + String(1);
    

    postData += "&temperature=" + String(send_Temp);

    postData += "&humidity=" + String(send_Humd);
    postData += "&status_read_sensor_dht11=" + send_Status_Read_DHT11;
    */
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------updateDHT11data.php");
    // Example : http.begin("http://192.168.0.0/ESP32_MySQL_Database/Test/updateDHT11data.php");
    //http.begin("http://192.168.1.112/ADES/Test/updateDHT11data.php");  //--> Specify request destination
    
    //String url = "http://192.168.3.3:8080/sensor/asistencia?rfid=" + String(DatoHex)
    http.begin("http://192.168.3.3:8080/sensor/asistencia?rfid=" + String(DatoHex));  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //--> Specify content-type header
    httpCode = http.GET(); //--> Send the request


    //http.begin("http://192.168.1.112/ADES/Test/updateDHT11data.php");  //--> Specify request destination
    //http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //--> Specify content-type header
    //httpCode = http.POST(); //--> Send the request
   
    //httpCode = http.GET(postData); //--> Send the request
    payload = http.getString();  //--> Get the response payload
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
    
    http.end();  //Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 
    
    delay(5000);
  }
  //---------------------------------------- 
}


String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}

//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<