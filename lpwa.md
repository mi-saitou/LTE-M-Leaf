# LTE-Mリーフ 利用ガイド(LPWAライブラリ)

## LPWAライブラリ

### 利用できる機能
このライブラリはLTE-MリーフをLPWA網に接続するものです    
・LPWA網接続、切断
・ネットワークサービス検出、電波強度取得  
・SIM状態取得  

### サンプルスケッチ
ArduinoIDEの次の場所にサンプルスケッチがインストールされていますので参考にしてください  
[スケッチ例]->[stp32lpwa]->[LPWA]  
・HttpGet,HttpPost,HttpPut,HttpDelete -- HTTP(S)通信  
・TcpWebClient -- TCP通信  
・UdpNtpClient -- UDP通信  
・ScanNetworks -- ネットワークサービス検出、電界強度取得  
・SIMcheck -- SIM状態取得  

### LPWA網接続、切断
ライブラリを用いてLPWA網接続、切断を行う方法を説明します  

#### デバイス起動
LPWAデバイスを起動する場合は次のようなコードを使用します  
```
LpwaAccess lpwaAccess;

  // LPWAデバイスの起動
  bool lpwa_enabled = false;
  while (!lpwa_enabled) {
    if (lpwaAccess.begin() == LPWA_READY) {
      lpwa_enabled = true;
    } else {
      Serial.println("starting LPWA device.");
      delay(1000);
    }
  }
```
begin()を呼ぶとLPWAデバイスの電源をオンを行い、LPWAデバイスが起動すると "LPWA_READY"が返ります  
もし失敗した場合は"LPWA_FAIL"が返ります  

\<tips\> LPWAデバイス起動時のメッセージ  
LPWAデバイス起動時はシリアルポートに動作状況のメッセージが出力されます  

"<info> EPS: trying to attach or searching"  
 基地局との接続中  
"<info> EPS: Registered, home network"  
 基地局との接続成功  
"<info> EPS: Not registered "  
 基地局との接続断  
"<info> EPS: Registration denied"  
 基地局との失敗  
  

#### LPWA網接続
LPWA網への接続を行う場合は次のようなコードを使用します  
APN等の設定値は付属のSORACOM SIMの値となっています  
```
// APN data
#define SECRET_GPRS_APN "soracom.io" // replace your GPRS APN
#define SECRET_GPRS_LOGIN "sora"     // replace with your GPRS login
#define SECRET_GPRS_PASSWORD "sora"  // replace with your GPRS password

const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

GPRS gprs;

  bool pdn_connected = false;
  while (!pdn_connected) {
    if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY) {
      pdn_connected = true;
    } else {
      Serial.println("connecting.");
      delay(1000);
    }
  }
  Serial.println("LPWA connected");
```
attachGPRS()を呼ぶとLPWA網(SORACOM回線)への接続を試み、接続が成功すると "GPRS_READY"が返ります
もし失敗した場合は"LPWA_FAIL"が返ります  

\<tips\> 接続/切断時のメッセージ  
LPWA接続時はシリアルポートに接続状況のメッセージが出力されます  
"<info> PDP: Connecting"  
 LPWA接続中  
"<info> PDP: Connected"  
 LPWA接続完了  
"<info> PDP: Failed to connect"  
 LPWA接続失敗  
"<info> PDP: Disconnected due to network"  
 LPWA網から切断  
"<info> PDP: Closed"  
 接続終了  

### ネットワークサービス検出、電波強度取得  

#### 通信オペレータ取得
現在接続可能な通信オペレータ(キャリア)を取得します
```
LpwaScanner scannerNetworks;

  scannerNetworks.begin();

  // currently connected carrier
  Serial.print("Current carrier: ");
  Serial.println(scannerNetworks.getCurrentCarrier());
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  
ボード添付のSIMの場合は"KDDI"が取得できます  

#### 電波強度取得
現在接続中の電波強度を取得します  
```
LpwaScanner scannerNetworks;

  scannerNetworks.begin();

  // returns strength and ber
  // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
  Serial.print("Signal Strength: ");
  Serial.print(scannerNetworks.getSignalStrength());
  Serial.println(" [0-31]");
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  
もし値が99の場合は圏外ですので設置場所を変えるようにしてください  

#### ネットワーク時間取得
現在接続中のLPWA網から時間を取得します  
```
LpwaModem lpwaModem;

  // network time
  Serial.print("Network Time: ");
  Serial.println(lpwaModem.getTime());
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  
通信オペレータが基地局から配信している時間ですのでNTP時間とは異なることに注意してください  

#### IMEI取得
LPWAデバイスの個体番号(IMEI)を取得します
```
LpwaModem lpwaModem;
String IMEI = "";

  // get modem parameters
  // IMEI, modem unique identifier
  Serial.print("Modem IMEI: ");
  IMEI = lpwaModem.getIMEI();
  if (IMEI != NULL) {
    Serial.println(IMEI);
  }
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  


### SIM状態取得  

#### ICCID取得
SIM個体識別番号であるICCIDを取得します  
```
LpwaModem lpwaModem;

  // IMEI, modem unique identifier
  Serial.print("SIM ICCID: ");
  String iccdid = lpwaModem.getIccid();
  if (iccdid != NULL) {
    Serial.println(iccdid);
  }
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  

#### PIN確認
取り付けたSIMがロックされているか確認します  
```
LpwaPin pinManager;
  pinManager.begin();
  // check if the SIM have pin lock
  switch (pinManager.isPIN()) {
  case 0:
    Serial.println("PIN lock is off.");
    break;
  case 1:
    Serial.println("PIN code locked.");
    break;
  case -1:
    Serial.print("PIN and PUK code locked.");
    break;
  case -2:
    // the worst case, PIN and PUK are locked
    Serial.println("PIN and PUK locked. Use PIN2/PUK2 in a mobile phone.");
```
取得の際は予めlpwaAccess.begin()によるデバイス起動を行っておいてください  
