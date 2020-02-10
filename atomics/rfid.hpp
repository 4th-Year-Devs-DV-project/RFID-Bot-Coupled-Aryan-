/**
* Ben Earle and Kyle Bjornson
* ARSLab - Carleton University
*
* Blinky:
* Simple modle to toggle the LED using DEVS internal transitions.
*/

#ifndef BOOST_SIMULATION_PDEVS_RFID_HPP
#define BOOST_SIMULATION_PDEVS_RFID_HPP

#include <stdio.h>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include "../drivers/MFRC522.cpp"
#include "../drivers/nRF24L01P.h"
#include <cadmium/real_time/arm_mbed/embedded_error.hpp>

using namespace cadmium;
using namespace std;

    
struct rfid_defs 
{
        //struct dataOut : public out_port<bool> { };
        struct dataOut : public out_port<double> { };
        //struct idOut : public out_port<float> { };
        struct in : public in_port<bool> { };
};


template<typename TIME>
class Rfid
{
    
  using defs=rfid_defs; // putting definitions in context
  public:

  TIME   slowToggleTime;

  Rfid(PinName sda, PinName scl, PinName scl2, PinName scl3, PinName scl4) 
  {
    slowToggleTime    = TIME("00:00:59:00");
    state.temp_humid_sensor = new drivers::MFRC522(sda,scl,scl2,scl3,scl4);
    state.newTag = false;
    state.isTag = false;
    state.light = false;

    /////////////////////////////////////
    //state.temp = new nRF24L01P(D11, D12, D3, D15,D14,D9);
    //state.temp->powerUp();
  
    //////////////////////////////////
  }
    
  // state definition
  struct state_type
  {
    uint8_t id[10];
    drivers::MFRC522* temp_humid_sensor;
    //nRF24L01P* temp;
    //char s[32] =  " Car 2  says hi ";
    bool newTag;
    bool isTag;
    bool light;
  }; 
  state_type state;

  using input_ports=std::tuple<>;
  using output_ports=std::tuple<typename defs::dataOut>;

  // internal transition
  void internal_transition() {
  	//printf("newtag is %f", state.newTag);
  	state.newTag = false;
    state.isTag = false;
    //sleep(50000);
    //////////////////////////
    
    /////////////////////////
    

    for(int i =0; i <10; i++)
    {
      printf("anything????\n");
      if (state.temp_humid_sensor->PICC_IsNewCardPresent() ) 
      {
        state.temp_humid_sensor->PICC_ReadCardSerial();
    
    printf("foundddddddddddddd\n");
        state.newTag = true;
        state.id[0] = state.temp_humid_sensor->uid.uidByte[0];
        state.id[1] = state.temp_humid_sensor->uid.uidByte[1];
        state.id[2] = state.temp_humid_sensor->uid.uidByte[2];
        state.id[3] = state.temp_humid_sensor->uid.uidByte[3];
        break;
        //}
      }
    }
    if( state.newTag == true)
    {
            /////////////////////////////////////////////////
      /*
            printf( "nRF24L01+ Frequency    : %d MHz\r\n",  state.temp->getRfFrequency() );
              printf( "nRF24L01+ Output power : %d dBm\r\n",  state.temp->getRfOutputPower() );
              printf( "nRF24L01+ Data Rate    : %d kbps\r\n", state.temp->getAirDataRate() );
              printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", state.temp->getTxAddress() );

                state.temp->setTransferSize(32);
                state.temp->setTransmitMode();
                state.temp->enable(); // enable ce pin
                state.temp->write(1, state.s,32); // writing hello world
                printf("%i \n",sizeof(state.s));
                printf( "Sending data:%s \n",state.s);
                state.temp->disable();
                */
              ////////////////////////////////////////////////////
        printf("new tag found");
       	for (uint8_t i = 0; i < 4; i++)
        {
        	printf(" %f", (double)state.id[i]);
        }
        printf("\n\r");
        //break;
        if(state.id[0] == 105)
        { 
          printf("yessssssssssssssssssssssssssssssssssssssssssssssssssss");
          state.isTag = true;
        }
        else
        {
          state.isTag = false;
          state.light = !state.light;
          printf("%d",state.light);
        }
    }  


    
    
  }


  // external transition
  void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

  }
  // confluence transition
  void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
    internal_transition();
    //external_transition(TIME(), std::move(mbs));
    printf("here");
  }

  // output function
  typename make_message_bags<output_ports>::type output() const {
    typename make_message_bags<output_ports>::type bags;
    //bool out;              
    double out=0;
    printf("hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
    //out = (state.newTag ? 1: 0);
    if(state.newTag == true)
    {

      if(state.isTag == true)
        out = 1;
      else
      {
        out = state.light;
        
      }
    }
    else 
      out = 0;
    if(state.newTag)
      out = state.id[0];
    else
      out = 0;
    
    printf("rfid: %f\n", out);
    //out = false;
    get_messages<typename defs::dataOut>(bags).push_back(out);                    
    return bags;
  }

  // time_advance function
  TIME time_advance() const {  
      return slowToggleTime;
  }

  friend std::ostringstream& operator<<(std::ostringstream& os, const typename Rfid<TIME>::state_type& i) {
    os << "Output: " << (i.newTag ? 1 : 0); 
     //os << "Output: " << false; 
    return os;
  }
};     
#endif // BLINKY_HPP