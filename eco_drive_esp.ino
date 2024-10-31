#include <Wire.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HardwareSerial.h>

#define simSerial Serial2
#define MCU_SIM_BAUDRATE 115200
#define MCU_SIM_TX_PIN 19
#define MCU_SIM_RX_PIN 18

String text1 = "";
String phoneNumber = "0934454166";
//String phoneNumber = "0975247197";
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int progress = 0;
bool alertSent = false;  // Biến cờ để theo dõi trạng thái cảnh báo

SocketIOclient socketIO;
WiFiManager wifiManager;

SoftwareSerial NanoSerial(4, 16);
unsigned long lastScreenUpdate = 0;         // Thời điểm cập nhật màn hình lần cuối
const unsigned long screenInterval = 3000;  // Khoảng thời gian giữa các màn hình (3 giây)
int currentScreen = 0;                      // Biến lưu trạng thái màn hình hiện tại

unsigned long lastUpdate = 0;
String data[16];



void setup() {
  Serial.begin(115200);
  NanoSerial.begin(9600);
  simSerial.begin(MCU_SIM_BAUDRATE, SERIAL_8N1, MCU_SIM_RX_PIN, MCU_SIM_TX_PIN);

  // Check AT Command
  sim_at_cmd("AT");
  sim_at_cmd("ATI");
  sim_at_cmd("AT+CPIN?");
  sim_at_cmd("AT+CSQ");
  sim_at_cmd("AT+CIMI");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(20, 45);
  display.print("System Starting");

  // Vẽ khung cho thanh tiến trình
  display.drawRect(0, 10, 128, 20, SSD1306_WHITE);

  // Vẽ thanh tiến trình
  display.drawRect(10, 15, progress, 10, SSD1306_WHITE);

  display.display();

  // Tăng tiến trình hoặc reset lại khi hoàn thành
  while (progress < 108) {
    progress++;
    delay(10);
    // Vẽ khung cho thanh tiến trình
    display.drawRect(0, 10, 128, 20, SSD1306_WHITE);

    // Vẽ thanh tiến trình
    display.drawRect(10, 15, progress, 10, SSD1306_WHITE);

    display.display();
  }
  delay(1000);
  display.clearDisplay();
  Serial.println("wifi connecting");
  wifiManager.autoConnect("Exhaust gas emission measurement box for cars", "66668888");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //ket noi server
  socketIO.begin("moitruongxanh.edu.vn", 80, "/socket.io/?EIO=4");  //socket.io/?EIO=4
  socketIO.onEvent(socketIOEvent);
}

void loop() {
  socketIO.loop();  // duy trif keet noi
  nhandulieu();

  unsigned long currentMillis = millis();
  if (currentMillis - lastScreenUpdate >= screenInterval) {
    lastScreenUpdate = currentMillis;  // Cập nhật thời gian của lần thay đổi gần nhất

    // Chuyển màn hình
    currentScreen++;
    if (currentScreen > 2) {
      currentScreen = 0;  // Quay về màn hình đầu tiên nếu đã vượt qua các màn hình hiện có
      sendData1();
      sendData2();
      Warning();
      extractPhoneNumber(text1);
      Serial.println(phoneNumber);
      

      // Serial.println(phoneNumber);
    }

    display.clearDisplay();

    switch (currentScreen) {
      case 0:
        // Màn hình 1
        displayScreen1();
        break;
      case 1:
        // Màn hình 2
        displayScreen2();
        break;
      case 2:
        // Màn hình 3
        displayScreen3();
        break;
    }
  }
}

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  text1 = (char *)payload;
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);
      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      Serial.println(text1);



      break;
    case sIOtype_ACK:
      // Serial.printf("[IOc] get ack: %u\n", length);
      //hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      //Serial.printf("[IOc] get error: %u\n", length);
      ////hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      // Serial.printf("[IOc] get binary: %u\n", length);
      //hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      // Serial.printf("[IOc] get binary ack: %u\n", length);
      //hexdump(payload, length);
      break;
  }
}
void sendData1() {
  DynamicJsonDocument doc(1024);  // Giảm kích thước bộ nhớ cho từng thông điệp
  JsonArray array1 = doc.to<JsonArray>();
  array1.add("message1");
  JsonObject param1 = array1.createNestedObject();
  param1["ID"] = data[0];  // ID =999
  param1["CO_BF"] = data[1];
  param1["HC_BF"] = data[2];
  param1["CO2_BF"] = data[3];
  param1["NH3_BF"] = data[4];
  param1["LPG_BF"] = data[5];
  param1["H2_BF"] = data[6];

  String output1;
  serializeJson(doc, output1);
  socketIO.sendEVENT(output1);
   Serial.println("Data1 sent: " + output1);  // Để debug
}

void sendData2() {
  DynamicJsonDocument doc(1024);  // Giảm kích thước bộ nhớ cho từng thông điệp
  JsonArray array2 = doc.to<JsonArray>();
  array2.add("message2");
  JsonObject param2 = array2.createNestedObject();

  param2["H2_BF"] = data[6];
  param2["CO_AT"] = data[7];
  param2["HC_AT"] = data[8];
  param2["CO2_AT"] = data[9];
  param2["NH3_AT"] = data[10];
  param2["LPG_AT"] = data[11];
  param2["H2_AT"] = data[12];
  param2["PM25"] = data[13];
  param2["PM10"] = data[14];
  param2["PM1"] = data[15];
  param2["SDT"] = "099999999";  // phoneNumber;

  String output2;
  serializeJson(doc, output2);
  socketIO.sendEVENT(output2);
   Serial.println("Data2 sent: " + output2);  // Để debug
}


void nhandulieu() {
  if (NanoSerial.available()) {                              // Kiểm tra xem có dữ liệu đang được gửi đến không
    String receivedData = NanoSerial.readStringUntil('\n');  // Đọc chuỗi dữ liệu cho đến khi gặp ký tự '\n'
    Serial.println(receivedData);
    // Tách dữ liệu bằng dấu phẩy
    socketIO.loop();
    int startIndex = 0;
    int commaIndex;

    for (int i = 0; i < 16; i++) {
      commaIndex = receivedData.indexOf(',', startIndex);
      if (commaIndex != -1) {
        data[i] = receivedData.substring(startIndex, commaIndex);
        startIndex = commaIndex + 1;
      } else {
        data[i] = receivedData.substring(startIndex);
        break;
      }
    }
  }
}



void displayScreen1() {
  display.clearDisplay();
  int rows = 4;
  int cols = 2;
  int cellWidth = SCREEN_WIDTH / cols;
  int cellHeight = SCREEN_HEIGHT / rows;

  for (int i = 1; i < cols; i++) {
    display.drawLine(i * cellWidth, 0, i * cellWidth, SCREEN_HEIGHT, SSD1306_WHITE);
  }
  for (int i = 1; i < rows; i++) {
    display.drawLine(0, i * cellHeight, SCREEN_WIDTH, i * cellHeight, SSD1306_WHITE);
  }

  display.setCursor(15, 3);
  display.print("INPUT");
  display.setCursor(75, 3);
  display.print("OUTPUT");
  display.setCursor(3, 19);
  display.print("CO:");
  display.print(data[1]);
  display.setCursor(69, 19);
  display.print("CO:");
  display.print(data[7]);
  display.setCursor(3, 35);
  display.print("LPG:");
  display.print(data[8]);
  display.setCursor(69, 35);
  display.print("LPG:");
  display.print(data[11]);
  display.setCursor(3, 51);
  display.print("H2:");
  display.print(data[6]);
  display.setCursor(69, 51);
  display.print("H2:");
  display.print(data[12]);
  display.display();
}
void displayScreen2() {
  display.clearDisplay();
  int rows = 4;
  int cols = 2;
  int cellWidth = SCREEN_WIDTH / cols;
  int cellHeight = SCREEN_HEIGHT / rows;

  for (int i = 1; i < cols; i++) {
    display.drawLine(i * cellWidth, 0, i * cellWidth, SCREEN_HEIGHT, SSD1306_WHITE);
  }
  for (int i = 1; i < rows; i++) {
    display.drawLine(0, i * cellHeight, SCREEN_WIDTH, i * cellHeight, SSD1306_WHITE);
  }

  display.setCursor(15, 3);
  display.print("INPUT");
  display.setCursor(75, 3);
  display.print("OUTPUT");
  display.setCursor(3, 19);
  display.print("HC:");
  display.print(data[2]);
  display.setCursor(69, 19);
  display.print("HC:");
  display.print(data[8]);
  display.setCursor(3, 35);
  display.print("CO2:");
  display.print(data[3]);
  display.setCursor(69, 35);
  display.print("CO2:");
  display.print(data[9]);
  display.setCursor(3, 51);
  display.print("NH3:");
  display.print(data[4]);
  display.setCursor(69, 51);
  display.print("NH3:");
  display.print(data[10]);
  display.display();
}

void displayScreen3() {
  display.clearDisplay();
  // Vẽ các đường dọc
  for (int i = 1; i < 2; i++) {
    display.drawLine(i * 128, 0, i * 128, SCREEN_HEIGHT, SSD1306_WHITE);
  }

  // Vẽ các đường ngang
  for (int i = 1; i < 4; i++) {
    display.drawLine(0, i * 16, SCREEN_WIDTH, i * 16, SSD1306_WHITE);
  }
  display.setCursor(45, 5);
  display.print("OUTPUT");
  display.setCursor(8, 21);
  display.print("PM 1.0:");
  display.print(data[15]);
  display.print(" ug/m3");
  display.setCursor(8, 37);
  display.print("PM 2.5: ");
  display.print(data[13]);
  display.print(" ug/m3");
  display.setCursor(8, 53);
  display.print("PM 10.0: ");
  display.print(data[14]);
  display.print(" ug/m3");
  display.display();
}



void sim_at_wait() {
  delay(100);
  while (simSerial.available()) {
    Serial.write(simSerial.read());
  }
}

bool sim_at_cmd(String cmd) {
  simSerial.println(cmd);
  sim_at_wait();
  return true;
}

void sim_at_send(char c) {
  simSerial.write(c);
}

void send_sms(String msg) {
  sim_at_cmd("AT+CMGF=1");
  String temp = "AT+CMGS=\"";
  temp += phoneNumber;
  temp += "\"";
  sim_at_cmd(temp);
  sim_at_cmd(msg);
  sim_at_send(0x1A);  // Kết thúc tin nhắn SMS
}



void Warning() {
  String warningMessage = "Warning! Values exceeded: ";
  bool shouldSendWarning = false;
  warningMessage = "The exhaust emissions quality does not meet the requirements.";
  // Kiểm tra các phần tử từ data[1] đến data[6]
  for (int i = 1; i <= 6; i++) {
    if (data[i].toFloat() > 8000) {
      shouldSendWarning = true;
    }
  }

  // Kiểm tra các phần tử từ data[13] đến data[15]
  for (int i = 13; i <= 15; i++) {
    if (data[i].toFloat() > 800) {
      shouldSendWarning = true;
    }
  }

  // Gửi cảnh báo nếu có giá trị vượt ngưỡng và cảnh báo chưa được gửi
  if (shouldSendWarning && !alertSent) {
    send_sms(warningMessage);
    Serial.println("Warning SMS sent: " + warningMessage);
    alertSent = true;  // Đặt cờ đã gửi cảnh báo
  }

  // Kiểm tra nếu tất cả các giá trị đã trở lại bình thường để đặt lại cờ
  if (!shouldSendWarning && alertSent) {
    Serial.println("All values are back to normal.");
    alertSent = false;  // Đặt lại cờ để cho phép gửi cảnh báo lần sau
  }
}
String extractPhoneNumber(String jsonString) {
  const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(1) + 60;  // Tăng kích thước bộ nhớ để tránh thiếu bộ nhớ
  DynamicJsonDocument doc(capacity);

  // Phân tích chuỗi JSON
  DeserializationError error = deserializeJson(doc, jsonString);

  // Kiểm tra lỗi khi phân tích cú pháp
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return "";  // Trả về chuỗi rỗng nếu có lỗi
  }

  // Kiểm tra xem trường "phone" có tồn tại hay không
  if (!doc[1].is<JsonObject>() || !doc[1]["phone"]) {
    Serial.println("Phone field is missing in JSON.");
    return "";  // Trả về chuỗi rỗng nếu không tìm thấy trường "phone"
  }

  // Truy xuất và trả về số điện thoại
  const char *phoneNumber = doc[1]["phone"];
  return String(phoneNumber);
}
