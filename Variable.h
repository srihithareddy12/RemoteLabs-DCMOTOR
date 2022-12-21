#define CSE_IP "esw-onem2m.iiit.ac.in"
#define CSE_PORT 443
#define OM2M_ORGIN "iCmX7m:7YXokk"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "Team-5"
#define OM2M_DATA_CONT "Node-1/Data"
#define INTERVAL 15000L

int motor1Pin1 = 32;
int motor1Pin2 = 33;
int enable1Pin = 27;
const int HALLSEN_A = 19;

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

long previousMillis = 0;
long currentMillis = 0;
long int prev_millis = 0;
unsigned long epochTime;
float rpm = 0;
boolean measureRpm = false;
int motorPwm = 0;

const int freq = 80000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;
int level;
float Rota = 0;

typedef struct datastructure {
  float v1;
  float r1;
} sensor_data;

char ssid[] = "Keerthi";
char password[] = "987654321";

char clientID[] = "DS8SIgw3AxI9JhMLMDwpHQw";
int mqttPort = 1883;
const char* servers = "mqtt3.thingspeak.com";
char mqttUserName[] = "DS8SIgw3AxI9JhMLMDwpHQw";
char mqttPass[] = "QB2ee6Y32oZ0B+xaooS8Bxus";
long writeChannelID = 1922367;
char writeAPIKey[] = "FEPWJ2GRVAVPRWQF";
String cse_ip = "192.168.8.51"; // YOUR IP from ipconfig/ifconfig
String cse_port = "8080";
String Server = "http://192.168.8.51:8080/~/in-cse/in-name/";
String ae = "Team-5";
String cnts[] = {"Voltage", "RPM"};

/// motor driver takes a voltage as input and create modulation signals at a given frequency
/// as a result, the supplied voltage will be different compared to the input signal
/// the following array of voltages is the estimated values of voltages ranging according to input voltages that range from 1 to 12
/// while the output voltage from driver that is the input voltage to motor will range between 0.06 to 10.62
/// the taken estimated values are for 12.24V supply from power supply

float v[12] = {0.06, 0.14, 0.2, 0.3, 0.4, 2, 4.23, 6.2, 7.5, 9.5, 10.59, 10.62};
int vol[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

int steps;
float steps_o = 0;
float t = 0;
int x = 0;
int prevlevel;
