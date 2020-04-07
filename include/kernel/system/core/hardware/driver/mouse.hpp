
#ifndef _MOUSE_HPP_
#define _MOUSE_HPP_

#include <common/types.hpp>
#include <kernel/system/core/hardware/port.hpp>
#include <kernel/system/core/hardware/driver/Driver.hpp>
#include <kernel/system/core/hardware/interrupts.hpp>

        class MouseEventHandler
        {
        public:
            MouseEventHandler(){}

            virtual void OnActivate()=0;
            virtual void OnMouseDown(uint8_t button)=0;
            virtual void OnMouseUp(uint8_t button)=0;
            virtual void OnMouseMove(int x, int y)=0;
        };


        class MouseDriver : public InterruptHandler, public Driver
        {
            Port_8Bit dataport;
            Port_8Bit commandport;
            uint8_t buffer[3];
            uint8_t offset;
            uint8_t buttons;

            MouseEventHandler* handler;
        public:
            MouseDriver(InterruptManager* manager, MouseEventHandler* handler):
            InterruptHandler(manager, 0x2C),
            dataport(0x60),
            commandport(0x64)
            {
                this->handler = handler;
            }
            ~MouseDriver(){}
            virtual uint32_t HandleInterrupt(uint32_t esp)final{
                uint8_t status = commandport.read();
                if (!(status & 0x20))
                    return esp;

                buffer[offset] = dataport.read();

                if(handler == 0)
                    return esp;

                offset = (offset + 1) % 3;

                if(offset == 0)
                {
                    if(buffer[1] != 0 || buffer[2] != 0)
                    {
                        handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
                    }

                    for(uint8_t i = 0; i < 3; i++)
                    {
                        if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
                        {
                            if(buttons & (0x1<<i))
                                handler->OnMouseUp(i+1);
                            else
                                handler->OnMouseDown(i+1);
                        }
                    }
                    buttons = buffer[0];
                }

                return esp;
            }
            virtual void Activate(){
                offset = 0;
                buttons = 0;

                if(handler != 0)
                    handler->OnActivate();

                commandport.Write(0xA8);
                commandport.Write(0x20); // command 0x60 = read controller command byte
                uint8_t status = dataport.read() | 2;
                commandport.Write(0x60); // command 0x60 = set controller command byte
                dataport.Write(status);

                commandport.Write(0xD4);
                dataport.Write(0xF4);
                dataport.read();
            }
            virtual void Reset()final {}
            virtual void Deactivate() final{}
        };
        class MouseToConsole : public MouseEventHandler
        {
            int8_t x, y;
        public:

            MouseToConsole()
            {
                uint16_t* VideoMemory = (uint16_t*)0xb8000;
                x = 40;
                y = 12;
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                    | (VideoMemory[80*y+x] & 0xF000) >> 4
                                    | (VideoMemory[80*y+x] & 0x00FF);
            }

            virtual void OnMouseMove(int xoffset, int yoffset)final
            {
                static int count=0;
                count++;
                if(count<2)
                  return;

                count=0;
                static uint16_t* VideoMemory = (uint16_t*)0xb8000;
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                    | (VideoMemory[80*y+x] & 0xF000) >> 4
                                    | (VideoMemory[80*y+x] & 0x00FF);

                x += xoffset;
                if(x >= 80) x = 79;
                if(x < 0) x = 0;
                y += yoffset;
                if(y >= 25) y = 24;
                if(y < 0) y = 0;

                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                    | (VideoMemory[80*y+x] & 0xF000) >> 4
                                    | (VideoMemory[80*y+x] & 0x00FF);
            }
            virtual void OnActivate() final{}
            virtual void OnMouseDown(uint8_t button)final {}
            virtual void OnMouseUp(uint8_t button)final{}
        };
#endif
