#include <Arduino.h>
#include <arduinoFFT.h>

arduinoFFT fft;

const int num_samples = 512;
const int sampling_freq = 100;
const int freq1 = 5, freq2 = 23, freq3 = 40;
const int amp1 = 14.0, amp2 = 20.0, amp3 = 75.0;

double real[num_samples];
double imag[num_samples];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("\nStarting...");

  for (int i = 0; i < num_samples; i++) {
    real[i] = amp1 * cos(twoPi * i / num_samples * freq1) + amp2 * cos(twoPi * i / num_samples * freq2) + amp3 * cos(twoPi * i / num_samples * freq3);
    imag[i] = 0;
  }

  fft = arduinoFFT(real, imag, num_samples, sampling_freq);

  fft.Compute(FFT_FORWARD);
  fft.ComplexToMagnitude();

  for (int i = 0; i < num_samples; i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.println(real[i]);
  }
  Serial.println("Finished.");
}

void loop() {
  // put your main code here, to run repeatedly:
}
