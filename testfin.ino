#include <SoftwareSerial.h>
#include <DHT.h>    //온습도 라이브러리 불러옴
SoftwareSerial BTSerial(0, 1);
#define DHTPIN A2 //온습도 핀 설정
#define DHTTYPE DHT11 //DHT22센서 종류 설정
DHT dht(DHTPIN, DHTTYPE);
int gassensor = A3;


int count, count2 = 0;           //카운트 값
int pee_alarm = 0;    //오줌 발생시 1로 증가되는 값
int poo_alarm = 0;    //똥 발생시 1로 증가되는 값 ( 안쓰였음)
float ddong, ddong2; //똥값
float ddong_sum, ddong_sum2 = 0; //가스 썸값
float ddong_average = 0; //가스 평균값
float humi, humi2, temp, temp2 = 0; //오줌값
float humi_sum, humi_sum2 = 0; //습도 썸값
float humi_average = 0; //습도 평균값
float temp_sum, temp_sum2 = 0 ; //온도 썸값
float temp_average = 0 ; //온도 평균값


unsigned long pt = 0;
unsigned long pt2 = 0;
unsigned long pt3 = 0;
unsigned long pt4 = 0;
unsigned long pt5 = 0;
unsigned long ct = 0;
unsigned long ct2 = 0;
unsigned long ct3 = 0;
unsigned long ct4 = 0;
unsigned long ct5 = 0;

//float first_humi;
//float first_temp;
//float first_ddong;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  BTSerial.begin(115200);
  dht.begin();
  //delay(70000);
  Serial.println("Hello World");
  BTSerial.println("Hi");
  pt = millis();
  pt2 = millis();
  pt3 = millis();
  pt4 = millis();
  pt5 = millis();
}

void loop() {

  // put your main code here, to run repeatedly:
  ct = millis();
  ct2 = millis();
  ct3 = millis();
  ct4 = millis();
  ct5 = millis();
  ddong = analogRead(gassensor);
  humi = dht.readHumidity();
  temp = dht.readTemperature();


  //시작 후 3분간 센서값 20번 측정
  if ((ct - pt) <= 180000) {

    if ((ct5 - pt5) >= 60000) {

      if (count < 21) {

        ddong = analogRead(gassensor);
        humi = dht.readHumidity();
        temp = dht.readTemperature();
        ddong_sum = ddong_sum + ddong;
        humi_sum = humi_sum + humi;
        temp_sum = temp_sum + temp;

        Serial.println(ddong);
        Serial.println(humi);
        Serial.println(temp);
        Serial.println(count);
        Serial.println(ct);
        count++;
        delay(4000);
      }
    }
  }

  //20번 측정한 센서값들의 평균 도출
  if (count >= 20) {
    ddong_average = ddong_sum / 20;
    humi_average = humi_sum / 20 ;
    temp_average = temp_sum / 20 ;

    Serial.print("ddong_average : ");
    Serial.println(ddong_average);
    Serial.print("humi_average : ");
    Serial.println(humi_average);
    Serial.print("temp_average : ");
    Serial.println(temp_average);
    Serial.println(ct);
    ddong_sum = 0;
    humi_sum = 0;
    temp_sum = 0;
    count = 0;
  }


  //30분 간격으로 재측정
  if ((ct2 - pt2) > 1800000) {
    if (count2 < 20) {
      ddong2 = analogRead(gassensor);
      humi2 = dht.readHumidity();
      temp2 = dht.readTemperature();
      ddong_sum2 = ddong_sum2 + ddong2;
      humi_sum2 = humi_sum2 + humi2;
      temp_sum2 = temp_sum2 + temp2;
      count2++;
      Serial.print("count2 =    ");
      Serial.println(count2);
      delay(4000);
    }
  }

  //30분 간격 재측정한 평균값 도출
  if (count2 == 20) {
    ddong_average = ddong_sum2 / 20;
    humi_average = humi_sum2 / 20 ;
    temp_average = temp_sum2 / 20 ;

    Serial.print("ddong_average : ");
    Serial.println(ddong_average);
    Serial.print("humi_average : ");
    Serial.println(humi_average);
    Serial.print("temp_average : ");
    Serial.println(temp_average);

    ddong_sum2 = 0;
    humi_sum2 = 0;
    temp_sum2 = 0;
    count2 = 0;
    pt2 = ct2;
  }

  //ct-pt는 몇초마다 작동할 것인가 설정
  //average는 10번 받은 평균값, a는 센서에서 측정된 값
  //a/average>2 라는 소리는 측정된 값이 평균값보다 2배 컸을때를 의미
  //2가지 조건을 만족하면  똥 쌈의 의미



  //5분 간격으로 측정 후 평균값과 비교
  if ( ((ct3 - pt3) > 300000)) {
    humi = dht.readHumidity();
    temp = dht.readTemperature();

    Serial.println((temp / temp_average));
    Serial.println((humi / humi_average));
    Serial.println((ddong / (ddong_average)));

    //5분마다 감지하기 위한 초기화
    pt3 = ct3;

    // 소변 감지
    if (   ((temp / temp_average) >= 1.2) && (    ((humi / humi_average) >= 1.15) || ((humi / humi_average) <= 0.9))  ) {


      ddong = analogRead(A3);

      if ((ddong / (ddong_average)) > 2.5) {
        //대변감지
        BTSerial.println("1");
      } else {
        //대변 감지 안되면 소변으로만 감지
        BTSerial.println("0");
      }
    }
  }

  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  // Serial –> Data –> BT
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
  delay(1000);
}
