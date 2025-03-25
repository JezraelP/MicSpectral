#ifndef _MICROPHONE_H_
#define _MICROPHONE_H_


extern volatile bool dma_transfer_complete;  // Global flag of DMA transfer completion
extern int dma_channel;  // Global variable for DMA channel

void Mic_Init(void);
void Mic_InitDMA(void);

void Mic_SampleRate(uint32_t sample_rate);
   
uint16_t Read_Mic(void);
void Mic_ReadBuffer(uint16_t *buffer, uint32_t size);
void Mic_ReadBufferDMA(uint16_t *buffer, uint32_t size);
void Mic_ReadBufferDMA_IT(uint16_t *buffer, uint32_t size);

void dma_handler(void);  //  Adicionado para evitar erro de reconhecimento

#endif // _MICROPHONE_H_