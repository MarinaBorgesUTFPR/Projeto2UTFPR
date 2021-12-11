//===================================================================================
//Data: 22/11/2021
//Autora: Marina Lourenço Borges
//Descrição: Lab_2 - Microcontrolados S23
//===================================================================================


//1. Declarações para o Software
//Declaração da definição da Porta F
#define NVIC_EN0_R (*((volatile unsigned long *)0xE000E100)) 
#define NVIC_PRI7_R (*((volatile unsigned long *)0xE000E41C))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_ICR_R (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_PDR_R (*((volatile unsigned long *)0x40025514))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_LOCK_R (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R (*((volatile unsigned long *)0x40025524))
#define SYSCTL_RCGCGPIO_R ( *( (volatile unsigned long * ) 0x400FE608 ) )
#define SYSCTL_RCGC2_GPIOF 0x00000020 

// Declaração da definição da Porta D
#define GPIO_PORTD_DATA_R (*((volatile unsigned long *)0x40007020))
#define GPIO_PORTD_DIR_R (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_AMSEL_R (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R (*((volatile unsigned long *)0x4000752C))


//2. Variaveis Utilizadas no projeto e Declaração de subrotinas para a Main
#include <stdint.h>
int duration;
unsigned long PD3;
unsigned long In;  		// Input para PF4
unsigned long Out; 		// Visor para PD3
unsigned long OutGreen;	 	// Visor para LED green
unsigned long OutBlue; 		// Visor para LED blue
unsigned long OutRed; 		// Visor para LED red
unsigned long OutRed2; 		// Visor para LED red
unsigned long OutLed; 		// Visor para LED red
unsigned int flash_Green = 0;	// Looping para LED Verde
unsigned int flash_Red = 0;	// Looping para LED Vermelho
unsigned int flash_Red2 = 0;	// Looping para LED Vermelho frequência 2.5 Hz
unsigned int flash_All = 0;	// Looping para todos os LEDs


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void); // Enable interrupts 7
long StartCritical (void); // previous I bit, disable interrupts
void EndCritical(long sr); // restore I bit to previous value
void PortF_Init(void);
void PortD_Init(void);
void Delay(void);
void Delay_1(void);



volatile uint32_t FallingEdges = 0;
#define PF1 (*((volatile uint32_t *)0x40025008))
#define PF2 (*((volatile uint32_t *)0x40025010))
#define PF3 (*((volatile uint32_t *)0x40025020))


void EdgeCounter_Init(void){
 SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
 FallingEdges = 0; // (b) initialize counter
 GPIO_PORTF_DIR_R = 0x1E; // (c) make PF4 in (built-in button)
 GPIO_PORTF_AFSEL_R = 0x00; // disable alt funct on PF4
 GPIO_PORTF_DEN_R |= 0x1E;
 GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
 GPIO_PORTF_AMSEL_R = 0; // diblob:https://web.whatsapp.com/6b46d50f-3fb9-4d78-938e-b9801968079esable analog functionality on PF
 GPIO_PORTF_PUR_R |= 0x11;
 GPIO_PORTF_IS_R &= 0x01; // (d) PF4 is edge-sensitive
 GPIO_PORTF_IBE_R &= ~0x01; // PF4 is not both edges
 GPIO_PORTF_IEV_R &= ~0x01; // PF4 falling edge event
 GPIO_PORTF_ICR_R = 0x01; // (e) clear flag4
 GPIO_PORTF_IM_R |= 0x01; // (f) arm interrupt on PF4 *** No IME bit as mentioned    in Book ***
 NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
 NVIC_EN0_R = 0x40000000; // (h) enable interrupt 30 in NVIC
 EnableInterrupts(); // (i) Clears the I bit

}

void GPIOPortF_Handler(void){
 GPIO_PORTF_ICR_R = 0x01; // acknowledge flag4
 FallingEdges = FallingEdges + 1;
}






//3. Código

int main(void){
PortD_Init(); //Chama a subrotina para iniciar a PF4 e PF2   
EdgeCounter_Init(); //Chama a subrotina para inicializar a interrupção
	
	while(1){
	GPIO_PORTF_DATA_R = 0x00;
	PD3 = GPIO_PORTD_DATA_R&0x08; //leitura da PD3

	if(FallingEdges==1)
	{
	// Estado 1 - LED  azul pisca em 2.5 Hz e PD3 desligado
				GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED acende Azul
					OutBlue = GPIO_PORTF_DATA_R&0x04; //Saída para o Led Azul
					Delay_1(); // Delay 2,5Hz
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED off
					OutBlue = GPIO_PORTF_DATA_R&0x04; //Saída para o Led Azul
					Delay_1();  // Delay 2,5Hz
			
					GPIO_PORTD_DATA_R = 0x00;  //Inicializa a PD3 como LOW 0x0000_0000
					Out = GPIO_PORTD_DATA_R&0x08;//Indicar o que tem na porta PD3
	}
		if(FallingEdges==2)
		{
			
			
		//Estado 2 - Se chave SW2 pressionada, vá para case 4. Caso contrário, ir para case 3	
	
		if((GPIO_PORTF_DATA_R&0x01)!=0x01)
		{
			// Estado 3 - LED verde pisca 5 vezes
			while (flash_Green < 5) 
				{
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED acende Verde
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar nível no LED green
					Delay();
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar nível no LED green
					Delay();	
					flash_Green = flash_Green + 1;
				}	
					FallingEdges=1;
						
			}
			
		else{ 
		 while (flash_Red < 2)
			{// Estado 4 - LED red piscar 2 vezes
				GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED is red
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar nível no LED red
				Delay();
				GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar nível no LED red
				Delay();
				flash_Red = flash_Red + 1;
			}
		}
		
		if(FallingEdges==3)
		{
//Estado 5 - Fazer LED red piscar na frequência 2.5 Hz => T = 0.4 (s 2 vezes)
			while (flash_Red2 < 2) 
			{
				GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED is red 2.5 Hz
				Delay_1();     // initialize timer1 (2.5Hz)
				OutRed2 = GPIO_PORTF_DATA_R&0x02;
				GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
				Delay_1();     // initialize timer1 (2.5Hz)
				OutRed2 = GPIO_PORTF_DATA_R&0x00;
				flash_Red2 = flash_Red2 + 1;
			}
		}
		
			if(FallingEdges==4)
			{
			//Estado 6 -  Piscar os 3 LEDs em sequência, 4 vezes e PD3 high
				GPIO_PORTD_DATA_R = 0x08;  // 0x0000_0000 PD3 is high
				Out = GPIO_PORTD_DATA_R&0x08;// indicar o que tem na porta PD3
				Delay_1();
				
				while (flash_All < 4)
				{
					GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED Vermelho
					Delay();
					GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED Azul
					Delay();
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED Verde
					Delay();
					flash_All = flash_All + 1;
				}
				if(FallingEdges==5)
				{
					FallingEdges = 0;
					
				}
			}
		}
	}
}

//4. SubRotinas

//Subrotina Padrão de iniciação
void PortF_Init(void){ volatile unsigned long delay;
SYSCTL_RCGC2_R |= 0x00000020; // 1) F clock
delay = SYSCTL_RCGC2_R; // delay
GPIO_PORTF_CR_R = 0x1F; // allow changes to PF4-0
GPIO_PORTF_PCTL_R = 0x0000FFFF; // 4) GPIO clear bit PCTL
}

void PortD_Init(void){ volatile unsigned long delay;
SYSCTL_RCGC2_R |= 0x00000008; // 1) D clock
delay = SYSCTL_RCGC2_R; // delay é necessário desbloquear algum pinono D com o
GPIO_PORTD_AMSEL_R = 0x00; // 3) disable analog function
GPIO_PORTD_PCTL_R = 0x00000000; // 4) GPIO configura como GPIO?
GPIO_PORTD_DIR_R = 0x08; // 5) PD3 output
GPIO_PORTD_AFSEL_R = 0x00; // 6) no alternate function
GPIO_PORTD_DEN_R = 0x1F; // 7) enable digital pins PD4-PD0
}




//Subrotina Delay para 0.2 segundos
void Delay(void){unsigned long volatile time;
  time = 727240*200/91;  // 0.2sec
  while(time){
		time--;
  }
}


//SubRotina Delay para fequencia 2.5Hz
void Delay_1(void){unsigned long volatile time;
  time = 727240*200*2/97;  // 0.4sec = T => f = 2.5 Hz
  while(time){
		time--;
  }
}
