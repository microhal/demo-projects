/**
 * @file
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief      board support package for nucleo-f411re board
 *
 * @authors    Pawel Okas
 * created on: 18-11-2016
 * last modification: <DD-MM-YYYY>
 *
 * @copyright Copyright (c) 2016, Paweł Okas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 	   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 * 	      documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bsp.h"
#include "microhal.h"
#include "ports/stm32f3xx/IOManager_stm32f3xx.h"

using namespace microhal;

CAN can1(registers::can1);

namespace bsp {
bool init() {
    bsp::debugPort.clear();

    bsp::debugPort.setDataBits(microhal::SerialPort::Data8);
    bsp::debugPort.setStopBits(microhal::SerialPort::OneStop);
    bsp::debugPort.setParity(microhal::SerialPort::NoParity);
    bsp::debugPort.open(microhal::SerialPort::ReadWrite);
    bsp::debugPort.setBaudRate(microhal::SerialPort::Baud115200);
    return true;
}

void deinit() {}

}  // namespace bsp

extern "C" int main(int, char**);

static void run_main(void*) {
    char* params[5];
    params[0] = "CAN";
    main(1, params);
    while (1)
        ;
}

void hardwareConfig(void) {
    stm32f3xx::Core::fpu_enable();
    stm32f3xx::ClockManager::PLL::clockSource(stm32f3xx::ClockManager::PLL::ClockSource::HSIDiv2);
    stm32f3xx::ClockManager::PLL::frequency(72000000);
    stm32f3xx::ClockManager::SYSCLK::source(stm32f3xx::ClockManager::SYSCLK::Source::PLL);

    stm32f3xx::IOManager::routeSerial<2, Txd, stm32f3xx::IOPin::PortA, 2>();
    stm32f3xx::IOManager::routeSerial<2, Rxd, stm32f3xx::IOPin::PortA, 3>();

    // stm32f3xx::IOManager::routeCAN<RX, stm32f3xx::IOPin::PortA, 11>();
    // stm32f3xx::IOManager::routeCAN<TX, stm32f3xx::IOPin::PortA, 12>();
    stm32f3xx::IOManager::routeCAN<RX, stm32f3xx::IOPin::PortB, 8>();
    stm32f3xx::IOManager::routeCAN<TX, stm32f3xx::IOPin::PortB, 9>();

    TaskHandle_t xHandle = NULL;

    xTaskCreate(run_main, "main", 500, NULL, tskIDLE_PRIORITY, &xHandle);

    vTaskStartScheduler();
}