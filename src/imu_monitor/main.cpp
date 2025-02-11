#include <Arduino.h>
#include <Wire.h>
#include <M5Unified.h>

#include "digital_filter.h"
#include "record_data.h"

float ax_lhs, ay_lhs, az_lhs;
float gx_lhs, gy_lhs, gz_lhs;
float ax_rhs, ay_rhs, az_rhs;
float gx_rhs, gy_rhs, gz_rhs;
float temp;

float ax_f, ay_f, az_f;
float gx_f, gy_f, gz_f;


hw_timer_t* timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
volatile uint32_t count;

#define UNIT_G 9.80665
#define UNIT_DEG2S 0.017453292519943


#define MAX_RECORD_SIZE 1024
int record_idx = 0;
RecordData records[MAX_RECORD_SIZE];


digital_filter::LowPassFilter ax_lpf(500.0f, 100.0f, 0.1f);
digital_filter::LowPassFilter ay_lpf(500.0f, 100.0f, 0.1f);
digital_filter::LowPassFilter az_lpf(500.0f, 100.0f, 0.1f);
digital_filter::LowPassFilter gx_lpf(500.0f, 100.0f, 0.1f);
digital_filter::LowPassFilter gy_lpf(500.0f, 100.0f, 0.1f);
digital_filter::LowPassFilter gz_lpf(500.0f, 100.0f, 0.1f);


void IRAM_ATTR timerEventTrigger() {
  count++;
  xSemaphoreGiveFromISR(timerSemaphore, NULL); 
}


void setup() {
  // グローバル変数の初期化
  ax_lhs = 0.0f;
  ay_lhs = 0.0f;
  az_lhs = 0.0f;
  gx_lhs = 0.0f;
  gy_lhs = 0.0f;
  gz_lhs = 0.0f;
  ax_rhs = 0.0f;
  ay_rhs = 0.0f;
  az_rhs = 0.0f;
  gx_rhs = 0.0f;
  gy_rhs = 0.0f;
  gz_rhs = 0.0f;
  temp = 0.0f;

  count = 0u;

  // Debugシリアルの初期化
  Serial.begin(115200);

  // M5Stack Core2の初期化
  auto cfg = M5.config();
  M5.begin(cfg);

  // 1kHzタイマ割り込み設定
  timerSemaphore = xSemaphoreCreateBinary(); 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &timerEventTrigger, true);
  timerAlarmWrite(timer, 5000, true);
  timerAlarmEnable(timer);  
}

void loop() {
  bool ret = false;
  if(count > 0) {
    // IMUの値を取得
    if(!M5.Imu.getAccel(&ax_lhs, &ay_lhs, &az_lhs)) {
      Serial.println("error to get imu accel");
      // while(1){ delay(100); }
    }
    if(!M5.Imu.getGyro(&gx_lhs, &gy_lhs, &gz_lhs)) {
      Serial.println("error to get imu gyro");
      // while(1){ delay(100); }
    }
    if(!M5.Imu.getTemp(&temp)) {
      Serial.println("error to get imu temp");
      // while(1){ delay(100); }
    }

    // 左手座標系から右手座標系へ変換
    ax_rhs = ax_lhs;
    ay_rhs = ay_lhs;
    az_rhs = - az_lhs;
    gx_rhs = - gx_lhs;
    gy_rhs = - gy_lhs;
    gz_rhs = gz_lhs;

    // 単位変換 G⇒m/s^2 deg/s⇒rad/s
    ax_rhs = ax_rhs * UNIT_G;
    ay_rhs = ay_rhs * UNIT_G;
    az_rhs = az_rhs * UNIT_G;
    gx_rhs = gx_rhs * UNIT_DEG2S;
    gy_rhs = gy_rhs * UNIT_DEG2S;
    gz_rhs = gz_rhs * UNIT_DEG2S;

    // フィルタ処理
    ax_f = ax_lpf.CalcOutput(ax_rhs);
    ay_f = ay_lpf.CalcOutput(ay_rhs);
    az_f = az_lpf.CalcOutput(az_rhs);
    gx_f = gx_lpf.CalcOutput(gx_rhs);
    gy_f = gy_lpf.CalcOutput(gy_rhs);
    gz_f = gz_lpf.CalcOutput(gz_rhs);

    // 割り込みリセット
    count--;
    if(record_idx < MAX_RECORD_SIZE) {
      records[record_idx].time = millis();
      records[record_idx].data[0]  = ax_rhs;
      records[record_idx].data[1]  = ax_f;
      records[record_idx].data[2]  = ay_rhs;
      records[record_idx].data[3]  = ay_f;
      records[record_idx].data[4]  = az_rhs;
      records[record_idx].data[5]  = az_f;
      records[record_idx].data[6]  = gx_rhs;
      records[record_idx].data[7]  = gx_f;
      records[record_idx].data[8]  = gy_rhs;
      records[record_idx].data[9]  = gy_f;
      records[record_idx].data[10] = gz_rhs;
      records[record_idx].data[11] = gz_f;
      record_idx++;
    }
    else {
      for(int i=0; i<MAX_RECORD_SIZE; i++) {
        Serial.print(records[i].time); Serial.print(", ");
        Serial.print(records[i].data[0]);  Serial.print(", ");
        Serial.print(records[i].data[1]);  Serial.print(", ");
        Serial.print(records[i].data[2]);  Serial.print(", ");
        Serial.print(records[i].data[3]);  Serial.print(", ");
        Serial.print(records[i].data[4]);  Serial.print(", ");
        Serial.print(records[i].data[5]);  Serial.print(", ");
        Serial.print(records[i].data[6]);  Serial.print(", ");
        Serial.print(records[i].data[7]);  Serial.print(", ");
        Serial.print(records[i].data[8]);  Serial.print(", ");
        Serial.print(records[i].data[9]);  Serial.print(", ");
        Serial.print(records[i].data[10]); Serial.print(", ");
        Serial.print(records[i].data[11]); Serial.print("\n");
      }
      while(true){}
    }
  }
}

