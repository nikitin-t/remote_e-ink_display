#!/usr/bin/python3

import pygatt
from binascii import hexlify

import time

from PIL import Image
import crcmod.predefined

TIMEOUT_READ = 5.0

CMD_START   = 0x80
CMD_END     = 0x81
CMD_ERROR   = 0xFF

STATUS_READY    = 0
STATUS_CRC      = 1
STATUS_ERROR    = -1

TRANSFER_DONE           = 0
TRANSFER_ERROR_TIMEOUT  = -1
TRANSFER_ERROR_CRC      = -2

class BLE():
    def __init__(self):
        self.in_count = bytearray([])
        self.in_crc = bytearray([])
        self.status = STATUS_READY
        
        self.characteristic = "a6e65511-80aa-11ea-ab12-0800200c9a66"
        self.adapter = pygatt.GATTToolBackend()
        self.adapter.start(reset_on_start=False)
        print("Scan...")
        scan_list = self.adapter.scan()
        device_address = ""
        for dev in scan_list:
            if dev["name"] == "Remote E-ink display":
                device_address = dev["address"]
                break
        
        if device_address == "":
            print("Remote E-ink display not found")
            self.status = STATUS_ERROR
        else:
            print("Connect to Remote E-ink display, address: %s" % device_address)
            self.device = self.adapter.connect(device_address)
            time.sleep(1)
            self.device.subscribe(self.characteristic, callback=self.handle_data, indication=True)
            time.sleep(1)
            
    def __del__(self):
        self.adapter.stop()
    
    def write(self, data):
        self.device.char_write(self.characteristic, data)
            
    def handle_data(self, handle, value):
        """
        handle -- integer, characteristic read handle the data was received on
        value -- bytearray, the data returned in the notification
        """
        if self.status == STATUS_READY:
            self.in_count = value
        else:
            self.in_crc += value
        
    def img_convert(self):
        
        print("IMG convert")
        img = Image.open("test_photo.jpg")
        img = img.resize((172, 72)) # Scale down # optionally add ANTIALIAS
        # CONVERT IMAGE TO BLACK AND WHITE
        imgx = img.size[0]
        imgy = img.size[1]
        newImg = Image.new("1", (imgx, imgy))
        for (pixIdx, pix) in enumerate(list(img.getdata())):
            x = int(pixIdx % 172)
            y = int(pixIdx / 172)
            if (pix[0] + pix[1] + pix[2]) / 3 > 100:
                newImg.putpixel((x,y), 0xffffff)
            else:
                newImg.putpixel((x,y), 0x000000)
        
        # COMPRESS WITH THE COMPRESSION SCHEME OF THE EINK DISPLAY
        self.img_bytes = []
        width = newImg.size[0]
        height= newImg.size[1]
        
        for i in range(0, width):
            for j in range(height, 0, -8):
                self.img_bytes.append(\
                    (int(newImg.getpixel((i,j-1)) > 0) << 0) |\
                    (int(newImg.getpixel((i,j-2)) > 0) << 1) |\
                    (int(newImg.getpixel((i,j-3)) > 0) << 2) |\
                    (int(newImg.getpixel((i,j-4)) > 0) << 3) |\
                    (int(newImg.getpixel((i,j-5)) > 0) << 4) |\
                    (int(newImg.getpixel((i,j-6)) > 0) << 5) |\
                    (int(newImg.getpixel((i,j-7)) > 0) << 6) |\
                    (int(newImg.getpixel((i,j-8)) > 0) << 7))
                self.img_bytes[-1] = 0xff - self.img_bytes[-1] # the epd flips the colors. so flip bits
    
    def img_transfer(self):
        
        print("IMG transfer")
        
        print("Start transfer... ")
        self.write(bytearray([CMD_START]))
        for i in range(172):
            timeout = time.time() + TIMEOUT_READ
            while (self.in_count != bytes([i])):
                if timeout > time.time():
                    pass
                else:
                    print("Timeout ERROR: Remote E-ink display not answer")
                    return TRANSFER_ERROR_TIMEOUT
            self.write(self.img_bytes[9*i:9*i+9])
            p = (i/172)*100
            print("Transfer %d%%" % p, end='\r')
        
        self.status = STATUS_CRC
        print("Transfer completed")
        print("Start CRC.. ")
        crc_img = self.crc_calc(bytearray(self.img_bytes))
        timeout = time.time() + TIMEOUT_READ
        while int.from_bytes(self.in_crc, byteorder='little', signed=False) != crc_img:
            if timeout > time.time():
                pass
            else:
                print("CRC ERROR: CRC: %s, Received CRC: %s" % (hexlify(bytearray(crc_img)), hexlify(self.in_crc)))
                self.write(bytearray([CMD_ERROR]))
                return TRANSFER_ERROR_CRC
        print("DONE")
        self.write(bytearray([CMD_END]))
        return TRANSFER_DONE
        
    
    def crc_calc(self, data):
        crc32_func = crcmod.predefined.mkCrcFun('crc-32-mpeg')
        return crc32_func(data)

if __name__ == '__main__':
    
    print("Start transfer IMG to Remote E-ink display")
    b = BLE()
    if b.status != STATUS_ERROR:
        b.img_convert()
        b.img_transfer()
