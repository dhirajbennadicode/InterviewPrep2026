
/*
void reg_set_bits(volatile uint32_t *reg, uint32_t mask); — 
set the bits in mask to 1 without disturbing other bits.

void reg_clear_bits(volatile uint32_t *reg, uint32_t mask); — 
clear the bits in mask to 0 without disturbing other bits.

void reg_toggle_bits(volatile uint32_t *reg, uint32_t mask); — flip the bits in mask.

bool reg_test_bits(volatile uint32_t *reg, uint32_t mask); — 
return whether all bits in mask are currently set.

Define bit positions with named constants/macros rather than magic numbers 
(e.g., #define UART_CTRL_TX_EN (1u << 3)), and demonstrate combining several 
named bits into one operation.

Explain precisely where volatile needs to go for a pointer to a 
hardware register (volatile uint32_t *reg vs uint32_t *volatile reg vs 
volatile uint32_t *volatile reg) 
and which mistake is most common and why it silently breaks optimization assumptions.

Discuss why a plain set/clear/toggle (read-modify-write) sequence 
is not atomic, and construct a concrete scenario where an 
ISR firing between the read and the write of an RMW sequence 
corrupts a bit that the main loop was trying to set — then say what 
you'd do to prevent it (disabling interrupts around the RMW, or a 
hardware bit-band/atomic-set-clear register if the MCU has one).

Discuss "write-1-to-clear" (W1C) status/interrupt-flag registers, 
common on real peripherals: writing 0 does nothing, 
writing 1 clears that specific flag bit, 
and a naive read-modify-write on this kind of 
register can accidentally clear flags it never meant to touch. 
Explain how you'd write a correct "clear this one interrupt flag" function 
for a W1C register, and why it looks different from reg_clear_bits above.

Briefly discuss C bitfields (struct { uint32_t en : 1; uint32_t mode : 2; ... };
 mapped onto a register) as an alternative to masks/shifts, 
 and why most embedded codebases avoid them for 
 hardware register access (implementation-defined bit order 
 and packing, no portability guarantee across compilers/architectures)
  even though they're convenient to read.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 0000 1010 1010 0101 0101
uint32_t register_value = 0xAA55;


void reg_set_bits(volatile uint32_t *reg, uint32_t mask)
{
	*reg = *reg | mask;
}

void reg_clear_bits(volatile uint32_t *reg, uint32_t mask)
{
	*reg = *reg & ~(mask);
}

void reg_toggle_bits(volatile uint32_t *reg, uint32_t mask)
{
	*reg = *reg ^ mask;
}

uint8_t leftMostBit(uint32_t value)
{
	if(!value) return 0;
	int count = 0;
	while(value > 0)
	{
		value = value >> 1;
		count++;
	}

	return count - 1;
}

bool reg_test_bits(volatile uint32_t *reg, uint32_t mask)
{
	// Right Most Set bit
	return (*reg & mask) == mask;
}

uint32_t get_reg_value()
{
	return register_value;
}

int main()
{
	register_value = 0x0F;
	//printf("%u\n", reg_test_bits(&register_value, 0x0F));
	//printf("%u\n", reg_test_bits(&register_value, 0xFF));
	printf("%u\n", reg_test_bits(&register_value, 0xF0));
	//printf("%u\n", reg_test_bits(&register_value, 0x05));
	//printf("%u\n", reg_test_bits(&register_value, 0x00));
	return 0;

}

/*
volatile uint32_t *volatile reg - is the correct notation, as the value held by the register 
is volatile that means the compiler should not optimize it nor assume its value, so it should be
loaded freshly every load, the same for register address

Set / Clear / Toggle are not atomic instructions as the step between the read and write can be interrupted
and new value can be read/written to the register value

Explain how you'd write a correct "clear this one interrupt flag" function 
for a W1C register, and why it looks different from reg_clear_bits above.

Check the value if its 1 before writing 
*/