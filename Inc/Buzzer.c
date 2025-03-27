#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "Buzzer.h"

//Em (HZ)
const uint note_frequency[] = {
    1319, 1319, 1319, 1319, 1319, 1319, 1319, 1568, 1047, 1175, 1319, 
    880, 1047, 1175, 1319, 1319, 1319, 1319, 1175, 1175, 1175, 1175, 
    1175, 1047, 1175, 1319, 1568, 1319, 1319, 1319, 1319, 1319, 1319, 
    1568, 1047, 1175, 1319, 880, 1047, 1175, 1319, 1319, 1319, 1319, 
    1175, 1175, 1175, 1175, 1175, 1047, 1175, 1319, 1568, 1319, 0, 
    1397, 1397, 1397, 1397, 1397, 1397, 1319, 1319, 1319, 1319, 
    1319, 1175, 1175, 1319, 1175, 1568
};
//Em (ms)
const uint note_duration[] = {
    300, 300, 600, 300, 300, 600, 300, 300, 300, 300, 600, 
    300, 300, 600, 900, 300, 300, 600, 300, 300, 600, 300, 
    300, 600, 300, 300, 600, 600, 300, 300, 600, 300, 300, 
    600, 300, 300, 300, 300, 300, 300, 600, 300, 300, 600, 
    300, 300, 600, 300, 300, 600, 600, 300, 900, 600, 
    300, 600, 300, 300, 600, 300, 300, 600, 300, 
    300, 600, 300, 300, 600, 900, 600
};

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin); //Captura o slice do PWM referente ao pino escolhido
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, CLK_DIV); 
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);  // Desliga o PWM inicialmente
}

void play_tone(uint pin, uint frequency, uint duration_ms){
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys)/CLK_DIV;
    uint32_t top = clock_freq / frequency - 1;
    pwm_set_wrap(slice_num, top);
    sleep_ms(duration_ms);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle
    sleep_ms(15);
}



void play_tone2(uint pin, uint frequency, uint duration_ms){
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys)/CLK_DIV;
    uint32_t top = clock_freq / frequency - 1;
    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle
}
// Verifica e toca a próxima nota da música se o tempo tiver passado
void check_and_play_next_note(uint pin) {

    static uint current_note = 0;  // Índice para acompanhar a música
    static uint32_t last_time = 0; // Para controlar o tempo da execução das notas
    uint32_t current_time = time_us_32() / 1000; // Em milissegundos
    static uint first = 1;
    if (first) {
        play_tone2(pin, note_frequency[current_note], note_duration[current_note]);
        first =0;
    }


    if (current_time - last_time >= (note_duration[current_note])){
        pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    }
    //Espera 15 ms a mais para trocar de nota
    if (current_time - last_time >= (note_duration[current_note]) + 15) {
        current_note++;
        if (note_frequency[current_note] != 0) {
            play_tone2(pin, note_frequency[current_note], note_duration[current_note]);
        }
        last_time = current_time;
        
        
        if (current_note >= sizeof(note_frequency) / sizeof(note_frequency[0])) {
            current_note = 0; // Reinicia a música
        }
    }
}