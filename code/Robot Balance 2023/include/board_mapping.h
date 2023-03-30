// Broches utilisées par le robot
// https://github.com/cegep-electronique-programmable/robot-balance-2023/#choix-des-broches-du-microcontr%C3%B4leur


// UART
#define GPIO_UART_TX            1
#define GPIO_UART_RX            3

// Boutons utilisateurs
#define GPIO_B1                 36  // Input only
#define GPIO_B2                 39  // Input only

// Points de test (entrées seulement)
#define GPIO_TP1                34  // Input only
#define GPIO_TP2                35  // Input only

// Activation des moteurs
#define GPIO_ENABLE_MOTEURS     32  

// Moteur droit
#define GPIO_DIR_D              33
#define GPIO_STEP_D             25
#define GPIO_MS1_D              14
#define GPIO_MS2_D              27
#define GPIO_MS3_D              26

// Moteur gauche
#define GPIO_DIR_G              17
#define GPIO_STEP_G             16
#define GPIO_MS1_G              2
#define GPIO_MS2_G              15
#define GPIO_MS3_G              13

// LED
#define GPIO_DATA_LED           4
#define NEOPIXEL_PIN            GPIO_DATA_LED
#define NEOPIXEL_COUNT          5 

// SPI
#define GPIO_VPSI_CS1           5
#define GPIO_VPSI_SCK           18
#define GPIO_VPSI_MISO          19
#define GPIO_VPSI_MOSI          23

// I2C
#define GPIO_I2C_SDA            21
#define GPIO_I2C_SCL            22