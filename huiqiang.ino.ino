//  转载请注明出处，www.wenzheng.club  阿正原创。

#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN A0
#define motor1pin1 4                              //定义IN1引脚
#define motor1pin2 5                              //定义IN2引脚
#define motor1pwm 11                             //定义ENA引脚
int low_flag =0;
int high_flag =0;
int set_flag =0;
int wendu =26;
int wendu_high = 30;
int wendu_low =15;
int flag = 0;
unsigned long Last_KeyTime = 0;
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ A4, /* data=*/ A5, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
double Fahrenheit(double celsius) 
{
        return 1.8 * celsius + 32;
}    //摄氏温度度转化为华氏温度

double Kelvin(double celsius)
{
        return celsius + 273.15;
}     //摄氏温度转化为开氏温度

// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}
// 快速计算露点，速度是5倍dewPoint()
// 参考: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}
void setup(void)
{
  Serial.begin(9600);
  pinMode(6,INPUT);
  pinMode(12,OUTPUT);
  digitalWrite(6,LOW);
  u8x8.begin();
  u8x8.setPowerSave(0); 
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(3,1,"JD-151");
  u8x8.drawString(5,5,"DHQ 201810");
  delay(1000);
  u8x8.clear();
  u8x8.drawString(3,1,"J_D 151");
  u8x8.drawString(1,3,"T_H: ");
  u8x8.drawString(1,4,"T_L: ");
   u8x8.setCursor(5, 3);
  u8x8.print(wendu);
  u8x8.setCursor(5, 4);
  u8x8.print(wendu);
}
void led(){
    digitalWrite(12,HIGH);
    delay(100);
    digitalWrite(12,LOW);
  }
void seting()
{
  if(digitalRead(6) == 1){
    led();
    set_flag = set_flag +1 ;
    if (set_flag == 3) set_flag = 0; 
    u8x8.drawString(3,1,"Seting!!!");
  }
  if(digitalRead(7) == 1)
  {
    Serial.print(set_flag);
   led();
   if(set_flag == 0)  u8x8.drawString(2,2,"please set!!!");
   if(set_flag == 1){
   u8x8.drawString(3,2,"   UP!!!   ");
   wendu_low =wendu_low +1;  
   }
   if (set_flag == 2){
   u8x8.drawString(3,2,"   UP!!!   ");
   wendu_high =wendu_high +1;  
   } 
  }
  if(digitalRead(8) == 1)
  {
    led();
  if(set_flag == 0)  u8x8.drawString(2,2,"please set!!!");
  if(set_flag == 1){
   u8x8.drawString(3,2,"   DOWN!!!    ");
   wendu_low =wendu_low -1;  
   }
   if (set_flag ==2){
   u8x8.drawString(3,2,"   DOWN!!   !");
   wendu_high =wendu_high -1; 
    }
  }
  }
  void panduan(){
    if((float)DHT11.temperature < wendu_low){
        motor(motor1pin1,motor1pin2,motor1pwm,80);                 
      }
    if((float)DHT11.temperature>wendu_low && (float)DHT11.temperature < wendu_high ){
       motor(motor1pin1,motor1pin2,motor1pwm,150);                 
      }
    if((float)DHT11.temperature > wendu_high){
       motor(motor1pin1,motor1pin2,motor1pwm,255);                
      }
    
    }
void loop(void)
{
  int chk = DHT11.read(DHT11PIN);
  seting();
  panduan();
  u8x8.setCursor(5, 3);
  u8x8.print(wendu_high);
  u8x8.setCursor(5, 4);
  u8x8.print(wendu_low);
  u8x8.setCursor(9, 7);
  u8x8.print((float)DHT11.humidity);
  u8x8.setCursor(1, 7);
  u8x8.print((float)DHT11.temperature);

}
void motor(int motorpin1,int motorpin2,int motorpwm,int val)  //定义一个电机转动函数
{
pinMode(motorpin1,OUTPUT);                              //输出第一个引脚
pinMode(motorpin2,OUTPUT);                             //输出第二个引脚
digitalWrite(motorpin2,0);                                    //将第二个引脚置低
digitalWrite(motorpin1,1);                                     //将第一个引脚抬高
analogWrite(motorpwm,val);                                //给EN引脚设定模拟值，设定转速
}
