#include "esp_camera.h"
#include <WiFi.h>
#include <Wire.h>
#include <base64.h>
#include <esp_now.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

static const int INPUT_W = 320;    //圖片寬度
static const int INPUT_H = 240;    //圖片長度
static const int SLICE_TIMES = 26; 
static const int MAX_ST = SLICE_TIMES * SLICE_TIMES; //分成26*26個區塊
static const int BLOCK_SIZE = 7;   //harris 角點分數範圍
static const float HARRIS_K = 0.04;
static const int PATCH_SIZE = 31;  //BRIEF範圍
static const int HALF_PATCH_SIZE = PATCH_SIZE / 3;
static const int MATCHING_FILTER = 24; //BRIEF的128個配對，錯誤數不能超過此範圍
static const int STP = 15; //不FAST檢測邊緣周圍STP個像素點
static const int MAX_CONNECT = 80;

const int plac[8][2] = { {0,0},{0,INPUT_H},{INPUT_W,0},{INPUT_W,INPUT_H},{0,0},{0,INPUT_H},{INPUT_W,0},{INPUT_W,INPUT_H}};
float CAMERA_INTRINSICS[3][3] = { { 275.5, 0, INPUT_W / 2 }, { 0, 275.5, INPUT_H / 2 }, { 0, 0, 1 } };
float inv_CAMERA_INTRINSICS[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

typedef struct struct_message {
  int ts = 0;
  int connectTimes = 0;
  int t = 0;
  float loss = 0;
  float a[3] = {0};
  float tr[4] = {0};
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// 發送數據時的回傳
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//接收端的ESP MAC地址
uint8_t broadcastAddress[] = {0xE0, 0x5A, 0x1B, 0x6B, 0x40, 0x44};  

//Keypoint 結構，裡面有座標、角度、harris分數、BRIED描述子群資訊
struct Keypoint {
  float angle = 0;
  float response = 0;
  int axis[2] = { 0 };
  uint8_t descriptors[16] = { 0 };
  bool index = false;
};

struct SaveKnP {
    int b_X = 0;
    int b_Y = 0;
    int n_X = 0;
    int n_Y = 0;
};

//BRIEF128個組座標
static const int8_t bit_pattern_31_[128 * 4] = {
  8,
  -3,
  9,
  5 /*mean (0), correlation (0)*/,
  4,
  2,
  7,
  -12 /*mean (1.12461e-05), correlation (0.0437584)*/,
  -11,
  9,
  -8,
  2 /*mean (3.37382e-05), correlation (0.0617409)*/,
  7,
  -12,
  5,
  -13 /*mean (5.62303e-05), correlation (0.0636977)*/,
  2,
  -13,
  2,
  12 /*mean (0.000134953), correlation (0.085099)*/,
  1,
  -7,
  1,
  6 /*mean (0.000528565), correlation (0.0857175)*/,
  -2,
  -10,
  -2,
  -4 /*mean (0.0188821), correlation (0.0985774)*/,
  -9,
  -7,
  -11,
  -8 /*mean (0.0363135), correlation (0.0899616)*/,
  -13,
  -3,
  -12,
  -9 /*mean (0.121806), correlation (0.099849)*/,
  10,
  4,
  11,
  9 /*mean (0.122065), correlation (0.093285)*/,
  -13,
  -8,
  -8,
  -9 /*mean (0.162787), correlation (0.0942748)*/,
  -11,
  7,
  -9,
  12 /*mean (0.21561), correlation (0.0974438)*/,
  7,
  7,
  12,
  6 /*mean (0.160583), correlation (0.130064)*/,
  -4,
  -5,
  -3,
  0 /*mean (0.228171), correlation (0.132998)*/,
  -13,
  2,
  -12,
  -3 /*mean (0.00997526), correlation (0.145926)*/,
  -9,
  0,
  -7,
  5 /*mean (0.198234), correlation (0.143636)*/,
  12,
  -6,
  12,
  -1 /*mean (0.0676226), correlation (0.16689)*/,
  -3,
  6,
  -2,
  12 /*mean (0.166847), correlation (0.171682)*/,
  -6,
  -13,
  -4,
  -8 /*mean (0.101215), correlation (0.179716)*/,
  9,
  -11,
  12,
  -8 /*mean (0.200641), correlation (0.192279)*/,
  4,
  7,
  5,
  1 /*mean (0.205106), correlation (0.186848)*/,
  5,
  -3,
  10,
  -3 /*mean (0.234908), correlation (0.192319)*/,
  3,
  -7,
  6,
  12 /*mean (0.0709964), correlation (0.210872)*/,
  -8,
  -7,
  -6,
  -2 /*mean (0.0939834), correlation (0.212589)*/,
  -2,
  11,
  -1,
  -10 /*mean (0.127778), correlation (0.20866)*/,
  -7,
  6,
  -8,
  10 /*mean (0.14783), correlation (0.206356)*/,
  -7,
  3,
  -5,
  -3 /*mean (0.182141), correlation (0.198942)*/,
  -4,
  2,
  -3,
  7 /*mean (0.188237), correlation (0.21384)*/,
  -10,
  -12,
  -6,
  11 /*mean (0.14865), correlation (0.23571)*/,
  5,
  -12,
  6,
  -7 /*mean (0.222312), correlation (0.23324)*/,
  5,
  -6,
  7,
  -1 /*mean (0.229082), correlation (0.23389)*/,
  1,
  9,
  4,
  -5 /*mean (0.241577), correlation (0.215286)*/,
  9,
  11,
  10,
  -10 /*mean (0.00338507), correlation (0.251373)*/,
  4,
  7,
  4,
  12 /*mean (0.131005), correlation (0.257622)*/,
  2,
  -7,
  4,
  4 /*mean (0.152755), correlation (0.255205)*/,
  -4,
  -12,
  -2,
  7 /*mean (0.182771), correlation (0.244867)*/,
  -8,
  -5,
  -7,
  -10 /*mean (0.186898), correlation (0.23901)*/,
  4,
  11,
  7,
  10 /*mean (0.226226), correlation (0.258255)*/,
  0,
  -8,
  1,
  -13 /*mean (0.0897886), correlation (0.274827)*/,
  -13,
  -2,
  -8,
  2 /*mean (0.148774), correlation (0.28065)*/,
  -3,
  -2,
  -2,
  3 /*mean (0.153048), correlation (0.283063)*/,
  -6,
  9,
  -4,
  -9 /*mean (0.169523), correlation (0.278248)*/,
  8,
  12,
  10,
  7 /*mean (0.225337), correlation (0.282851)*/,
  0,
  9,
  1,
  3 /*mean (0.226687), correlation (0.278734)*/,
  7,
  -5,
  11,
  -10 /*mean (0.00693882), correlation (0.305161)*/,
  -13,
  -6,
  -11,
  0 /*mean (0.0227283), correlation (0.300181)*/,
  10,
  7,
  12,
  1 /*mean (0.125517), correlation (0.31089)*/,
  -6,
  -3,
  -6,
  12 /*mean (0.131748), correlation (0.312779)*/,
  10,
  -9,
  12,
  -4 /*mean (0.144827), correlation (0.292797)*/,
  -10,
  6,
  -8,
  -12 /*mean (0.149202), correlation (0.308918)*/,
  -13,
  0,
  -8,
  -4 /*mean (0.160909), correlation (0.310013)*/,
  3,
  3,
  7,
  8 /*mean (0.177755), correlation (0.309394)*/,
  5,
  7,
  10,
  -7 /*mean (0.212337), correlation (0.310315)*/,
  -1,
  7,
  1,
  -12 /*mean (0.214429), correlation (0.311933)*/,
  3,
  -10,
  5,
  6 /*mean (0.235807), correlation (0.313104)*/,
  2,
  -4,
  3,
  -10 /*mean (0.00494827), correlation (0.344948)*/,
  -13,
  0,
  -13,
  5 /*mean (0.0549145), correlation (0.344675)*/,
  -13,
  -7,
  -4,
  12 /*mean (0.103385), correlation (0.342715)*/,
  -13,
  3,
  -11,
  8 /*mean (0.134222), correlation (0.322922)*/,
  -7,
  12,
  -4,
  7 /*mean (0.153284), correlation (0.337061)*/,
  6,
  -10,
  12,
  8 /*mean (0.154881), correlation (0.329257)*/,
  -9,
  -1,
  -7,
  -6 /*mean (0.200967), correlation (0.33312)*/,
  -2,
  -5,
  0,
  12 /*mean (0.201518), correlation (0.340635)*/,
  -12,
  5,
  -7,
  5 /*mean (0.207805), correlation (0.335631)*/,
  3,
  -10,
  8,
  -13 /*mean (0.224438), correlation (0.34504)*/,
  -7,
  -7,
  -4,
  5 /*mean (0.239361), correlation (0.338053)*/,
  -3,
  -2,
  -1,
  -7 /*mean (0.240744), correlation (0.344322)*/,
  2,
  9,
  5,
  -11 /*mean (0.242949), correlation (0.34145)*/,
  -5,
  -13,
  -5,
  -13 /*mean (0.244028), correlation (0.336861)*/,
  -1,
  6,
  7,
  -2 /*mean (0.247571), correlation (0.343684)*/,
  5,
  -3,
  5,
  2 /*mean (0.000697256), correlation (0.357265)*/,
  -4,
  -13,
  -4,
  12 /*mean (0.00213675), correlation (0.373827)*/,
  -9,
  -6,
  -9,
  6 /*mean (0.0126856), correlation (0.373938)*/,
  -12,
  -10,
  -8,
  -4 /*mean (0.0152497), correlation (0.364237)*/,
  10,
  2,
  12,
  -3 /*mean (0.0299933), correlation (0.345292)*/,
  7,
  12,
  4,
  12 /*mean (0.0307242), correlation (0.366299)*/,
  -7,
  -13,
  -6,
  5 /*mean (0.0534975), correlation (0.368357)*/,
  -4,
  9,
  -3,
  4 /*mean (0.099865), correlation (0.372276)*/,
  7,
  -1,
  12,
  2 /*mean (0.117083), correlation (0.364529)*/,
  -7,
  6,
  -5,
  1 /*mean (0.126125), correlation (0.369606)*/,
  -3,
  11,
  -12,
  5 /*mean (0.130364), correlation (0.358502)*/,
  -3,
  7,
  -2,
  -6 /*mean (0.131691), correlation (0.375531)*/,
  7,
  -8,
  12,
  -7 /*mean (0.160166), correlation (0.379508)*/,
  -8,
  -7,
  -7,
  11 /*mean (0.167848), correlation (0.353343)*/,
  1,
  -3,
  5,
  10 /*mean (0.183378), correlation (0.371916)*/,
  2,
  -6,
  3,
  0 /*mean (0.228711), correlation (0.371761)*/,
  -4,
  3,
  -2,
  -13 /*mean (0.247211), correlation (0.364063)*/,
  -1,
  -13,
  1,
  9 /*mean (0.249325), correlation (0.378139)*/,
  7,
  1,
  8,
  -6 /*mean (0.000652272), correlation (0.411682)*/,
  3,
  -5,
  3,
  12 /*mean (0.00248538), correlation (0.392988)*/,
  9,
  1,
  12,
  6 /*mean (0.0206815), correlation (0.386106)*/,
  -1,
  -9,
  -1,
  3 /*mean (0.0364485), correlation (0.410752)*/,
  -6,
  -8,
  -10,
  5 /*mean (0.0376068), correlation (0.398374)*/,
  7,
  7,
  10,
  12 /*mean (0.0424202), correlation (0.405663)*/,
  12,
  -5,
  12,
  9 /*mean (0.0942645), correlation (0.410422)*/,
  6,
  3,
  7,
  11 /*mean (0.1074), correlation (0.413224)*/,
  5,
  -13,
  6,
  10 /*mean (0.109256), correlation (0.408646)*/,
  2,
  -12,
  2,
  3 /*mean (0.131691), correlation (0.416076)*/,
  3,
  8,
  4,
  -6 /*mean (0.165081), correlation (0.417569)*/,
  2,
  6,
  1,
  -13 /*mean (0.171874), correlation (0.408471)*/,
  9,
  -12,
  10,
  3 /*mean (0.175146), correlation (0.41296)*/,
  -8,
  4,
  -7,
  9 /*mean (0.183682), correlation (0.402956)*/,
  -11,
  6,
  -4,
  -6 /*mean (0.184672), correlation (0.416125)*/,
  1,
  12,
  2,
  -8 /*mean (0.191487), correlation (0.386696)*/,
  6,
  -9,
  7,
  -4 /*mean (0.192668), correlation (0.394771)*/,
  2,
  3,
  3,
  -2 /*mean (0.200157), correlation (0.408303)*/,
  6,
  3,
  11,
  0 /*mean (0.204588), correlation (0.411762)*/,
  3,
  -3,
  8,
  -8 /*mean (0.205904), correlation (0.416294)*/,
  7,
  8,
  9,
  3 /*mean (0.213237), correlation (0.409306)*/,
  -11,
  -5,
  -6,
  -4 /*mean (0.243444), correlation (0.395069)*/,
  -10,
  11,
  -5,
  10 /*mean (0.247672), correlation (0.413392)*/,
  -5,
  -8,
  -3,
  12 /*mean (0.24774), correlation (0.411416)*/,
  -10,
  5,
  -9,
  0 /*mean (0.00213675), correlation (0.454003)*/,
  8,
  -1,
  12,
  -6 /*mean (0.0293635), correlation (0.455368)*/,
  4,
  -6,
  6,
  -11 /*mean (0.0404971), correlation (0.457393)*/,
  -10,
  12,
  -8,
  7 /*mean (0.0481107), correlation (0.448364)*/,
  4,
  -2,
  6,
  7 /*mean (0.050641), correlation (0.455019)*/,
  -2,
  8,
  -2,
  12 /*mean (0.0525978), correlation (0.44338)*/,
  -5,
  -8,
  -5,
  2 /*mean (0.0629667), correlation (0.457096)*/,
  7,
  -6,
  10,
  12 /*mean (0.0653846), correlation (0.445623)*/,
  -9,
  -13,
  -8,
  -8 /*mean (0.0858749), correlation (0.449789)*/,
  -5,
  -13,
  -5,
  -2 /*mean (0.122402), correlation (0.450201)*/,
  8,
  -8,
  9,
  -13 /*mean (0.125416), correlation (0.453224)*/,
  -9,
  -11,
  -9,
  0 /*mean (0.130128), correlation (0.458724)*/,
  1,
  -8,
  -6,
  -4 /*mean (0.132467), correlation (0.440133)*/,
  7,
  -4,
  9,
  1 /*mean (0.132692), correlation (0.454)*/,
  -2,
  10,
  -1,
  -4 /*mean (0.135695), correlation (0.455739)*/,
  9,
  -6,
  5,
  11 /*mean (0.142904), correlation (0.446114)*/,
};
//讓BRIEF更好去找值
struct Pattern {
  int8_t x = 0;
  int8_t y = 0;
};

#define CAMERA_MODEL_AI_THINKER

//浮點常數的限制
#define FLT_MIN 1.175494351e-38F
#define FLT_EPSILON 1.192092896e-07F
#define DBL_EPSILON 2.2204460492503131E-16
#define CV_PI 3.1415926535897932384626433832795

#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "Rightoo";           //Rightoo ASUS
const char *password = "123457899";  //123457899 123456789


int k_size_3[16][2] = {
  { 0, 3 },
  { 1, 3 },
  { 2, 2 },
  { 3, 1 },
  { 3, 0 },
  { 3, -1 },
  { 2, -2 },
  { 1, -3 },
  { 0, -3 },
  { -1, -3 },
  { -2, -2 },
  { -3, -1 },
  { -3, 0 },
  { -3, 1 },
  { -2, 2 },
  { -1, 3 },
};

uint8_t threshold_tab[512];
float scale_sq_sq;
int ofs[BLOCK_SIZE * BLOCK_SIZE] = { 0 };
int umax[10 + 2] = { 0 };
int framTo[2] = { 0, 1 };
int pixel[25];

long int time_s;

Pattern _pattern[256];

float fastatan2(float y, float x);
int cornerScore(uint8_t *, int[], int);
void gaussianBlur1(uint8_t *input, int H, int W);
void gaussianBlur2(uint8_t *input, int H, int W);
void connect_keypoint(struct Keypoint, int *, float, bool);
void initKeyPoint();
void findKeyPoint(SaveKnP * saveKnP);
int svd(float *matSrc, float *matD, float *matU, float *matVt, const int tm, const int tn);
void jacobiSVD(float *At, float *_W, float *Vt, const int m, const int n);
bool reconstructH1(float *H21);
void inv(float *In_mat, float *Out_mat, const int n);
void matMult(float *In_mat1, float *In_mat2, float *Out_mat, const int m, const int n, const int o);

void startCameraServer();

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  //ESPNOW相關
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;    //320x240
  //config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.pixel_format = PIXFORMAT_GRAYSCALE;  //灰階模式
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    }
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_special_effect(s, 2);
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

//連接至網路wifi
/*
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
*/

  //與 fast 算法相關
  for (int i = -255; i <= 255; i++) {
    threshold_tab[i + 255] = (uint8_t)(i < -10 ? 1 : i > 10 ? 2
                                                            : 0);
  }

  //與 harris 算法相關
  for (int i = 0; i < BLOCK_SIZE; i++) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
      ofs[i * BLOCK_SIZE + j] = i * INPUT_W + j;
    }
  }
  float scale = 1.f / (4 * BLOCK_SIZE * 255.f);
  scale_sq_sq = scale * scale * scale * scale;

  //與 IC_Angel 算法相關，計算出 umax 為主軸，尋找一個圓的?層y軸有多少框框在此圓中
  //ex:半徑為15的圓，當y=1時，有15個框框在此圓以內，當y=14時，有3個框框在此圓以內
  int v, v0, vmax = floor(HALF_PATCH_SIZE * sqrt(2.f) / 2 + 1);
  int vmin = ceil(HALF_PATCH_SIZE * sqrt(2.f) / 2);
  for (v = 0; v <= vmax; ++v) {
    umax[v] = round(sqrt((double)HALF_PATCH_SIZE * HALF_PATCH_SIZE - v * v));
  }
  for (int v = HALF_PATCH_SIZE, v0 = 0; v >= vmin; --v) {
    while (umax[v0] == umax[v0 + 1]) {
      ++v0;
    }
    umax[v] = v0;
    ++v0;
  }

  //在以3為半徑上共有16個點，但是為了方便25後面8個是最前面的8個
  for (int i = 0; i < 16; i++) {
    pixel[i] = k_size_3[i][0] + k_size_3[i][1] * INPUT_W;
  }
  for (int i = 16; i < 25; i++) {
    pixel[i] = pixel[i - 16];
  }

  for (int i = 0; i < 256; i++) {
    _pattern[i].x = bit_pattern_31_[i * 2];
    _pattern[i].y = bit_pattern_31_[i * 2 + 1];
  }

  delay(1000);
  time_s = millis();
}


//--------------------------------------------------------------------------------------------------------------------------------

float H_A[16][9] = { 0 };
float H_w[9][1] = { 0 };
float H_u[16][16] = { 0 };
float H_vt[9][9] = { 0 };
float H[3][3] = { 0 };
Keypoint keypoint[MAX_ST];
float response_sort[100] =  {0};
long int time1;
camera_fb_t *fb = NULL;
bool is_ti = true;
int previous_connectTimes = 0;
int saveR[MAX_CONNECT] = { 0 };  //紀錄配對結果
int saveD[MAX_CONNECT] = { 0 };  //紀錄每個特徵點相距長度

const int lrud[4][2] = { {0,0} ,{INPUT_W,0} ,{0,INPUT_H} ,{INPUT_W,INPUT_H} };

void loop() {
  is_ti = true;
  for (int i = 0;i < MAX_ST;i++){
    keypoint[i].index = false;
  }
  for (int i = 0;i < 100;i++){
    response_sort[i] = 0;
  }

  fb = esp_camera_fb_get();  //生成照片
  initKeyPoint();
  fb = NULL;   

  int t = 0;
  while(1){
    SaveKnP saveKnP[MAX_CONNECT];
    for (int i = 0; i < MAX_CONNECT; i++) {
      saveR[i] = 0;
      saveD[i] = 0;
    }
    //time1 = millis();
    myData.loss = 0;
    myData.a[0] = 0;
    myData.a[1] = 0;
    myData.a[2] = 0;
    myData.tr[0] = 0;
    myData.tr[1] = 0;
    myData.tr[2] = 0;
    myData.tr[3] = 0;
    myData.connectTimes = 0;
    time1 = micros();

    fb = esp_camera_fb_get();
    findKeyPoint(saveKnP);
    fb = NULL;

    myData.ts = t;  //循環次數
    myData.t = millis() - time1;  //運行時間

    is_ti = is_ti == true ? false : true;
    float dis, BL = 100000;

    if (myData.connectTimes >= 16) {
      time_s = millis();
      for (int j = 0; j < 8; j++) {
        float HTo[3][3] = { 0 };
        for (int k = 0; k < 8; k++) {
          int u1 = saveKnP[k + j].b_X;
          int v1 = saveKnP[k + j].b_Y;
          int u2 = saveKnP[k + j].n_X;
          int v2 = saveKnP[k + j].n_Y;

          H_A[k * 2][0] = u1;
          H_A[k * 2][1] = v1;
          H_A[k * 2][2] = 1;
          H_A[k * 2][3] = 0;
          H_A[k * 2][4] = 0;
          H_A[k * 2][5] = 0;
          H_A[k * 2][6] = -u1 * u2;
          H_A[k * 2][7] = -v1 * u2;
          H_A[k * 2][8] = -u2;

          H_A[k * 2 + 1][0] = 0;
          H_A[k * 2 + 1][1] = 0;
          H_A[k * 2 + 1][2] = 0;
          H_A[k * 2 + 1][3] = -u1;
          H_A[k * 2 + 1][4] = -v1;
          H_A[k * 2 + 1][5] = -1;
          H_A[k * 2 + 1][6] = u1 * v2;
          H_A[k * 2 + 1][7] = v1 * v2;
          H_A[k * 2 + 1][8] = v2;
        }

        svd(*H_A, *H_w, *H_u, *H_vt, 16, 9);
        for (int k = 0; k < 3; k++) {
          for (int l = 0; l < 3; l++) {
            HTo[k][l] = H_vt[8][k * 3 + l] / H_vt[8][8];
          }
        }

        myData.loss = 0;
        for (int k = 0; k < 8; k++) {
          int u1 = saveKnP[k + j].b_X;
          int v1 = saveKnP[k + j].b_Y;
          int u2 = saveKnP[k + j].n_X;
          int v2 = saveKnP[k + j].n_Y;
          dis = (u1 * HTo[2][0] + v1 * HTo[2][1] + 1 * HTo[2][2]);
          myData.loss += pow(u2 - (u1 * HTo[0][0] + v1 * HTo[0][1] + 1 * HTo[0][2]) / dis, 2) + pow(v2 - (u1 * HTo[1][0] + v1 * HTo[1][1] + 1 * HTo[1][2]) / dis, 2);
        }

        if (BL > myData.loss) {
          for (int i = 0; i < 3; i++) {
            for (int k = 0; k < 3; k++) {
              H[i][k] = HTo[i][k];
            }
          }
          BL = myData.loss;
        }
      }
      reconstructH1(*H);
      int outlrud[4][2] = {0};
      for (int k = 0; k < 4; k++) {
        int i = lrud[k][0];
        int j = lrud[k][1];
        dis = (H[2][0] * i + (H[2][1] * j) + (H[2][2] * 1));
        outlrud[k][0] = int(((H[0][0] * i) + (H[0][1] * j) + (H[0][2] * 1)) / dis);
        outlrud[k][1] = int(((H[1][0] * i) + (H[1][1] * j) + (H[1][2] * 1)) / dis);
      }
      myData.tr[0] = -((outlrud[0][0] + outlrud[1][0] + outlrud[2][0] + outlrud[3][0]) - 2 * INPUT_W);
      myData.tr[1] = (outlrud[0][1] + outlrud[1][1] + outlrud[2][1] + outlrud[3][1]) - 2 * INPUT_H;
      myData.tr[2] = (outlrud[1][0] - outlrud[0][0]) - (outlrud[3][0] - outlrud[2][0]);
      myData.tr[3] = -((outlrud[2][1] - outlrud[0][1]) - (outlrud[3][1] - outlrud[1][1]));
    }
    Serial.println("");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));  //ESP32NOW傳送資訊
    t += 1;
    if(millis()-time_s > 1000){
      break;
    }
  }
}

//尋找第一幀特徵點
void initKeyPoint() {
  
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  uint8_t *p_input = fb->buf;    //取得像素點記憶體位置
  
  gaussianBlur1(p_input, INPUT_H, INPUT_W);
  gaussianBlur2(p_input, INPUT_H, INPUT_W);

  int ptidx = 0;
  for (ptidx = 0; ptidx < MAX_ST; ptidx++) {
    keypoint[ptidx].axis[0] = 0;
  }
  int keypointO[SLICE_TIMES][SLICE_TIMES] = { 0 };
  //fast 在圖像中找出特徵點，並且用區域抑制 (尋找鄰域範圍3x3內是否有相同的特徵點、把畫面切成25x25份，限制每份只能有一個特徵點)
  //與角點抑制的方式篩選角點
  previous_connectTimes = 0;
  for (int h = STP; h < INPUT_H - STP; h++) {
    uint8_t *ptr = p_input + h * INPUT_W + STP;
    uint8_t curr = 0;

    for (int w = STP; w < INPUT_W - STP; w++, ptr++) {
      int v = ptr[0];
      uint8_t *tab = &threshold_tab[0] - v + 255;
      int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];
      if (d == 0) {
        continue;
      }

      d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
      d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
      d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];
      if (d == 0) {
        continue;
      }

      d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
      d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
      d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
      d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];
      if (d & 1) {
        int vt = v - 10, count = 0;
        for (int k = 0; k < 25; k++) {
          int x = ptr[pixel[k]];
          if (x < vt) {
            if (++count > 8) {
              curr = (uint8_t)cornerScore(ptr, pixel, 10);
              break;
            }
          } else {
            count = 0;
          }
        }
      }

      if (d & 2) {
        int vt = v + 10, count = 0;
        for (int k = 0; k < 25; k++) {
          int x = ptr[pixel[k]];
          if (x > vt) {
            if (++count > 8) {
              curr = (uint8_t)cornerScore(ptr, pixel, 10);
              break;
            }
          } else {
            count = 0;
          }
        }
      }

      int sh = h * SLICE_TIMES / INPUT_H;
      int sw = w * SLICE_TIMES / INPUT_W;
      if (keypointO[sh][sw] == 0) {
        keypoint[previous_connectTimes].axis[0] = w;
        keypoint[previous_connectTimes].axis[1] = h;
        keypoint[previous_connectTimes].response = (float)curr;
        keypointO[sh][sw] = previous_connectTimes;
        previous_connectTimes += 1;
      } else if (keypoint[keypointO[sh][sw]].response < curr) {
        keypoint[keypointO[sh][sw]].axis[0] = w;
        keypoint[keypointO[sh][sw]].axis[1] = h;
        keypoint[keypointO[sh][sw]].response = (float)curr;
      }
    }
  }

  uint8_t *ptr00 = p_input;
  int r = BLOCK_SIZE / 2;
  //harris 角點分數
  for (ptidx = 0; ptidx < MAX_ST; ptidx++) {
    if (keypoint[ptidx].axis[0] == 0) {
      continue;
    }

    uint8_t *ptr0 = ptr00 + (keypoint[ptidx].axis[1] - r) * INPUT_W + (keypoint[ptidx].axis[0] - r);  //目標點向上r格再向左r格的像素點
    int a = 0, b = 0, c = 0;
    for (int k = 0; k < BLOCK_SIZE * BLOCK_SIZE; k++) {
      uint8_t *ptr = ptr0 + ofs[k];
      int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-INPUT_W + 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[INPUT_W - 1]);
      int Iy = (ptr[INPUT_W] - ptr[-INPUT_W]) * 2 + (ptr[INPUT_W - 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[-INPUT_W + 1]);
      a += Ix * Ix;
      b += Iy * Iy;
      c += Ix * Iy;
    }
    keypoint[ptidx].response = ((float)a * b - (float)c * c - HARRIS_K * ((float)a + b) * ((float)a + b)) * scale_sq_sq * 10000000;
  }
  //harris 排序
  int xt,yt,ts = 0;
  float response;
  for (int i = 0; i < previous_connectTimes; i++) {
    for (int j = i+1; j < previous_connectTimes; j++) {
      if (keypoint[i].response < keypoint[j].response) {
        xt = keypoint[j].axis[0];
        yt = keypoint[j].axis[1];
        response = keypoint[j].response;

        keypoint[j].axis[0] = keypoint[i].axis[0];
        keypoint[j].axis[1] = keypoint[i].axis[1];
        keypoint[j].response = keypoint[i].response;

        keypoint[i].axis[0] = xt;
        keypoint[i].axis[1] = yt;
        keypoint[i].response = response;
      }
    }
    if (i % 7 == 0){
      response_sort[ts] = keypoint[i].response;
      ts++;
    }
  }

  //IC_Angle
  for (ptidx = 0; ptidx < previous_connectTimes; ptidx++) {
    uint8_t *center = p_input + keypoint[ptidx].axis[1] * INPUT_W + keypoint[ptidx].axis[0];

    int m_01 = 0, m_10 = 0;

    for (int u = -10; u <= 10; ++u)
      m_10 += u * center[u];

    for (int v = 1; v <= 10; ++v) {
      int v_sum = 0;
      int d = umax[v];
      for (int u = -d; u <= d; ++u) {
        int val_plus = center[u + v * INPUT_W], val_minus = center[u - v * INPUT_W];
        v_sum += (val_plus - val_minus);
        m_10 += u * (val_plus + val_minus);
      }
      m_01 += v * v_sum;
    }
    keypoint[ptidx].angle = fastatan2((float)m_01, (float)m_10);
  }

  //BRIEF描述子，在特徵點周圍以一定模式隨機取N個點對，將此點對比對大小的組合
  //能使用描述子來尋找兩張照片的特徵點中，哪裡的特徵點相同。
  float x, y;
  int ix, iy;
  for (ptidx = 0; ptidx < previous_connectTimes; ptidx++) {
    float angle = keypoint[ptidx].angle;
    angle *= (float)(3.141592 / 180.f);
    float a = (float)cos(angle), b = (float)sin(angle);

    uint8_t *center = p_input + keypoint[ptidx].axis[0] + keypoint[ptidx].axis[1] * INPUT_W;
    Pattern *pattern = _pattern;
    uint8_t *desc = keypoint[ptidx].descriptors;
    for (int i = 0; i < 16; ++i, pattern += 16)  //每個特徵描述子長度爲8個字節
    {
      int t0, t1, k;
      uint8_t val = 0;
      for (int i = 0; i < 8; i++) {
        k = i * 2;
        x = (pattern[k].x * a - pattern[k].y * b);
        y = (pattern[k].x * b + pattern[k].y * a);
        ix = (int)(x + (x >= 0 ? 0.5f : -0.5f));
        iy = (int)(y + (y >= 0 ? 0.5f : -0.5f));
        t0 = *(center + ix + iy * INPUT_W);

        k++;
        x = (pattern[k].x * a - pattern[k].y * b);
        y = (pattern[k].x * b + pattern[k].y * a);
        ix = (int)(x + (x >= 0 ? 0.5f : -0.5f));
        iy = (int)(y + (y >= 0 ? 0.5f : -0.5f));
        t1 = *(center + ix + iy * INPUT_W);

        val |= (t0 + 2 < t1) << i;
      }
      desc[i] = uint8_t(val);
    }
  }
  esp_camera_fb_return(fb);
}

//尋找特徵點並配對
void findKeyPoint(SaveKnP * saveKnP) {
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  uint8_t *p_input = fb->buf;

  //time1 = micros();
  gaussianBlur1(p_input, INPUT_H, INPUT_W);
  gaussianBlur2(p_input, INPUT_H, INPUT_W);

  time1 = micros();
  int ptidx = 0;
  for (int H = 0; H < SLICE_TIMES; H++) {
    int H_size_Start = (int)((float)INPUT_H / SLICE_TIMES * H);
    int H_size_Stop = (int)((float)INPUT_H / SLICE_TIMES * (H + 1));
    if (H_size_Start < STP) {
      H_size_Start = STP;
      if (H_size_Stop < STP) {
        H_size_Stop = STP;
      }
    }
    if (H_size_Stop >= INPUT_H - STP) {
      H_size_Stop = INPUT_H - STP;
      if (H_size_Start >= INPUT_H - STP) {
        H_size_Start = INPUT_H - STP;
      }
    }
    for (int W = 0; W < SLICE_TIMES; W++) {
      Keypoint S_keypoint;
      int W_size_Start = (int)((float)INPUT_W / SLICE_TIMES * W);
      int W_size_Stop = (int)((float)INPUT_W / SLICE_TIMES * (W + 1));
      if (W_size_Start < STP) {
        W_size_Start = STP;
        if (W_size_Stop < STP) {
          W_size_Stop = STP;
        }
      }
      if (W_size_Stop >= INPUT_W - STP) {
        W_size_Stop = INPUT_W - STP;
        if (W_size_Start >= INPUT_W - STP) {
          W_size_Start = INPUT_W - STP;
        }
      }
      for (int h = H_size_Start; h < H_size_Stop; h++) {
        uint8_t *ptr = p_input + h * INPUT_W + W_size_Start;
        uint8_t curr = 0;
        for (int w = W_size_Start; w < W_size_Stop; w++, ptr++) {
          int v = ptr[0];
          uint8_t *tab = &threshold_tab[0] - v + 255;
          int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

          if (d == 0)
            continue;

          d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
          d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
          d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];
          if (d == 0)
            continue;

          d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
          d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
          d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
          d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];


          if (d & 1) {
            int vt = v - 10, count = 0;
            for (int k = 0; k < 25; k++) {
              int x = ptr[pixel[k]];
              if (x < vt) {
                if (++count > 8) {
                  curr = (uint8_t)cornerScore(ptr, pixel, 10);
                  break;
                }
              } else
                count = 0;
            }
          }

          if (d & 2) {
            int vt = v + 10, count = 0;
            for (int k = 0; k < 25; k++) {
              int x = ptr[pixel[k]];
              if (x > vt) {
                if (++count > 8) {
                  curr = (uint8_t)cornerScore(ptr, pixel, 10);
                  break;
                }
              } else
                count = 0;
            }
          }

          if (S_keypoint.response < curr) {
            S_keypoint.axis[0] = w;
            S_keypoint.axis[1] = h;
            S_keypoint.response = curr;
          }
        }
      }

      if (S_keypoint.axis[0] != 0) {
        //time1 = micros();
        uint8_t *ptr00 = p_input;
        int r = BLOCK_SIZE / 2;
        //harris 角點分數
        for (ptidx = 0; ptidx < 1; ptidx++) {
          uint8_t *ptr0 = ptr00 + (S_keypoint.axis[1] - r) * INPUT_W + (S_keypoint.axis[0] - r);  //目標點向上r格再向左r格的像素點
          int a = 0, b = 0, c = 0;
          for (int k = 0; k < BLOCK_SIZE * BLOCK_SIZE; k++) {
            uint8_t *ptr = ptr0 + ofs[k];
            int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-INPUT_W + 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[INPUT_W - 1]);
            int Iy = (ptr[INPUT_W] - ptr[-INPUT_W]) * 2 + (ptr[INPUT_W - 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[-INPUT_W + 1]);
            a += Ix * Ix;
            b += Iy * Iy;
            c += Ix * Iy;
          }
          S_keypoint.response = ((float)a * b - (float)c * c - HARRIS_K * ((float)a + b) * ((float)a + b)) * scale_sq_sq * 10000000;
        }

        //IC_Angle
        for (ptidx = 0; ptidx < 1; ptidx++) {
          uint8_t *center = p_input + S_keypoint.axis[1] * INPUT_W + S_keypoint.axis[0];

          int m_01 = 0, m_10 = 0;

          for (int u = -10; u <= 10; ++u)
            m_10 += u * center[u];

          for (int v = 1; v <= 10; ++v) {
            int v_sum = 0;
            int d = umax[v];
            for (int u = -d; u <= d; ++u) {
              int val_plus = center[u + v * INPUT_W], val_minus = center[u - v * INPUT_W];
              v_sum += (val_plus - val_minus);
              m_10 += u * (val_plus + val_minus);
            }
            m_01 += v * v_sum;
          }
          S_keypoint.angle = fastatan2((float)m_01, (float)m_10);
        }

        float x, y;
        int ix, iy;
        //brief
        for (ptidx = 0; ptidx < 1; ptidx++) {
          float angle = S_keypoint.angle;
          angle *= (float)(3.141592 / 180.f);
          float a = (float)cos(angle), b = (float)sin(angle);

          uint8_t *center = p_input + S_keypoint.axis[0] + S_keypoint.axis[1] * INPUT_W;
          Pattern *pattern = _pattern;
          uint8_t *desc = S_keypoint.descriptors;
          for (int i = 0; i < 16; ++i, pattern += 16)  //每個特徵描述子長度爲8個字節
          {
            int t0, t1, k;
            uint8_t val = 0;
            for (int i = 0; i < 8; i++) {
              k = i * 2;
              x = (pattern[k].x * a - pattern[k].y * b);
              y = (pattern[k].x * b + pattern[k].y * a);
              ix = (int)(x + (x >= 0 ? 0.5f : -0.5f));
              iy = (int)(y + (y >= 0 ? 0.5f : -0.5f));
              t0 = *(center + ix + iy * INPUT_W);

              k++;
              x = (pattern[k].x * a - pattern[k].y * b);
              y = (pattern[k].x * b + pattern[k].y * a);
              ix = (int)(x + (x >= 0 ? 0.5f : -0.5f));
              iy = (int)(y + (y >= 0 ? 0.5f : -0.5f));
              t1 = *(center + ix + iy * INPUT_W);

              val |= (t0 + 2 < t1) << i;
            }
            desc[i] = uint8_t(val);
          }
        }

        int c5 = myData.connectTimes * 5;
        int keyTo[2] = { 0, 1000 };
        connect_keypoint(S_keypoint, keyTo, 2.5, is_ti);
        if (keyTo[1] < MATCHING_FILTER) {
          if(myData.connectTimes < MAX_CONNECT){
            saveKnP[myData.connectTimes].b_X = keypoint[keyTo[0]].axis[0];
            saveKnP[myData.connectTimes].b_Y = keypoint[keyTo[0]].axis[1];
            saveKnP[myData.connectTimes].n_X = S_keypoint.axis[0];
            saveKnP[myData.connectTimes].n_Y = S_keypoint.axis[1];
            saveR[myData.connectTimes] = keyTo[1];
            saveD[myData.connectTimes] = sqrt((S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) * (S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) +
                                       (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]) * (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]));
            keypoint[keyTo[0]].index = is_ti;
          }else{
            int j[2] = {-1,0};
            for (int i = 0;i < MAX_CONNECT; i++){
              int d5 = i*5;
              if (j[0] < saveR[i] && saveR[i] > keyTo[1]){
                j[0] = saveR[i];
                j[1] = i;
              }
            }
            if(j[0] >= 0){
              saveKnP[myData.connectTimes].b_X = keypoint[keyTo[0]].axis[0];
              saveKnP[myData.connectTimes].b_Y = keypoint[keyTo[0]].axis[1];
              saveKnP[myData.connectTimes].n_X = S_keypoint.axis[0];
              saveKnP[myData.connectTimes].n_Y = S_keypoint.axis[1];
              saveR[myData.connectTimes] = keyTo[1];
              saveD[myData.connectTimes] = sqrt((S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) * (S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) +
                                         (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]) * (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]));
              keypoint[keyTo[0]].index = is_ti;
            }
          }
          myData.connectTimes += 1;
        }
      }
    }
  }

  esp_camera_fb_return(fb);

  for(int i = 0; i < previous_connectTimes;i++){
    keypoint[i].index = is_ti;
  }  
  if (myData.connectTimes > MAX_CONNECT) {
    myData.connectTimes = MAX_CONNECT;
  }

  //對brief配對數進行排序
  for (int i = 0; i < myData.connectTimes; i++) {
    for (int j = i; j < myData.connectTimes; j++) {
      if (saveR[i] > saveR[j]) {
        std::swap(saveKnP[i], saveKnP[j]);
        std::swap(saveR[i], saveR[j]);
        std::swap(saveD[i], saveD[j]);
      }
    }
  }

  float toi = 0, SD = 0;
  for (int i = 0; i < myData.connectTimes; i++) {
    toi += saveD[i];
  }
  if (toi != 0) {
    toi /= myData.connectTimes;
    for (int i = 0; i < myData.connectTimes; i++) {
      SD += (saveD[i] - toi) * (saveD[i] - toi);
    }

    if (SD != 0) {
      SD /= float(myData.connectTimes);
      SD = sqrt(SD);
      int kl = 0;
      for (int p = 0; p < myData.connectTimes; p++) {
        if (saveD[p] > toi + (SD * 1.5 + 1) || saveD[p] < toi - (SD * 1.5 + 1)) {
           saveR[p] = 10000;
           kl += 1;
        }
      }
    }
  }

  float sec, sc;
  int ths;
  //篩選出距離相差最大的8個點，並且排到最前面去優先計算，這樣能夠更精準的求出單應矩陣
  for (int i = 0; i < 8; i++) {
    ths = 0;
    sec = 100000000;
    for (int j = i; j < myData.connectTimes; j++) {
      if (float(saveR[j]) > float(saveR[i]) * 1.5) continue;
      sc = pow(plac[i][0] - saveKnP[j].n_X, 2)*0.9 + pow(plac[i][1] - saveKnP[j].n_Y, 2)*1.6;
      if (sc < sec) {
        ths = j;
        sec = sc;
      }
    }
    if (ths != 0) {
      std::swap(saveKnP[i], saveKnP[ths]);
      std::swap(saveR[i], saveR[ths]);
      std::swap(saveD[i], saveD[ths]);
    }
  }
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
  if (x < 0)  //锐角求出后，根据x和y的正负性确定向量的方向，即角度。
    a = 180.f - a;
  if (y < 0)
    a = 360.f - a;
  return a;
}

//角點分數
int cornerScore(uint8_t *ptr, int pixel[], int threshold) {
  const int K = 8, N = 25;
  int k, v = ptr[0];
  short d[N];
  for (k = 0; k < N; k++)
    d[k] = (short)(v - ptr[pixel[k]]);

  int a0 = threshold;
  for (k = 0; k < 16; k += 2) {
    int a = std::min((int)d[k + 1], (int)d[k + 2]);
    a = std::min(a, (int)d[k + 3]);

    if (a <= a0)
      continue;

    a = std::min(a, (int)d[k + 4]);
    a = std::min(a, (int)d[k + 5]);
    a = std::min(a, (int)d[k + 6]);
    a = std::min(a, (int)d[k + 7]);
    a = std::min(a, (int)d[k + 8]);
    a0 = std::max(a0, std::min(a, (int)d[k]));
    a0 = std::max(a0, std::min(a, (int)d[k + 9]));
  }

  int b0 = -a0;
  for (k = 0; k < 16; k += 2) {
    int b = std::max((int)d[k + 1], (int)d[k + 2]);
    b = std::max(b, (int)d[k + 3]);
    b = std::max(b, (int)d[k + 4]);
    b = std::max(b, (int)d[k + 5]);
    if (b >= b0)
      continue;
    b = std::max(b, (int)d[k + 6]);
    b = std::max(b, (int)d[k + 7]);
    b = std::max(b, (int)d[k + 8]);

    b0 = std::min(b0, std::max(b, (int)d[k]));
    b0 = std::min(b0, std::max(b, (int)d[k + 9]));
  }

  threshold = -b0 - 1;

  return threshold;
}

//高斯模糊水平方向
void gaussianBlur1(uint8_t *input, int H, int W) {
  int i, j, p, s1, s2;
  for (i = 1; i < H - 1; i++) {
    p = i * W;
    s1 = input[p - 1];
    for (j = 1; j < W - 1; j++) {
      p = i * W + j;
      s2 = input[p];
      input[p] = 0.25f * (s1 + input[p + 1]) + 0.5f * input[p];
      s1 = s2;
    }
  }
}

//高斯模糊垂直方向
void gaussianBlur2(uint8_t *input, int H, int W) {
  int i, j, p, s1, s2;
  for (i = 1; i < W - 1; i++) {
    p = i * W;
    s1 = input[p - 1 * W];
    for (j = 1; j < H - 1; j++) {
      p = i + j * W;
      s2 = input[p];
      input[p] = 0.25f * (s1 + input[p + 1 * W]) + 0.5f * input[p];
      s1 = s2;
    }
  }
}

//BRIEF配對
void connect_keypoint(struct Keypoint target, int *keyTo, float range, bool isT) {
  float minR = target.response / range;
  float maxR = target.response * range;
  int ooe = 0;
  for(int j = 0; j < 100; j++){
    if (keypoint[j*7].axis[0] == 0){
      break;
    }

    if (response_sort[j] < minR){
      break;
    }

    if(response_sort[j+1] > maxR){
      continue;
    }

    for (int i = 0; i < 7; i++) {
      int s=j*7+i;
      if (keypoint[s].index == isT){
        continue;
      }
      int answ = 0;
      //tt2 += 1;
      for (int l = 0; l < 16; l++) {
        unsigned char app = target.descriptors[l] ^ keypoint[s].descriptors[l];
        while (app != 0) {
          answ += 1;
          app &= app - 1;
        }
        if (answ > MATCHING_FILTER) break;
      }
      if (answ < keyTo[1]) {
        *keyTo = s;
        *(keyTo + 1) = answ;
      }
    }
  }
}

//奇異值分解
int svd(float *matSrc, float *matD, float *matU, float *matVt, const int tm, const int tn) {
  int m = tm;
  int n = tn;
  bool at = false;
  if (m < n) {
    int s = m;
    m = n;
    n = s;
    at = true;
  }

  float *tmp_u = new float[m * m];
  float *tmp_v = new float[n * n];
  float *tmp_a = new float[m * n];
  float *tmp_a_ = new float[m * m];

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < m; ++j) {
      tmp_u[i * n + j] = matU[i * n + j];
    }
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      tmp_v[i * n + j] = matVt[i * n + j];
    }
  }

  if (!at) {
    int k = 0;
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++, k++)
        tmp_a[k] = matSrc[j * n + i];
  } else {
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        tmp_a[i * n + j] = matSrc[i * n + j];
  }

  if (m == n) {
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        tmp_a_[i * n + j] = tmp_a[i * n + j];
  } else {
    for (int i = 0; i < m; i++)
      for (int j = 0; j < m; j++)
        if (j >= n)
          tmp_a_[i * n + j] = 0;
        else
          tmp_a_[i * n + j] = tmp_a[i * n + j];
  }

  jacobiSVD(tmp_a_, matD, tmp_v, m, n);

  if (!at) {
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        matVt[i * n + j] = tmp_v[i * n + j];
    for (int i = 0; i < m; i++)
      for (int j = 0; j < m; j++)
        matU[j * m + i] = tmp_a_[i * m + j];
  } else {
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        matU[i * n + j] = tmp_v[j * n + i];
    for (int i = 0; i < m; i++)
      for (int j = 0; j < m; j++)
        matVt[i * m + j] = tmp_a_[i * m + j];
  }

  delete [] tmp_u;
  delete [] tmp_v;
  delete [] tmp_a;
  delete [] tmp_a_;

  return 0;
}

void jacobiSVD(float *At, float *_W, float *Vt, const int m, const int n) {
  int p = 0;
  double minval = FLT_MIN;
  auto eps = (float)(FLT_EPSILON * 2);
  const int n1 = m;

  double *W = new double[n];
  for (int i = 0; i < n; i++)
    W[i] = 0;

  for (int i = 0; i < n; i++) {
    double sd{ 0. };
    for (int k = 0; k < m; k++) {
      float t = At[i * m + k];
      sd += (double)t * t;
    }
    W[i] = sd;

    for (int k = 0; k < n; k++)
      Vt[i * n + k] = 0;
    Vt[i * n + i] = 1;
  }

  int max_iter = std::max(m, 30);
  for (int iter = 0; iter < max_iter; iter++) {
    bool changed = false;
    float c, s;

    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        float *Ai = At + i * m, *Aj = At + j * m;
        double a = W[i], p = 0, b = W[j];

        for (int k = 0; k < m; k++)
          p += (double)Ai[k] * Aj[k];

        if (std::abs(p) <= eps * std::sqrt((double)a * b))
          continue;

        p *= 2;
        double beta = a - b, gamma = hypot((double)p, beta);
        if (beta < 0) {
          double delta = (gamma - beta) * 0.5;
          s = (float)std::sqrt(delta / gamma);
          c = (float)(p / (gamma * s * 2));
        } else {
          c = (float)std::sqrt((gamma + beta) / (gamma * 2));
          s = (float)(p / (gamma * c * 2));
        }

        a = b = 0;
        for (int k = 0; k < m; k++) {
          float t0 = c * Ai[k] + s * Aj[k];
          float t1 = -s * Ai[k] + c * Aj[k];
          Ai[k] = t0;
          Aj[k] = t1;

          a += (double)t0 * t0;
          b += (double)t1 * t1;
        }
        W[i] = a;
        W[j] = b;

        changed = true;

        float *Vi = Vt + i * n, *Vj = Vt + j * n;

        for (int k = 0; k < n; k++) {
          float t0 = c * Vi[k] + s * Vj[k];
          float t1 = -s * Vi[k] + c * Vj[k];
          Vi[k] = t0;
          Vj[k] = t1;
        }
      }
    }
    if (!changed)
      break;
  }

  for (int i = 0; i < n; i++) {
    double sd{ 0. };
    for (int k = 0; k < m; k++) {
      float t = At[i * m + k];
      sd += (double)t * t;
    }
    W[i] = std::sqrt(sd);
  }

  for (int i = 0; i < n - 1; i++) {
    int j = i;
    for (int k = i + 1; k < n; k++) {
      if (W[j] < W[k])
        j = k;
    }
    if (i != j) {
      float s = W[j];
      W[j] = W[i];
      W[i] = s;

      for (int k = 0; k < m; k++) {
        s = At[j * m + k];
        At[j * m + k] = At[i * m + k];
        At[i * m + k] = s;
      }

      for (int k = 0; k < n; k++) {
        s = Vt[j * m + k];
        Vt[j * m + k] = Vt[i * m + k];
        Vt[i * m + k] = s;
      }
    }
  }

  for (int i = 0; i < n; i++)
    _W[i] = (float)W[i];

  srand(time(nullptr));

  for (int i = 0; i < n1; i++) {
    double sd = i < n ? W[i] : 0;

    for (int ii = 0; ii < 100 && sd <= minval; ii++) {
      // if we got a zero singular value, then in order to get the corresponding left singular vector
      // we generate a random vector, project it to the previously computed left singular vectors,
      // subtract the projection and normalize the difference.
      const auto val0 = (float)(1. / m);
      for (int k = 0; k < m; k++) {
        unsigned long int rng = rand() % 4294967295;  // 2^32 - 1
        float val = (rng & 256) != 0 ? val0 : -val0;
        At[i * m + k] = val;
      }
      for (int iter = 0; iter < 2; iter++) {
        for (int j = 0; j < i; j++) {
          sd = 0;
          for (int k = 0; k < m; k++)
            sd += At[i * m + k] * At[j * m + k];
          float asum = 0;
          for (int k = 0; k < m; k++) {
            auto t = (float)(At[i * m + k] - sd * At[j * m + k]);
            At[i * m + k] = t;
            asum += std::abs(t);
          }
          asum = asum > eps * 100 ? 1 / asum : 0;
          for (int k = 0; k < m; k++)
            At[i * m + k] *= asum;
        }
      }

      sd = 0;
      for (int k = 0; k < m; k++) {
        float t = At[i * m + k];
        sd += (double)t * t;
      }
      sd = std::sqrt(sd);
    }

    float s = (float)(sd > minval ? 1 / sd : 0.);
    for (int k = 0; k < m; k++)
      At[i * m + k] *= s;
  }
  
  delete [] W;
}

//單應矩陣求解R與t
bool reconstructH1(float *H21) {
  float invK[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
  float *B = new float[3 * 3];
  float *A = new float[3 * 3];

  if (inv_CAMERA_INTRINSICS[0][0] == 1){
    inv(*CAMERA_INTRINSICS, *inv_CAMERA_INTRINSICS, 3);
  }

  matMult(*inv_CAMERA_INTRINSICS, H21, B, 3, 3, 3);
  matMult(B, *CAMERA_INTRINSICS, A, 3, 3, 3);

  float w[3][1] = { 0 };
  float U[3][3] = { 0 };
  float Vt[3][3] = { 0 };
  float V[3][3] = { 0 };

  svd(A, *w, *U, *Vt, 3, 3);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      V[i][j] = Vt[j][i];
    }
  }

  float s = (U[0][0] * U[1][1] * U[2][2] + U[0][1] * U[1][2] * U[2][0] + U[0][2] * U[1][0] * U[2][1] - U[0][2] * U[1][1] * U[2][0] - U[0][1] * U[1][0] * U[2][2] - U[0][0] * U[1][2] * U[2][1]) * (Vt[0][0] * Vt[1][1] * Vt[2][2] + Vt[0][1] * Vt[1][2] * Vt[2][0] + Vt[0][2] * Vt[1][0] * Vt[2][1] - Vt[0][2] * Vt[1][1] * Vt[2][0] - Vt[0][1] * Vt[1][0] * Vt[2][2] - Vt[0][0] * Vt[1][2] * Vt[2][1]);

  float d1 = w[0][0];
  float d2 = w[1][0];
  float d3 = w[2][0];

  if (d1 / d2 < 1.00001 || d2 / d3 < 1.00001) {
    return false;
  }

  float vR[8][3][3] = { 0 };
  float vt[8][3][1] = { 0 };
  float vn[8][3][1] = { 0 };

  float aux1 = sqrt((d1 * d1 - d2 * d2) / (d1 * d1 - d3 * d3));
  float aux3 = sqrt((d2 * d2 - d3 * d3) / (d1 * d1 - d3 * d3));
  float x1[] = { aux1, aux1, -aux1, -aux1 };
  float x3[] = { aux3, -aux3, aux3, -aux3 };

  float aux_stheta = sqrt((d1 * d1 - d2 * d2) * (d2 * d2 - d3 * d3)) / ((d1 + d3) * d2);

  float ctheta = (d2 * d2 + d1 * d3) / ((d1 + d3) * d2);
  float stheta[] = { aux_stheta, -aux_stheta, -aux_stheta, aux_stheta };

  for (int i = 0; i < 4; i++) {
    float Rp[3][3] = { { ctheta, 0, -stheta[i] }, { 0, 1, 0 }, { stheta[i], 0, ctheta } };
    float R[3][3] = { 0 };

    matMult(*U, *Rp, B, 3, 3, 3);
    matMult(B, *Vt, *R, 3, 3, 3);
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        vR[i][j][k] = R[j][k] * s;
      }
    }

    float tp[3][1] = { { x1[i] * (d1 - d3) }, { 0 }, { -x3[i] * (d1 - d3) } };
    float t[3][1] = { 0 };

    matMult(*U, *tp, *t, 3, 3, 1);
    float norm = sqrt(t[0][0] * t[0][0] + t[1][0] * t[1][0] + t[2][0] * t[2][0]);
    vt[i][0][0] = t[0][0];
    vt[i][1][0] = t[1][0];
    vt[i][2][0] = t[2][0];

    float np[3][1] = { { x1[i] }, { 0 }, { x3[i] } };
    float n[3][1] = { 0 };

    matMult(*V, *np, *n, 3, 3, 1);
    if (n[2][0] < 0)
      for (int i = 0; i < 3; i++)
        n[i][0] = -n[i][0];
    vn[i][0][0] = n[0][0];
    vn[i][1][0] = n[1][0];
    vn[i][2][0] = n[2][0];

    //print_array(*vR[i], 3, 3);
    //print_array(*vt[i], 1, 3);
    //print_array(*vn[i], 1, 3);
    if (i == 0) {
      myData.a[0] = fastatan2(vR[i][2][1], vR[i][2][2]);
      myData.a[1] = fastatan2(-vR[i][2][0], sqrt(vR[i][2][1]*vR[i][2][1] + vR[i][2][2]*vR[i][2][2]));
      myData.a[2] = fastatan2(vR[i][1][0], vR[i][0][0]);

      myData.a[0] = myData.a[0] > 180 ? myData.a[0] - 360 : myData.a[0];
      myData.a[1] = myData.a[1] > 180 ? myData.a[1] - 360 : myData.a[1];
      myData.a[2] = myData.a[2] > 180 ? myData.a[2] - 360 : myData.a[2];
    }
  }

  delete [] A;
  delete [] B;

  return false;
}

//逆矩陣
void inv(float *In_mat, float *Out_mat, const int n) {
  float *cb_In_mat = new float[n * n];
  for (int i = 0; i < n * n; i++) {
    cb_In_mat[i] = In_mat[i];
  }

  //to 倒三角矩陣
  for (int i = 0; i < n; i++) {
    float p = cb_In_mat[i * n + i];
    for (int j = 0; j < n; j++)
      cb_In_mat[i * n + j] /= p;
    for (int j = 0; j < i + 1; j++)
      Out_mat[i * n + j] /= p;
    for (int j = i + 1; j < n; j++) {
      float d = cb_In_mat[j * n + i] / cb_In_mat[i * n + i];
      for (int k = 0; k < i + 1; k++)
        Out_mat[j * n + k] -= d * Out_mat[i * n + k];
      for (int k = i; k < n; k++)
        cb_In_mat[j * n + k] -= cb_In_mat[i * n + k] * d;
    }
  }

  //to 單位矩陣
  for (int i = n - 1; i > 0; i--) {
    for (int j = i; j > 0; j--) {
      for (int k = 0; k < 3; k++)
        Out_mat[(j - 1) * n + k] -= cb_In_mat[(j - 1) * n + i] * Out_mat[i * n + k];
      cb_In_mat[(j - 1) * n + i] = 0;
    }
  }

  delete [] cb_In_mat;
}

//矩陣相乘
void matMult(float *In_mat1, float *In_mat2, float *Out_mat, const int m, const int n, const int o) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < o; j++) {
      float sum = 0;
      for (int k = 0; k < n; k++) {
        sum += In_mat1[k + i * n] * In_mat2[k * o + j];
      }
      Out_mat[i * o + j] = sum;
    }
  }
}
