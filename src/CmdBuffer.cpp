/* Copyright 2016 Pascal Vizeli <pvizeli@syshack.ch>
 * BSD License
 *
 * https://github.com/pvizeli/CmdParser
 */

#include "CmdBuffer.hpp"

CmdBufferObject::CmdBufferObject() : m_endChar(CMDBUFFER_CHAR_LF)
{
    this->clear();
}

bool CmdBufferObject::readFromSerial(HardwareSerial *serial, uint32_t timeOut)
{
    uint32_t isTimeOut;
    uint32_t startTime;
    bool     over;
    bool     stopRead;
    size_t   readPtr;
    uint8_t  readChar;
    uint8_t *buffer = this->getBuffer();

    // UART initialize?
    if (serial == NULL) {
        return false;
    }

    ////
    // init buffers
    this->clear();

    // init Counter
    readPtr ^= readPtr;

    ////
    // Calc Timeout
    if (timeOut != 0) {
        startTime = millis();
        isTimeOut = startTime + timeOut;

        // overloaded
        if (isTimeOut < startTime) {
            over = true;
        } else {
            over = false;
        }
    }

    ////
    // process serial reading
    stopRead = false;
    do {

        // if data in serial input buffer
        while (serial->available()) {

            // is buffer full?
            if (readPtr >= this->getBufferSize()) {
                return false;
            }

            // read into buffer
            readChar = serial->read();

            // is that the end of command
            if (readChar == m_endChar) {
                return true;
            }

            // is a printable character
            if (readChar > CMDBUFFER_CHAR_PRINTABLE) {
                buffer[readPtr++] = readChar;
            }
        }

        // Timeout is active?
        if (timeOut != 0) {
            // calc diff timeout
            if (over) {
                if (startTime > millis()) {
                    over = false;
                }
            }

            // timeout is receive
            if ((isTimeOut > millis() && !over) || over) {
                stopRead = true;
            }
        }

    } while (!stopRead); // timeout

    return false;
}
