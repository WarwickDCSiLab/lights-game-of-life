//Raycon light strip  sgv May2013

typedef volatile unsigned int ioreg;

#define	PIO_PER		(ioreg *)	0xfffff400	// PIO Enable Register
#define	PIO_OER		(ioreg *)	0xfffff410	// Output Enable Register
#define	PIO_SODR	(ioreg *)	0xfffff430	// Set Output Data Register
#define PIO_IFER	(ioreg *)	0xfffff420	// Input Glitch Filter Enable Register
#define	PIO_CODR	(ioreg *)	0xfffff434	// Clear Output Data Register
#define	PIO_PPUDR 	(ioreg *)	0xfffff460	// Pullup Disable Register
#define	PIO_PDSR	(ioreg *)	0xfffff43c	// Pin Data Status Register
#define	PMC_PCER	(ioreg *)	0xfffffc10	// Peripheral Clock Enable Register

#define CK1			0x00000002
#define SD1			0x00000001

#define	PIOA_IDENTIFIER		0x00000004// Bit 2 of register - corresponds to peripheral identifier no. 2
					// i.e. the peripheral identifier for parallel i/o interface A

#define DELAY		0x00500000				//
#define STEP_DELAY	0x00010000				//
#define OUTPUTS		CK1|SD1

#define STRIP_SIZE	32
#define LBUFSIZE	(STRIP_SIZE * 3)
#define DIVIDER		0xf

typedef unsigned char bool;
#define true ((bool)1)
#define false ((bool)0)

char lbuf[ LBUFSIZE ];

void delay(unsigned int count);
void white();
void byte();
void byte2();
void clear();

void * memcpy(void * restrict destination, void const * restrict source, unsigned int num);
void lupdate(void);
void lcopy(void);

typedef struct triad {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} triad_t;

bool triad_equal(triad_t const * left, triad_t const * right)
{
	return left->red == right->red &&
		left->green == right->green &&
		left->blue == right->blue;
}

triad_t strip[ STRIP_SIZE ];
triad_t buffer[ STRIP_SIZE ];

#define RED ((triad_t){ 128, 0, 0 })
#define BLUE ((triad_t){ 0, 0, 128 })
#define GREEN ((triad_t){ 0, 128, 0 })
#define MAGENTA ((triad_t){ 128, 0, 128 })
#define YELLOW ((triad_t){ 128, 128, 0 })
#define CYAN ((triad_t){ 0, 128, 128 })

bool check_pattern(unsigned int i, triad_t left, triad_t middle, triad_t right)
{
	return triad_equal(&strip[i-1], &left) &&
		triad_equal(&strip[i], &middle) &&
		triad_equal(&strip[i+1], &right);
}


int main(void)
{
	*PIO_PER = OUTPUTS; // Enable control of I/O pins from PIO Controller
	*PIO_OER = OUTPUTS; // Enable output drivers for relevant pins

	*PMC_PCER = PIOA_IDENTIFIER;

	*PIO_CODR = CK1;


	for (unsigned int i = 0; i != STRIP_SIZE; i++) 
		strip[ i ] = RED;

	//for (i = 0; i < LBUFSIZE; i++)
	//	lbuf[ i ] = 0;


	delay(16 * 1000);

 	for (unsigned int loopy = 0; ; loopy++)
	{
		// Copy buffer to be worked on 
		memcpy(buffer, strip, sizeof(strip));

		// Work out the next value for each of the LEDs
		for (unsigned int current = 1; current != (STRIP_SIZE - 1); ++current)
		{
			if (check_pattern(current, RED, RED, RED))
			{
				buffer[current] = GREEN;
				break;
			}
			else if (check_pattern(current, RED, GREEN, RED))
			{
				buffer[current] = BLUE;
			}
			else if (check_pattern(current, BLUE, RED, GREEN))
			{
				buffer[current] = CYAN;
			}
			else if (check_pattern(current, BLUE, CYAN, BLUE))
			{
				buffer[current-1] = MAGENTA;
			}
			else if (check_pattern(current, CYAN, MAGENTA, CYAN))
			{
				buffer[current-1] = GREEN;
			}
		}


		// Copy the buffer back to be rendered
		memcpy(strip, buffer, sizeof(buffer));

		lcopy();
		lupdate();

		delay(16 * 1000 * 100); // delay 16,000 clock cycles - long pulse - ~0.5ms - will go down to 3000
	}

}

void * memcpy(void * restrict destination, void const * restrict source, unsigned int count)
{
	char * restrict dst8 = (char * restrict)destination;
        char const * restrict src8 = (char const * restrict)source;

        while (count--)
	{
            *dst8++ = *src8++;
        }

        return destination;
}

void delay(unsigned int count)
{
	volatile unsigned int i;
	for (i = count; i != 0; i--);
}

void lupdate(void)
{
	for (unsigned int i = 0; i != LBUFSIZE; i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			if (lbuf[ i ] & (1 << j))
			{
				*PIO_CODR=SD1;
				*PIO_CODR=CK1;
				*PIO_SODR=SD1;
				*PIO_SODR=CK1;
			}
			else
			{
				*PIO_SODR=SD1;
				*PIO_CODR=CK1;
				*PIO_SODR=SD1;
				*PIO_SODR=CK1;
	 		}
		}
	}
}

void lcopy(void)
{
	for (unsigned int i = 0; i != STRIP_SIZE; i++)
	{
		lbuf[ i * 3 ] = strip[ i ].red;
		lbuf[ i * 3 + 1 ] = strip[ i ].green;
		lbuf[ i * 3 + 2 ] = strip[ i ].blue;
	}
}
