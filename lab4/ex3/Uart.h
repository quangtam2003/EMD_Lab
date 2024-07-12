#define IER_EnableRBR                 0x01
#define IER_EnableTHRE                0x02
#define IER_EnableRLS                 0x04

#define IIR_ReceiveLineStatus		      0x03
#define IIR_ReceiveDataAvailable      0x02
#define IIR_CharacterTimeoutIndicator	0x06
#define IIR_THREInterrupt           	0x01

#define LSR_RecieverDataReady		      0x01
#define LSR_OverrunError              0x02
#define LSR_ParityError               0x04
#define LSR_FramingError              0x08
#define LSR_BreakInterrupt            0x10
#define LSR_TransmitterHoldingRegisterEmpty	0x20
#define LSR_TransmitterEmpty          0x40
#define LSR_ErrorInRxFifo             0x80

#define BUFFER_SIZE		                0x40

extern unsigned int UART_Buffer_Count;
extern unsigned char UART_Buffer[BUFFER_SIZE];
extern void UART_Initialize(unsigned int baudrate);
extern void UART_Send(char *bufferPtr, unsigned int length);
void uart_TxChar(char ch);
