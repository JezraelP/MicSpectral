#define BUZZER_PIN_A 21
#define BUZZER_PIN_B 10
#define CLK_DIV 4.0f

void pwm_init_buzzer(uint pin);

void play_tone2(uint pin, uint frequency, uint duration_ms);

void check_and_play_next_note(uint pin);