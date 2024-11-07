/*==============================================================================
 * Copryright Laboratory10 ALL RIGHTS RESERVED
 *
 * Any use, modification, or redistribution is prohibited without prior written
 * permission. Contact laboratory0010@gmail.com for details.  
 *============================================================================*/

/**
 * \file
 * \brief This file contains the sole source of code running on the Base 
 * Station board. The purpose of this hardware is to be listening to messages
 * being sent from AURO and the Launch Station relay them via Serial to the 
 * ground computer it is connected to for display/logging.
 */

#include <RH_RF69.h>
#include <SPI.h>
#include <stdio.h>

#define RADIO_FREQ 915.0 //Center frequency for RF_69 Radio
// Feather M0 w/Radio
#define RADIO_CS    8 //Chip select pin for the radio
#define RADIO_INT   3 //Radio interrupt pin
#define RADIO_RST   4 //Radio reset pin
#define LED        13 //Red LED pin
#define MAX_PACKET_LENGTH 60 //Max message length for serial and radio
#define RSSI_REPORT_FREQ 200 //How often RSSI should be reported (message count)

RH_RF69 radio(RADIO_CS, RADIO_INT); //Radio instance

uint8_t outbound_packet[MAX_PACKET_LENGTH]; //Packet being sent to AURO
uint8_t inbound_packet[MAX_PACKET_LENGTH]; //Packet received from AURO
uint8_t packet_size = MAX_PACKET_LENGTH; //How many bytes radio copied to buffer
uint16_t last_rssi_update = 0; //Messages since last RSSI update (message count)
char bst_msg[60]; //Message buffer used to stage Base Station messages

/**
 * \brief This function is called by main() (which is the arduino entrypoint).
 * It initializes the Base Station.
 */
void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);

  //Force radio reset
  pinMode(RADIO_RST, OUTPUT);
  digitalWrite(RADIO_RST, LOW);
  digitalWrite(RADIO_RST, HIGH);
  delay(10);
  digitalWrite(RADIO_RST, LOW);
  delay(10);

  radio.init();
  radio.setFrequency(RADIO_FREQ);
  radio.setTxPower(20, true);
  uint8_t key[] = {0xd6, 0xa9, 0x1b, 0x42, 0x63, 0x21, 0x52, 0x6d, 
                   0x27, 0xbc, 0x04, 0xff, 0xc4, 0xa9, 0x8c, 0xf0};
  radio.setEncryptionKey(key);
  while (!Serial);
}

/**
 * \brief This function is called by main() (which is the arduino entrypoint)
 * after setup is run. It checks continuously for new messages received by the 
 * radio (which would be inbound from AURO) and serial (which would be outbound
 * from the Ground System).
 */
void loop() {
  check_serial();
  check_radio();
}

/**
 * \brief This function is called continuously by loop(). It checks the serial
 * interface for messages and prepares them for transmission to AURO when a 
 * groundspeak session is active. When an outbound message is ready, this 
 * function waits for AURO to send the special packet that indicates the start 
 * of a groundspeak session. During this time, AURO will not transmit and give 
 * the Base Station and Launch Station an opportunity to transmit.
 */
void check_serial() {
  if (Serial.available()) {
    uint8_t message_position = 0;
    memset(outbound_packet, '\0', MAX_PACKET_LENGTH);
    while (message_position < MAX_PACKET_LENGTH) {
      //The message MUST be 60 bytes long, wait a bit if we haven't read 60 yet
      uint32_t start_time = millis();
      while (Serial.available() == false) {
        //Force timeout quit, just in case
        //See 'Base Station Timeout' test in ETE_test.py
        if ((millis() - start_time) > 100) {
          break;
        }
        delay(1);
      }
      if (Serial.available()) {
        outbound_packet[message_position] = Serial.read();
        message_position++;
      } else {
        break;
      }
    }
    //Start waiting for the next groundspeak session
    bool tx_ready = false;
    while (!tx_ready) {
      packet_size = MAX_PACKET_LENGTH;
      //Check for next radio message, maybe it will be groundspeak, maybe not
      if (radio.available() && radio.recv(inbound_packet, &packet_size)) {
        if (inbound_packet[0] == '-') {
          tx_ready = true;
        } else {
          //A message was received but it wasn't the groundspeak package
          //We can continue to wait, but this message also need to be sent along
          Serial.write(inbound_packet, packet_size);
        }
      }
    }
    //The while loop was exited, which means we are in a groundspeak session
    (void)radio.send(outbound_packet, MAX_PACKET_LENGTH);
    //Calling waitPacketSent() isn't needed here, just move along
  }
}

/**
 * \brief This function is called continuously by loop(). It checks the radio
 * for messages and prepares them for relay to the System. It also 
 * updates BST channels (CTL channels that refer to Base Station values).
 */
void check_radio() {
  //Look in radio buffer for a packet that is inbound to the Ground System
  if (radio.available())  {
    packet_size = MAX_PACKET_LENGTH;
    if (radio.recv(inbound_packet, &packet_size)) {
      if (inbound_packet[0] != '-') {
        //Send along serial to the Ground System
        Serial.write(inbound_packet, packet_size);
      }
    }
  }
}
