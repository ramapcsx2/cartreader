This Arduino sketch can read and write Nintendo Power SF Memory cartridges without an external clock source like the Adafruit Clock Generator. It also uses the Arduino Serial Monitor instead of the OLED display and buttons.      

Copy the rom and map file to the root directory of your SD card and make sure that the filenames are below 8 characters in length.   

Configure the switches like so and use the USB port that is connected directly to the Arduino Mega:   

![image](https://dl.dropboxusercontent.com/s/yzewnnx5mb2ajk0/flash_firmware.jpg?dl=1)  

In the Arduino IDE select the correct Serial Port under Tools then open the Serial Monitor, make sure the baud is set to "9600" and in the box in front of the baudrate there should be "No line ending" selected.   

First send "8" via the text box and "Send" button at the top of the Serial Monitor windows. This will switch the cart to "hirom all" mode. It should give you the following feedback somewhere in the text box. Check that it did succeed:   
```   
(8)HIROM ALL 0x04    
Success    
2A 4 2A 2A FE 61 A5 0
```  
In case it failed close the Serial Monitor and switch the Arduino Mega off and on again and start from the beginning.      

Next send "q" to unlock the write protection:   
```   
(Q)Unlock WP   
Success.  
2A 4 2A 2A FE 61 A5 0    
```    

Before you write anything you should always do a backup of your rom and mapping.    
So next send "h" and then send a filename like "mapping.map" to save your carts original mapping.    
```   
Reading mapping into file mapping.map
Done.
```    

Now save the flash by sending "d" and again choosing a filename like "flash.bin", it should now beginn to print dots to indicate progress.    
```  
Reading flash into file flash.bin   
................................................................
```  

Finally you can split the flash.bin file into the individual roms and the menu using the NP Split program:    
https://github.com/sanni/cartreader/tree/master/extras/npsplit    

To flash something new you need to provide a mapping.map and a flash.bin file either from your backup, from another NP cart or you can create those files with the SF Memory Binary Maker: https://github.com/moldov/SF-Memory-Binary-Maker     
In the second case rename SHVC-MENU.bin you got from NP Split into menu.sfc and copy it next to the .exe. You can also use a Hex Editor to cut-out the first 512KB of your original flash dump and rename it to menu.sfc instead.     
In SF Memory Binary Maker you got two options: either create a standalone map file from a single rom up to 4MB or you can add multiple roms and create a combined .bin and .map file. The sum of all the roms together can't exceed 3.5MB however since you still need 512KB for the menu. Roms need to be unheadered and have a valid checksum.   

In any case you should end up with a 512B map and 4MB bin file, rename both so their filename is very short and without any special characters and copy them to the root of the SD card. Switch the NP cart into HIROM ALL, unlock WP, erase the mapping, erase the flash and then write the mapping and write the flash.   

![](https://dl.dropboxusercontent.com/s/7ptv5hdf4iwi0lb/npwriter10.jpg?dl=1)    
