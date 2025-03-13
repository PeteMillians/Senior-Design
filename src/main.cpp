#include <iostream>
#include <utility>
#include <chrono>
#include <thread>

using namespace std;

// Constants
const int SAMPLING_FREQUENCY = 2000;    // 0.5ms Sampling Frequency

// Method Prototypes
int ReadInput(int pinNumber);
int Filter(int data);
void ReturnOutput(int data, int pinNumber);
pair<bool, int> _TryReadInput(int pinNumber);
pair<bool, int> _TryFilter(int data);
pair<bool, int> _TryConvolve(int data);
bool _TryReturnOutput(int data, int pinNumber);


int main() {
    /* Declare input pin as INPUT_PIN */
    // inputData = ReadInput(INPUT_PIN);

    /* Filter inputData*/
    // outputData = Filter(inputData);

    /* Declare output pin as OUTPUT_PIN */
    // ReturnOutput(outputData, OUTPUT_PIN);
}