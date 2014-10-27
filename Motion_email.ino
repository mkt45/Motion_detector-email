// define some values used by the panel and buttons
#define trigPin 12                                                                        // Pin 12 trigger output
#define echoPin 11                                                                      // Pin 2 Echo input
#define motionLedPin 13                                                             // Pin 13 motion detector indicator output

#define MOTION_BASE_SIZE 24                                                 // Length of the motion base samples array
#define MOTION_CURRENT_SIZE 6                                            // Length of the motion current value array
volatile int motion_base_array[MOTION_BASE_SIZE];                   // Motion base array
volatile int motion_current_array[MOTION_CURRENT_SIZE];         // Motion current value array
volatile int motion_base;                                                               // Long term motion base value
volatile int motion_current;                                                            // Current motion distance value
volatile bool motion_detected = false;                         // True when motion output triggering        
int t;

long duration, distance;


void setup()
{  
  pinMode(trigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(echoPin, INPUT);                            // Echo pin set to input
  pinMode(motionLedPin, OUTPUT);               // Motion detector indicator pin set to output
 
  Serial.begin (9600);                                // Initialise the serial monitor output 
}

void loop()
{
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;  // acknowledge current distance
  
  motion_detector();                  // activating motion detection

  if(motion_detected)
  {
int minSecsBetweenEmails = 60; // 1 min
long lastSend = minSecsBetweenEmails * 1000;
long now = millis();

     if (now > (lastSend + minSecsBetweenEmails * 1000))
        {
          Serial.println("MOVEMENT");
          lastSend = now;
        }
     else
        {
          Serial.println("Too soon");
        }
      }
   delay(500);

}

// --------------------------
// motion_detector() 
// Detects when an object approaches the sensor and triggers the output.
//
// The distance sensor often flips between reading objects at different
// distances. This makes it difficult to use individual sensor readings
// for motion detection. To mitigate the multi reading problem the readings
// are accumulated and averaged before use.
//
// Two averages are maintained. The first average has a long time constant and
// is used as the base reading. The second average has a shorter time constant
// to react quickly to changes. The difference in the two average values can
// be used to detect motion.
// --------------------------
void motion_detector()
{
  int i;
  static int base_ind = 0;                                 // Index into the base array
  static int current_ind = 0;                              // Index into the current array
  int base_sum;                                              // Sum of the base array
  int current_sum;                                          // Sum of the current array
  int diff;                                                 // Difference between base and current
  
  if (distance > 1000){
  distance = 1000;                                          // Limit the distance value
  }
  
  /*if (distance < 0) {                                     // Not necessary. It was getting negative nr but changed distance int for long  
   distance = 0; 
  }
  */
  
  motion_base_array[base_ind++] = distance;                // Insert value into the base array
  
  if (base_ind >= MOTION_BASE_SIZE)                     // Wrap the base index to the start
  {
    base_ind = 0;
  }
  
  base_sum = 0;                                                       // Start the base value processing 
  
  if (distance != 0){
    for (i = 0; i < MOTION_BASE_SIZE; i++)
    {
      base_sum += motion_base_array[i];                      // Sum the contents of the array
    }
  }
  motion_base = base_sum / MOTION_BASE_SIZE;              // Divide by the array size

  Serial.println(motion_base);

  motion_current_array[current_ind++] = distance;         // Insert value into the current array
  
  if (current_ind >= MOTION_CURRENT_SIZE)              // Wrap the current index to the start
  {
    current_ind = 0;
  }

  current_sum = 0;                                    // Start the current value processing

  if (distance != 0){
    for (i = 0; i < MOTION_CURRENT_SIZE; i++)
    {
    current_sum += motion_current_array[i];           // Sum the contents of the array
    }
  }
  
  motion_current = current_sum / MOTION_CURRENT_SIZE;// Divide by the array size
  
  Serial.println(motion_current);
  
  
  diff = motion_base - motion_current;               // Compute the difference
  if ((diff > 40) || (diff<-40))                                                  // Check for movement towards the sensor
  {
    motion_detected = true;                               // Action Trigger
  }
  else
  {
  motion_detected = false;                               // Deactivate Trigger
  }
}




