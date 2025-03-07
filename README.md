# ARCH2025-EmbeddedSystems
This branch is for testing and implementation of an array of ESP32 sensors and how their data is relayed to Kanga and the Base Station.

---
Message Format
| Component | Desc                          |
|-----------|-------------------------------|
| $         |Message Start                  |
| ID        | Sensor ID No from table below |
| ~         | Seperate ID and Message       |
| String/No.| Message Content               |
| `         | Message End                   |

**Full Format: $<"ID">~<"Content">`**


---
Sensor ID Table
| Sensor            | Model No. | Data Format Desc |  Format             | ID |
|-------------------|-----------|------------------|---------------------|----|
| RFID              | RC522     | Output String    | $1~ThisIsInfoFrom`  | 1  |
| Potentiometer     | Unknown   | Int Percentage   | $2~30`              | 2  |
| IMU               | RC522     | Row1 Data        | $3~<ENTER>`         | 3  |
| NIR Spectroscopy  | AS7265x   | Row2 Data        | $4~<ENTER>`         | 4  |
