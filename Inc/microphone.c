#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "microphone.h"
#include "hardware/irq.h"

#define MIC_PIN 28
#define ADC_CLOCK 48000000
#define ADC_CYCLES 96


volatile bool dma_transfer_complete = false;
int dma_channel = 2;  

dma_channel_config dma_cfg;

// Handler da interrupção do DMA 

__attribute__((weak)) void dma_handler() {
    dma_hw->ints0 = 1u << dma_channel;  
    dma_transfer_complete = true;
}

void Mic_Init(void){
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);  // Select ADC4 (GPIO28)
}

void Mic_InitDMA(void){
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);  // Select ADC4 (GPIO28)

    adc_fifo_setup(
        true, // Habilitar FIFO
        true, // Habilitar request de dados do DMA
        1, // Threshold para ativar request DMA é 1 leitura do ADC
        false, // Não usar bit de erro
        false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
      );
    
    dma_channel = dma_claim_unused_channel(true);
    dma_cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_cfg, false); //Don't increment the read address (ADC)
    channel_config_set_write_increment(&dma_cfg, true); //Increment the write address (Buffer)
    channel_config_set_dreq(&dma_cfg, DREQ_ADC); //DMA request from ADC

    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

}


//OBS: the microphone of BitDogLab has 1.65V as offset
uint16_t Read_Mic(void){
    uint16_t result = adc_read();
    //float voltage = (result * 3.3) / 4095;
    return result; //its more interesting to return the raw value
}


// Adc needs 96 clocks to sample
void Mic_SampleRate(uint32_t sample_rate){
    /*if (sample_rate > ADC_MAX_SAMPLE_RATE)
        sample_rate = ADC_MAX_SAMPLE_RATE;*/  //This is take by hardware
    float clk_div = (clock_get_hz(clk_adc) / (1.0 *sample_rate)) - 1; //-1 its necessary, see datasheet

    adc_set_clkdiv(clk_div);
}

void Mic_ReadBuffer(uint16_t *buffer, uint32_t size){
    for (uint32_t i = 0; i < size; i++){
        buffer[i] = Read_Mic();
    }
}



void Mic_ReadBufferDMA(uint16_t *buffer, uint32_t size){
    adc_fifo_drain(); //Clean the FIFO
    adc_run(false);  //Stop the ADC

    dma_channel_configure(
        dma_channel,
        &dma_cfg,
        buffer,
        &adc_hw->fifo,
        size,
        true // Start DMA
    );
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel); //Remove this if dont want to block
    adc_run(false);
}


void Mic_ReadBufferDMA_IT(uint16_t *buffer, uint32_t size){
    adc_fifo_drain(); //Clean the FIFO
    adc_run(false);  //Stop the ADC

    dma_channel_configure(
        dma_channel,
        &dma_cfg,
        buffer,
        &adc_hw->fifo,
        size,
        true // Start DMA
    );

    dma_channel_set_irq0_enabled(dma_channel, true);

    adc_run(true);
}

