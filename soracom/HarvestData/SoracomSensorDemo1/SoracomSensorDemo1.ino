/**
 * @file SoracomSensorDemo.ino
 * @brief SORACOM binary parser を使って気温、湿度、気圧を送信<br>
 * 
 * @date  2020/5/20 create
 * @date  2021/2/2  update for LTE-M leaf Ver.3
 * @author kt-nakamura@kddi-tech.com
 */

/** SORACOM クラウドの設定
 *  バイナリパーサー,Harvest Dataのみ設定
 *　送信エンドポイント harvest.soracom.io:8514
 *  
 *  バイナリパーサー設定、次のいずれかを設定画面に入力してください
 *  temp:0:uint:16:/100
 *  hum:2:uint:16:/10 
 *  press:4:uint:16:
 *  
 *  Harvest Dataを有効→グラフ表示
 */

/** このサンプルコードは次のライブラリをインストールしてください
 * ・Grove - Barometer Sensor (BME280)
 * https://github.com/Seeed-Studio/Grove_BME280
 * インストール後にSeeed_BME280.hを次のように編集
 * #define BME280_ADDRESS   0x76
 */

/* 
 * TCP,UDP接続選択 
 */
//#define USE_UDP

#include "Seeed_BME280.h"
#include <Wire.h>
#include <Lpwa.h>
#include "arduino_secrets.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GPRS gprs;
Lpwa gsmAccess;
LpwaAccess lpwaAccess;
LpwaCtrl pmctrl;
BME280 bme280;

// connection state
bool connected;
bool valid_sensor;

#ifdef USE_UDP
// A UDP instance to let us send and receive packets over UDP
LpwaUdp Udp;

/**
 * SORACOM HARVEST送信（UDP)<br>
 * 送信エンドポイント harvest.soracom.io:8514
 * binary parserのフォーマットは "temp:0:uint:16:/100 hum:2:uint:16:/10 press:4:uint:16:"
 * @param 送信データ
 * @return なし
 */
bool soracom_send(int temp, int hum, int hpa) {
  Serial.println("@@@@@ soracom_send(UDP): ");

  Udp.begin();
  uint8_t sendBytes[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

  // 送信データの準備
  // データ列のデフォルトはビッグエンディアン
  sendBytes[0] = temp >> 8;
  sendBytes[1] = temp & 0xff;
  sendBytes[2] = hum >> 8;
  sendBytes[3] = hum & 0xff;
  sendBytes[4] = hpa >> 8;
  sendBytes[5] = hpa & 0xff;

  Serial.print("@@@@@@ senddata:");
  Serial.print(sendBytes[0],HEX);
  Serial.print(",");
  Serial.print(sendBytes[1],HEX);
  Serial.print(",");
  Serial.print(sendBytes[2],HEX);
  Serial.print(",");
  Serial.print(sendBytes[3],HEX);
  Serial.print(",");
  Serial.print(sendBytes[4],HEX);
  Serial.print(",");
  Serial.print(sendBytes[5],HEX);
  Serial.println("");

  // UDPライブラリを初期化
  if (Udp.begin(LOCAL_PORT)) {
    Serial.println("@@@@@@ UDP init sucess");
  } else {
    // 失敗した場合は関数から抜ける
    Serial.println("@@@@@@ UDP init fail");
    return false;
  }
  // SORACOMサーバにデータ列をUDP送信
  Udp.beginPacket(ENDPOINT_URL, ENDPOINT_PORT);
  if (!Udp.write(sendBytes,sizeof(sendBytes))) {
      Serial.println("UDP write failed");
      return false;
  }
  Udp.endPacket();
  
  Udp.stop();
  Serial.println("UDP send OK");
  return true;
}

#else // USE_UDP
LpwaClient Client;
/**
 * SORACOM HARVEST送信（TCP)<br>
 * 送信エンドポイント harvest.soracom.io:8514
 * binary parserのフォーマットは "temp:0:uint:16:/100 hum:2:uint:16:/10 press:4:uint:16:"
 * @param 送信データ
 * @return なし
 */
bool soracom_send(int temp, int hum, int hpa) {
  Serial.println("@@@@@ soracom_send_tcp(TCP): ");
  uint8_t sendBytes[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

  // 送信データの準備
  // データ列のデフォルトはビッグエンディアン
  sendBytes[0] = temp >> 8;
  sendBytes[1] = temp & 0xff;
  sendBytes[2] = hum >> 8;
  sendBytes[3] = hum & 0xff;
  sendBytes[4] = hpa >> 8;
  sendBytes[5] = hpa & 0xff;

 
  // TCPソケットをオープン
  if (!Client.connect(ENDPOINT_URL,ENDPOINT_PORT)) {
    Serial.println("TCP connection failed!");
    return false;
  } else {
    Serial.println("TCP connected");

    // TCP 送信
    if (!Client.write(sendBytes,sizeof(sendBytes))) {
      Serial.println("TCP write failed");
      return false;
    }

    // TCPソケットをクローズ
    Client.stop();
  }
  Serial.println("TCP send OK");
  return true;
}
#endif // USE_UDP

/**
 * Arduinoアプリ初期化<br>
 * @param なし
 * @return なし
 */
void setup() {
  Serial.begin(115200);
#ifdef USBD_USE_CDC
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif //  USBD_USE_CDC

  // set I2C pin
  Wire.setSDA(I2C2_SDA);
  Wire.setSCL(I2C2_SCL);
 
  delay(1000);
  Serial.println("Starting SORACOM demo.");
  
  // connection state
  connected = false;
  valid_sensor = false;
  
  // After starting the modem with LpwaAccess.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((gsmAccess.begin() == LPWA_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("connecting.");
      delay(1000);
    }
  }

  Serial.println("LPWA connected");

  pmctrl.enableSensor(true);
  if(!bme280.init()){
    Serial.println("Device error!");
  } else {
    valid_sensor = true;
  }
}

/**
 * Arduinoアプリタスク<br>
 * @param なし
 * @return なし
 */
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("@@@@ loop() start");

  if (connected) {
    pmctrl.setLed(false, true, false); // Green
    int temp = (int)(bme280.getTemperature()*100);
    Serial.print("Temp: ");
    Serial.println(temp);
    int hum = (int)(bme280.getHumidity()*10);
    Serial.print("Humidity: ");
    Serial.println(hum);
    int hpa = (int)(bme280.getPressure()/100);
    Serial.print("Pressure: ");
    Serial.println(hpa);
    if(!soracom_send(temp, hum, hpa)) {
      pmctrl.setLed(true, false, false);  // Red
    } else {
      pmctrl.setLed(false, false, false);
    }
  }
  
  delay(30000); // データを流しすぎないよう待機
  Serial.println("@@@@ loop() end");
}
