# LTE-Mリーフ 利用ガイド(TCP,UDPライブラリ)

## TCP,UDPライブラリ

### 利用できる機能
このライブラリはIPv4上で次のような通信通信を行うことができます  
・TCP socket通信  
・UDP通信  

### サンプルスケッチ
ArduinoIDEの次の場所にサンプルスケッチがインストールされていますので参考にしてください  
[スケッチ例]->[stp32lpwa]->[LPWA]  
・TcpWebClient -- TCP通信  
・UdpNtpClient -- UDP通信  

### TCP socket通信
ライブラリを用いてTCP socket通信を行う方法を説明します  
Arduino のEternet Client classに準拠したAPIを提供していますので既存のTCP利用スケッチから流用することができます  
参照： https://www.arduino.cc/en/Reference/ClientConstructor  

#### ソケット接続開始、切断
connect()を使うとTCP ソケットをオープンすることができます  
またstop()によりソケットをクローズします  
```
LpwaClient client;
char server[] = "example.org";
int port = 80; // port 80 is the default for HTTP

client.connect(server, port);

client.stop();
```

#### TCPデータ送信
ASCIIデータ送信でしたらprint(),println()を使うことができます
```
char data[] = "GET /";

client.print("GET ");
client.print(path);
client.println(" HTTP/1.1");
client.print("Host: ");
client.println(server);
client.println("Connection: close");
client.println();
```
またデータ列の場合はwrite()で送信もできます。


#### TCPデータ受信
受信データが到着しているかどうかをavailable()で取得できます  
まだ受信データについてはread()にて取得できます  
```
if (client.available()) {
  char c = client.read();
  Serial.print(c);
}
```


### UDP通信
ライブラリを用いてUDP通信を行う方法を説明します  
Arduino のEthernetUDP classに準拠したAPIを提供していますので既存のTCP利用スケッチから流用することができます  
参照： https://www.arduino.cc/en/Reference/EthernetUDPBegin

#### UDPクラスの利用
LpwaUdp クラスを定義して begin()を呼び出します  
引数にポート番号を設定することもできます  
```
LpwaUdp Udp;

Udp.begin();

```

#### UDPデータ受信
parsePacket()でデータ受信の確認ができます  
受信データがある場合はread()にてデータの取得を行います  
read()の引数に指定がない場合は1バイト単位の読み出しとなります  

```
  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, packetSize); // read the packet into the buffer
```

#### UDPデータ受信
beginPacket()で送信相手のURLとポート番号を指定し、write()で送信データのセット  
endPacket()で実際にデータ送信が行われます  
```
  Udp.beginPacket(host, port);
  Udp.write(packetBuffer, packetSize);
  Udp.endPacket();
```
