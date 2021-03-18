#include "gpioaregisters.hpp" //for Gpioa
#include "rccregisters.hpp"   //for RCC
#include "nvicregisters.hpp"  //for NVIC
#include "usart2registers.hpp" //for UART
#include "tim2registers.hpp"   //for TIM2

extern "C"
{
  
  
  
  int __low_level_init(void)
  {
    
    RCC::CR::HSEON::On::Set();
    while (!RCC::CR::HSERDY::Ready::IsSet())
    { }
    RCC::CFGR::SW::Hse::Set();
    while (!RCC::CFGR::SWS::Hse::IsSet())
    { }
    RCC::CR::HSION::Off::Set();
    RCC::AHB1ENR::GPIOAEN::Enable::Set();
    
       
    RCC::APB1ENR::USART2EN::Enable::Set();   
    
    
    GPIOA::MODER::MODER2::Alternate::Set();
    GPIOA::MODER::MODER3::Alternate::Set();
    GPIOA::AFRL::AFRL2::Af7::Set();
    GPIOA::AFRL::AFRL3::Af7::Set();
    
    GPIOA::OTYPER::OT2::OutputPushPull::Set();//двухтактный выход
    GPIOA::OTYPER::OT3::OutputPushPull::Set();
    GPIOA::PUPDR::PUPDR2::PullUp::Set();//Подтяжка к 1
    GPIOA::PUPDR::PUPDR3::PullUp::Set();
    
    uint32_t USART_DIV = 16'000'000/(19'200*8*(2 - USART2::CR1::OVER8::Get()));
    USART2::BRR::DIV_Mantissa::Set(USART_DIV);
    
    
    USART2::CR1::OVER8::OversamplingBy8::Set();
    USART2::CR1::M::Data8bits::Set();
    USART2::CR2::STOP::Value2::Set();
    USART2::CR1::PCE::ParityControlDisable::Set(); 
    
    NVIC::ISER1::Write(1<<6);
    
  
    
    USART2::CR1::UE::Enable::Set(); //Включение USART
    
    RCC::APB1ENR::TIM2EN::Enable::Set();
    TIM2::PSC::Write(7999) ;     
    TIM2::ARR::Write(1000) ;
    TIM2::SR::UIF::NoInterruptPending::Set();
    TIM2::CNT::Write(0);
    TIM2::CR1::CEN::Enable::Set(); 
 
    return 1;
  }
}

int main()
{
  char buff[] = "Hello, world!\n";
  for(;;)
  {   
      USART2::CR1::TE::Enable::Set();
      USART2::CR1::TXEIE::InterruptWhenTXE::Set();
      USART2::DR::Write(buff[0]);
      while(TIM2::SR::UIF::NoInterruptPending::IsSet()) ;
      TIM2::SR::UIF::NoInterruptPending::Set();    
      
  }
  return 1;
}