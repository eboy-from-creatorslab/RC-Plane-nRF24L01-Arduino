/*
pins connections
 
vcc 3.3
gnd gnd
ce pin9
scn pin10
sck pin13
mosi pin11
miso pin12
 
 
*/
#include <SPI.h>
#include <Servo.h>
 #include <nRF24L01.h>
 #include <RF24.h>
 #define CE_PIN 9
 #define CSN_PIN 10
 
Servo servoLeft;
Servo servoRight;
Servo BMotor;
 
int B_Motor = 3; 
int LeftServo = 5;
int RightServo = 6;
 
int ud_data; // up down data
int lr_data; // left right data
int speed_data; // brushless motor speed data
 
int receiver_reset = 8; 
/* the purpose of of defining the receiver_reset pin is to programmly reset the arduino, While using the NRF24 Transceiver modules
 *  you will notice one most prominent issue, which is the connection is lost, Some guys are using the technique of using the default values 
 *  and then keep waiting until the connection is established again, this is not a good idea. 
 *  After testing a series of tests, i found out, reseting the arduino is the best option. if the connection is lost lets say for 2 seconds then the arduino 
 *  resets automatically. After doing this thing i didn't notice any abnormal code execution. 
 *  
 */
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
RF24 radio(CE_PIN, CSN_PIN);
byte data[6]; // depending on the number of sensors used
 
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;
 
int secs = 0; 
 
void setup()
 {
    digitalWrite(receiver_reset, HIGH);
  delay(200);
 pinMode(receiver_reset, OUTPUT);
 Serial.begin(9600); 
 pinMode(B_Motor, OUTPUT);
servoLeft.attach(LeftServo); // pin 5
servoRight.attach(RightServo); // pin 6
BMotor.attach(B_Motor);
 Serial.println("Nrf24L01 Receiver Starting");
 
radio.begin();
 radio.openReadingPipe(1,pipe);
 radio.startListening();
 resetData();
 
 //default values
 
 servoLeft.write(90);
 servoRight.write(90);
 BMotor.write(0);
 }
 
void loop()
 {
 if ( radio.available() )
 {
 bool done = false;
 while (!done)
 {
 done = radio.read( data, sizeof(data) );
 lastReceiveTime = millis(); // At this moment we have received the data
 secs = 0; 
 // updown
if (data[2] == 1)
{ 
ud_data = map(data[0], 0 , 255, 0 , 180);
servoLeft.write(ud_data);
servoRight.write(ud_data);
delay(50);
}
// Left Right
 
if (data[2] == 0)
{  
servoLeft.write(map(data[1], 0,255,0, 180)); 
servoRight.write(map(data[1], 0,255,180, 0));
delay(50);
}
 
speed_data = map(data[3],0, 255, 1000, 2000); 
BMotor.writeMicroseconds(speed_data);
delay(50); 
 
 }
 
 }
 else
 {
currentTime = millis();
if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
 
// 
 
secs = secs + 1; 
if(secs >= 2)
{
  digitalWrite( receiver_reset, LOW); 
  secs = 0; 
}
 
 }
 }
 }
 
void resetData() 
{
// we are going to place our default code over here. 
Serial.println("Connection Lost");
BMotor.writeMicroseconds(1500); 
 
}
