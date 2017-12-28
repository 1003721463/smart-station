#include <SoftwareSerial.h>
#include "DHT.h"
#include "ESP8266.h"
#include "I2Cdev.h" 
#define DHTPIN  A1 
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
unsigned int dutyCycle;  //占空比
unsigned int tempMin = 15;  //零速温度，设为串口观察到的环境温度
unsigned int tempMax = 35;  //满速温度，设为串口观察到的手握元件温度
#include <RSCG12864B.h>
#include <Wire.h>  
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "20460XXX"//设备ID，改
#define PROJECTID "106XXX"//项目ID，改
#define HOST_PORT   (80)
String jsonToSend;
int temp, hum, val;                    //传感器温度、湿度、光照   
char  temptem_c[7], hum_c[7], val_c[7] ;    //换成char数组传输
String postString;
String apiKey="5nMP0c6Wu0bvR0NknOv2FhhXXXX= ";//API密匙
char buf[10];
// GY-30  
// BH1750FVI  
// in ADDR 'L' mode 7bit addr  
#define ADDR 0b0100011  
// addr 'H' mode  
// #define ADDR 0b1011100  

SoftwareSerial mySerial(3, 2); // RX, TX

int sounder = 5;
void setup()
{
// Open serial communications and wait for port to open:
Serial.begin(115200);
while (!Serial) {
; // wait for serial port to connect. Needed for Leonardo only
}
Serial.println("Goodnight moon!");
Wire.begin();  
  Wire.beginTransmission(ADDR);  
  Wire.write(0b00000001);  
  Wire.endTransmission();  
// set the data rate for the SoftwareSerial port
mySerial.begin(115200);
mySerial.println("Hello, world?");
pinMode(11,OUTPUT);  //设置led电平输出口
pinMode(13,OUTPUT);  //设置led电平输出口
pinMode(9,OUTPUT); //设置led电平输出口
pinMode(6,OUTPUT); //设置led电平输出口
pinMode(4,OUTPUT); //设置led电平输出口
pinMode(sounder,INPUT);//设置音量输入

}
void loop() // run over and over
{ RSCG12864B.begin();//串口初始化
   RSCG12864B.brightness(255);//屏幕亮度
  RSCG12864B.clear();//每次循环清空一次屏幕
   RSCG12864B.font_revers_on();
 
  
   RSCG12864B.font_revers_off();
   

  
  
  
  delay(200); 
 int hum = dht.readHumidity();//读取湿度
 char humlcd[10];
   itoa(hum, humlcd, 10);//c语言函数，用于将数字转换为字符串
    RSCG12864B.print_string_12_xy(0,45,"hum:");
   RSCG12864B.print_string_12_xy(70,45,humlcd);
   RSCG12864B.print_string_12_xy(90,45,"%");//显示
  
  int temp = dht.readTemperature();//读取温度
  char templcd[10];
   itoa(temp, templcd, 10);
   RSCG12864B.print_string_12_xy(0,0,"temp:");
      RSCG12864B.print_string_12_xy(70,0,templcd);
      RSCG12864B.print_string_12_xy(90,0,"^C");
      int val = 0; 

  // Read temperature as Fahrenheit (isFahrenheit = true)
  int f = dht.readTemperature(true);
    //获取光照
    if (isnan(hum) || isnan(temp) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
    Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %\t");
  if (hum>= 10 )                         //对湿度进行判断
        digitalWrite(11,HIGH);
    else
        digitalWrite(11,LOW);
        
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" *C ");
  if (temp>= 36 )                         //对温度进行判断
        digitalWrite(13,HIGH);
    else
        digitalWrite(13,LOW);
        
        delay(100);
        if (temp < tempMin){
      dutyCycle = 0;
      }
  else if (temp < tempMax){
      dutyCycle = (temp-tempMin)*255/(tempMax-tempMin);
      }                                                 //按照占空比计算电机转速
  else {
      dutyCycle = 255;
      }
  analogWrite(10,dutyCycle);  //产生PWM

  
sound1();//执行读取声音判断是否音量过高的的函数，写在后面了

delay(1000); 
  // 初始化光强模块  
  Wire.beginTransmission(ADDR);  
  Wire.write(0b00000111);  
  Wire.endTransmission();  
   
  Wire.beginTransmission(ADDR);  
  Wire.write(0b00100000);  
  Wire.endTransmission();  
  // typical read delay 120ms  
  delay(120);  
  Wire.requestFrom(ADDR, 2); // 2byte every time  
  for (val = 0; Wire.available() >= 1; ) {  
    char c = Wire.read();  
    Serial.println(c, HEX);  
    val = (val << 8) + (c & 0xFF);  
  }  
  val1 = val / 0.012;  //获取电压并换算成光通量
        

      val=val1;


   
  
  delay(100);
  if (val>= 150 )                         //对光强进行判断
       { digitalWrite(6,HIGH);
     
       }
    else
        {digitalWrite(6,LOW);
       }
       char vallcd[10];
   itoa(val1, vallcd, 10);
   RSCG12864B.print_string_12_xy(0,15,"light:");
   RSCG12864B.print_string_12_xy(50,15,vallcd);
   RSCG12864B.print_string_12_xy(90,15,"cdl"); 

  

  
   
        

delay(200);
 RSCG12864B.clear();  
RSCG12864B.print_string_12_xy(20,45,"Updating");
RSCG12864B.print_string_12_xy(20,62,"PLEASE WAIT");//由于程序无法烧写到硬件里，所以只能一遍一遍的执行，
                                                   //这里显示正在上传，请稍后

Serial.println("exit pass-through mode");//每次循环重新初始化ESP8266
mySerial.print("+++");
digitalWrite(4,HIGH);//发送一条指令，闪一下灯，说明有数据传输
delay(100);
digitalWrite(4,LOW);
delay(50);//根据不同种类的指令类型，要留出不同的时间等待完成
mySerial.print("AT+CWMODE=3\r\n");//设置模式3，STA+AP，其实我们只用了STA模式
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(100);
mySerial.print("AT+RST\r\n");//让模块复位，能在接收信息最下面看到 ready，说明模块复位成功。复位后就是STA+AP模式了
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(100);
mySerial.print("AT+CWJAP=\"ohfuck\",\"shalebaji\"\r\n");//输入你无线路由器的用户名和密码，一定要写正确，注意大小写
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(3000);

mySerial.print("AT+CIFSR\r\n");//查询IP，因为有STA和AP两个模式，所以有两组MAC和IP，常见的是192.168.x.x局域网IP
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(100);
mySerial.print("AT+CIPMUX=0\r\n");//设置单链接，透传只能在单连接模式下进行，所以在建立连接之前一定要用（AT+CIPMUX=0 设置单连接）
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(100);
mySerial.print("AT+CIPMODE=1\r\n");//设置透传模式，ESP8266要求必须透传
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
Serial.println("TCP connect begin");
mySerial.print("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n");//创建TCP链接，具体格式参照官方文档
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(200);
Serial.println("TCP connect begin");
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(500);
mySerial.print("AT+CIPSEND\r\n");//启动透传发送，ESP8266上传准备完成，利用AT指令只能完成到这一步
digitalWrite(4,HIGH);
delay(100);
digitalWrite(4,LOW);
delay(50);
Serial.println("uploading data......");
updateSensorData();


}

void updateSensorData() {
  
    Serial.print("create tcp ok\r\n");//tcp链接已经用AT指令的方式创建，具体格式参照官方文档

jsonToSend="{\"Temperature\":";
    dtostrf(temp,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Humidity\":";
    dtostrf(hum,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Light\":";
    dtostrf(val,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+="}";



    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
  
  delay(500);
}
void sound1()
{int sound = digitalRead(sounder);//读取音量
if (sound==0){
   digitalWrite(9,HIGH); 
   
   RSCG12864B.print_string_12_xy(0,30,"voice:");//显示
      RSCG12864B.print_string_12_xy(70,30,"Attention");//如果音量超出阈值，显示“注意”
   }
   else{
   digitalWrite(9,LOW);
   RSCG12864B.print_string_12_xy(0,30,"voice:");
      RSCG12864B.print_string_12_xy(70,30,"No one!");}}//如果音量超出阈值，显示“没人”
