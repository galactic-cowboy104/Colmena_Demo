#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// Definición de los pines de salida para el control de los motores
#define OUTA1 5
#define OUTA2 6
#define OUTB1 10
#define OUTB2 11

// Definición del pin y número de LEDs para los NeoPixels
#define PIXELS_PIN 4
#define NUMPIXELS 6

// Definición del pin del servomotor
#define SERVO_PIN 3

// Definición del pin del sensor de efecto Hall
#define SENSOR_PIN A0

// Creación de un objeto NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELS_PIN, NEO_GRB + NEO_KHZ800);

// Creación de un objeto Servo
Servo servo; 

// Variables para almacenar el comando recibido y el último comando procesado
String command = "";
String lastCommand = "";

// Variables para las lecturas del sensor de efecto Hall
int sensorValue = 0, calibrationOffset = 0, difference = 0;

// Variables para el control de colores
uint8_t lastColor = 0, currentColor = 0;

void setup() {
  
  // Inicializa la comunicación serie a 9600 baudios
  Serial.begin(9600);

  // Inicializa los LEDs NeoPixel y limpia su estado
  pixels.begin();
  pixels.clear();

  // Conecta el servo al pin 3
  servo.attach(SERVO_PIN, 900, 1900); 
  servo.write(90);

  // Ciclo para encender cada LED en rojo, uno por uno
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Establece el color rojo para el LED actual
    pixels.show(); // Muestra el cambio en el LED
    delay(100); // Espera 100 ms antes de encender el siguiente LED
  }

  // Calibrar el sensor y establecer el valor offset
  calibrateSensor();

  // Configura los pines de los motores como salidas
  pinMode(OUTA1, OUTPUT);
  pinMode(OUTA2, OUTPUT);
  pinMode(OUTB1, OUTPUT);
  pinMode(OUTB2, OUTPUT);

  // Ciclo para encender cada LED en verde, uno por uno
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Establece el color verde para el LED actual
    pixels.show(); // Muestra el cambio en el LED
    delay(100); // Espera 100 ms antes de encender el siguiente LED
  }
  
}

void loop() {
  
  // Verifica si hay datos disponibles en el puerto serie
  if (Serial.available() > 0) {
    
    // Lee la cadena de texto hasta el salto de línea
    command = Serial.readStringUntil('\n');
    command.trim(); // Elimina espacios en blanco adicionales

    // Limpiamos el buffer para evitar datos residuales
    Serial.flush();

    // Si el comando es diferente al último procesado, lo procesa
    if (command != lastCommand) {
      lastCommand = command; // Actualiza el último comando
      handleCommand(command); // Llama a la función para manejar el comando
    }
    
  }

  sensorValue = analogRead(SENSOR_PIN); // Leer el valor del sensor
  difference = abs(sensorValue - calibrationOffset); // Sacar la diferencia con respecto al offset

  // Cambiar el color de los LEDs
  changeColor();
  
}

void handleCommand(String cmd) {
  // Verifica el comando recibido y ejecuta la acción correspondiente
  if (cmd == "forward") {
    moveForward(); // Mueve el robot hacia adelante
  } else if (cmd == "right") {
    moveRight(); // Gira el robot a la derecha
  } else if (cmd == "left") {
    moveLeft(); // Gira el robot a la izquierda
  } else if (cmd == "backward") {
    moveBackward(); // Mueve el robot hacia atrás
  } else if (cmd == "stop") {
    stopMovement(); // Detiene el movimiento del robot
  } else if (cmd.startsWith("servo-")) {
    int angle = cmd.substring(6).toInt(); // Extrae el valor numérico después de "servo-" y lo convierte a entero
    servo.write(angle); // Mueve el servo al ángulo especificado
  }
}

void changeColor() {

  // Determinamos el color actual en función de "difference"
  if (difference >= 0 && difference <= 2) {
    currentColor = 0; // Verde
  } else if (difference > 2 && difference <= 7) {
    currentColor = 1; // Amarillo
  } else {
    currentColor = 2; // Rojo
  }

  // Solo ejecuta el ciclo si el color actual es diferente al último color
  if (currentColor != lastColor) {
    
    lastColor = currentColor; // Actualiza el último color

    uint32_t color;
    if (currentColor == 0) {
      color = pixels.Color(0, 255, 0); // Verde
    } else if (currentColor == 1) {
      color = pixels.Color(255, 255, 0); // Amarillo
    } else {
      color = pixels.Color(255, 0, 0); // Rojo
    }

    // Actualiza los LEDs con el nuevo color
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, color); // Establece el color a cambiar
    }
    
    pixels.show(); // Muestra los cambios en todos los LEDs a la vez
    
  }

}

void calibrateSensor() {
  
  // Variables de iteración
  int sum = 0, count = 0; 

  // Ejecutar bucle while durante 1 segundo
  while (count <= 10) {
    sensorValue = analogRead(SENSOR_PIN); // Leer el valor del sensor
    sum += sensorValue; // Acumular el valor
    count ++; // Contar la lectura
    delay(100);
  }

  // Calcular el promedio y asignar a calibrationOffset
  calibrationOffset = sum / count;
  
}

void moveForward() {
  // Configura los pines de los motores para moverse hacia adelante
  digitalWrite(OUTA1, HIGH);
  digitalWrite(OUTA2, LOW);
  digitalWrite(OUTB1, HIGH);
  digitalWrite(OUTB2, LOW);
}

void moveRight() {
  // Configura los pines de los motores para girar a la derecha
  digitalWrite(OUTA1, HIGH);
  digitalWrite(OUTA2, LOW);
  digitalWrite(OUTB1, LOW);
  digitalWrite(OUTB2, HIGH);
}

void moveLeft() {
  // Configura los pines de los motores para girar a la izquierda
  digitalWrite(OUTA1, LOW);
  digitalWrite(OUTA2, HIGH);
  digitalWrite(OUTB1, HIGH);
  digitalWrite(OUTB2, LOW);
}

void moveBackward() {
  // Configura los pines de los motores para moverse hacia atrás
  digitalWrite(OUTA1, LOW);
  digitalWrite(OUTA2, HIGH);
  digitalWrite(OUTB1, LOW);
  digitalWrite(OUTB2, HIGH);
}

void stopMovement() {
  // Configura los pines de los motores para detener el movimiento
  digitalWrite(OUTA1, LOW);
  digitalWrite(OUTA2, LOW);
  digitalWrite(OUTB1, LOW);
  digitalWrite(OUTB2, LOW);
}
