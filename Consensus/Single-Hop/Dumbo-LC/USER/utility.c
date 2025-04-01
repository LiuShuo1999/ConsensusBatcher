#include "utility.h"


//crypto 
char public_key_string[13][192] = {
"63 43 ef 73 c9 8a 88 9b b4 6d 7c 16 e2 2d 1f d1 ff 0f 74 82 a5 97 b7 1d 0c 62 d8 bf cf 77 f1 5f 94 ba dd 4c 8b 81 5e b5 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"06 39 48 2a 7d d3 73 7e 35 f9 a8 df aa b7 19 d7 e6 79 09 21 2c 81 63 14 60 b7 3b 53 4c bb b9 84 97 32 ad 1a 97 5a bb a8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"63 ea 52 a4 6f b1 cc 68 3b 08 67 ba c8 74 eb 0c 2c 85 de ff ec 9a 6c b6 26 7e ac 42 f1 ab ff bb 6d 4d 1a 35 21 94 16 9a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"61 78 78 65 ab 82 65 91 b7 a4 17 b4 f5 70 9d ba 2c 68 80 90 02 2a 37 b1 a3 71 b9 d2 da 1c 79 3f 53 95 d7 0a 3b df aa 73 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"25 12 83 ec 52 92 57 e8 26 00 f2 56 ad 47 43 03 02 b3 48 3d 90 66 71 2c 3b bd f7 6d 9a cc fb 5d 76 c1 db e3 b6 c1 09 be 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"b9 95 d5 e4 b1 e0 f4 56 15 7f 57 ac d6 0d d8 f2 75 e8 06 20 a7 35 7e a4 fc 4c 15 42 a2 9f 28 cb 67 58 04 70 e6 75 12 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"13 90 1a d0 b1 53 f4 46 eb d2 50 b2 50 b5 25 00 e2 01 b7 eb 10 98 33 62 eb 7c f6 fe 4d 36 ee bf 6f 8f b1 7a 7c 12 92 4e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"c8 6e ca a5 fd dd 39 c1 bd 57 cd 69 bd 8d f6 03 b4 49 8b 74 5e 1c 62 79 b3 12 c2 3e 75 32 d8 52 8d d2 c2 e2 ea 59 bf e0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"30 96 f8 d5 c1 9f 44 11 25 e7 ed f9 26 ea 9b 06 c0 d0 08 ce 93 88 29 a0 cc 48 58 84 c5 63 9c b1 fc 7c 0d f2 77 b1 ff ae 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"75 7f e2 fe 9d 0b c2 cd e4 0f e5 37 33 3d d7 9c c7 e5 19 bc 52 d6 9b 06 13 16 39 7d ab ed 0f 88 4e 72 e3 1b 30 29 e4 f5 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"85 45 b9 31 93 0a cd 4d fe 10 55 39 1f 7a 9d 49 26 3a c8 32 a8 1c 75 19 3c 9a f7 9f f0 b9 99 18 45 f7 58 cc bc 0b 3a d5 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"ec 9b f5 0c eb 44 a7 49 d5 7a 47 31 84 8e 9c 1d 88 5b 89 e7 de 06 53 4c 77 b7 8e 25 e9 4f fb e2 49 f0 e8 b6 3a 2c ab a0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"1a 3c 0e d9 5f a4 5a 9a 07 f2 f0 b6 53 ca a8 46 df e0 75 56 9a 07 b0 3b 30 16 30 f9 cb 85 ff 56 ec b0 18 34 86 b2 cf 26 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" 
};
char private_key_string[13][96] = {
"00 0d 89 de cc 58 2a 35 8d 58 7d 9f f1 c5 dd d3 40 32 7b 5c 67 00 00 00 00 00 00 00 00 00 00 00", 
"00 4f 9f 66 a9 e3 f8 41 15 df 4d 5f 3e e2 0a 94 83 09 7d 2d e3 00 00 00 00 00 00 00 00 00 00 00", 
"00 06 22 41 74 b6 ae e8 01 3e 26 8b f0 98 4d 74 92 76 c8 a8 af 00 00 00 00 00 00 00 00 00 00 00", 
"00 56 ec c8 d0 36 a9 39 0b c8 d8 4e 94 56 16 4a be ce 2c 44 2f 00 00 00 00 00 00 00 00 00 00 00", 
"00 ab 9b 57 9c 35 50 bf 54 8a bf 28 3e 94 15 c9 46 b9 4e 34 0f 00 00 00 00 00 00 00 00 00 00 00", 
"00 92 48 7b 46 4e f7 b9 ad b4 cf d6 58 d0 3f b1 e4 1a 06 80 72 00 00 00 00 00 00 00 00 00 00 00", 
"00 43 f3 39 ef 26 6c de 98 a3 b5 c1 5e e1 0f a7 af 65 d8 df 5c 00 00 00 00 00 00 00 00 00 00 00", 
"00 ea 6e de 11 c2 64 03 2c f1 30 59 b8 52 ad 85 97 28 74 d3 67 00 00 00 00 00 00 00 00 00 00 00", 
"00 e6 4c 79 9b c8 f7 02 e1 43 94 e9 9e 54 0f 6e 5a 80 d5 db d4 00 00 00 00 00 00 00 00 00 00 00", 
"00 01 e3 81 8b 15 03 51 ba 8b c6 dd 56 2f 7e 53 55 a2 f9 b4 8a 00 00 00 00 00 00 00 00 00 00 00", 
"00 97 91 cd 76 cd c9 b9 71 25 31 27 72 e0 9c 59 b2 e6 95 74 37 00 00 00 00 00 00 00 00 00 00 00", 
"00 fb 1c 4a f1 76 47 c2 f3 c2 03 53 b7 4d 6b 49 a2 42 c3 38 ee 00 00 00 00 00 00 00 00 00 00 00", 
"00 ee b7 74 f0 cf 3b 11 a4 e2 fe 5b fd 6c b5 af c1 17 04 84 36 00 00 00 00 00 00 00 00 00 00 00" 
};
uint8_t private_key[13+1][32] = {0};
uint8_t public_key[13+1][64] = {0};

void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{	
//	TIM2->CNT = 0;
	//关闭某些低功耗模式下不使用的外设时钟
	//SystemClock_Config_low();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOF_CLK_DISABLE();
	__HAL_RCC_GPIOG_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
	
//	pre_time[pre_idx] = TIM2->CNT;
//	pre_idx++;
//	if(pre_idx==200)pre_idx = 0;
}

//退出低功耗模式以后需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
//	TIM2->CNT = 0;
	//退出低功耗模式以后打开那些被关闭的外设时钟
	//SystemClock_Config();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
//	post_time[post_idx] = TIM2->CNT;
//	post_idx++;
//	if(post_idx==200)post_idx = 0;
}

u8 Sixteen2Ten(char ch){
	if( (ch >= '0') && ( ch <= '9' ) )	return ch - '0';
	if((ch >= 'A') && ( ch <= 'Z' )) return 10 + (ch - 'A');
	return 10 + (ch - 'a');
}

static int my_RNG(uint8_t *dest, unsigned size){
	u32 random;
	for(int i=0;i<size;i++){
		u8 Random_idx = i % 4;
		if( Random_idx % 4 == 0) random = RNG_Get_RandomNum();
		dest[i] = (random >> (Random_idx * 8) ) & 0xFF;
	}
	return 1;
}


void init_public_key(){
	while(RNG_Init())	 		    //初始化随机数发生器
	{ 
		delay_ms(200); 
	} 
	u8 string_idx=0;
	u8 key_idx = 0;
	for(u8 l=1;l<=13;l++){
		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(public_key_string[l-1]);){
			public_key[l][key_idx] = Sixteen2Ten(public_key_string[l-1][string_idx])*16+Sixteen2Ten(public_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}

		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(private_key_string[l-1]);){
			private_key[l][key_idx] = Sixteen2Ten(private_key_string[l-1][string_idx])*16 + Sixteen2Ten(private_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}
		
	}
	

    int num_curves = 0;
	//curves[num_curves++] = uECC_secp192r1();
//#if uECC_SUPPORTS_secp160r1
//    curves[num_curves++] = uECC_secp160r1();
//#endif
#if uECC_SUPPORTS_secp192r1
    curves[num_curves++] = uECC_secp192r1();
#endif
//#if uECC_SUPPORTS_secp224r1
//    curves[num_curves++] = uECC_secp224r1();
//#endif
//#if uECC_SUPPORTS_secp256r1
//    curves[num_curves++] = uECC_secp256r1();
//#endif
//#if uECC_SUPPORTS_secp256k1
//    curves[num_curves++] = uECC_secp256k1();
//#endif
    uECC_set_rng(my_RNG);
}






/////////////////System Configure
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;//RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
//  RCC_OscInitStruct.PLL.PLLM = 8;
//  RCC_OscInitStruct.PLL.PLLN = 432;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 9;
//  RCC_OscInitStruct.PLL.PLLR = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }

  /* Activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1) {};
  }


  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    while(1) {};
  }
}


void SystemClock_Config_low(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;//RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
//  RCC_OscInitStruct.PLL.PLLM = 8;
//  RCC_OscInitStruct.PLL.PLLN = 432;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 9;
//  RCC_OscInitStruct.PLL.PLLR = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }

  /* Activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1) {};
  }


  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    while(1) {};
  }
}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
	SCB_EnableICache();

  /* Enable D-Cache */
	SCB_EnableDCache();
	SCB->CACR|=1<<2;   												/* 强制D-Cache透写,如不开启,实际使用中可能遇到各种问题 */

}