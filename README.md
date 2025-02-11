# SAB-chan

SAB-chan (Stabilized Autonomy Balancer) is inverted pendulum robot. This is a home-built robot using M5Stack and Roller485!

<img src="https://github.com/user-attachments/assets/1d006362-02ce-4f63-b6ef-c19bad98a29b" width="30%">

---

### Note

* Communication error occurs when communicating with IMU at 1 kHz, so drop to 200 Hz.

```diff
# main.cpp
- timerAlarmWrite(timer, 1000, true);
+ timerAlarmWrite(timer, 5000, true);
```

* ~~Changed i2c communication baud rate with IMU from 400 kHz to 10 MHz~~
\* Revision cancelled due to lack of improvement in communication speed.

```diff
# MPU6886_Class.hpp
- MPU6886_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);
+ MPU6886_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 1000000, I2C_Class* i2c = &In_I2C);
```

### To-Do

* [x] Low-Pass filter class add
* [x] Get to constant frequency imu data
* [ ] Add a posture estimation process using the extended Kalman filter
* [ ] IMU acquisition process and posture estimation process are executed in separate tasks