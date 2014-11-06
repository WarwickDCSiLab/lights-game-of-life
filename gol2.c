#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STRIP_SIZE	32
#define LBUFSIZE	(STRIP_SIZE * 3)
#define DIVIDER		0xf

typedef unsigned char bool;
#define true ((bool)1)
#define false ((bool)0)

void delay(unsigned int count);
/*void white();
void byte();
void byte2();
void clear();*/

//void * memcpy(void * destination, void const * source, unsigned int num);
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
	unsigned int to_i = (i >= BOARD_I_MAX - 1) ? BOARD_I_MAX - 1 : i + 1;

	unsigned int from_j = (j == 0) ? 0 : j - 1;
        unsigned int to_j = (j >= BOARD_J_MAX - 1) ? BOARD_J_MAX - 1 : j + 1;

	for (unsigned int ii = from_i; ii <= to_i; ++ii)
	{
		for (unsigned int jj = from_j; jj <= to_j; ++jj)
		{
			if (ii != i || jj != j)
			{
				//printf("(%u, %u)\n", ii, jj);

				count += board[ii][jj];
			}
		}
	}

	if (count != 0)
	{
		//printf("i %u-%u j %u-%u\n", from_i, to_i, from_j, to_j);
		//printf("neighbour(%u, %u) = %u\n", i, j, count);

		//*((char *)0) = 0;
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
		colour.i |= ((board[i][j] ? 1 : 0) << j);
	}

	//printf("%u %u %u %u\n", i, colour.triad.red, colour.triad.green, colour.triad.blue);

	return colour.triad;
}

void gol_to_strip(void)
{
	for (unsigned int i = 0; i != BOARD_I_MAX; ++i)
	{
		strip[i] = gol_col_to_colour(i);
	}
}


/*bool check_pattern(unsigned int i, triad_t left, triad_t middle, triad_t right)
{
	return triad_equal(&strip[i-1], &left) &&
		triad_equal(&strip[i], &middle) &&
		triad_equal(&strip[i+1], &right);
}*/

void gol_print(void)
{
	system("clear");

	for (unsigned int i = 0; i != BOARD_I_MAX; ++i)
	{
		for (unsigned int j = 0; j != BOARD_J_MAX; ++j)
		{
			printf("%c", board[i][j] ? 'x' : ' ');
		}
		printf("\n");
	}
}

int main(void)
{
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
		gol_iteration();
		gol_to_strip();

		gol_print();

		sleep(1);
	}

	return 0;
}

/*void * memcpy(void * restrict destination, void const * restrict source, unsigned int count)
{
	char * restrict dst8 = (char * restrict)destination;
        char const * restrict src8 = (char const * restrict)source;

        while (count--)
	{
            *dst8++ = *src8++;
        }

        return destination;
}*/

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

