# PEPPER-Data-Generator
A Raspberry Pi Pico-based emulator to generate instrument samples and output them via SPI

In `main.c` SPI0 is set up as a master.

```
Pin   Signal
GP19  SPI0_TX
GP18  SPI0_SCK
GP17  SPI0_CSn -- not used
GP16  SPI0_RX
GP20  CSn -- used instead of GP17
```

The default SPI0_CSn chip select signal cannot be used because we want to transmit 4 bytes at a go.
This matches what the actual PEPPER FPGA does.

The format of the 4 byte "packet" is peculiar to the approach to acquiring and passing along the 
samples from the 24-bit ADC. The first 3 bytes of the 4-byte packet correspond to the 24 bits from one
ADC sample. The final byte is set to `0x00`.

