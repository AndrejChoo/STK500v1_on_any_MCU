#ifndef USART_H_
#define USART_H_



void usart_init(void);
void uart_sendChar(char ch);
void uart_sendString(char *str);

#endif /* USART_H_ */