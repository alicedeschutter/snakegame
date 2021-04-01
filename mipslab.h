#define RIGHT 2
#define LEFT 4
#define UP 3
#define DOWN 5

void display_image(const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
void create_object(int opposite, int x, int y, int width, int height, const uint8_t src[], uint8_t dest[32][128]);
void create_screen(int opposite, int x, int y, int width, int height, uint8_t dest[32][128]);
int get_character(int opposite, int x, int y, char c, uint8_t dest[32][128]);
int create_string(int opposite, int x, int y, const char* s, uint8_t dest[32][128]);
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );


extern uint8_t screen[32][128];
extern const uint8_t const font[128*8];
extern const uint8_t const uppercase[28][25];
extern const uint8_t const number[11][15];
extern char textbuffer[4][16];

void game_start(void);
void game_logic();
void ai_game_logic();
int levels();
void draw_snake();
void create_food();
int rand();
void menu();
void draw_score();
void highscore_list();
void str_cpy(char* target, char* src, int size);

void delay(int);
void time2string( char *, int );
int getbtns(void);
int getsw(void);
int getbtn1(void);
void enable_interrupt(void);
