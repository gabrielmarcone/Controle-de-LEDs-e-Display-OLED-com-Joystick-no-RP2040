#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

// Definindo os pinos do LED RGB
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Definindo os pinos dos botões
#define BUTTON_A 5
#define BUTTON_JOYSTICK 22

// Função para configurar os pinos GPIO
void config_gpio() {
    // Inicializando e configurando LEDs como saída
    gpio_init(LED_GREEN);
    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);

    // Inicializando os botões como entrada e resistores pull-up
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_JOYSTICK);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_JOYSTICK, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_JOYSTICK);
}

int main() {
    stdio_init_all();
    config_gpio();

    while (true) {
    }
}
