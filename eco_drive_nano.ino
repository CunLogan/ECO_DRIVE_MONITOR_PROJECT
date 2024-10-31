
#include <MQUnifiedsensor.h>
#include <SD_ZH03B.h>
#include <SoftwareSerial.h>




SoftwareSerial ZHSerial(5, 6);  // RX, TX

SD_ZH03B ZH03B(ZHSerial, SD_ZH03B::SENSOR_ZH03B);


SoftwareSerial ESPSerial(7, 8);  // RX, TX

MQUnifiedsensor MQ135_BF("Arduino NANO", 5, 10, A5, "MQ-135");
MQUnifiedsensor MQ7_BF("Arduino NANO", 5, 10, A3, "MQ-7");
MQUnifiedsensor MQ2_BF("Arduino NANO", 5, 10, A4, "MQ-2");

MQUnifiedsensor MQ135_AT("Arduino NANO", 5, 10, A2, "MQ-135");
MQUnifiedsensor MQ7_AT("Arduino NANO", 5, 10, A0, "MQ-7");
MQUnifiedsensor MQ2_AT("Arduino NANO", 5, 10, A1, "MQ-2");


int pm25, pm10, pm1;

unsigned long LastUpdate = 0;


void setup() {
  // put your setup code here, to run once:
  ESPSerial.begin(9600);
  Serial.begin(115200);
  ZHSerial.begin(9600);

  delay(100);
  ZH03B.setMode(SD_ZH03B::IU_MODE);


  MQ135_BF.setRegressionMethod(1);
  MQ7_BF.setRegressionMethod(1);
  MQ2_BF.setRegressionMethod(1);
  MQ135_AT.setRegressionMethod(1);
  MQ7_AT.setRegressionMethod(1);
  MQ2_AT.setRegressionMethod(1);

  MQ135_BF.init();
  MQ7_BF.init();
  MQ2_BF.init();
  MQ135_AT.init();
  MQ7_AT.init();
  MQ2_AT.init();
  Serial.print("Calibrating please wait.");

  float calcR0_MQ7_BF = 0;
  float calcR0_MQ2_BF = 0;
  float calcR0_MQ135_BF = 0;
  float calcR0_MQ7_AT = 0;
  float calcR0_MQ2_AT = 0;
  float calcR0_MQ135_AT = 0;
  for (int i = 1; i <= 10; i++) {
    MQ7_BF.update();  // Update data, the arduino will read the voltage from the analog pin
    MQ2_BF.update();
    MQ135_BF.update();
    MQ7_AT.update();  // Update data, the arduino will read the voltage from the analog pin
    MQ2_AT.update();
    MQ135_AT.update();
    calcR0_MQ7_BF += MQ7_BF.calibrate(27.5);
    calcR0_MQ2_BF += MQ2_BF.calibrate(9.83);
    calcR0_MQ135_BF += MQ135_BF.calibrate(3.6);
    calcR0_MQ7_AT += MQ7_AT.calibrate(27.5);
    calcR0_MQ2_AT += MQ2_AT.calibrate(9.83);
    calcR0_MQ135_AT += MQ135_AT.calibrate(3.6);
    Serial.print(".");
  }
  MQ7_BF.setR0(calcR0_MQ7_BF / 10);
  MQ2_BF.setR0(calcR0_MQ2_BF / 10);
  MQ135_BF.setR0(calcR0_MQ135_BF / 10);
  MQ7_AT.setR0(calcR0_MQ7_AT / 10);
  MQ2_AT.setR0(calcR0_MQ2_AT / 10);
  MQ135_AT.setR0(calcR0_MQ135_AT / 10);
  Serial.println("  done!.");

  if (isinf(calcR0_MQ7_BF)) {
    Serial.println("Lỗi R0 infinite MQ7_BF");
    while (1)
      ;
  }
  if (calcR0_MQ7_BF == 0) {
    Serial.println("Lỗi R0 =0 MQ7_BF");
    while (1)
      ;
  }
  if (isinf(calcR0_MQ2_BF)) {
    Serial.println("Lỗi R0 infinite MQ2_BF");
    while (1)
      ;
  }
  if (calcR0_MQ2_BF == 0) {
    Serial.println("Lỗi R0 =0 MQ2_BF");
    while (1)
      ;
  }
  if (isinf(calcR0_MQ135_BF)) {
    Serial.println("Lỗi R0 infinite MQ135_BF");
    while (1)
      ;
  }
  if (calcR0_MQ135_BF == 0) {
    Serial.println("Lỗi R0 =0 MQ135_BF");
    while (1)
      ;
  }
  if (isinf(calcR0_MQ7_AT)) {
    Serial.println("Lỗi R0 infinite MQ7_AT");
    while (1)
      ;
  }
  if (calcR0_MQ7_AT == 0) {
    Serial.println("Lỗi R0 =0 MQ7_AT");
    while (1)
      ;
  }
  if (isinf(calcR0_MQ2_AT)) {
    Serial.println("Lỗi R0 infinite MQ2_AT");
    while (1)
      ;
  }
  if (calcR0_MQ2_AT == 0) {
    Serial.println("Lỗi R0 =0 MQ2_AT");
    while (1)
      ;
  }
  if (isinf(calcR0_MQ135_AT)) {
    Serial.println("Lỗi R0 infinite MQ135_AT");
    while (1)
      ;
  }
  if (calcR0_MQ135_AT == 0) {
    Serial.println("Lỗi R0 =0 MQ135_AT");
    while (1)
      ;
  }



  //ESPSerial.begin(9600);
}

void loop() {


  readSensor();
}


void readSensor() {
  MQ7_BF.update();
  MQ2_BF.update();
  MQ135_BF.update();
  MQ7_AT.update();
  MQ2_AT.update();
  MQ135_AT.update();




  if (ZH03B.readData()) {
    pm25 = ZH03B.getPM2_5();   // Lấy giá trị PM2.5
    pm10 = ZH03B.getPM10_0();  // Lấy giá trị PM10
    pm1 = ZH03B.getPM1_0();    // Lấy giá trị PM10
  }
  ////////////////////////////////////////////
  MQ135_BF.setA(605.18);
  MQ135_BF.setB(-3.937);
  float CO_1_MQ135 = MQ135_BF.readSensor();

  MQ135_BF.setA(77.255);
  MQ135_BF.setB(-3.18);
  float Alcohol_1_MQ135 = MQ135_BF.readSensor();

  MQ135_BF.setA(110.47);
  MQ135_BF.setB(-2.862);
  float CO2_1_MQ135 = MQ135_BF.readSensor();

  MQ135_BF.setA(44.947);
  MQ135_BF.setB(-3.445);
  float Toluen_1_MQ135 = MQ135_BF.readSensor();

  MQ135_BF.setA(102.2);
  MQ135_BF.setB(-2.473);
  float NH4_1_MQ135 = MQ135_BF.readSensor();

  MQ135_BF.setA(34.668);
  MQ135_BF.setB(-3.369);
  float Aceton_1_MQ135 = MQ135_BF.readSensor();


  //___________________________________________________________
  MQ135_AT.setA(605.18);
  MQ135_AT.setB(-3.937);
  float CO_2_MQ135 = MQ135_AT.readSensor();

  MQ135_AT.setA(77.255);
  MQ135_AT.setB(-3.18);
  float Alcohol_2_MQ135 = MQ135_AT.readSensor();

  MQ135_AT.setA(110.47);
  MQ135_AT.setB(-2.862);
  float CO2_2_MQ135 = MQ135_AT.readSensor();

  MQ135_AT.setA(44.947);
  MQ135_AT.setB(-3.445);
  float Toluen_2_MQ135 = MQ135_AT.readSensor();

  MQ135_AT.setA(102.2);
  MQ135_AT.setB(-2.473);
  float NH4_2_MQ135 = MQ135_AT.readSensor();

  MQ135_AT.setA(34.668);
  MQ135_AT.setB(-3.369);
  float Aceton_2_MQ135 = MQ135_AT.readSensor();
  //===================================================================================


  MQ7_BF.setA(69.014);
  MQ7_BF.setB(-1.374);
  float H2_1_MQ7 = MQ7_BF.readSensor();

  MQ7_BF.setA(700000000);
  MQ7_BF.setB(-7.703);
  float LPG_1_MQ7 = MQ7_BF.readSensor();

  MQ7_BF.setA(60000000000000);
  MQ7_BF.setB(-10.54);
  float CH4_1_MQ7 = MQ7_BF.readSensor();

  MQ7_BF.setA(99.042);
  MQ7_BF.setB(-1.518);
  float CO_1_MQ7 = MQ7_BF.readSensor();

  MQ7_BF.setA(40000000000000000);
  MQ7_BF.setB(-12.35);
  float Alcohol_1_MQ7 = MQ7_BF.readSensor();


  //----------------------------------------------
  MQ7_AT.setA(69.014);
  MQ7_AT.setB(-1.374);
  float H2_2_MQ7 = MQ7_AT.readSensor();

  MQ7_AT.setA(700000000);
  MQ7_AT.setB(-7.703);
  float LPG_2_MQ7 = MQ7_AT.readSensor();

  MQ7_AT.setA(60000000000000);
  MQ7_AT.setB(-10.54);
  float CH4_2_MQ7 = MQ7_AT.readSensor();

  MQ7_AT.setA(99.042);
  MQ7_AT.setB(-1.518);
  float CO_2_MQ7 = MQ7_AT.readSensor();

  MQ7_AT.setA(40000000000000000);
  MQ7_AT.setB(-12.35);
  float Alcohol_2_MQ7 = MQ7_AT.readSensor();
  //===============================================================


  MQ2_BF.setA(987.99);
  MQ2_BF.setB(-2.162);
  float H2_1_MQ2 = MQ2_BF.readSensor();

  MQ2_BF.setA(574.25);
  MQ2_BF.setB(-2.222);
  float LPG_1_MQ2 = MQ2_BF.readSensor();

  MQ2_BF.setA(36974);
  MQ2_BF.setB(-3.109);
  float CO_1_MQ2 = MQ2_BF.readSensor();

  MQ2_BF.setA(3616.1);
  MQ2_BF.setB(-2.675);
  float Alcohol_1_MQ2 = MQ2_BF.readSensor();

  MQ2_BF.setA(658.71);
  MQ2_BF.setB(-2.168);
  float Propane_1_MQ2 = MQ2_BF.readSensor();


  //==================================================================
  MQ2_AT.setA(987.99);
  MQ2_AT.setB(-2.162);
  float H2_2_MQ2 = MQ2_AT.readSensor();

  MQ2_AT.setA(574.25);
  MQ2_AT.setB(-2.222);
  float LPG_2_MQ2 = MQ2_AT.readSensor();

  MQ2_AT.setA(36974);
  MQ2_AT.setB(-3.109);
  float CO_2_MQ2 = MQ2_AT.readSensor();

  MQ2_AT.setA(3616.1);
  MQ2_AT.setB(-2.675);
  float Alcohol_2_MQ2 = MQ2_AT.readSensor();

  MQ2_AT.setA(658.71);
  MQ2_AT.setB(-2.168);
  float Propane_2_MQ2 = MQ2_AT.readSensor();

  float Carbon_Monoxide_CO_BF = formatValue((CO_1_MQ135 + CO_1_MQ7 + CO_1_MQ2) / 3);
  float Hydrocarbons_HC_BF = formatValue((Toluen_1_MQ135 + Alcohol_1_MQ135 + Aceton_1_MQ135 + LPG_1_MQ7 + CH4_1_MQ7 + Alcohol_1_MQ7 + LPG_1_MQ2 + Alcohol_1_MQ2 + Propane_1_MQ2) / 9);
  float Carbon_Dioxide_CO2_BF = formatValue(CO2_1_MQ135)+400;
  float Ammonia_NH3_BF = formatValue(NH4_1_MQ135);
  float Liquefied_Petroleum_Gas_BF = formatValue((LPG_1_MQ7 + LPG_1_MQ2) / 2);
  float Hydrogen_H2_BF = formatValue((H2_1_MQ7 + H2_1_MQ2) / 2);

  float Carbon_Monoxide_CO_AT = formatValue((CO_2_MQ135 + CO_2_MQ7 + CO_2_MQ2) / 3);
  float Hydrocarbons_HC_AT = formatValue((Toluen_2_MQ135 + Alcohol_2_MQ135 + Aceton_2_MQ135 + LPG_2_MQ7 + CH4_2_MQ7 + Alcohol_2_MQ7 + LPG_2_MQ2 + Alcohol_2_MQ2 + Propane_2_MQ2) / 9);
  float Carbon_Dioxide_CO2_AT = formatValue(CO2_2_MQ135)+400;
  float Ammonia_NH3_AT = formatValue(NH4_1_MQ135);
  float Liquefied_Petroleum_Gas_AT = formatValue((LPG_2_MQ7 + LPG_2_MQ2) / 2);
  float Hydrogen_H2_AT = formatValue((H2_2_MQ7 + H2_2_MQ2) / 2);

  delay(1000);




  if (millis() - LastUpdate > 3000) {
    update_data(Carbon_Monoxide_CO_BF, Hydrocarbons_HC_BF, Carbon_Dioxide_CO2_BF, Ammonia_NH3_BF, Liquefied_Petroleum_Gas_BF, Hydrogen_H2_BF, Carbon_Monoxide_CO_AT, Hydrocarbons_HC_AT, Carbon_Dioxide_CO2_AT, Ammonia_NH3_AT, Liquefied_Petroleum_Gas_AT, Hydrogen_H2_AT, pm25, pm10, pm1);  // 14 arguments only
    LastUpdate = millis();
  }

  //debug2(Carbon_Monoxide_CO_BF, Hydrocarbons_HC_BF, Carbon_Dioxide_CO2_BF, Ammonia_NH3_BF, Liquefied_Petroleum_Gas_BF, Hydrogen_H2_BF, Carbon_Monoxide_CO_AT, Hydrocarbons_HC_AT, Carbon_Dioxide_CO2_AT, Ammonia_NH3_AT, Liquefied_Petroleum_Gas_AT, Hydrogen_H2_AT, pm25, pm10);
}


// void debug() {
//   // In các giá trị của các biến lên Serial Monitor
//   Serial.println("MQ135 Sensor Readings:");
//   Serial.print("CO_1_MQ135: ");
//   Serial.print(CO_1_MQ135);
//   Serial.print("\tAlcohol_1_MQ135: ");
//   Serial.print(Alcohol_1_MQ135);
//   Serial.print("\tCO2_1_MQ135: ");
//   Serial.print(CO2_1_MQ135);
//   Serial.print("\tToluen_1_MQ135: ");
//   Serial.print(Toluen_1_MQ135);
//   Serial.print("\tNH4_1_MQ135: ");
//   Serial.print(NH4_1_MQ135);
//   Serial.print("\tAceton_1_MQ135: ");
//   Serial.println(Aceton_1_MQ135);
//   Serial.print("CO_2_MQ135: ");
//   Serial.print(CO_2_MQ135);
//   Serial.print("\tAlcohol_2_MQ135: ");
//   Serial.print(Alcohol_2_MQ135);
//   Serial.print("\tCO2_2_MQ135: ");
//   Serial.print(CO2_2_MQ135);
//   Serial.print("\tToluen_2_MQ135: ");
//   Serial.print(Toluen_2_MQ135);
//   Serial.print("\tNH4_2_MQ135: ");
//   Serial.print(NH4_2_MQ135);
//   Serial.print("\tAceton_2_MQ135: ");
//   Serial.println(Aceton_2_MQ135);

//   Serial.println("\nMQ7 Sensor Readings:");
//   Serial.print("H2_1_MQ7: ");
//   Serial.print(H2_1_MQ7);
//   Serial.print("\tLPG_1_MQ7: ");
//   Serial.print(LPG_1_MQ7);
//   Serial.print("\tCH4_1_MQ7: ");
//   Serial.print(CH4_1_MQ7);
//   Serial.print("\tCO_1_MQ7: ");
//   Serial.print(CO_1_MQ7);
//   Serial.print("\tAlcohol_1_MQ7: ");
//   Serial.println(Alcohol_1_MQ7);
//   Serial.print("H2_2_MQ7: ");
//   Serial.print(H2_2_MQ7);
//   Serial.print("\tLPG_2_MQ7: ");
//   Serial.print(LPG_2_MQ7);
//   Serial.print("\tCH4_2_MQ7: ");
//   Serial.print(CH4_2_MQ7);
//   Serial.print("\tCO_2_MQ7: ");
//   Serial.print(CO_2_MQ7);
//   Serial.print("\tAlcohol_2_MQ7: ");
//   Serial.println(Alcohol_2_MQ7);

//   Serial.println("\nMQ2 Sensor Readings:");
//   Serial.print("H2_1_MQ2: ");
//   Serial.print(H2_1_MQ2);
//   Serial.print("\tLPG_1_MQ2: ");
//   Serial.print(LPG_1_MQ2);
//   Serial.print("\tCO_1_MQ2: ");
//   Serial.print(CO_1_MQ2);
//   Serial.print("\tAlcohol_1_MQ2: ");
//   Serial.print(Alcohol_1_MQ2);
//   Serial.print("\tPropane_1_MQ2: ");
//   Serial.println(Propane_1_MQ2);
//   Serial.print("H2_2_MQ2: ");
//   Serial.print(H2_2_MQ2);
//   Serial.print("\tLPG_2_MQ2: ");
//   Serial.print(LPG_2_MQ2);
//   Serial.print("\tCO_2_MQ2: ");
//   Serial.print(CO_2_MQ2);
//   Serial.print("\tAlcohol_2_MQ2: ");
//   Serial.print(Alcohol_2_MQ2);
//   Serial.print("\tPropane_2_MQ2: ");
//   Serial.println(Propane_2_MQ2);

//   Serial.println("\n--------------------------\n");

//   delay(1000);  // Đợi 1 giây trước khi lặp lại để tránh tràn Serial Monitor
// }

// void debug1() {
//   // Nhóm các chỉ số khí CO
//   Serial.println("CO Levels:");
//   Serial.print("CO_1_MQ135: ");
//   Serial.println(CO_1_MQ135);
//   Serial.print("CO_2_MQ135: ");
//   Serial.println(CO_2_MQ135);
//   Serial.print("CO_1_MQ7: ");
//   Serial.println(CO_1_MQ7);
//   Serial.print("CO_2_MQ7: ");
//   Serial.println(CO_2_MQ7);
//   Serial.print("CO_1_MQ2: ");
//   Serial.println(CO_1_MQ2);
//   Serial.print("CO_2_MQ2: ");
//   Serial.println(CO_2_MQ2);

//   Serial.println("\nAlcohol Levels:");
//   Serial.print("Alcohol_1_MQ135: ");
//   Serial.println(Alcohol_1_MQ135);
//   Serial.print("Alcohol_2_MQ135: ");
//   Serial.println(Alcohol_2_MQ135);
//   Serial.print("Alcohol_1_MQ7: ");
//   Serial.println(Alcohol_1_MQ7);
//   Serial.print("Alcohol_2_MQ7: ");
//   Serial.println(Alcohol_2_MQ7);
//   Serial.print("Alcohol_1_MQ2: ");
//   Serial.println(Alcohol_1_MQ2);
//   Serial.print("Alcohol_2_MQ2: ");
//   Serial.println(Alcohol_2_MQ2);

//   Serial.println("\nCO2 Levels:");
//   Serial.print("CO2_1_MQ135: ");
//   Serial.println(CO2_1_MQ135);
//   Serial.print("CO2_2_MQ135: ");
//   Serial.println(CO2_2_MQ135);

//   Serial.println("\nToluen Levels:");
//   Serial.print("Toluen_1_MQ135: ");
//   Serial.println(Toluen_1_MQ135);
//   Serial.print("Toluen_2_MQ135: ");
//   Serial.println(Toluen_2_MQ135);

//   Serial.println("\nNH4 Levels:");
//   Serial.print("NH4_1_MQ135: ");
//   Serial.println(NH4_1_MQ135);
//   Serial.print("NH4_2_MQ135: ");
//   Serial.println(NH4_2_MQ135);

//   Serial.println("\nAceton Levels:");
//   Serial.print("Aceton_1_MQ135: ");
//   Serial.println(Aceton_1_MQ135);
//   Serial.print("Aceton_2_MQ135: ");
//   Serial.println(Aceton_2_MQ135);

//   Serial.println("\nH2 Levels:");
//   Serial.print("H2_1_MQ7: ");
//   Serial.println(H2_1_MQ7);
//   Serial.print("H2_2_MQ7: ");
//   Serial.println(H2_2_MQ7);
//   Serial.print("H2_1_MQ2: ");
//   Serial.println(H2_1_MQ2);
//   Serial.print("H2_2_MQ2: ");
//   Serial.println(H2_2_MQ2);

//   Serial.println("\nLPG Levels:");
//   Serial.print("LPG_1_MQ7: ");
//   Serial.println(LPG_1_MQ7);
//   Serial.print("LPG_2_MQ7: ");
//   Serial.println(LPG_2_MQ7);
//   Serial.print("LPG_1_MQ2: ");
//   Serial.println(LPG_1_MQ2);
//   Serial.print("LPG_2_MQ2: ");
//   Serial.println(LPG_2_MQ2);

//   Serial.println("\nCH4 Levels:");
//   Serial.print("CH4_1_MQ7: ");
//   Serial.println(CH4_1_MQ7);
//   Serial.print("CH4_2_MQ7: ");
//   Serial.println(CH4_2_MQ7);

//   Serial.println("\nPropane Levels:");
//   Serial.print("Propane_1_MQ2: ");
//   Serial.println(Propane_1_MQ2);
//   Serial.print("Propane_2_MQ2: ");
//   Serial.println(Propane_2_MQ2);

//   Serial.println("\n--------------------------\n");

//   delay(1000);  // Đợi 1 giây trước khi lặp lại để tránh tràn Serial Monitor
// }
// void debug2(float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8, float data9, float data10, float data11, float data12, int data13, int data14) {
//   // In các giá trị ra Serial Monitor
//   Serial.println("=== Before Filtering (BF) Readings ===");
//   Serial.print("Carbon Monoxide CO BF: ");
//   Serial.println(data1);
//   Serial.print("Hydrocarbons HC BF: ");
//   Serial.println(data2);
//   Serial.print("Carbon Dioxide CO2 BF: ");
//   Serial.println(data3);
//   Serial.print("Ammonia NH3 BF: ");
//   Serial.println(data4);
//   Serial.print("Liquefied Petroleum Gas BF: ");
//   Serial.println(data5);
//   Serial.print("Hydrogen H2 BF: ");
//   Serial.println(data6);

//   Serial.println("\n=== After Treatment (AT) Readings ===");
//   Serial.print("Carbon Monoxide CO AT: ");
//   Serial.println(data7);
//   Serial.print("Hydrocarbons HC AT: ");
//   Serial.println(data8);
//   Serial.print("Carbon Dioxide CO2 AT: ");
//   Serial.println(data9);
//   Serial.print("Ammonia NH3 AT: ");
//   Serial.println(data10);
//   Serial.print("Liquefied Petroleum Gas AT: ");
//   Serial.println(data11);
//   Serial.print("Hydrogen H2 AT: ");
//   Serial.println(data12);

//   // In ra giá trị PM2.5 và PM10
//   Serial.print("PM2.5: ");
//   Serial.print(data13);
//   Serial.print(" µg/m3\tPM10: ");
//   Serial.print(data14);
//   Serial.println(" µg/m3");

//   Serial.println("\n--------------------------\n");
//   delay(2000);
// }


// Hàm gửi dữ liệu qua Serial
void update_data(float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8, float data9, float data10, float data11, float data12, int data13, int data14, int data15) {
  String data = "999," + String(data1, 1) + "," + String(data2, 1) + "," + String(data3, 1) + "," + String(data4, 1) + "," + String(data5, 1) + "," + String(data6, 1) + "," + String(data7, 1) + "," + String(data8, 1) + "," + String(data9, 1) + "," + String(data10, 1) + "," + String(data11, 1) + "," + String(data12, 1) + "," + String(data13) + "," + String(data14) + "," + String(data15) + "\n";
  ESPSerial.print(data);
  Serial.print(data);
}
float formatValue(float value) {
  if (value > 10000) {
    return 9999;  // Nếu giá trị vượt quá 10,000 thì gán bằng 9,999
  } else {
    return value;  // Trả về giá trị gốc nếu nhỏ hơn 100
  }
}
