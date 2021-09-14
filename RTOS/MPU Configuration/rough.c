//Memory Protection Unit Registers
#define MPU_CTRL		 (*((volatile unsigned long*) 0xE000ED94))	//Memory Protection unit-> Controll Register
#define MPU_RNR			 (*((volatile unsigned long*) 0xE000ED98))	//Memory Protection unit-> Region Number Register
#define MPU_RBAR		 (*((volatile unsigned long*) 0xE000ED9C))	//Memory Protection unit-> Region Base Address Register
#define MPU_RASR		 (*((volatile unsigned long*) 0xE000EDA0))	//Memory Protection unit-> Region Attribute and Size Register

#define SCB_SHCSR		 (*((volatile unsigned long*) 0xE000ED24))	//System handler control and state register 
#define NVIC_ISER		 (*((volatile unsigned long*) 0xE000E100))	//Interrupt set enable register 
	
void Registers_Init(void)
{		
	//MPU Configuring
	MPU_RNR = 0x00000000;							// marking bit 0 to 0, this means that we are using region 0 of mpu.
	MPU_RBAR = 0x20000000;						// storing base address of memory region. (Base address must be multiple of region size.)
	MPU_RASR = 0x1608FF0B;						// Storing size and permissions, 64 bytes, ro/ro, cachable, not subregions.
//	NVIC_ISER |= 0x00000018;						// Interrupt set enable register
	SCB_SHCSR |= 0x00010000;					// enable MemManage Fault	
	MPU_CTRL = 0x00000005;						// enable memory protection unit,guaranteeing default priviliged access
}

void MemManage_Handler(void)
{
	__asm(
			"MOV R4, 0x77777777\n\t"
			"MOV R5, 0x77777777\n\t"
	);
}
	
void MemFault_Handler(void)				// Interrupt service routines, Bad memory access/MPU Exceptions
{	
	__asm(
			"MOV R4, 0x77777777\n\t"
			"MOV R5, 0x77777777\n\t"
	);
}

void HardFault_Handler(void)
{ 
   __asm(
			"MOV R4, 0x77777777\n\t"
			"MOV R5, 0x77777777\n\t"
	);
}


int main(void)
{
	Registers_Init();

	while(1)
	{
		__asm(
			"LDR R0, =0x20000000\n\t"
			"MOV R1, 0x77777777\n\t"
			"STR R1, [R0,#4]"
		);
	}
	return (1);
}

void SystemInit(void)
{
}