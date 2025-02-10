#include <Arduino.h>
#include <Wire.h>
#include <M5Unified.h>

#include "digital_filter.h"

float ax_lhs, ay_lhs, az_lhs;
float gx_lhs, gy_lhs, gz_lhs;
float ax_rhs, ay_rhs, az_rhs;
float gx_rhs, gy_rhs, gz_rhs;
float temp;

float ax_f;

#define UNIT_G 9.80665
#define UNIT_DEG2S 0.017453292519943

digital_filter::LowPassFilter accx_lpf(500.0f, 100.0f, 0.1f);

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

  // Debugシリアルの初期化
  Serial.begin(115200);

  // M5Stack Core2の初期化
  auto cfg = M5.config();
  M5.begin(cfg);  
}

void loop() {
  // IMUの値を取得
  M5.Imu.getAccel(&ax_lhs, &ay_lhs, &az_lhs);
  M5.Imu.getGyro(&gx_lhs, &gy_lhs, &gz_lhs);
  M5.Imu.getTemp(&temp);

  // 左手座標系から右手座標系へ変換
  ax_rhs = ax_lhs;
  ay_rhs = ay_lhs;
  az_rhs = - az_rhs;
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

  ax_f = accx_lpf.CalcOutput(ax_rhs);
}

