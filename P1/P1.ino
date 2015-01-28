/*
Project 1 - Milestone 1
Authors: Justin Guze and Paul Hunter

*/
//#define DEBUG 1

/* Global Variables */
#define IRS_DEBUG_PIN 7


//IR Variables
#define IR_TRANSMIT_PIN 13 //PWM Output 13.
int ir_send = 0;
int ir_msg_pos = -2;
int ir_msg_bit = 0;
char ir_msg = 'A'; //0110 0101

//Joystick Variables
#define JOYSTICK_X_PIN 0
#define JOYSTICK_SW_PIN 8 //PWM Pin 8
int js_x; //value of the X axis of the J joystick. 
int js_buttonReset = 0;

//Servo Variables
#define SERVO_PIN 9
int servo_pulseWidth = 0;
int servo_pos = 90; //0 to 180 degrees

/* Constants */
#define MAX_JOYSTICK_VAL 3

/*
setup()
Configure and set the pin modes and timers for the sytsem.
*/
void setup()
{
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  servo_init();
  js_init();
  ir_init();

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop()
{
#ifdef DEBUG
  Serial.print("JoyStickPress: ");
  Serial.println(js_buttonPressed() == HIGH);
#endif
  js_x = js_sample();
  
  /*
  If we are not actively sending a message we want to check if
  the operator would like to send another message.
  */
  if(!js_buttonPressed())
  {
   js_buttonReset = 0; 
  }
  
  if(!ir_send && js_buttonPressed() && !js_buttonReset)
  {
#ifdef DEBUG
    Serial.println("Fire!");
#endif
    ir_send = 1;
    js_buttonReset = 1;
    digitalWrite(6, HIGH);
  }
  
  /* If we are not currently sending a signal we will chec to see
  if the operator would like to move the 'ray gun' */
  if(!ir_send)
  {
    servo_pos += js_x;
    servo_pos = servo_setPos(servo_pos);
    delay(30);
  }
}

/*-------- Joystick Methods --------*/

void js_init()
{
  //Configure the button pin as input
  pinMode(JOYSTICK_SW_PIN, INPUT); 
}

int js_sample()
{
  int val = analogRead(JOYSTICK_X_PIN);
  val = map(val, 0, 1023, -1 * MAX_JOYSTICK_VAL, MAX_JOYSTICK_VAL);
  if(val <= 1 && val >= -1) val = 0; //Create a dead zone of no movement at the middle of the movement range.
  return val;
}

//Check if the Joystick button been pressed
int js_buttonPressed()
{
  return digitalRead(JOYSTICK_SW_PIN) == LOW;
}

/*-------- SERVO Control --------*/

void servo_init()
{
  pinMode(SERVO_PIN, OUTPUT);
}

/*
servo_setPos 
int target - the desired angle to position the servo at between [0,180]

*/
int servo_setPos(int target)
{
  /* Bound the input value to ensure we do not request the servo to move
     further than its bounds */
  if(target > 180) target = 180;
  else if(target < 0) target = 0;
  
  /* This method uses the information provided by r.goldstein at
  http://forum.arduino.cc/index.php?topic=5983.0 */
  servo_pulseWidth = (target * 10) + 500; //TODO - this line is 'safe', but does not give us our full range of motion.
  
  
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(servo_pulseWidth);
  digitalWrite(SERVO_PIN, LOW);
  return target;
}

/*-------- IR Configuration --------*/

void ir_init()
{
  /* In order to trigger the IR signals with the appropriate frequencies and pulse widths
  we must use two timers, one to create a resting carrier frequency, an another to allow to 
  send each bt of the system. 
  We will use timer three as our interupt timer to change the high low signal and timer one as
  our 38KHz generator*/
  
  /* Debug Pins */
  pinMode(IRS_DEBUG_PIN, OUTPUT);
  digitalWrite(IRS_DEBUG_PIN, LOW);
  
  /* Configure Ports */
  pinMode(IR_TRANSMIT_PIN, OUTPUT);
  
  /* Configure timer three as our interupt timer */
  TCCR3A = TCCR3B = 0;
  //Set CTC - mode four
  TCCR3B |= (1 << WGM32);
  //Set prescalar value to one
  TCCR3B |= (1 << CS30);
  //Set the TOP value to allow for us to have 500 microsecond interupts. 
  OCR3A = 8000;
  //Enable interupt A for timer three so we have a handle to it. 
  TIMSK3 |= (1 << OCIE3A);
  //reset the timer
  TCNT3 = 0;
  
  /* Configure timer one as a 38KHz generator for our carrier frequency */
  TCCR1A = TCCR1B = 0;
  TIMSK1 &= ~(1 << OCIE1C); //TODO - Figure out what this line is doing...
  //Set the timer to Fast PWM - mode fifteen
  TCCR1A |= (1 << WGM10) | (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);  
  //Enable output C
  TCCR1A |= (1 << COM1C1);
  //Clear the prescalar
  TCCR1B |= (1 << CS10);
  //Set the top value to create 38KHz oscillation
  OCR1A = 421;
  //Disable the signal for now
  OCR1C = 0;
}

/* ISR for Timer 3, fired every 500 microseconds */
ISR(TIMER3_COMPA_vect)
{
  digitalWrite(IRS_DEBUG_PIN, HIGH);
  if(ir_send)
  {
    if(ir_msg_pos < 0)
    {
      /* We use the position of the message to indicate which signal 
         we want to send. If the value is less than 0, we are still sending
         our handshake. -2 is te first bit, a 1, -1 is the second, a 0, and
         the 0 position is the start of our message */
      ir_msg_bit = ir_msg_pos == -2 ? 1: 0;
    }
    else
    {
      /* In this line we mask out the value of the bit in the message
         we are currently looking to send. We then set our signal appropriately */
      ir_msg_bit = ((ir_msg & (1 << ir_msg_pos)) > 0 ? 1 : 0);
    }
    
    /* Adjust the current signal based on the value we are to be sending */
    if(ir_msg_bit)
    {
      OCR1C = 140; //Send a HIGH
      digitalWrite(5, HIGH);
    } 
    else
    {
      OCR1C = 0; //Send a LOW
      digitalWrite(5, LOW);
    }
    
    //Incremem to the next position in the message
    ir_msg_pos ++;
    
    /* check if we have completed sending our message */
    if(ir_msg_pos >= 8)
    {
      ir_msg_pos = -2;
      ir_send = 0; 
      digitalWrite(6, 0);
    }
  }
  else
  {
   OCR1C = 0; //Ensure the generator is off if we are not sending a message. 
   digitalWrite(5, LOW);
  }
  digitalWrite(IRS_DEBUG_PIN, LOW);
}


