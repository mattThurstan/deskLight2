/*----------------------------notes----------------------------*/

/* 
 * WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
 * 
 * Pinout Wemos D1 Mini (ESP-8266)
 * RX  3 (GPIO3) [ NeoPixelBus - For Esp8266, the Pin is omitted and it uses GPIO3 (RX) due to DMA hardware use.  ]
 * TX  1
 * A0  Analog input, max 3.2V input  A0
 * D0  16  IO  GPIO16   - no PWM or I2C or interrupt
 * D1  5  IO, SCL GPIO5  I2C
 * D2  4  IO, SDA GPIO4  I2C
 * D3  0  IO, 10k Pull-up GPIO0
 * D4  2  IO, 10k Pull-up, BUILTIN_LED  GPIO2
 * D5  14  IO, SCK GPIO14
 * D6  12  IO, MISO  GPIO12
 * D7  13  IO, MOSI  GPIO13
 * D8  15  IO, 10k Pull-down, SS GPIO15
 * 
 * All have external interrupts except GPIO16 (D0)
 *
 * 
 * http://wiki.keyestudio.com/index.php/Ks0052_keyestudio_PIR_Motion_Sensor
 * 
 * Grove - 12 Key Capacitive I2C Touch Sensor V2 (MPR121)
 * http://wiki.seeedstudio.com/Grove-12_Key_Capacitive_I2C_Touch_Sensor_V2-MPR121/
 * 
 * 
 * https://hackaday.com/2017/01/20/cheating-at-5v-ws2812-control-to-use-a-3-3v-data-line/
 * https://github.com/FastLED/FastLED/wiki/Interrupt-problems
 * 
 * 
 */
 
