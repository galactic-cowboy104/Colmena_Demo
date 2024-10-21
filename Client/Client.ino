#include <Adafruit_NeoPixel.h>

// Definición de los pines de salida para el control de los motores
#define OUTA1 5
#define OUTA2 6
#define OUTB1 10
#define OUTB2 11

// Definición del pin y número de LEDs para los NeoPixels
#define PIXELS_PIN 4
#define NUMPIXELS 6

// Creación de un objeto NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELS_PIN, NEO_GRB + NEO_KHZ800);

// Variables para almacenar el comando recibido y el último comando procesado
String command = "";
String lastCommand = "";

void setup() {
  
  // Inicializa la comunicación serie a 9600 baudios
  Serial.begin(9600);

  // Inicializa los LEDs NeoPixel y limpia su estado
  pixels.begin();
  pixels.clear();

  // Ciclo para encender cada LED en rojo, uno por uno
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Establece el color rojo para el LED actual
    pixels.show(); // Muestra el cambio en el LED
    delay(100); // Espera 100 ms antes de encender el siguiente LED
  }

  // Espera de 1 segundo antes de continuar
  delay(1000);

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

    // Si el comando es diferente al último procesado, lo procesa
    if (command != lastCommand) {
      lastCommand = command; // Actualiza el último comando
      handleCommand(command); // Llama a la función para manejar el comando
    }
    
  }
  
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
  }
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
