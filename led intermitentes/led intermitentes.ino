/*===== Importamos las librerias necesarias =====*/
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/*===== Definir los leds =====*/
#define LED1 4 //intermitente derecha
#define LED2 5 //faro
#define LED3 21 //faro
#define LED4 23 //intermitente izquierda

/*===== Nos conectamos a nuestra red Wifi =====*/
const char *ssid = "Ctrl - Z";
const char *pass = "56584011";


/*===== Definimos el broker =====*/
const char *mqtt_server = "servidorhall.sytes.net";
/*===== Definimos el puerto =====*/
const int mqtt_port = 1883;
/*===== Definimos el usuario =====*/
const char *mqtt_user = "test";
/*===== Definimos la contraseña =====*/
const char *mqtt_pass = "test";
/*===== Definimos el topico al cual nos vamos a suscribir =====*/
char *root_topic_subscribe = "TOj7joSr0VnGeZ9/UPT";
/*===== Definimos el topico sobre el cual publicaremos =====*/
char *root_topic_publish = "TOj7joSr0VnGeZ9/Estancia";


WiFiClient espClient;
/*===== Método para publicar =====*/
PubSubClient client(espClient);


/*===== Definimos el callback =====*/
/*===== El callback es el evenento que se ejecutará cuando reciba un mensaje publicado al topico suscrito =====*/
void callback(char *topic, byte *payload, unsigned int length)
{
  String datos;
  /*===== El arreglo de bytes payload se convierte a string =====*/
  for (int i = 0; i < length; i++)
  {
    datos += (char)payload[i];
  }
  Serial.println("El bkroker envió al tópico: " + (String)topic);
  Serial.println("El broker envió: " + datos);

  /*===== Deserializamos nuestro JSON que vamos a recibir desde alexa =====*/
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, datos);

  /*===== Se asignan las propiedadeds del objeto a las variables correspondientes =====*/
  String estado = doc["estado"];
  String direccion = doc["direccion"];
  Serial.println((String) estado + direccion);


  /*===== Hacemos los condicionales que serán los que controlen el estado de nuestros leds =====*/
  if (direccion == "luces" || direccion == "focos")
  {
    if (estado == "prender" || estado == "encender")
    {
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
    }
    if (estado == "apagar")
    {
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    }
  }

  if (direccion == "izquierda" || direccion == "intermitentes" )
  {
    if (estado == "prender" || estado == "encender")
    {
      for(int i = 0; i<=10;i++){
      digitalWrite(LED1, HIGH);
      delay(500);
      digitalWrite(LED1, LOW);
      delay(500);
      }
    }
    if (estado == "apagar")
    {
      digitalWrite(LED1, LOW);
    }
  }
  if (direccion == "derecha" || direccion == "intermitentes")
  {
    if (estado == "prender" || estado == "encender")
    {
       for(int i = 0; i<=10;i++){
      digitalWrite(LED4, HIGH);
      delay(500);
      digitalWrite(LED4, LOW);
      delay(500);
      }
    }
    if (estado == "apagar")
    {
      digitalWrite(LED4, LOW);
    }
  }
  /*
  if(direccion == "intermitentes")
  {
    if(estado == "prender" || estado == "encender")
    {
       for(int i = 0; i<=50;i++){
      digitalWrite(LED4, HIGH);
      digitalWrite(LED1, HIGH);
      delay(150);   
      digitalWrite(LED4, LOW);
      digitalWrite(LED1, LOW);
      delay(150);  
      }
    }
    if (estado == "apagar")
    {
      digitalWrite(LED4, LOW);
      digitalWrite(LED1, LOW);
    }
    
  }
 */
}

/*===== Método para conectarse al broker cuando se desconecte =====*/
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando MQTT...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Conectado");
      client.publish(root_topic_publish, "Conectado a la APP");
      client.subscribe(root_topic_subscribe);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" volver a intentar en 5 segundos.");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a... ");
  Serial.println(ssid);
  Serial.print("IP del servidor: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  /*===== Inicializamos nuestros leds =====*/
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
}
int contador = 0;

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
  delay(100);
}