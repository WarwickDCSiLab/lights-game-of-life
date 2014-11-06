//Raycon light strip  Matthew Bradbury

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

#define	PIOA_IDENTIFIER		0x00000004	// Bit 2 of register - corresponds to peripheral identifier no. 2
										// i.e. the peripheral identifier for parallel i/o interface A

#define DELAY		0x00500000			//
#define STEP_DELAY	0x00010000			//
#define OUTPUTS		CK1|SD1

#define STRIP_SIZE	32
#define LBUFSIZE	(STRIP_SIZE * 3)
#define DIVIDER		0xf

typedef unsigned char bool;
#define true ((bool)1)
#define false ((bool)0)

char lbuf[ LBUFSIZE ];

void delay(unsigned int count);
/*void white();
void byte();
void byte2();
void clear();*/

void * memcpy(void * restrict destination, void const * restrict source, unsigned int num);
//void * memset(void * destination, unsigned char value, unsigned int num);
void lupdate(void);
void lcopy(void);

typedef struct triad {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} triad_t;

/*bool triad_equal(triad_t const * left, triad_t const * right)
{
	return left->red == right->red &&
		left->green == right->green &&
		left->blue == right->blue;
}*/

triad_t strip[ STRIP_SIZE ];

#define RED ((triad_t){ 128, 0, 0 })
#define BLUE ((triad_t){ 0, 0, 128 })
#define GREEN ((triad_t){ 0, 128, 0 })
#define MAGENTA ((traid_t){ 128, 0, 128 })
#define YELLOW ((triad_t){ 128, 128, 0 })
#define CYAN ((triad_t){ 0, 128, 128 })


#define BOARD_I_MAX STRIP_SIZE
#define BOARD_J_MAX 24

bool board[BOARD_I_MAX][BOARD_J_MAX];
bool buffer[BOARD_I_MAX][BOARD_J_MAX];

unsigned int gol_neighbour_count(unsigned int i, unsigned int j)
{
	unsigned int count = 0;

	unsigned int from_i = (i == 0) ? 0 : i - 1;
	unsigned int to_i = (i >= BOARD_I_MAX - 1) ? BOARD_I_MAX - 1: i + 1;

	unsigned int from_j = (j == 0) ? 0 : j - 1;
		unsigned int to_j = (j >= BOARD_J_MAX - 1) ? BOARD_J_MAX - 1 : j + 1;

	for (unsigned int ii = from_i; ii <= to_i; ++ii)
	{
		for (unsigned int jj = from_j; jj <= to_j; ++jj)
		{
			if (ii != i || jj != j)
			{
				count += board[ii][jj];
			}
		}
	}

	return count;
}

void gol_iteration(void)
{
	for (unsigned int i = 0; i != BOARD_I_MAX; ++i)
	{
		for (unsigned int j = 0; j != BOARD_J_MAX; ++j)
		{
			unsigned int count = gol_neighbour_count(i, j);

			if (board[i][j])
			{
				if (count < 2)
				{
					buffer[i][j] = false;
				}
				else if (count == 2 || count == 3)
				{
					buffer[i][j] = true;
				}
				else /*if (count > 3)*/
				{
					buffer[i][j] = false;
				}
			}
			else
			{
				if (count == 3)
				{
					buffer[i][j] = true;
				}
				else
				{
					buffer[i][j] = false;
				}
			}
		}
	}

	memcpy(board, buffer, sizeof(bool) * BOARD_I_MAX * BOARD_J_MAX);
}

typedef union triadi
{
	triad_t triad;
	unsigned int i;
} triadi_t;


triad_t gol_col_to_colour(unsigned int i)
{
	triadi_t colour;
	colour.i = 0;

	for (unsigned int j = 0; j != BOARD_J_MAX; ++j)
	{
		colour.i |= (((unsigned int)board[i][j]) << j);
	}

	return colour.triad;
}

void gol_to_strip(void)
{
	for (unsigned int i = 0; i != BOARD_I_MAX; ++i)
	{
		strip[i] = gol_col_to_colour(i);
	}
}


int main(void)
{
	*PIO_PER = OUTPUTS; // Enable control of I/O pins from PIO Controller
	*PIO_OER = OUTPUTS; // Enable output drivers for relevant pins

	*PMC_PCER = PIOA_IDENTIFIER;

	*PIO_CODR = CK1;


	for (unsigned int i = 0; i != STRIP_SIZE; i++)
		strip[ i ] = (triad_t){ 0, 0, 0 };

	for (unsigned int i = 0; i != BOARD_I_MAX; ++i)
		for (unsigned int j = 0; j != BOARD_J_MAX; ++j)
			board[i][j] = false;

	for (unsigned int j = 0; j != BOARD_J_MAX; ++j)
	{
		board[16][j] = true;
		board[10][j] = true;
	}

	for (unsigned int loopy = 0; ; loopy++)
	{
		gol_to_strip();

		lcopy();
		lupdate();

		delay(16 * 1000 * 50); // delay 16,000 clock cycles - long pulse - ~0.5ms - will go down to 3000

		gol_iteration();
	}

	return 0;
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

/*void * memset(void * destination, unsigned char value, unsigned int num)
{
	unsigned char * p = (unsigned char *)destination;
	while(n--)
		*p++ = (unsigned char)value;
	return destination;
}*/

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
