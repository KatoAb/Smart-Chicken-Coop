


//----------- TFT LCD ------------------
#define TFT_CS         13  // Case select connect to pin 5
#define TFT_RST        15 // Reset connect to pin 15
#define TFT_DC         21 // AO connect to pin 19
#define TFT_MOSI       12 // Data = SDA connect to pin 23
#define TFT_SCLK       14 // Clock = SCK connect to pin 18

#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 27 // ESP32 pin GPIO27

//--------------------------------------

//-------------WIFI-------------------
const char* ssid = "SSID1";
const char* ssid2 = "SSID2";
const char* password =  "PW1";
const char* password2 = "PW2";
const char* mqttServer = "Server ip";
const int mqttPort = 1883;

WiFiClient eggCounter;
PubSubClient client(eggCounter);
//--------------------------------------

//-------------Sensor values------------
float airTemp;
float airHum;
float airVOC;
float waterTemp;
float PHValue;
String foodTime = "";
float waterLevel;


float oldAirTemp;
float oldAirHum;
float oldAirVOC;
float oldWaterTemp;
float oldPHValue;
String oldFoodTime = "";
float oldWaterLevel;

//--------------------------------------

//-------------KEYPAD-------------------
const int ROW_NUM = 4; // Four rows
const int COLUMN_NUM = 4; // Four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {33, 25, 26, 27}; // Connect to the row pinouts of the keypad
byte pin_columns[COLUMN_NUM] = {16, 4, 32, 17}; // Connect to the column pinouts of the keypad

//--------------------------------------


String inputString = ""; // String to store the input
String previousInputString = "";
unsigned long dataSentTimer = 0;
int sentTime = 3000; //Display message for 3 sec


unsigned long bootTimer = 0;
int bootLength = 4000; //4 sec


uint8_t caseState = 0;  //Variable to define what case statement that should run, example: "1" to run the bootscreen, "2" to diplay homescreens etc
uint8_t lastState = 0;  //Variable to store last case statement
int currentHomeScreen = 0;     //Var to store what homescreen is beeing displayed
unsigned long picTimer = 0;    //
unsigned long sensorDataTimeOut = 0;
int sensorTime = 20000; //20 sec
int timeOutTime = 30000; //30 sec
unsigned long timeOutTimer = 0;
unsigned long timeOutShowTimer = 0;
int timeOutShowTime = 4000; //show timeout for 4 sec
int sensorTimerWarning = 5000;
unsigned long cardUnknownTimer = 0;
int cardUnknownDisplayTime = 4000; //display warning for 4 sec
int homeTime = 4000;
int startX = 43;
//----------RFID VARIABLES -----------
unsigned long rfidAllow = 0;
char cardUID[4]; // scanned card UID saved here

char knownCards[][4] = {
  {0xCD, 0x7C, 0xA4, 0x81}, //card UID for user 1
  {0x8A, 0x88, 0x1E, 0x76}, //card UID for user 2
  {0x6E, 0x7F, 0xB3, 0xE9}, // card for user 3
  {0xE4, 0xB9, 0xD8, 0x31}, // card for user 4
  {0x5A, 0x4A, 0x95, 0x65},
  // Add more cards if necessary
};

char pinCodes[][6] = {
  "3838", // code for user 1
  "4893", // code for user 2 
  "9696",  //code for user 3
  "9696", //code for user 4
  "1234", //code for user 5
};

String inputCode = "";

int checkValue = 0; //value to store what array number of code to check for.

char knownInfo[][20] = { //this info CAN be sent to server and logged when someone enter. 
  "Adam", //info about user 0
  "Amanda", //info about user 1 
};

bool knownCard = false;

int accessTime = 5000; //show access denied or granted for 5  sec
unsigned long accessTimer = 0;

int printCaseIntervall = 800;
unsigned long casePrintTimer = 0;


//-----------------SensorDataTimer---------------
unsigned long displaySensorDataTimer = 0;
int displaySensorDataTime = 2000;

String Access = "1";

enum textColor {
    RED = 0xF800,     // 
    GREEN = 0x07E0,   // 
    BLUE = 0x001F,    // 
    WHITE = 0xFFFF,   // 
    BLACK = 0x0000,   // 
    YELLOW = 0xFFE0,  // 
    ORANGE = 0xFC00,  // 
    PURPLE = 0x8010   // 
    // ... Add more colors if wanted
};

textColor PHtextColor;
textColor IAQtextColor;
textColor waterLevelTextColor;
textColor defaultTextColor = WHITE;

unsigned long energySaveTimer;
long energySaveIntervall = 120000; //2 minutes //(120 sec * 1000 ms/sec)





