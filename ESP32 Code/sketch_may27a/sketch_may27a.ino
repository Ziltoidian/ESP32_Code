#define         MQ1                       (2)     //define la entrada analogica para el sensor
#define         RL_VALOR             (5)     //define el valor de la resistencia mde carga en kilo ohms
#define         RAL       (9.83)  // resistencia del sensor en el aire limpio / RO, que se deriva de la                                             tabla de la hoja de datos
#define         GAS_LP                      (0)
String inputstring = "";                                                        //Cadena recibida desde el PC
float           LPCurve[3]  =  {2.3,0.21,-0.47};
float           Ro           =  10;

#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 4
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Totalplay-119C";
const char *pwd = "jamonensalsademiel8";
const char *servidor = "http://192.168.100.48:3050/add";
String payload = "";

void setup(){
Serial.begin(115200);  
WiFi.begin(ssid, pwd);

while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi...");
}

Serial.println("Conectado a la red WiFi");
Serial.println(WiFi.localIP());

// Comenzamos el sensor DHT
  dht.begin();
  //Inicializa Serial a 115200 baudios
 Serial.println("Iniciando ...");
   //configuracion del sensor
  Serial.print("Calibrando...\n");
  Ro = Calibracion(MQ1);                        //Calibrando el sensor. Por favor de asegurarse que el sensor se encuentre en una zona de aire limpio mientras se calibra
  Serial.print("Calibracion finalizada...\n");
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}
 
void loop()
{
   Serial.println("Censando...");
   delay(50);
   int gas = porcentaje_gas(lecturaMQ(MQ1)/Ro,GAS_LP);
   // Leemos la humedad relativa
   float h = dht.readHumidity();
   // Leemos la temperatura en grados centígrados (por defecto)
   float t = dht.readTemperature();
   delay(50);
   if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(servidor);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST

      payload.concat("humedad=");
      payload.concat(h);
      payload.concat("&temperatura=");
      payload.concat(t);
      payload.concat("&ppm=");
      payload.concat(gas);
      
      int httpResponseCode = http.POST(payload);
     
      Serial.print("Codigo de respuesta: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
      payload = "";
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   delay(200);
}
 
float calc_res(int raw_adc)
{
  return ( ((float)RL_VALOR*(1023-raw_adc)/raw_adc));
}
 
float Calibracion(float mq_pin){
  int i;
  float val=0;
    for (i=0;i<50;i++) {                                                                               //tomar múltiples muestras
    val += calc_res(analogRead(mq_pin));
    delay(500);
  }
  val = val/50;                                                                                         //calcular el valor medio
  val = val/RAL;
  return val;
}
 
float lecturaMQ(int mq_pin){
  int i;
  float rs=0;
  for (i=0;i<5;i++) {
    rs += calc_res(analogRead(mq_pin));
    delay(50);
  }
rs = rs/5;
return rs;
}
 
int porcentaje_gas(float rs_ro_ratio, int gas_id){
   if ( gas_id == GAS_LP ) {
     return porcentaje_gas(rs_ro_ratio,LPCurve);
   }
  return 0;
}
 
int porcentaje_gas(float rs_ro_ratio, float *pcurve){
  return (pow(10, (((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
