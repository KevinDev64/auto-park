/*
 
/######################################\
|                                      |
|           AUTO-PARK v.3.3            |
|                                      |
|~> This project is working CORRECTLY! |
|                                      |
|Coded by ~~~>  RussianBear08          |
|                                      |
|Thank you for watching desciption :3  |
|                                      |
\######################################/


*/

// Определение режима соединения и подключение библиотек
#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <RemoteXY.h>
#include <AFMotor.h>

// Настройки соединения по BlueTooth LE
#define REMOTEXY_SERIAL_RX A0
#define REMOTEXY_SERIAL_TX A1
#define REMOTEXY_SERIAL_SPEED 9600
#define REMOTEXY_ACCESS_PASSWORD "7355608"

// Глобальные переменные для датчиков
#define TRIGGER_LEFT     A2
#define ECHO_LEFT        A3

#define TRIGGER_RIGHT    A4
#define ECHO_RIGHT       A5

#define TRIGGER_FORWARD  9
#define ECHO_FORWARD     10

#define TRIGGER_BACKWARD 2
#define ECHO_BACKWARD    13

#define MAX_DISTANCE 400


// Конфигурация интерфейса  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,10,0,37,0,31,2,13,29,4,
  130,1,49,38,4,4,2,36,130,1,
  56,28,4,4,2,36,130,1,49,18,
  4,4,2,36,130,1,42,28,4,4,
  2,36,130,1,68,23,32,40,1,30,
  130,1,63,0,37,24,1,30,2,0,
  77,1,22,7,1,2,26,31,31,79,
  78,0,79,70,70,0,129,0,64,1,
  11,3,1,36,79,78,47,79,70,70,
  0,131,1,2,57,13,5,1,2,31,
  77,65,73,78,0,131,0,16,57,13,
  5,2,2,31,83,69,78,83,0,5,
  32,71,34,28,28,1,2,26,31,129,
  0,72,29,26,4,1,37,77,97,110,
  117,97,108,32,109,111,100,101,0,1,
  1,20,13,16,8,1,2,31,208,146,
  208,191,209,128,208,176,208,178,208,190,
  43,208,146,208,191,208,181,209,128,209,
  145,208,180,0,1,1,1,13,17,8,
  1,2,31,208,146,208,187,208,181,208,
  178,208,190,43,208,146,208,191,208,181,
  209,128,209,145,208,180,0,1,1,20,
  23,16,8,1,2,31,208,146,208,191,
  209,128,208,176,208,178,208,190,0,1,
  1,1,23,17,8,1,2,31,208,146,
  208,187,208,181,208,178,208,190,0,3,
  130,78,10,20,11,1,166,26,129,0,
  64,12,13,4,1,37,77,111,100,101,
  115,0,129,0,1,3,30,9,1,24,
  208,146,209,139,208,181,208,183,208,180,
  0,130,0,44,20,14,20,2,17,67,
  5,27,27,13,6,2,31,26,9,67,
  5,44,10,14,6,2,31,26,9,67,
  5,62,27,13,6,2,31,26,9,67,
  5,44,44,14,5,2,31,26,9,129,
  0,1,50,55,3,0,36,208,146,208,
  189,208,184,208,188,208,176,209,130,208,
  181,208,187,209,140,208,189,208,190,32,
  209,129,208,187,208,181,208,180,46,32,
  208,183,208,176,32,209,129,208,184,209,
  130,46,32,208,189,208,176,32,208,180,
  208,190,209,128,208,190,208,179,208,181,
  33,0,1,1,1,33,17,8,1,2,
  31,208,146,208,187,208,181,208,178,208,
  190,43,208,157,208,176,208,183,208,176,
  208,180,0,1,1,20,33,16,8,1,
  2,31,208,146,208,191,209,128,208,176,
  208,178,208,190,43,208,157,208,176,208,
  183,208,176,208,180,0,70,48,38,2,
  10,10,1,26,36,36,136,16,0,129,
  0,1,1,73,6,2,24,208,160,208,
  176,209,129,209,129,209,130,208,190,209,
  143,208,189,208,184,208,181,32,208,180,
  208,190,32,208,190,208,177,209,138,208,
  181,208,186,209,130,208,190,208,178,0 };
  
// Структура определяет все переменные и события интерфейса управления 
struct {

    // input variables
  uint8_t switch_1; // =1 если переключатель включен и =0 если отключен 
  int8_t joystick_1_x; // =-100..100 координата x положения джойстика 
  int8_t joystick_1_y; // =-100..100 координата y положения джойстика 
  uint8_t buttonRF; // =1 если кнопка нажата, иначе =0 
  uint8_t buttonLF; // =1 если кнопка нажата, иначе =0 
  uint8_t buttonRR; // =1 если кнопка нажата, иначе =0 
  uint8_t buttonLL; // =1 если кнопка нажата, иначе =0 
  uint8_t mode_select; // =0 если переключатель в положении A, =1 если в положении B, =2 если в положении C, ... 
  uint8_t buttonLB; // =1 если кнопка нажата, иначе =0 
  uint8_t buttonRB; // =1 если кнопка нажата, иначе =0 

    // output variables
  char text_L[9];  // =строка UTF8 оканчивающаяся нулем 
  char text_F[9];  // =строка UTF8 оканчивающаяся нулем 
  char text_R[9];  // =строка UTF8 оканчивающаяся нулем 
  char text_B[9];  // =строка UTF8 оканчивающаяся нулем 
  uint8_t led; // led state 0 .. 3 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

// Инициальзация датчиков

NewPing sonar_left(TRIGGER_LEFT, ECHO_LEFT, MAX_DISTANCE);
NewPing sonar_right(TRIGGER_RIGHT, ECHO_RIGHT, MAX_DISTANCE);
NewPing sonar_forward(TRIGGER_FORWARD, ECHO_FORWARD, MAX_DISTANCE);
NewPing sonar_backward(TRIGGER_BACKWARD, ECHO_BACKWARD, MAX_DISTANCE);

// Объявление переменных для значений датчиков
long left_sens = 0;     // Начало объявления   ---BEGIN---
long right_sens = 0; 
long forward_sens = 0;
long backward_sens = 0; // Конец объявления    ----END----

int i = 0;    // Начало объявления временных переменных
int ii = 0;   /////////////////////////////////////////
int LFF = 0;  /////////////////////////////////////////
int LBB = 0;  /////////////////////////////////////////
int LLL = 0;  /////////////////////////////////////////
int RFF = 0;  /////////////////////////////////////////
int RRR = 0;  /////////////////////////////////////////
int RBB = 0;  //Конец объявления временных переменных//

// Инициальзация моторов

AF_DCMotor motor_left(4);
AF_DCMotor motor_right(3);

void setup() 
{
  RemoteXY_Init ();   
  RemoteXY.led = 0;   // Изменение состояния на OFF
  Serial.begin(9600);

  // 4 Строки вывода надписи OFF на экран
  sprintf (RemoteXY.text_L, "OFF");
  sprintf (RemoteXY.text_R, "OFF");
  sprintf (RemoteXY.text_F, "OFF");
  sprintf (RemoteXY.text_B, "OFF");

  // Конечная инициализация ЛЕВОГО мотора и его перевод в STOP
  motor_left.setSpeed(255);
  motor_left.run(RELEASE);
  
  // Конечная инициализация ПРАВОГО мотора и его перевод в STOP
  motor_right.setSpeed(255);
  motor_right.run(RELEASE);
  
  RemoteXY.mode_select = 1;
}

void loop() 
{ 
  RemoteXY_Handler ();
  // Проверка значений Переключателя питания
  if(RemoteXY.switch_1 == 1) {
    // Изменения состояние на AUTO
    RemoteXY.led = 3;
  while (i == 0) {
    // ЛОГИ
    Serial.println("POWER >> DEVICE IS ON! GOOD LUCK ON ROAD                                     CODE: 1 Power");
    Serial.println("SENS  >> SENSORS IS ACTIVE!                                                  CODE: 1 SensA");
    i++;
    ii = 0;
  }
  // Запуск сенсоров и режима автопроверки установленного значения на Переключателе Режимов
  sensors();
  modes();
  }
  else {
    // Изменение режима на OFF
    RemoteXY.led = 0;
    while (ii == 0) {
      // ЛОГИ
      Serial.println("POWER >> DEVICE IS OFF!                                                      CODE: 0 Power");

      // Вывод на экран сенсоров значение OFF
      sprintf (RemoteXY.text_L, "OFF");
      sprintf (RemoteXY.text_R, "OFF");
      sprintf (RemoteXY.text_F, "OFF");
      sprintf (RemoteXY.text_B, "OFF");
      motor_left.run(RELEASE);
      motor_right.run(RELEASE);
      ii++;
      i = 0;
      }
    }


}

void sensors(){
  // Получение значений с сенсоров
  delay(50);
  left_sens = sonar_left.ping_cm();         // Recieving data from left sens
  right_sens = sonar_right.ping_cm();       // Recieving data from right sens
  forward_sens = sonar_forward.ping_cm();   // Recieving data from forward sens
  backward_sens = sonar_backward.ping_cm(); // Recieving data from backward sens

  // Вывод значений на экран
  itoa (left_sens, RemoteXY.text_L, 8);     // Data on screen ---BEGIN---
  itoa (forward_sens, RemoteXY.text_F, 8);  // --------------------------
  itoa (backward_sens, RemoteXY.text_B, 8); // --------------------------
  itoa (right_sens, RemoteXY.text_R, 8);    // Data on screen ----END----

  }

void modes(){
  // Проверка установленного режима работы AUTO/MANUAL

  if (RemoteXY.mode_select == 0) {

      // Запуск модуля АВТОУПРАВЛЕНИЯ
    autopark();
    }
  if (RemoteXY.mode_select == 1) {

    // Запуск модуля Ручного Управления
    manualpark();
    }
  
  }

void manualpark(){
  // Возврат временных переменных к начальному значению 
  LFF = 0;
  LLL = 0;
  LBB = 0;

  RFF = 0;
  RRR = 0;
  RBB = 0;

  RemoteXY.led = 1;

  // Проверка значений получаемых с джойстика и дальнейшее включение соответствующего модуля 

  if (RemoteXY.joystick_1_x == 100)  {   right();    }
  if (RemoteXY.joystick_1_x == -100) {   left();     }
  if (RemoteXY.joystick_1_y == 100)  {   forward();  }
  if (RemoteXY.joystick_1_y == -100) {   backward(); }
  
  if (RemoteXY.joystick_1_x == 0 && RemoteXY.joystick_1_y == 0) {  STOP();  }
  }

void right(){

  // Установка режима MANUAL-ON
  RemoteXY.led = 2;
  
  // Установка скорости на двигатели 150/255
  motor_left.setSpeed(150);
  motor_right.setSpeed(150);

  // Запуск моторов на движение вправо
  motor_left.run(FORWARD);
  motor_right.run(BACKWARD);
  }
  
void left(){

  // Установка режима MANUAL-ON
  RemoteXY.led = 2;

  // Установка скорости на двигатели 150/255
  motor_left.setSpeed(150);
  motor_right.setSpeed(150);
  
  motor_left.run(BACKWARD);
  motor_right.run(FORWARD);
  }

void forward(){

  // Установка режима MANUAL-ON
  RemoteXY.led = 2;

  // Установка скорости на двигатели 175/255
  motor_left.setSpeed(175);
  motor_right.setSpeed(175);
  
  motor_left.run(FORWARD);
  motor_right.run(FORWARD);
  }
  
void backward(){

  // Установка режима MANUAL-ON
  RemoteXY.led = 2;

  // Установка скорости на двигатели 175/255
  motor_left.setSpeed(175);
  motor_right.setSpeed(175);
  
  motor_left.run(BACKWARD);
  motor_right.run(BACKWARD);
  }

void STOP(){
  
  // Установка режима MANUAL
  RemoteXY.led = 1;

  // Установка скорости на двигатели 0/255
  motor_left.setSpeed(0);
  motor_right.setSpeed(0);

  // Остановка двигателей
  motor_left.run(RELEASE);
  motor_right.run(RELEASE);
  }


// Начало АВТОМАТИЧЕСКОЙ части

void autopark() {
  motor_left.setSpeed(150);
  motor_right.setSpeed(150);
  
  // Проверка нажатия определённой кнопки

  // Левая часть кнопок ------------------------------
  if (RemoteXY.buttonLF == 1 && left_sens >= 23 && forward_sens >= 8 && backward_sens >= 8 && right_sens >= 8) {
    RemoteXY.led = 2;
    
    motor_left.run(BACKWARD);
    motor_right.run(FORWARD);

    delay(300);

    motor_left.run(RELEASE);
    motor_right.run(RELEASE);

    
    if (LFF == 1) {
      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(900);
  
      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(BACKWARD);

      delay(550);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);

      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(350);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);
      

      RemoteXY.mode_select = 1;
      
      LFF = 2;
    }
    
    if (LFF == 0) {
      LFF = 1;
      }
      
    delay(300);
    
  }
  if (RemoteXY.buttonLL == 1) {
    RemoteXY.led = 1;
    }
  
  if (RemoteXY.buttonLB == 1 && left_sens >= 23 && forward_sens >= 8 && backward_sens >= 8 && right_sens >= 8) {
    RemoteXY.led = 2;
    
    motor_left.run(BACKWARD);
    motor_right.run(FORWARD);

    delay(300);

    motor_left.run(RELEASE);
    motor_right.run(RELEASE);

    
    if (LBB == 1) {
      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(900);
  
      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(BACKWARD);

      delay(550);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);

      motor_left.run(BACKWARD);
      motor_right.run(BACKWARD);

      delay(300);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);
      

      RemoteXY.mode_select = 1;
      
      LBB = 2;
    }
    
    if (LBB == 0) {
      LBB = 1;
      }
      
    delay(300);
    }

  // Правая часть кнопок -----------------------------
  
  if (RemoteXY.buttonRF == 1 && left_sens >= 8 && forward_sens >= 8 && backward_sens >= 8 && right_sens >= 23) {
    RemoteXY.led = 2;
    
    motor_left.run(FORWARD);
    motor_right.run(BACKWARD);

    delay(300);

    motor_left.run(RELEASE);
    motor_right.run(RELEASE);

    
    if (RFF == 1) {
      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(900);
  
      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(BACKWARD);

      delay(550);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);

      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(350);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);
      

      RemoteXY.mode_select = 1;
      
      RFF = 2;
    }
    
    if (RFF == 0) {
      RFF = 1;
      }
      
    delay(300);
    }
    
  if (RemoteXY.buttonRR == 1) {
    RemoteXY.led = 1;
    }
    
  if (RemoteXY.buttonRB == 1 && left_sens >= 8 && forward_sens >= 8 && backward_sens >= 8 && right_sens >= 23) {
    RemoteXY.led = 2;
    
    motor_left.run(FORWARD);
    motor_right.run(BACKWARD);

    delay(300);

    motor_left.run(RELEASE);
    motor_right.run(RELEASE);

    
    if (RBB == 1) {
      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(FORWARD);

      delay(900);
  
      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);
      
      motor_left.run(FORWARD);
      motor_right.run(BACKWARD);

      delay(550);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);

      delay(500);

      motor_left.run(BACKWARD);
      motor_right.run(BACKWARD);

      delay(300);

      motor_left.run(RELEASE);
      motor_right.run(RELEASE);
      

      RemoteXY.mode_select = 1;
      
      RBB = 2;
    }
    
    if (RBB == 0) {
      RBB = 1;
      }
      
    delay(300);
    }

    
}
