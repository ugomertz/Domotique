#include <ESP8266WiFi.h>
#include <aREST.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFunBME280.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 6

const char* ssid = "Bbox-ugo";
const char* password = "wastedskills";
WiFiServer server(80);
aREST rest = aREST();

DHT dht(DHTPIN,DHTTYPE,15);
BME280 bme;
float temperature=0;
int humidity=0;
int mesure_humidity=0;

//---------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(0,OUTPUT);
 // Ouverture du port serie en 115200 baud pour envoyer des messages de debug à l'ide par exemple
  Serial.begin(115200);
  delay(10);
// préparation du capteur
  bme.settings.commInterface = I2C_MODE;
  bme.settings.I2CAddress = 0x76;
  bme.settings.runMode = 3;
  bme.settings.tStandby = 0;
  bme.settings.filter = 0;
  bme.settings.tempOverSample = 1 ;
  bme.settings.pressOverSample = 1;
  bme.settings.humidOverSample = 1;
  delay(10);
  Serial.println("Starting BME280... ");
  delay(200);       // attente de la mise en route du capteur. 2 ms minimum

  // chargement de la configuration du capteur
  bme.begin();
  Serial.print("ID(0xD0): 0x");
  Serial.println(bme.readRegister(BME280_CHIP_ID_REG), HEX);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // On se connecte a reseau WiFi avec le SSID et le mot de passe precedemment configure
  WiFi.begin(ssid, password);
  
  // On sort de la boucle uniquement lorsque la connexion a ete etablie.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

 // exposer les variables dans l'api arest puis on initialise l'id et le nom de la carte
 rest.variable("temperature",&temperature);
 rest.variable("humidity",&humidity);
 rest.set_id("1");
 rest.set_name("sensor_module");

  // connexion OK, on demarre le server web // Start the server
  server.begin();
  Serial.println("Server started");

  // On indique sur le port serie l'adresse ip de l'ESP pour le trouver facilement / Print the IP address
  Serial.println(WiFi.localIP());

  dht.begin();
}

//---------------------------------------------------------------------------------------------------------
void loop() {

 //vérification qu'un client est connecté et gestion de la requête
 temperature = bme.readTempC();
 humidity = dht.readHumidity();

 WiFiClient client = server.available();
 if (!client){
  return;
 }
 while(!client.available()){
  delay(1);
 }
 rest.handle(client);

}
