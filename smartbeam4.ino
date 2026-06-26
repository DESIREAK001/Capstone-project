// --- Pin Definitions ---
const int ambientLdrPin = A0;   // Detects incoming ambient light
const int feedbackLdrPin = A1;  // Pointed at the torch to read its current brightness
const int relayPin = 7;         // Relay control pin

// --- Configuration Thresholds ---
const int ambientLightThreshold = 500; 

// --- Feedback Calibration Thresholds ---
// You will need to test your specific torch and adjust these 3 numbers!
const int feedbackDimMax  = 300;  // Anything below this is OFF
const int feedbackLowMax  = 400;  // Between DimMax and this is DIM_BEAM
const int feedbackHighMax = 500;  // Between LowMax and this is LOW_BEAM
                                  // Anything above this is HIGH_BEAM

enum TorchState {
  HIGH_BEAM,
  LOW_BEAM,
  DIM_BEAM,
  OFF
};

TorchState currentTorchState; // Removed the hardcoded assumption

void setup() {
  Serial.begin(9600);
  
  pinMode(ambientLdrPin, INPUT);
  pinMode(feedbackLdrPin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(relayPin, LOW); 
  
  // Determine actual starting state right at bootup using feedback
  updateStateFromFeedback();
  Serial.print("System Initialized. Actual starting state detected as: ");
  printState(currentTorchState);
}

void loop() {
  int ambientLightLevel = analogRead(ambientLdrPin);
  
  Serial.print("Ambient Light: ");
  Serial.println(ambientLightLevel);

  // Determine what state we *want* to be in
  TorchState targetState;
  if (ambientLightLevel > ambientLightThreshold) {
    targetState = DIM_BEAM;  // Light detected -> Switch to DIM
  } else {
    targetState = HIGH_BEAM; // Dark -> Switch to HIGH
  }

  Serial.print("Target State: ");
  printState(targetState);

  // Adjust the torch until the hardware feedback matches the target
  if (currentTorchState != targetState) {
    Serial.println("Mismatch detected! Cycling torch...");
    pulseRelay();
    
    // Crucial step: wait for the light to physically turn on before reading it
    delay(200); 
    
    // Read the feedback sensor to see what state the pulse pushed us into
    updateStateFromFeedback(); 
    
    Serial.print("New Verified State: ");
    printState(currentTorchState);
  }

  delay(1000); 
}

// --- Reads the feedback LDR and updates the state variable ---
void updateStateFromFeedback() {
  int feedbackVal = analogRead(feedbackLdrPin);
  Serial.print("[Feedback Raw Value: ");
  Serial.print(feedbackVal);
  Serial.print("] -> ");

  if (feedbackVal < feedbackDimMax) {
    currentTorchState = OFF;
  } else if (feedbackVal < feedbackLowMax) {
    currentTorchState = DIM_BEAM;
  } else if (feedbackVal < feedbackHighMax) {
    currentTorchState = LOW_BEAM;
  } else {
    currentTorchState = HIGH_BEAM;
  }
}

// --- Simulates a single quick click of the button ---
void pulseRelay() {
  digitalWrite(relayPin, HIGH); 
  delay(150);                  
  digitalWrite(relayPin, LOW);  
}

// --- Helper function to print states cleanly to serial monitor ---
void printState(TorchState state) {
  switch(state) {
    case HIGH_BEAM: Serial.println("HIGH_BEAM"); break;
    case LOW_BEAM:  Serial.println("LOW_BEAM");  break;
    case DIM_BEAM:  Serial.println("DIM_BEAM");  break;
    case OFF:       Serial.println("OFF");       break;
  }
}
