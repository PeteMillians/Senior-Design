/* Constants declared in header */
const int SIGNAL_THRESHOLD = 500;   // Example voltage threshold level in range (0 : 1023)

struct Pair {
	bool success;
	float data;
};

void setup() {

  Serial.begin(9600);  // Start serial communication
}

void loop() {

  float signal = rand() % 1024;	// Random number between 0 and 1023
	float filteredSignal = Filter(signal);	// Filter the signal

	Serial.print("Random Signal = ");
	Serial.println(signal);
	Serial.print("Filtered Signal = ");
	Serial.println(filteredSignal);
	Serial.println();
  
}

float Filter(int data) {
	/*
	- Function:
		- Public method for filtering the digital data. Calls the private filtering method.
		- Ensures that the data is properly filtered, even if errors occur.
	- Arguments:
		- data (int): the digital data passed which needs to be filtered
	- Returns:
		- float value of filtered data
	*/

	Pair filterPair = _TryFilter(data);

	if (!filterPair.success) {
		Serial.println("Filter Failure");
		return 0.0;
	}
	
	return filterPair.data;

}

Pair _TryFilter(int data) {
	/*
	- Function:
		- Attempts to filter the given data using a specific voltage threshold.
	- Arguments:
		- data (int): The raw EMG data 
	- Returns:
		- Returns a boolean indicating if filtering was successful, and the filtered value.
	*/

	Pair tryFilter;
	
	if(data < SIGNAL_THRESHOLD) {
		tryFilter.data = 0.0;
		tryFilter.success = true;
	}
	else {
		tryFilter.data = data;
		tryFilter.success = true;
	}

	return tryFilter;

}