#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define DISPLAY_WIDTH 1540
#define DISPLAY_HEIGHT 1870
#define MAX_ROW 9
#define MAX_COL 8
#define GRID_OFFSET_HEIGHT 200
#define GRID_OFFSET_WIDTH 50
#define BALL_SZ 180
#define FPS 100

typedef struct {
	int row;
	int col;
	int sel;
	int ncolor;
	ALLEGRO_BITMAP *bcolor;
}Ball;

bool inicializar();
int newMaxScore();
void changegamestate(int gamestate);
void initmatrix(Ball matrix[MAX_ROW][MAX_COL]);
void initstate1();
int insideplay(int x, int y);
void initstate2(Ball matrix[MAX_ROW][MAX_COL]);
int insidebutton(int x, int y);
void initstate3();
void color(Ball matrix[MAX_ROW][MAX_COL], int row1, int col1);
int insidegrid(int x, int y);
Ball get_piece(int mouse_x, int mouse_y, Ball matrix[MAX_ROW][MAX_COL]);
int checkrelativeposition(Ball b1, Ball b2);
void swapballsanimation(Ball matrix[MAX_ROW][MAX_COL], Ball b1, Ball b2);
void swapballs(Ball matrix[MAX_ROW][MAX_COL], Ball b1, Ball b2);
void zero_aux_matrix(int m[MAX_ROW][MAX_COL]);
void printmatrix(int m[MAX_ROW][MAX_COL]);
void possiblemoves(Ball matrix[MAX_ROW][MAX_COL]);
void findsequence(Ball m[MAX_ROW][MAX_COL]);
void mark_row(int startrow, int startcol, int sequence);
void mark_col(int startrow, int startcol, int sequence);
void zero_matrix_sequence(Ball matrix[MAX_ROW][MAX_COL]);
int remainingSequence();
void updatematrix(Ball matrix[MAX_ROW][MAX_COL]);
void randmatrix(Ball matrix[MAX_ROW][MAX_COL]);
void movematrix(Ball matrix[MAX_ROW][MAX_COL]);
int insidebutton(int x, int y);
void restart_aux_ball(Ball sp[], int i);
void handlemouseclick(Ball matrix[MAX_ROW][MAX_COL], Ball sp[], Ball current);
void play_sound();
void createtimer();


ALLEGRO_DISPLAY 	*display = NULL;
ALLEGRO_BITMAP 		*fundo = NULL;
ALLEGRO_BITMAP 		*fundo2 = NULL;
ALLEGRO_BITMAP 		*bggameover = NULL;
ALLEGRO_BITMAP 		*score = NULL;
ALLEGRO_BITMAP 		*ballcolor[10];
ALLEGRO_BITMAP 		*fundomenu;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT_QUEUE *fila_timer   = NULL;
ALLEGRO_TIMER 		*timer = NULL;
ALLEGRO_FONT        *font = NULL;
ALLEGRO_FONT        *smallfont = NULL;
ALLEGRO_SAMPLE 		*som_ponto0=NULL;
ALLEGRO_SAMPLE 		*som_ponto1=NULL;
ALLEGRO_SAMPLE 		*som_ponto2=NULL;
ALLEGRO_SAMPLE 		*win_sound=NULL;
ALLEGRO_SAMPLE 		*bounce_sound=NULL;


int auxmatrix[MAX_ROW][MAX_COL];
Ball matrix[MAX_ROW][MAX_COL];

int player_score;
int player_moves;
int max_score;
int gamestate = 1;

int main(void)
{
	//srand(time(NULL));
	if (!inicializar())
	{al_flip_display();
		return -1;
	}

	Ball auxball[2] = {
		{.row = -1, .col = -1, .sel=0, .bcolor=NULL},
		{.row = -2, .col = -2, .sel=0, .bcolor=NULL}
	};
	
	changegamestate(gamestate);

	while (1)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
		{
			break;
		}

		else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
		{
			if (gamestate == 1)
			{
				//if(insideplay(event.mouse.x, event.mouse.y))
					gamestate = 2;
					changegamestate(gamestate);
			}
			else if(gamestate == 2)
			{
				if(insidegrid(event.mouse.x, event.mouse.y))
				{
					Ball current = get_piece(event.mouse.x, event.mouse.y, matrix);
					current.sel = 1;

					handlemouseclick(matrix, auxball, current);
				}
			}
			else if(gamestate == 3)
			{
				if(insidebutton(event.mouse.x, event.mouse.y))
				{
					gamestate = 1;
					changegamestate(gamestate);	
				}
			}
		}
	}
		
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);	

	return 0;
}

void changegamestate(int gamestate)
{
	if(gamestate == 1)
	{
		initstate1();
	} 
	
	if(gamestate == 2)
	{
		player_score = 0;
		player_moves = 10;
		max_score = 0;
		newMaxScore();

		initmatrix(matrix);
		initstate2(matrix);
		al_flip_display();
	}

	else if(gamestate == 3)
	{
		newMaxScore();
		al_clear_to_color(al_map_rgb(0,0,0));
		initstate2(matrix);
		initstate3();
		al_flip_display();

	}
}

void initmatrix(Ball matrix[MAX_ROW][MAX_COL])
{
	int i,j, colorir;
	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0; j < MAX_COL; j++)
		{
			matrix[i][j].row = i;
			matrix[i][j].col = j;
			matrix[i][j].sel = 0;
			matrix[i][j].ncolor = 0;
		}
	}

	do
	{
		zero_aux_matrix(auxmatrix);
		for(i = 0; i < MAX_ROW; i++)
		{
			for(j = 0; j < MAX_COL; j++)
			{
				if(matrix[i][j].ncolor == 0)
				{
					colorir = 1 + rand() % 5;
					matrix[i][j].ncolor = colorir;
					matrix[i][j].bcolor = ballcolor[colorir];
				}	
			}
		}
		findsequence(matrix);
		zero_matrix_sequence(matrix);

	}while(remainingSequence());
}


void initstate1()
{
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_bitmap(fundomenu, 0, 0, 0);
	al_flip_display();
}

int insideplay(int x, int y)
{
	if (x >= 617 && x <= 953 && y >= 1253 && y <= 1390)
		return 1;
	return 0;
}

void initstate2(Ball matrix[MAX_ROW][MAX_COL])
{
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_bitmap(fundo, 0, 0, 0);
	al_draw_bitmap(score, GRID_OFFSET_WIDTH, 0, 0);

	al_draw_textf(font, al_map_rgb(255, 255, 255), DISPLAY_WIDTH / 2, 50, ALLEGRO_ALIGN_CENTRE, "%d", player_moves);
	al_draw_textf(smallfont, al_map_rgb(255, 255, 255), 595, 50, ALLEGRO_ALIGN_CENTRE, "%d", player_score);
	al_draw_textf(smallfont, al_map_rgb(255, 255, 255), 943, 50, ALLEGRO_ALIGN_CENTRE, "%d", max_score);
	al_draw_text(font, al_map_rgb(255, 255, 255), 300, 40, ALLEGRO_ALIGN_CENTRE, "SCORE");
	al_draw_text(font, al_map_rgb(255, 255, 255), 1250, 40, ALLEGRO_ALIGN_CENTRE, "GOAL");

	int i = 0, j = 0;
	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0; j < MAX_COL; j++)
		{
			color(matrix, i, j);
		}
	}
}

void initstate3()
{
	al_draw_bitmap(bggameover, 0, 0, 0);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 300, ALLEGRO_ALIGN_LEFT, "YOUR SCORE IS..........%d", player_score);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 500, ALLEGRO_ALIGN_LEFT, "THE HIGHEST SCORE IS...%d", max_score);
	al_draw_text(font, al_map_rgb(255, 255, 255), 675, 1255, ALLEGRO_ALIGN_LEFT, "MENU");
	
	if(player_score>max_score)
	{
		al_play_sample(win_sound, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
		max_score = player_score;
		al_draw_text(font, al_map_rgb(255, 255, 255), 760, 800, ALLEGRO_ALIGN_CENTRE, "CONGRATULATIONS!");
		al_draw_textf(font, al_map_rgb(255, 255, 255), 760, 900, ALLEGRO_ALIGN_CENTRE, "THE HIGHEST SCORE IS NOW %d", max_score);
	}
	else
	{
		al_draw_text(font, al_map_rgb(255, 255, 255), 760, 800, ALLEGRO_ALIGN_CENTRE, "BETTER LUCK NEXT TIME");
	}
	return;
}

int insidebutton(int x, int y)
{
	if (x >= 617 && x <= 953 && y >= 1253 && y <= 1390)
		return 1;
	return 0;
}

void color(Ball matrix[MAX_ROW][MAX_COL], int row1, int col1)
{
	if(matrix[row1][col1].sel==0)

		al_draw_bitmap(matrix[row1][col1].bcolor, col1 * BALL_SZ + GRID_OFFSET_WIDTH, row1 * BALL_SZ + GRID_OFFSET_HEIGHT, 0);
	
	return;
}

int insidegrid(int x, int y)
{
	if((x > GRID_OFFSET_WIDTH && x < DISPLAY_WIDTH - GRID_OFFSET_WIDTH) && (y > GRID_OFFSET_HEIGHT && y < DISPLAY_HEIGHT + GRID_OFFSET_HEIGHT))
		return 1;
	else
		return 0;	
}

Ball get_piece(int mouse_x, int mouse_y, Ball matrix[MAX_ROW][MAX_COL])
{
	int col = (mouse_x - GRID_OFFSET_WIDTH) / BALL_SZ;
	int row = (mouse_y - GRID_OFFSET_HEIGHT) / BALL_SZ;
	return matrix[row][col];
}

void handlemouseclick(Ball matrix[MAX_ROW][MAX_COL], Ball auxball[], Ball current)
{
	if(auxball[0].sel == 0)
	{
		auxball[0] = current;
		matrix[auxball[0].row][auxball[0].col].sel = 1;
	}

	else if(checkrelativeposition(auxball[0], current))
	{
		auxball[1] = current;
		matrix[auxball[1].row][auxball[1].col].sel = 1;

		player_moves = player_moves - 1;
		swapballsanimation(matrix, auxball[0], auxball[1]);
		swapballs(matrix, auxball[0], auxball[1]);
		restart_aux_ball(auxball, 0);
		restart_aux_ball(auxball, 1);

		possiblemoves(matrix);
	}
	else
	{
		matrix[auxball[0].row][auxball[0].col].sel = 0;
		auxball[0] = current;
		matrix[auxball[0].row][auxball[0].col].sel = 1;
	}
}	

int checkrelativeposition(Ball b1, Ball b2)
{
	if(b1.row == b2.row && b1.col == b2.col + 1)
		return 1;
	else if(b1.row == b2.row && b1.col == b2.col - 1)
		return 1;
	else if (b1.col == b2.col && b1.row == b2.row + 1)
		return 1;
	else if (b1.col == b2.col && b1.row == b2.row - 1)
		return 1;
	else
		return 0;
}


void swapballsanimation(Ball matrix[MAX_ROW][MAX_COL], Ball b1, Ball b2)
{
	ALLEGRO_EVENT ev;
	createtimer();
	int move = 0;

	if (b1.col == b2.col)
	{ 
		if (b1.row > b2.row)
		{

			while ( !(b1.row * BALL_SZ + GRID_OFFSET_HEIGHT - move == b2.row * BALL_SZ + GRID_OFFSET_HEIGHT) )
			{
				al_wait_for_event(fila_timer, &ev);
				move += 10;
				initstate2(matrix);
				al_draw_bitmap(matrix[b1.row][b1.col].bcolor, b1.col * BALL_SZ + GRID_OFFSET_WIDTH, b1.row * BALL_SZ + GRID_OFFSET_HEIGHT - move, 0);
				al_draw_bitmap(matrix[b2.row][b2.col].bcolor, b2.col * BALL_SZ + GRID_OFFSET_WIDTH, b2.row * BALL_SZ + GRID_OFFSET_HEIGHT + move, 0);
				al_flip_display();
			}
		}

		else if (b1.row < b2.row)
		{

			while ( !(b1.row * BALL_SZ + GRID_OFFSET_HEIGHT + move == b2.row * BALL_SZ + GRID_OFFSET_HEIGHT) )
			{
				al_wait_for_event(fila_timer, &ev);
				move += 10;
				initstate2(matrix);
				al_draw_bitmap(matrix[b1.row][b1.col].bcolor, b1.col * BALL_SZ + GRID_OFFSET_WIDTH, b1.row * BALL_SZ + GRID_OFFSET_HEIGHT + move, 0);
				al_draw_bitmap(matrix[b2.row][b2.col].bcolor, b2.col * BALL_SZ + GRID_OFFSET_WIDTH, b2.row * BALL_SZ + GRID_OFFSET_HEIGHT - move, 0);
				al_flip_display();
			}
		}
	}

	else if (b1.row == b2.row)
	{
		if (b1.col > b2.col)
		{

			while( !(b1.col * BALL_SZ + GRID_OFFSET_WIDTH - move == b2.col * BALL_SZ + GRID_OFFSET_WIDTH) )
			{
				al_wait_for_event(fila_timer, &ev);
				move += 10;
				initstate2(matrix);
				al_draw_bitmap(matrix[b1.row][b1.col].bcolor, b1.col * BALL_SZ + GRID_OFFSET_WIDTH - move, b1.row * BALL_SZ + GRID_OFFSET_HEIGHT, 0);
   				al_draw_bitmap(matrix[b2.row][b2.col].bcolor, b2.col * BALL_SZ + GRID_OFFSET_WIDTH + move, b2.row * BALL_SZ + GRID_OFFSET_HEIGHT, 0);
				al_flip_display();
			}
		}

		else if ( b1.col < b2.col )
		{
			while( !(b1.col * BALL_SZ + GRID_OFFSET_WIDTH + move == b2.col * BALL_SZ + GRID_OFFSET_WIDTH) )
			{
				al_wait_for_event(fila_timer, &ev);
				move += 10;
				initstate2(matrix);
				al_draw_bitmap(matrix[b1.row][b1.col].bcolor, b1.col * BALL_SZ + GRID_OFFSET_WIDTH + move, b1.row * BALL_SZ + GRID_OFFSET_HEIGHT, 0);
   				al_draw_bitmap(matrix[b2.row][b2.col].bcolor, b2.col * BALL_SZ + GRID_OFFSET_WIDTH - move, b2.row * BALL_SZ + GRID_OFFSET_HEIGHT, 0);
				al_flip_display();
			}
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(fila_timer);
	return;
}

void createtimer()
{
	timer = al_create_timer(1.0 / FPS);
	if( !timer )
	{
		fprintf(stderr,"Falha ao criar temporizador");
		exit(1);
	}
	
	fila_timer = al_create_event_queue();
	if (!fila_timer)
	{
		fprintf(stderr, "Falha ao criar fila de events");
		exit(1);
	}

	al_register_event_source(fila_timer, al_get_timer_event_source(timer));
	al_start_timer(timer);
	return;
}


void swapballs(Ball matrix[MAX_ROW][MAX_COL], Ball b1, Ball b2)
{
	matrix[b1.row][b1.col].row = b2.row;
	matrix[b1.row][b1.col].col = b2.col;
	matrix[b1.row][b1.col].sel = 0;

	matrix[b2.row][b2.col].row = b1.row;
	matrix[b2.row][b2.col].col = b1.col;
	matrix[b2.row][b2.col].sel = 0;
	
	Ball aux = matrix[b2.row][b2.col];
	matrix[b2.row][b2.col] = matrix[b1.row][b1.col];
	matrix[b1.row][b1.col] = aux;

	return;
}

	
void restart_aux_ball(Ball auxball[], int i)
{
	auxball[i].row = -1;
	auxball[i].col = -1;
	auxball[i].sel = 0;
	auxball[i].bcolor=NULL;
}

void possiblemoves(Ball matrix[MAX_ROW][MAX_COL])
{

	zero_aux_matrix(auxmatrix);
	findsequence(matrix);

	if(!remainingSequence())
		play_sound();

	while(remainingSequence())
	{
		printf("\n\n");
		printmatrix(auxmatrix);

		play_sound();
		zero_matrix_sequence(matrix);

		// points
		player_score = player_score + 3 + (remainingSequence() - 3) * 2;

		initstate2(matrix);
		al_flip_display();
		al_rest(1);

		updatematrix(matrix);
		randmatrix(matrix);
		movematrix(matrix);
		al_rest(0.7);

		zero_aux_matrix(auxmatrix);
		findsequence(matrix);

		if(remainingSequence())
			play_sound();
	}

	if(player_moves == 0)
	{
		gamestate = 3;
		changegamestate(gamestate);
	}

}

void zero_aux_matrix(int m[MAX_ROW][MAX_COL])
{
	int i,j;
	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0; j < MAX_COL; j++)
		{
			m[i][j] = 0;
		}
	}
}

void printmatrix(int m[MAX_ROW][MAX_COL])
{
	int i,j;
	for(i = 0;i < MAX_ROW; i++)
	{
		for(j = 0; j < MAX_COL; j++)
		{
			printf("%d ", m[i][j]);
		}	
		printf("\n");
	}
}



void findsequence(Ball m[MAX_ROW][MAX_COL])
{
	int i, j;
	int counter;

	for(i = 0; i < MAX_ROW; i++)
	{
		counter = 1;

		for(j = 0; j < MAX_COL - 1; j++)
		{
			if(m[i][j].ncolor == m[i][j + 1].ncolor)
			{
				counter++;

				if(j == MAX_COL - 2 && counter >= 3)
				{
					mark_row(i, j + 2 - counter, counter);
					counter = 1;
				}
			}
			else
			{
				if(counter >= 3)
				{
					mark_row(i, j + 1 - counter, counter);
				}
				counter = 1;
			}
		}
	}

	for(i = 0; i < MAX_COL; i++)
	{
		counter = 1;

		for(j = 0; j < MAX_ROW - 1; j++)
		{
			if(m[j][i].ncolor == m[j + 1][i].ncolor)
			{
				counter++;

				if(j == MAX_ROW - 2 && counter >= 3)
				{
					mark_col(j + 2 - counter, i, counter);
				}
			}
			else
			{
				if(counter >= 3)
				{
					mark_col(j + 1 - counter, i, counter);
				}
				counter = 1;
			}
		}
	}
}

void mark_row(int startrow, int startcol, int sequence)
{
	int j;
	for(j = startcol; j < startcol + sequence; j++)
	{
		auxmatrix[startrow][j] = 1;
	}
}


void mark_col(int startrow, int startcol, int sequence)
{
	int i;
	for(i = startrow; i < startrow + sequence; i++)
	{
		auxmatrix[i][startcol] = 1;
	}
}


void zero_matrix_sequence(Ball matrix[MAX_ROW][MAX_COL])
{
	int i,j;
	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0; j < MAX_COL; j++)
		{
			if(auxmatrix[i][j] == 1)
			{
				matrix[i][j].ncolor = 0;
				matrix[i][j].bcolor = ballcolor[0];
			}
		}
	}
}


int remainingSequence()
{
	int numsequence= 0;
	int i,j;
	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0;j < MAX_COL; j++)
		{
			if(auxmatrix[i][j] == 1)
			{
				numsequence++;
			}
		}
	}
	return numsequence;
}

void updatematrix(Ball matrix[MAX_ROW][MAX_COL])
{
	Ball aux;
	int i, j, k, n;

	for(j = 0; j < MAX_COL; j++)
	{
		n = 0;

		for(i = MAX_ROW - 1; i > n; i--)
		{
			while(matrix[i][j].ncolor == 0 && n != i)
			{
				aux = matrix[i][j];
				for(k = i; k > n; k--)
				{
					matrix[k-1][j].sel = 1;
					matrix[k][j] = matrix[k-1][j];
				}
				matrix[n][j] = aux;
				n++;
			}
		}
		
	}
}

void randmatrix(Ball matrix[MAX_ROW][MAX_COL])
{
	int i,j;

	for(i = 0; i < MAX_ROW; i++)
	{
		for(j = 0;j < MAX_COL; j++)
		{
			if(matrix[i][j].ncolor == 0)
			{	
				matrix[i][j].sel = 0;
				matrix[i][j].row = i;
				matrix[i][j].col = j;
				matrix[i][j].ncolor = 1 + rand() % 5;
				matrix[i][j].bcolor = ballcolor[matrix[i][j].ncolor];
			}
		}
	}
}

void movematrix(Ball matrix[MAX_ROW][MAX_COL])
{

	int completed = 0;
	int move_y = 0;

	ALLEGRO_EVENT ev;
	createtimer();

	while(!completed)
	{
		al_wait_for_event(fila_timer, &ev);
		completed = 1;
		int i,j;
		initstate2(matrix);
		for(i = 0;i < MAX_ROW; i++)
		{
			for(j = 0; j < MAX_COL; j++)
			{
				if(matrix[i][j].sel == 1)
				{
					if( !(matrix[i][j].row * BALL_SZ + GRID_OFFSET_HEIGHT + move_y == i * BALL_SZ + GRID_OFFSET_HEIGHT))
					{
						completed = 0;
						al_draw_bitmap(matrix[i][j].bcolor, matrix[i][j].col * BALL_SZ + GRID_OFFSET_WIDTH, matrix[i][j].row * BALL_SZ + GRID_OFFSET_HEIGHT + move_y, 0);
					}
					else
					{
						al_play_sample(bounce_sound, 1.0, 0.0,2,ALLEGRO_PLAYMODE_ONCE,NULL);
						al_draw_bitmap(matrix[i][j].bcolor, matrix[i][j].col * BALL_SZ + GRID_OFFSET_WIDTH, matrix[i][j].row * BALL_SZ + GRID_OFFSET_HEIGHT + move_y, 0);
						matrix[i][j].sel = 0;
						matrix[i][j].row = i;
						matrix[i][j].col = j;
					}
				}
			}
		}
		al_flip_display();
		move_y = move_y + 10;
	}
	al_destroy_timer(timer);
	al_destroy_event_queue(fila_timer);
	return;	
}


int newMaxScore() 
{
	FILE *arq = fopen("maxscore.txt", "r");
	max_score = 0;
	if(arq != NULL) {
		fscanf(arq, "%d", &max_score);
		fclose(arq);
	}
	if(max_score < player_score ) {
		arq = fopen("maxscore.txt", "w");
		fprintf(arq, "%d", player_score);
		fclose(arq);
		return 1;
	}
	return 0;
	
}

void play_sound()
{
	int sequence = remainingSequence();

	if(sequence < 3)
	{
		al_play_sample(som_ponto0, 1.0, 0.0,1.2,ALLEGRO_PLAYMODE_ONCE,NULL);
	}
	else if(sequence == 3|| sequence == 4)
	{
		al_play_sample(som_ponto1, 1.0, 0.0,1.3,ALLEGRO_PLAYMODE_ONCE,NULL);
	}
	else
	{
		al_play_sample(som_ponto2, 1.0, 0.0,1.5,ALLEGRO_PLAYMODE_ONCE,NULL);
	}
	return;
}

bool inicializar()
{
	if (!al_init())
	{
		fprintf(stderr, "Faliled to initialize the Allegro library.\n");
		return false;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Failed to initialize the primitives add-on.\n");
		return false;
	}

	display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if (!display)
	{
		fprintf(stderr, "Failed to create the display.\n");
		return false;
	}

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		fprintf(stderr, "Failed to create the event queue");
		return false;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, "Failed to install mouse");
		return false;
	}

	if (!al_init_image_addon())
	{
		fprintf(stderr,"Failed to initialize the image add-on");
		return false;
	}

	if (!al_init_font_addon())
	{
		fprintf(stderr,"Failed to initialize the font add-on");
		return false;
	}

	if (!al_init_ttf_addon())
	{
        fprintf(stderr, "Failed to initialize the tff font add-on");
        return false;
    }

    font = al_load_font("scoreboard.ttf", 100, 0);
    if (!font)
    {
        fprintf(stderr, "Falha ao criar fonte");
        return false;
    }

    smallfont = al_load_font("scoreboard.ttf", 80, 0);
    if (!smallfont)
    {
        fprintf(stderr, "Falha ao criar fonte");
        return false;
    }

    if (!al_install_audio())
	{
		fprintf(stderr, "Failed to install the audio");
		return false;
	}

	if (!al_init_acodec_addon())
	{
		fprintf(stderr, "Failed to itinialize the acodec add-on");
	}

	//cria o mixer (e torna ele o mixer padrao), e adciona 5 samples de audio nele
    al_reserve_samples(5);
    win_sound = al_load_sample("win.ogg");
    som_ponto0 = al_load_sample("ponto0.wav");
    som_ponto1 = al_load_sample("ponto1.wav");
    som_ponto2 = al_load_sample("ponto2.wav");
    bounce_sound = al_load_sample("bounce.wav");

    ballcolor[0] = al_load_bitmap("bolabranca.png");
	ballcolor[1] = al_load_bitmap("bolarosa.png");
	ballcolor[2] = al_load_bitmap("bolaazul.png");
	ballcolor[3] = al_load_bitmap("bolaverde.png");
	ballcolor[4] = al_load_bitmap("bolalaranja.png");
	ballcolor[5] = al_load_bitmap("bolaamarela.png");
	ballcolor[6] = al_load_bitmap("bolaverdeagua.png");
	
	fundo = al_load_bitmap("fundo.png");
	fundo2 = al_load_bitmap("fundo2.png");
	score = al_load_bitmap("score.png");
	bggameover = al_load_bitmap("bggameover.png");
	fundomenu = al_load_bitmap("campo.png");

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	return true;
}

