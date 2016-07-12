/*
	9860g header file
	Last update: 2010.2.5
*/

/*  无release为sd版本，pc_test为sdk调试定义  */
//# define RELEASE 1

# ifndef  RELEASE
# define PC_TEST 1
# endif

# ifndef  GLOBAL

# include "fxlib.h"
# include "filebios.h"
# include "dispbios.h"
# include "timer.h"
# include <math.h>
# include <stddef.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <stdarg.h>
# include <setjmp.h>

/*  enum常量  */
enum token_type{
	TOKEN_ERR = -1,
	
	TOKEN_COMMENT= 256,
	TOKEN_TITLE,	
	TOKEN_ID,   	/* 	标识符	*/
	TOKEN_EQUAL,	/* 	等号		*/
	TOKEN_NUM,
	TOKEN_FOLDER,	/*  文件路径  */
};
enum xian_duan {
/*
第一直线
第一缓和曲线
圆曲线
第二缓和曲线
第二直线
*/
	XD_ERROR = -1 ,
	XD_DYZX = 256,
	XD_DYQX,
	XD_YQX,
	XD_DEQX,
	XD_DEZX,
} ;

enum parse_prog{
	PROG_ERR = -1,
	
	PROG_REGCD= 256,	//reg set
	PROG_SN,
	PROG_STR,
	PROG_STATION,		//station set
	PROG_X1,
	PROG_Y1,
	PROG_H1,
	PROG_X2,
	PROG_Y2,
	PROG_H2,
	PROG_STH,
	PROG_PRH,
	PROG_NUM,
	PROG_PROJSET,		//porject set
	PROG_STAKED,
	PROG_WDMAX,
	PROG_XLTYPE,
	PROG_PJMODE,
	PROG_DATABASE,	//database
	PROG_CUPQ,
	PROG_PQ1,
	PROG_PQ2,
	PROG_PQ3,
	PROG_PQ4,
	PROG_PQ5,
	PROG_CUSQ,
	PROG_SQ1,
	PROG_SQ2,
	PROG_SQ3,
	PROG_SQ4,
	PROG_SQ5,
	PROG_VER,
	PROG_CUPLD,
	PROG_FOLDER,
	PROG_CUYS,
	PROG_YS1,
	PROG_YS2,
	PROG_YS3,
	PROG_YS4,
	PROG_YS5,
	PROG_YS6,
	PROG_YS7,
	PROG_YS8,
	PROG_CUHP,
	PROG_CUSD,
	PROG_SD1,
	PROG_SD2,
	PROG_SD3,
	PROG_SD4,
	PROG_SD5,
	PROG_SD6,
	PROG_SD7,
	PROG_SD8,
};

typedef unsigned long       DWORD ;
typedef unsigned char       BYTE ;
typedef unsigned short      WORD ;
typedef unsigned char	      BOOL ;

# define TRUE 1 
# define FALSE 0 
# define FSOK 2
# define FS_NOPOPUP 168
# define FS_POPUP 199
# define EXIT_F 3
//# define SET  1
# define CLEAR 0
# define PI 3.14159265358979
# define  GLOBAL
/*   配置文件各项   */
# define INI_ERR 0 
# define INI_REGCHECK 1
# define LATER 0
# define NOW 1


extern const DISPBOX clear_area ;
typedef struct tag_PPOS {
	unsigned int sk ;
	unsigned int wd ;
	unsigned int xl ;
	unsigned int pj ;
} P_FPOS ;

struct tag_SET {
	double stake_d ;
	double width_max ;
	double xl_type ;
	double pj_mode ;
	P_FPOS fpos ;
} ;
extern struct tag_SET allset ;

typedef struct tag_DPOS {
	unsigned int cupq ;
	unsigned int pq1 ;
	unsigned int pq2 ;
	unsigned int pq3 ;
	unsigned int pq4 ;
	unsigned int pq5 ;
} D_FPOS ;
typedef struct tag_DSPOS {
	unsigned int cusq ;
	unsigned int sq1 ;
	unsigned int sq2 ;
	unsigned int sq3 ;
	unsigned int sq4 ;
	unsigned int sq5 ;
} DS_FPOS ;
typedef struct tag_DYPOS {
	unsigned int cuys ;
	unsigned int ys1 ;
	unsigned int ys2 ;
	unsigned int ys3 ;
	unsigned int ys4 ;
	unsigned int ys5 ;
	unsigned int ys6 ;
	unsigned int ys7 ;
	unsigned int ys8 ;
} DY_FPOS ;

typedef struct tag_SDPOS {
	unsigned int cusd ;
	unsigned int sd1 ;
	unsigned int sd2 ;
	unsigned int sd3 ;
	unsigned int sd4 ;
	unsigned int sd5 ;
	unsigned int sd6 ;
	unsigned int sd7 ;
	unsigned int sd8 ;
} DD_FPOS ;

struct db_SET {
	char* current_pq;
	char pq1[5] ;
	char pq2[5] ;	
	char pq3[5] ;	
	char pq4[5] ;	
	char pq5[5] ;	
	D_FPOS fpos ;
	char* current_sq;
	char sq1[5] ;
	char sq2[5] ;	
	char sq3[5] ;	
	char sq4[5] ;	
	char sq5[5] ;
	DS_FPOS s_fpos ;
	unsigned int pld_pos ;
	char* current_ys ;
	char ys1[5] ;
	char ys2[5] ;
	char ys3[5] ;
	char ys4[5] ;
	char ys5[5] ;
	char ys6[5] ;
	char ys7[5] ;
	char ys8[5] ;
	DY_FPOS ys_fpos ;
	char current_hp[5] ;
	unsigned int hp_pos ;
	char* current_sd ;
	char sd1[5] ;
	char sd2[5] ;
	char sd3[5] ;
	char sd4[5] ;
	char sd5[5] ;
	char sd6[5] ;
	char sd7[5] ;
	char sd8[5] ;
	DD_FPOS sd_fpos ;
} ;
extern struct db_SET dbset ;

# endif

# ifndef  FUNCTION
int printfMINI(const char *fmt1, ...) ;
int Print_zh(char* input, BYTE x, BYTE y, BYTE type) ;	/* x(0~127), y(0~63) 每行8个字，总共4行 */
void Error(char* tips) ;
void Warning(char* tips, int time) ;
int DebugS(const char *tips, ...);
int parse(char* string);
struct tkbox* lexan();
struct tkbox* lexan_num();
struct tkbox* get(int token);

int OpenFont(void) ;
void CloseFont(void) ;
int SetStation(void) ;

double* ReadFloat(BYTE list_num, BYTE list, int size) ;
extern double X_C(double M9, double Ls1, double R) ;

double InputAngle(void) ;
int Suidao_DM(void);	/*	隧道断面 By croptree	*/

# define FUNCITON
# endif

# ifndef  STRUCT
/*  结构声明  */
struct tkbox {
    char* str ;
    int token  ;
    double value ;
} ; /*  元素纸盒  */
extern struct tkbox statebox ; /*  token盒子  */

typedef struct tag_COORDINATE {
	double x ;	/* 坐标点 */
	double y ;
	double z ;
} S_POINT ;
typedef struct tag_FPOS {
	unsigned int x1 ;
	unsigned int y1 ;
	unsigned int h1 ;
	unsigned int x2 ;
	unsigned int y2 ;
	unsigned int h2 ;
	unsigned int sth ;
	unsigned int prh ;
} S_FPOS ;

typedef struct tag_LISTPAGE {
	BYTE total ;
	char o1[5] ;
	char o2[5] ;
	char o3[5] ;
	char o4[5] ;
	char o5[5] ;
	char o6[5] ;
} L_PAGE ;
static L_PAGE page ;

struct tag_TOTAL {
	S_POINT stat_point ;	/* 测站点 */
	S_POINT bak_point ;	/* 后视点 */
	double azimuth ;	/* 方位角 */	
	double setangle ;	/* 置角度 */
	double station_ht ;	/* 仪高 */
	double prism_ht ;	/* 镜高 */
	S_FPOS fpos ;
} ;
extern struct tag_TOTAL total_set ;

#define N_LINE 6
#define FILENAMEMAX 13
#define STRING_BUFFER_SIZE 128

typedef struct 
{
	char filename[FILENAMEMAX];
	unsigned long filesize;
}Files;


# define STRUCT
# endif

# ifndef  FONT_ZH
static int fontptr ;
static char font_buf[200] ;
DWORD GetPosWithMbcs(WORD code) ;

# define NORMAL	0
# define REVERSE	1
# define UNDER_LINE 	2
# define VERT_REV	3
# define VERT 4

# define  FONT_ZH
# endif

# ifndef QXYS
//extern int xx, yy ;		//used for function "locatestr"

extern int list_size ; 
extern double x;
extern double y;
extern BYTE qxys_flag ;
extern double* qx_ZH  ;
extern double* qx_HY ;
extern double* qx_YH ;
extern double* qx_HZ ;
extern double* qx_Stake ;
extern double* qx_X ;
extern double* qx_Y ;
extern double* qx_R ;
extern double* qx_Ls1 ;
extern double* qx_T1 ;
extern double* qx_angel ;
extern double* qx_zy ;
extern double* qx_ZJ ;

extern BYTE sqx_flag ;
extern int sqx_size;
extern double* sq_Stake ;
extern double* sq_GC  ;
extern double* sq_R ;
extern double DesignGC ;
extern double fs_zh ;	//反算得出的里程偏距，专用于设计高程计算 
extern double fs_pj ;

extern BYTE pld_flag ; //批量坐标文件标志
extern FONTCHARACTER pld_FileName[50];
extern char name[FILENAMEMAX] ;
extern unsigned int ptlist_count ;

extern BYTE SDcard_F ;
extern BYTE Fls_F ;

/* 线元要素 */
extern int xyys_size;
extern double* xy_zd_stake ;		/*线元终点里程*/
extern double* xy_zd_R ;		/*线元终点半径*/
extern double* xy_zd_X  ;
extern double* xy_zd_Y  ;
extern double* xy_zd_angle ;
extern BYTE xyf_flag ;
extern double PJ_dist  ;
extern double j_to_h ;



struct XYstat{
	double Mile ;	//里程
	double x ;
	double y ;
	double a ;		//切线方位角
	double Ro ;	//曲率
};

struct CurSec{
	struct XYstat START_Sta ;
	struct XYstat END_Sta ;
};

extern struct XYstat Sta ;

/* 路幅辅助参数 */
extern int LF_size ;
extern double* LF_L_Width ;
extern double* LF_L_HP ;
extern double* LF_R_Width ;
extern double* LF_R_HP ;
extern double* LF_Start ;
extern double* LF_End ;
extern unsigned int key_HP ;
extern unsigned int LF_JS(double stake) ;
extern BYTE LF_flag ;

/* 隧道全局变量 */
extern BYTE sd_flag ;
extern int SD_size;
extern double* SD_start_x ;	/*起点坐标X*/
extern double* SD_start_y ;	/*起点坐标Y*/
extern double* SD_end_x ;	/*终点坐标X*/
extern double* SD_end_y ;	/*终点坐标Y*/
extern double* SD_circle_x ;/*圆心X*/
extern double* SD_circle_y ;/*圆心Y*/
extern double* SD_R ;		/*半径*/

# define QXYS
# endif

# ifndef  IO
extern double number ;
extern char ipstr[5] ;
extern double angel ;
extern double CA ; 
extern double CB ;
extern double CC ;
extern BYTE timeout  ;



double Cut_Double(double val) ;
double Cut_3(double val) ;
double Cut_2(double val) ;
int Print_C(int x, int y, const double a, unsigned int e);
int Print_C_Val(const double a, const double b) ;
int Print_CC_Val(int x, int y, const double a) ;
void timeoutjump(void) ;
double Sixty2ten(double sixty) ;
double Ten2sixty(double ten) ;
double FS_D(double x1, double y1, double x2, double y2) ;
double FS_A(double x1, double y1, double x2, double y2,int type) ;
int do_test(void) ;

# define MAXLINE  256
# define GETCH *forward++
# define RETCH forward--
extern char Linebuffer[MAXLINE] ;
extern char key[MAXLINE]  ;     /*  词素  */
extern char* begin ;
extern char* forward ; /*  无敌双指针  */
extern int stateT,stateK,stateV ;	//解析状态，T标题，K关键字，V值
extern int fpos ;  //文件指针记录

# define  IO
# endif

# ifndef  STATION_SET
extern FONTCHARACTER configfile[] ;
extern S_POINT point ;
# define  STATION_SET
# endif

# ifndef  POINT
static int back_handle ;
static int s_handle ;
enum {THIS =1, LAST, NEXT} ;
# define  POINT
# endif



