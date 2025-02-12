#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definindo os pinos do LED RGB
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Definindo os pinos dos botões
#define BUTTON_A 5
#define BUTTON_JOYSTICK 22

// Debounce time
#define DEBOUNCE_TIME 200  // 200 ms

// Estado dos LEDs e botões
volatile bool state_green_led = false;
volatile bool state_blue_led = false;
volatile bool state_red_led = false;
volatile uint32_t last_press_time_A = 0;
volatile uint32_t last_press_time_J = 0;

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

// Callback para interrupções dos botões
void button_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == BUTTON_A) { // Aciona os LEDs Vermelho e azul por PWM
        if (current_time - last_press_time_A > DEBOUNCE_TIME * 1000) { // Debounce
            last_press_time_A = current_time;  // Atualiza o tempo da última pressão
            // Lógica para a alteração por PWM
        }
    } 
    else if (gpio == BUTTON_JOYSTICK) { // Liga/Desliga o LED Verde e controla a borda do display
        if (current_time - last_press_time_J > DEBOUNCE_TIME * 1000) { // Debounce
            last_press_time_J = current_time;  // Atualiza o tempo da última pressão
            state_green_led = !state_green_led;  // Alterna o estado
            gpio_put(LED_GREEN, state_green_led);
            printf("Botão Joystick pressionado. LED Verde: %d\n", state_green_led);
        }
    }
}

int main() {
    stdio_init_all();
    config_gpio();

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);

    while (true) {
    }
}