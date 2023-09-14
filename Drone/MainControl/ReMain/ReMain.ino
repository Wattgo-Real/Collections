#include <WiFi.h>
#include "MPU9250.h"
#include <DFRobot_BMP3XX.h>
#include <esp_now.h>
//#include <Servo.h>

#include <ESP32Servo.h> // ESP32Servo library installed by Library Manager
#include "ESC.h" // RC_ESP library installed by Library Manager

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void writeFile(fs::FS &fs, const char *path, String message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    //Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, String message) {
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    //Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

//浮點常數的限制
#define FLT_MIN 1.175494351e-38F
#define FLT_EPSILON 1.192092896e-07F
#define DBL_EPSILON 2.2204460492503131E-16
#define CV_PI 3.1415926535897932384626433832795

// The serial connection to the GPS module
//bool r = false, s = false, o = false;

const unsigned char UBLOX_INIT[] PROGMEM = {

  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A,  // GxGLL off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31,  // GxGSA off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38,  // GxGSV off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F,  // GxRMC off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46,  // GxVTG off

  0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12,  //(10Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};

#include <SoftwareSerial.h>
SoftwareSerial ss(34, 3);  // 腳位 TX=34 RX=3

MPU9250 mpu;
MPU9250Setting setting;

DFRobot_BMP390L_I2C sensor(&Wire, sensor.eSDOVDD);

static const float rad_to_deg = 180 / 3.141592654;
static const float deg_to_rad = 3.141592654 / 180;
static const float GX_init = -1.45, GY_init = -2.28, GZ_init = 0.20;   //陀螺儀初始值
static const float AX_init = 0.0251, AY_init = 0.0516, AZ_init = -0.075;  //加速度計初始值
static const float MX_init = 245, MY_init = 503, MZ_init = -432;  //磁力計初始值
float GVT = 0;

//輸出相關參數
float OUT1 = 1000;  //主速度 OUT1
float OUT2[4];      //輸出速度
long int OUT1time;
float a[4] = {0,6,3,0}; 
float aMix[6];  //四個分速 a[4]
//aMix -> Pitch(x軸)Poll(y軸)Rotate(旋轉)Altitude(高度)Latitude(緯度)Longitude(經度)

//元件接收相關參數
float GX, GY, GZ;    //陀螺儀值(角速度)值
float AX, AY, AZ;    //加速度計(重力方向分量)值
float MX, MY, MZ;    //磁力計
float readAltitude;  //氣壓計

float GPSH;  //GPS高度
//String GPSX, GPSY;  //GPS經度與緯度
long long int GPSX_value,GPSY_value;
int tts = 0;
bool is_GPS_read = false;

//GPS接收相關參數
byte ByteArray[80];
String ByteArray_Copy;  //複製一份ByteArray的資訊
int GPS_t = 0;
int GPS_Count = 0;

//運算相關參數
float KP[6] = { 2, 2, 120, 160, 0.3, 0.3 };  //PID 係數
float KI[6] = { 0.0003, 0.0003, 0.05, 0.05, 0, 0 };
float KD[6] = { 0.24, 0.24, 60, 60, 0.01, 0.01 };
float Target_L[6];  //目標值
float Target_L_Last[6];
float TargetR[2];  //遙控器給的目標值
float L[6];        //測量值
float X_T, Y_T;
float L_sum[6];              //誤差加總
float L_last[6];             //上個誤差
float sin_roll, cos_roll;    //x軸角度，用於地磁補償、GPS向量用地磁回推到無人機向量
float sin_pitch, cos_pitch;  //y軸角度，用於地磁補償
String p = "";
float GPSX, GPSY, GPSX_Last, GPSY_Last;     //GPS可用數據轉換實際比例
float GPSX_Get, GPSY_Get, GPSX_Get_Last, GPSY_Get_Last;                   //GPS位置轉換為可用數據
long long int Target_R_4, Target_R_5;                    //GPS 初始值
//Component_Altitude_ERROR為GPSH與氣壓計之差，LL為加速度濾波裝置
float Component_Altitude_ERROR, Altitude_dis, Altitude_last, Balance_Altitude_last;

static const int lc = 1;
int test_l[lc] = {5};
float test_L[lc];
float test_L_last[lc];
float test_L_sum[lc];
float test_aMix[lc];

//控制相關參數
bool ControwByAltitude, InitAltitude, SearchGPS, UseGPS;  //是否控制

//計時相關參數
int too;                      //loop幾次(測試用)
long int tpp;                 //loop延遲(測試用)
long int nowTime;             //現在時間，計算出每次loop時間
long int totalTime;           //用於GPS
long int control_delta_Time;  //用於控制
long int tTime;               //用於控制

#define ESC1_PIN 32
#define ESC2_PIN 26
#define ESC3_PIN 25
#define ESC4_PIN 33
ESC ESC1 (ESC1_PIN, 1000, 2000, 500); 
ESC ESC2 (ESC2_PIN, 1000, 2000, 500); 
ESC ESC3 (ESC3_PIN, 1000, 2000, 500); 
ESC ESC4 (ESC4_PIN, 1000, 2000, 500); 

//Servo ESC1;
//Servo ESC2;
//Servo ESC3;
//Servo ESC4;

float V_Ar[30] = { 0 };   //從加速度計來的前 r 個的垂直加速度數值。
float Altitude = 0;
float ascent_speed = 0;
float last_Altitude = 0;
float save_take = 0;

float GPSX_s = 0;
float GPSY_s = 0;
float GPSX_last = 0;
float GPSY_last = 0;

typedef struct struct_message {
  float foward_and_backward = 0;
  float altitude = 0;
  float left_and_right = 0;
  float up_and_down = 0;
  float turn = 0;
  bool Start_Up = false;

  float p = 2.5;
  float i = 0.0004;
  float d = 1.2;
  //float a1 = 0;
  //float a2 = 0;
  //float a3 = 0;
  //float a4 = 0;
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  //Serial.println(*mac);
  memcpy(&myData, incomingData, sizeof(myData));
  //a[0] = myData.a1;
  //a[1] = myData.a2;
  //a[2] = myData.a3;
  //a[3] = myData.a4;
  KP[0] = myData.p;
  KI[0] = myData.i;
  KD[0] = myData.d;
  KP[1] = myData.p;
  KI[1] = myData.i;
  KD[1] = myData.d;
  save_take = 200;
}

float Degree(float a, float b, float c) {
  return (atan((a) / sqrt(pow((b), 2) + pow((c), 2))) * rad_to_deg);
}

void mpu_SUp() {
  mpu.verbose(true);
  setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
  setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
  setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
  setting.gyro_fchoice = 0x03;
  setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;
  setting.accel_fchoice = 0x01;
  setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ;
  delay(10);

  mpu.calibrateAccelGyro();
  Serial.println("Finish");
  mpu.verbose(false);
}

//fastaten2參數
static const float atan2_p1 = 0.9997878412794807f * (float)(180 / CV_PI);
static const float atan2_p3 = -0.3258083974640975f * (float)(180 / CV_PI);
static const float atan2_p5 = 0.1555786518463281f * (float)(180 / CV_PI);
static const float atan2_p7 = -0.04432655554792128f * (float)(180 / CV_PI);

float fastatan2(float y, float x) {
  float ax = std::abs(x), ay = std::abs(y);  //首先不分象限，求得一个锐角角度
  float a, c, c2;
  if (ax >= ay) {
    c = ay / (ax + (float)DBL_EPSILON);
    c2 = c * c;
    a = (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
  } else {
    c = ax / (ay + (float)DBL_EPSILON);
    c2 = c * c;
    a = 90.f - (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
  }
  if (x < 0)  //銳角求出後，根據x和y的正負性確定向量的方向，即角度。
    a = 180.f - a;
  if (y < 0)
    a = 360.f - a;
  return a;
}

bool save_Out = true;
bool save_rx = true; bool save_ry = true; bool save_h = true;
bool save_r = true; bool save_x = true; bool save_y = true;
bool K_PID = false; bool Mix_O = true; bool To_O = true;

TaskHandle_t SD_Sent_Task;
void SD_Record(void *pvParameters) {
  deleteFile(SD, "/record_data.txt");
  String R = "Time Out1 X_speed X_position ";

  if (save_Out) {
    R += "OUT21 OUT22 OUT23 OUT24 ";
  }
  if (save_rx) {
    R += "X_ang ";
    if (To_O) { R += "Target_X_ang "; }
    if (K_PID) { R += "KP_X_ang KI_X_ang KD_X_ang "; }
    if (Mix_O) { R += "Output_X_ang "; }
  }
  if (save_ry) {
    R += "Y_ang ";
    if (To_O) { R += "Target_Y_ang "; }
    if (K_PID) { R += "KP_Y_ang KI_Y_ang KD_Y_ang "; }
    if (Mix_O) { R += "Output_Y_ang "; }
  }
  if (save_h) {
    R += "High ";
    if (To_O) { R += "Target_High "; }
    if (K_PID) { R += "KP_High KI_High KD_High "; }
    if (Mix_O) { R += "Output_High "; }
  }
  if (save_r) {
    R += "Z_ang ";
    if (To_O) { R += "Target_Z_ang "; }
    if (K_PID) { R += "KP_Z_ang KI_Z_ang KD_Z_ang "; }
    if (Mix_O) { R += "Output_Z_ang "; }
  }
  if (save_x) {
    R += "X ";
    if (To_O) { R += "Target_X "; }
    if (K_PID) { R += "KP_X KI_X KD_X "; }
  }
  if (save_y) {
    R += "Y ";
    if (To_O) { R += "Target_Y "; }
    if (K_PID) { R += "KP_Y KI_Y KD_Y "; }
  }
  writeFile(SD, "/record_data.txt", R + "\n");

  long int Sv_Time = millis();
  while (true) {
    // Serial.printf("%f, %f\n", L[2]*10,  readAltitude*10);
    while (ss.available()) {
      char j = ss.read();
      if (j == '$') {
        GPS_t = 0;
        is_GPS_read = true;
      }
      if (j == '\n') {
        is_GPS_read = false;
        ByteArray_Copy = (char *)ByteArray;

        for (int i = 0; i < 80; i++) {
          ByteArray[i] = 0x00;
        }
      }
      if (is_GPS_read == true) {
        ByteArray[GPS_t] = j;

        GPS_t += 1;
      }
    }

    if (OUT1 == 1000) {
      delay(2);
      continue;
    }

    String T = String(float(millis() - Sv_Time) / 1000.0) + " " + String(OUT1 - 1000) + " " + String(test_aMix[0], 4) + " " + String(test_aMix[1], 4) + " ";
    if (save_Out) {
      T += String(OUT2[0] - 1000) + " " + String(OUT2[1] - 1000) + " " + String(OUT2[2] - 1000) + " " + String(OUT2[3] - 1000) + " ";
    }
    if (save_rx) {
      T += String(L[0], 4) + " ";
      if (To_O) { T += String(Target_L[0], 4) + " "; }
      if (K_PID) { T += String(KP[0], 4) + " " + String(KI[0], 4) + " " + String(KD[0], 4) + " "; }
      if (Mix_O) { T += String(aMix[0], 4) + " "; }
    }
    if (save_ry) {
      T += String(L[1], 4) + " ";
      if (To_O) { T += String(Target_L[1], 4) + " "; }
      if (K_PID) { T += String(KP[1], 4) + " " + String(KI[1], 4) + " " + String(KD[1], 4) + " "; }
      if (Mix_O) { T += String(aMix[1], 4) + " "; }
    }
    if (save_h) {
      T += String(L[2], 4) + " ";
      if (To_O) { T += String(Target_L[2], 4) + " "; }
      if (K_PID) { T += String(KP[2], 4) + " " + String(KI[2], 4) + " " + String(KD[2], 4) + " "; }
      if (Mix_O) { T += String(aMix[2], 4) + " "; }
    }
    if (save_r) {
      T += String(L[3], 4) + " ";
      if (To_O) { T += String(Target_L[3], 4) + " "; }
      if (K_PID) { T += String(KP[3], 4) + " " + String(KI[3], 4) + " " + String(KD[3], 4) + " "; }
      if (Mix_O) { T += String(aMix[3], 4) + " "; }
    }
    if (save_x) {
      T += String(L[4], 4) + " ";
      if (To_O) { T += String(Target_L[4], 4) + " "; }
      if (K_PID) { T += String(KP[4], 4) + " " + String(KI[4], 4) + " " + String(KD[4], 4) + " "; }
    }
    if (save_y) {
      T += String(L[5], 4) + " ";
      if (To_O) { T += String(Target_L[5], 4) + " "; }
      if (K_PID) { T += String(KP[5], 4) + " " + String(KI[5], 4) + " " + String(KD[5], 4) + " "; }
    }

    appendFile(SD, "/record_data.txt", T + "\n");
  }
}

void setup() {
  Serial.begin(19200);
  ss.begin(19200);
  Wire.begin(17, 16);  // (sda, scl)

  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  while (ERR_OK != sensor.begin()) {  //BMP390L 啟動加偵錯
    Serial.println(F("Error!!"));
    delay(1000);
  }
  sensor.setSamplingMode(sensor.eNormalPrecision2);
  
  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }


  mpu_SUp();

  esp_now_register_recv_cb(OnDataRecv);
  for (unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {
    ss.write(pgm_read_byte(UBLOX_INIT + i));
  }

  //ESC1.attach(32);               //屬於電調 1 腳位
  //ESC2.attach(26);               //屬於電調 2 腳位
  //ESC3.attach(25);               //屬於電調 3 腳位
  //ESC4.attach(33);               //屬於電調 4 腳位
  //ESC1.writeMicroseconds(OUT1);  //電調 1 的 初始(範圍1000~2000之間，2000最大)
  //ESC2.writeMicroseconds(OUT1);
  //ESC3.writeMicroseconds(OUT1);
  //ESC4.writeMicroseconds(OUT1);

  pinMode(ESC1_PIN, OUTPUT);
  pinMode(ESC2_PIN, OUTPUT);
  pinMode(ESC3_PIN, OUTPUT);
  pinMode(ESC4_PIN, OUTPUT);
  ESC1.arm();
  ESC2.arm();
  ESC3.arm();
  ESC4.arm();

  float SetTimes = 100;               //拿幾次數據做平均
  for (int i = 0; i < SetTimes; i++)  //初始地磁方向
  {
    if (mpu.update()) {
      AX = mpu.getAccX();
      AY = mpu.getAccY();
      AZ = mpu.getAccZ();
      MX = mpu.getMagX();
      MY = mpu.getMagY();
      MZ = mpu.getMagZ();
    }

    float MXf = (MX - MX_init) * 0.707 + (MY - MY_init) * 0.707;
    float MYf = (MX - MX_init) * 0.707 - (MY - MY_init) * 0.707;

    Target_L[3] += atan2(-MYf, MXf);
    Target_L[2] += sensor.readAltitudeM();

    GVT += sqrt((AZ-AZ_init) * (AZ-AZ_init) + (AY-AY_init) * (AY-AY_init) + (AX-AX_init) * (AX-AX_init));
    mpu.update();
    delay(5);
  }
  GVT /= SetTimes;
  Target_L[2] /= SetTimes;
  Target_L[3] /= SetTimes;
  L[2] = Target_L[2];
  L[3] = Target_L[3];
  Altitude = sensor.readAltitudeM();
  last_Altitude = Altitude;


  bool isSS = true;
  while (isSS) {
    while (ss.available()) {
      isSS = false;
      float j = ss.read();
      //Serial.write(j);
    }
  }
  Serial.println("Finish2");

  nowTime = millis();
  totalTime = millis();
  control_delta_Time = millis();
  tTime = millis();

  while (1) {
    if (!SD.begin()) {
      Serial.println("Card Mount Failed");
      delay(1000);
    } else {
      break;
    }
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  xTaskCreatePinnedToCore(SD_Record, "SD_Sent_Task", 10000, NULL, 0, &SD_Sent_Task, 0);

  UseGPS = false;
  SearchGPS = false;
}

int app = 0;
bool is_Mag_s = false;
bool start_Up = false;
bool start_rise_1_p_5m = false;
float init_rise;
bool start_set = false;

void loop() {
  //------------------------------------------讀取
  if (millis() - control_delta_Time >= 50) {
    OUT1 += float(myData.up_and_down) / 2.5;
    Target_L[0] = float(myData.foward_and_backward) * 1.2;
    Target_L[1] = -float(myData.left_and_right) * 1.2;
    Target_L[2] += float(myData.altitude) * 0.005;
    Target_L[3] += float(myData.turn) * 0.01;
    if (start_Up == false && myData.Start_Up == true) {
      start_Up = true;
      start_rise_1_p_5m = true;
      init_rise = L[2];
    } else if (start_Up == true && myData.Start_Up == false) {
      start_Up = false;
      start_rise_1_p_5m = false;
      init_rise = 0;
    }

    for (int i = 0; i < 6; i++) {
      if (Target_L_Last[i] != Target_L[i]) {
        Target_L_Last[i] = Target_L[i];
        L_last[i] = Target_L[i] - L[i];
      }
    }

    OUT1 = OUT1 < 1000 ? 1000 : OUT1;
    control_delta_Time += 50;
  }

  //------------------------------------------讀取
  if (mpu.update()) {
    app += 1;
    save_take -= 1;
    if (save_take <= 0) {
      Target_L[2] -= 0.002;
      save_take = 0;
    }
    AX = mpu.getAccX();
    AY = mpu.getAccY();
    AZ = mpu.getAccZ();
    GX = mpu.getGyroX();
    GY = mpu.getGyroY();
    GZ = mpu.getGyroZ();
    MX = mpu.getMagX();
    MY = mpu.getMagY();
    MZ = mpu.getMagZ();
    if (AX == 0 && AY == 0 && AZ == 0) {
      while (1) {
        mpu_SUp();
        delay(10);
        if (mpu.update()) {
          if (mpu.getAccZ() != 0) {
            break;
          }
        }
      }
      AX = mpu.getAccX();
      AY = mpu.getAccY();
      AZ = mpu.getAccZ();
      GX = mpu.getGyroX();
      GY = mpu.getGyroY();
      GZ = mpu.getGyroZ();
      MX = mpu.getMagX();
      MY = mpu.getMagY();
      MZ = mpu.getMagZ();
    }
    float deltaTime = (millis() - nowTime) * 0.001;  //一次循環幾秒
    nowTime = millis();

    //------------------------------------------X,Y軸角度(X(0,1),(2,3),Y(1,2),(3,0))
    //因為我無人機MPU9250擺放的位置不太對，所以陀螺儀、加速度計、磁力計都要旋轉到側邊
    float GXf = ((GX - GX_init) * 0.707 - (GY - GY_init) * 0.707); //旋轉到側邊//旋轉陀螺儀數值
    float GYf = ((GX - GX_init) * 0.707 + (GY - GY_init) * 0.707); //旋轉到側邊

    float AXf = (AX-AX_init) * 0.707 - (AY-AY_init) * 0.707; //旋轉到側邊//旋轉加速度計數值
    float AYf = (AX-AX_init) * 0.707 + (AY-AY_init) * 0.707; //旋轉到側邊
    
    //濾波
    X_T = 0.995 * (X_T + GXf * deltaTime) + 0.005 * Degree(AYf , AXf , (AZ-AZ_init));
    Y_T = 0.995 * (Y_T + GYf * deltaTime) + 0.005 * Degree(-AXf , AYf , (AZ-AZ_init));
    L[0] += (X_T - L[0]) * (1 - exp(-(X_T - L[0]) * (X_T - L[0]) / 0.05));
    L[1] += (Y_T - L[1]) * (1 - exp(-(Y_T - L[1]) * (Y_T - L[1]) / 0.05));

    //------------------------------------------高度(氣壓計)
    float Xr = abs(AYf * sin(L[0] * deg_to_rad));
    float Yr = abs(AXf * sin(L[1] * deg_to_rad));
    float Zr = (AZ-AZ_init) * cos(L[0] * deg_to_rad) * cos(L[1] * deg_to_rad);
    float VA = Xr + Yr + Zr;  //垂直軸加速度

    readAltitude = sensor.readAltitudeM();
    
    if (abs(Altitude - readAltitude) < 100) {
      Altitude += (readAltitude - Altitude) * (1 - exp(-(readAltitude - Altitude) * (readAltitude - Altitude) / 0.8));
    }
    //Serial.printf("%f, %f, %f, %f, %f\n", Xr, Yr, Zr, VA, GVT);
    V_Ar[0] = (VA - GVT) * 12;  //因為氣壓計延遲，所以讓0.15秒之前的加速度計累加值去跟氣壓計斜率互補(兩著為二次微分關係，ex:加速度與距離)。
    if(V_Ar[0] - V_Ar[1] > 12){
      V_Ar[0] = V_Ar[1];
    }
    for(int i = 14; i > 0;i--){
      V_Ar[i] = V_Ar[i-1];
    }

    ascent_speed = 0.98 * (ascent_speed + V_Ar[14] * deltaTime) + 0.02 * (Altitude - last_Altitude) / deltaTime;

    L[2] = 0.995 * (L[2] + ascent_speed * deltaTime) + 0.005 * Altitude;  //Altitude 校準值、ascent_speed 累加值
    //Serial.printf("%f, %f\n", L[2]*10,  readAltitude*10);

    last_Altitude = Altitude;
    
    //------------------------------------------1.23ms 陀螺儀(-180~180,0是北)
    //要做的事 -> 要升起時再target
    float MXf = (MX - MX_init) * 0.707 + (MY - MY_init) * 0.707;
    float MYf = (MX - MX_init) * 0.707 - (MY - MY_init) * 0.707;

    //float L[2] = atan2(-MY, MX) * rad_to_deg;           //指南針(只適用於水平面)
    float pitch = L[0] * deg_to_rad;  //pitch
    float roll = L[1] * deg_to_rad;   //roll
    float accX = sin(pitch);
    float accY = sin(roll);
    float Xh = MXf * cos(pitch) + MYf * sin(roll) * accX + (MZ - MZ_init) * cos(roll) * accX;
    float Yh = MYf * cos(roll) + (MZ - MZ_init) * sin(roll);  //XY軸磁力計的傾斜補償
    float sb = atan2(-Yh, Xh);                                //yaw

    if (Target_L[3] < -3.14) { Target_L[3] += 6.283;}  //限制範圍
    else if (Target_L[3] > 3.14) { Target_L[3] -= 6.283;}

    if (Target_L[3] - sb > 3.14) { sb += 6.28;}  //最近的目標點
    else if (Target_L[3] - sb < -3.14) { sb -= 6.28;}

    if (L_last[3] - 3.14 > Target_L[3] - sb) { L_last[3] -= 6.28; }  //避免KI太大
    else if (L_last[3] + 3.14 < Target_L[3] - sb) { L_last[3] += 6.28;}

    if (sb - L[3] >= 3.14) { L[3] += 6.283; }  //L[3]與sb差過大校準
    else if (sb - L[3] <= -3.14) { L[3] -= 6.283;}

    if (is_Mag_s == false) {
      L[3] = sb;
      is_Mag_s = true;
      Serial.print("er");
    } else {
      L[3] += 0.85 * ((GZ - GZ_init) / 14000) + 0.15 * (sb - L[3]) * (1 - exp(-(sb - L[3]) * (sb - L[3]) / 0.06));
    }

    //------------------------------------------GPS  //樓上陽台面相門再往右邊轉一點，這樣就會面向y軸
    //角度=加速度，加速度累加=速度，but運行久後會逐漸錯
    //GPS=位置，位置之差=速度，此速度值是精確但很浮動的
    if (millis() - totalTime >= 100) {
      totalTime = millis();

      p = "";
      float GPS_S = 0;
      if (ByteArray_Copy[1] == 'G' && ByteArray_Copy[2] == 'N') {
        for (int i = 1; i < 80; i++) {
          if (ByteArray_Copy[i] == ',') {
            if (GPS_S == 2) {
              bool ok = true;
              for(int j = 0; j < p.length(); j++)
                if((isDigit(p[j]) == false) && (p[j] != '.'))
                  ok = false;

              if(ok) //緯度讀取
                GPSX_value = p.substring(5, p.length()).toInt() + p.substring(0, 4).toInt()*100000;
              
            }  
            else if (GPS_S == 4) {
              bool ok = true;
              for(int j = 0; j < p.length(); j++)
                if((isDigit(p[j]) == false) && (p[j] != '.'))
                  ok = false;

              if(ok) //經度讀取
                GPSY_value = p.substring(5, p.length()).toInt() + p.substring(0, 4).toInt()*100000;

            } 
            else if (GPS_S == 9) {
              bool ok = true;
              for(int j = 0; j < p.length(); j++)
                if((isDigit(p[j]) == false) && (p[j] != '.'))
                  ok = false;
              
              if(ok) {//海拔高度讀取
                GPSH = p.substring(0, 7).toFloat();
                if (ControwByAltitude && L[2] != 0 && GPSH != 0) {
                  Component_Altitude_ERROR = (L[2] - GPSH);
                }
              }

            }  
            else if (GPS_S == 10) { break;}
            GPS_S += 1;
            p = "";
          } else {
            p += ByteArray_Copy[i];
          }
        }
      }

      if (GPSX_value != 0 && GPSY_value != 0) {
        if (SearchGPS == false) {  //初始值設定，並且沒收到訊，或是太多錯誤，過久後會啟動
          Target_R_4 = GPSX_value;
          Target_R_5 = GPSY_value;
          Target_L[4] = Target_L[5] = 0;
          Target_L_Last[4] = Target_L_Last[5] = 0;
          GPSX_Get = GPSY_Get = 0;
          GPSX = GPSY = 0;
          SearchGPS = true;
        }
        if (SearchGPS){
          GPSX_Get_Last = GPSX_Get;    
          GPSY_Get_Last = GPSY_Get;
          GPSX_Get = float(GPSX_value - Target_R_4)*0.01;
          GPSY_Get = float(GPSY_value - Target_R_5)*0.01;

          if (abs(GPSX_Get_Last - GPSX_Get) > 1 || abs(GPSY_Get_Last - GPSY_Get) > 1) {
            GPS_Count += 1;
            GPSX_Get = GPSX_Get_Last;
            GPSY_Get = GPSY_Get_Last;
          }else{
            GPS_Count = 0;
          }
        }
      } else {
        GPS_Count += 1;
      }

      //太久沒有正確數值，重來一次
      if (GPS_Count >= 20) {
        SearchGPS = false;
      }

      //Serial.print(String(Target_R_4h)+"."+String(Target_R_4));
      //Serial.print(" , ");
      //Serial.print(String(Target_R_5h)+"."+String(Target_R_5));
      //Serial.print(" , ");
      //Serial.println(ByteArray_Copy);
    }
    if (SearchGPS == true) {
      float frac_GPS = float(millis() - totalTime)*0.01;
      //各個地區的經緯度變化都不一樣
      float OutGPSX = (GPSX_Get_Last * (1-frac_GPS) + GPSX_Get * frac_GPS) * 1.03;
      float OutGPSY = (GPSY_Get_Last * (1-frac_GPS) + GPSY_Get * frac_GPS) * 1.11;
      float cosL3 = cos(L[3]);
      float sinL3 = sin(L[3]);
      GPSX_Last = GPSX;
      GPSY_Last = GPSY;
      GPSX = -OutGPSX * cosL3 + OutGPSY * sinL3;
      GPSY = OutGPSX * sinL3 + OutGPSY * cosL3;

      GPSX_s = 0.96 * (GPSX_s + accX * deltaTime * 100) + 0.04 * ((GPSX - GPSX_Last) / deltaTime);  //速度濾波
      GPSY_s = 0.96 * (GPSY_s + accY * deltaTime * 100) + 0.04 * ((GPSX - GPSX_Last) / deltaTime);

      L[4] = 0.98 * (L[4] + GPSX_s * deltaTime) + 0.02 * GPSX;  //位置濾波
      L[5] = 0.98 * (L[5] + GPSY_s * deltaTime) + 0.02 * GPSY;
      
      test_aMix[0] = accY * deltaTime * 100;
      test_aMix[1] = (GPSY - GPSY_Last) / deltaTime;
    }

    //------------------------------------------開始飛行
    //起飛到 1.5m 的位置
    if (start_rise_1_p_5m == true) {
      if (L[2] - init_rise < 0.5) {
        if (OUT1 < 1500) {
          OUT1 += 0.4;
        }
      }
      Target_L[2] = init_rise + 1.5;
      L_last[2] = Target_L[2] - L[2];
      if (L[2] - init_rise > 1) {
        start_rise_1_p_5m = false;
      }
    } else if (start_Up == false && OUT1 - 1000 > -aMix[2]) {
      Target_L[2] -= 0.001;
      L_last[2] = Target_L[2] - L[2];
    } else if (OUT1 - 1000 <= -aMix[2] ) {
      OUT1 = 1000;
      Target_L[2] = L[2];
      L_last[2] = Target_L[2] - L[2];
    }

    for (int i = 0; i < 6; i++) {        //L[0]~L[5] 分別是 pitch(x),roll(y),海拔,yaw(z),經度,緯度
      float L_dis = Target_L[i] - L[i];  //測量與目標值的差值,這輪與上輪 L_dis 的誤差
      L_sum[i] += L_dis;
      float L_gradient = L_dis - L_last[i];
      if (i != 4 && i != 5) {
        L_last[i] = L_dis;
      }
      aMix[i] = KP[i] * L_dis + KI[i] * L_sum[i] + KD[i] * L_gradient / deltaTime;
    }

    if (false) {  //測試用
      for (int i =0; i < lc; i++){
        int test_i = test_l[i];
        float test_L_dis = Target_L[test_i] - test_L[i];
        test_L_sum[i] += test_L_dis;
        float test_L_gradient = test_L_dis - test_L_last[i];
        test_L_last[i] = test_L_dis;
        test_aMix[i] = KP[test_i] * test_L_dis + KI[test_i] * test_L_sum[i] + KD[test_i] * test_L_gradient / deltaTime;
      }
    }

    //快摔倒時停止
    if (abs(L[0]) > 40 || abs(L[1]) > 40) {
      OUT1 = 1000;
    }

    //PID
    OUT2[0] = OUT1 + a[0] - aMix[0] - aMix[1] - aMix[3] + aMix[2];
    OUT2[1] = OUT1 + a[1] + aMix[0] - aMix[1] + aMix[3] + aMix[2];
    OUT2[2] = OUT1 + a[2] + aMix[0] + aMix[1] - aMix[3] + aMix[2];
    OUT2[3] = OUT1 + a[3] - aMix[0] + aMix[1] + aMix[3] + aMix[2];
    //Serial.printf("%f, %f, %f, %f\n", aMix[0], aMix[1], aMix[2], aMix[3]);
    
    if (OUT1 < 1050) {  //速度太小不運作
      OUT2[0] = 1000;
      OUT2[1] = 1000;
      OUT2[2] = 1000;
      OUT2[3] = 1000;
      for (int i = 0; i < 6; i++) {
        L_sum[i] = 0;
      }
      start_set = true;
    }else if (start_set == true){
      start_set = false;
      Target_L[3] = L[3];
      L_last[3] = Target_L[3] - L[3];
    }

    int maxPower = 1700;
    //輸出給馬達訊號
    if (OUT2[0] < maxPower && OUT2[1] < maxPower && OUT2[2] < maxPower && OUT2[3] < maxPower) {
      //ESC1.writeMicroseconds(OUT2[0]);
      //ESC2.writeMicroseconds(OUT2[1]);
      //ESC3.writeMicroseconds(OUT2[2]);
      //ESC4.writeMicroseconds(OUT2[3]);
      ESC1.speed(OUT2[0]);
      ESC2.speed(OUT2[1]);
      ESC3.speed(OUT2[2]);
      ESC4.speed(OUT2[3]);
    }
  }


}
