// Importando as bibliotecas necessárias
# include <ESP8266WiFi.h>
# include <PubSubClient.h>

// Define configs do MQTT
# define ID_MQTT "sensor_gas_mqtt"
# define TOPICO_PUBLISH_SENSOR "topico_sensor_gas"

// Endereço e a porta do broker
const char * BROKER = "test.mosquitto.org";
int PORTA = 1883;

// Definindo os pinos dos dispositivos
#define MQ2_SENSOR    A0 //A0
#define RED_LED       12 //D6
#define Buzzer        14 //D5

// Informações da rede WiFi
char * SSID = "Mira gomes";
char * PASSWORD = "Carrossel";

WiFiClient espClient;
PubSubClient MQTT(espClient);
int MQ2_SENSOR_Value = 0;
int gas= 0;
char * msg = "";

// Função que se conecta ao MQTT
void ReconectMQTT(void) {
  if (!MQTT.connected()){
    while (!MQTT.connected()) {
      Serial.print("* Conectando-se ao MQTT: ");
      Serial.println(BROKER);
      if (MQTT.connect(ID_MQTT)) {
        Serial.println("Conectado com sucesso");
      } else {
        Serial1.println("Falha ao se conectar.");
        Serial.println("Reiniciando em 2s");
        delay(2000);
      }
    }
  }
}

// Iniciando o MQTT
void StartMQTT(void) {
  MQTT.setServer(BROKER, PORTA);
}

// Função que reconecta o serviço de WiFi
void ReconectWIFI(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado na rede ");
}


void ConsultaConexoes(void) {
  ReconectWIFI(); 
  ReconectMQTT(); 
}

int getSensorData()
{
  MQ2_SENSOR_Value = map(analogRead(MQ2_SENSOR), 0, 1024, 0, 100);
  if (MQ2_SENSOR_Value > 32 ){
    digitalWrite(RED_LED, HIGH);
    digitalWrite(Buzzer, HIGH);
    return MQ2_SENSOR_Value;
  }
  else{
    digitalWrite(RED_LED, LOW);
    digitalWrite(Buzzer, LOW);
    return MQ2_SENSOR_Value;
  }
}

void setup()
{
  Serial.begin(9600);
 
  pinMode(MQ2_SENSOR, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(Buzzer, LOW);

  StartMQTT();
}

void loop()
{
  ConsultaConexoes();
  gas = getSensorData();
  sprintf(msg, "%d", gas);

  MQTT.publish(TOPICO_PUBLISH_SENSOR, msg);
  MQTT.loop();
  delay(2000);

}
