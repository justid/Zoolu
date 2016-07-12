/*
	Input function  输入函数
	Output function
	Last update: 2010.5.05
*/



# include "9860g.h"




/* 显示当前值 */
double CA ; 
double CB ;
int Print_C(int x, int y, const double a, unsigned int e)
{
	char ftoa[10] ;	/* 10 Byte print string buff (include \0) */
	memset(ftoa, '\0', 10) ;  
	PrintXY(x-49, y, (unsigned char *)"         ", 0) ;
	switch (e){
	case 1:
		sprintf(ftoa, "%.1f", a) ;	
		break ;
	case 2:
		sprintf(ftoa, "%.2f", a) ;	
		break ;	
	case 3:
		sprintf(ftoa, "%.3f", a) ;	
		break ;
	case 4:
		sprintf(ftoa, "%.4f", a) ;	
		break ;
	case 5:
		sprintf(ftoa, "%.5f", a) ;	
		break ;
	case 6:
		sprintf(ftoa, "%.6f", a) ;	
		break ;	
	default:
		return FALSE ;
		break ;
	}
	PrintXY(x - strlen(ftoa) * 6, y, (unsigned char *)ftoa, 0) ;
	return TRUE ;	
}

int Print_C_Val(const double a, const double b)
{
	Print_C(108, 24, a, 3);
	Print_C(108, 38, b, 3);	
}

int Print_CC_Val(int x, int y, const double a)
{
	Print_C(x, y, a, 4);
}

/*  保留三位小数  */
double Cut_3(double val)
{
	char temp_str[9] ;

	sprintf(temp_str, "%.3f", val) ;
	return atof(temp_str) ;
}

double Cut_2(double val)
{
	double temp_v ;
	temp_v= val * 100 ;
	temp_v =floor(temp_v) ;
	temp_v = temp_v / 100 ;
	return temp_v ;
}

/* 截取double浮点数后8位有效数字 */
double Cut_Double(double val)
{
	return (double)(val - ((int)val / 100000)*100000) ;
}

int Print_Float(int x, int y, double fl_num)
{
	char p_buff[16] ;	/* 16Byte buffer */	
	
	memset(p_buff, '\0', 16);  
	sprintf(p_buff, "%.3f", fl_num) ;
	PrintXY(x, y, (unsigned char*)p_buff, 0) ;
	return strlen(p_buff);
}

int Print_Val(int x, int y, int num, char flag)
{
	char p_buff[8] ;	/* 8Byte buffer */	

	memset(p_buff, '\0', 8);  
	p_buff [sprintf(p_buff, "%d", num)] = flag ;
	PrintXY(x, y, (unsigned char*)p_buff, 0) ;
	return strlen(p_buff);
}

extern double angel = 0.0 ;
/* 度分秒输出 */
int Print_Angel(BYTE x, BYTE y)
{
	double temp_azi = 0.0 ; 

	angel = (angel / PI) * 180 ;
	if (angel > 360) angel -= 360 ;
	if (angel < 0) angel += 360 ;

	temp_azi = floor(angel) ;
	x += Print_Val(x, y, (int) temp_azi, '\x8C') * 6 ;
	temp_azi = 60 * fmod(angel, 1) ;
	x += Print_Val(x, y, (int) temp_azi, '\'') * 6 ;
	temp_azi = ( temp_azi - (int)(temp_azi)  ) * 60 ;
	Print_Val(x, y, (int) temp_azi, '\"') ;
}

double number ;

int InputVal(int x, int y)
{
	unsigned int key ;
	char buf[] = {0, 0, 0, 0, 0, 0, 0, 0, 0,'\0'} ;	/* 11 Byte number (include point '\0') */
	BYTE pos = 0;    /* keep track of current position/offset in buffer */
	BYTE once = TRUE ;	
	number = 0.0 ;

	BuildBox(x, y) ;	/* 绘制长方形输入框 */
	
	while (TRUE){
		GetKey(&key) ;
		switch (key) {
		case KEY_CTRL_AC :
			once = TRUE ;
			pos = 0 ;	memset(buf, '\0', 10);  
			break ;
		case KEY_CTRL_EXE:
			ClearBox(x, y) ;	/* 清除长方形输入框 */
			number = atof((char*) buf) ;	/* 转换为数字，用于计算 */
			return key ; break ;
		case KEY_CTRL_UP :
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; break ;
		case KEY_CTRL_DOWN :
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; break ; 
		case KEY_CTRL_EXIT: case KEY_CTRL_OPTN:
		case KEY_CTRL_F1: case KEY_CTRL_F2: case KEY_CTRL_F3:
		case KEY_CTRL_F4: case KEY_CTRL_F5: case KEY_CTRL_F6:
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; 
			break ;
		case KEY_CTRL_DEL:
			if (buf[0]) {
				PrintXY(x+58-pos*6,y+2, (unsigned char *)" ", 0) ;
				buf[--pos] = '\0' ;
			} 
			if (pos == 0) {
				PrintXY(x+6,y+2, (unsigned char *)"         ", 0) ;
				return key ;
			}
			break ;
		case KEY_CTRL_LEFT:
		case KEY_CTRL_RIGHT:
		case KEY_CTRL_SHIFT:
			return key ;
			break ;
		default:  break ;
		}
		
		if (pos < 9) {
			if (once) { PrintXY(x+2,y+2, (unsigned char *)"          ", 0) ;once = FALSE ;}
			if ((key >= KEY_CHAR_0 && key <= KEY_CHAR_9) || key == KEY_CHAR_DP) {/* Numbers and point */
				buf[pos] = (char)key ;	
				++pos ;
			}	
			else if (key == KEY_CHAR_MINUS) {
				buf[pos] = '-' ;
				++pos ;
			}		
		}
		PrintXY(x+58-pos*6,y+2, (unsigned char *)buf, 0) ;
	}
	
	return FALSE;
}

int InputValP(int x, int y)
{
	unsigned int key ;
	char buf[] = {0, 0, 0, 0, 0, 0, 0, 0, 0,'\0'} ;	/* 11 Byte number (include point '\0') */
	BYTE pos = 0;    /* keep track of current position/offset in buffer */
	BYTE once = TRUE ;		
	BOOL ipflag = FALSE ;

	number = 0.0 ;
	BuildBox(x, y) ;	/* 绘制长方形输入框 */
	
	while (TRUE){
		GetKey(&key) ;
		switch (key) {
		case KEY_CTRL_AC :
			once = TRUE ;
			pos = 0 ;	memset(buf, '\0', 10);  
			break ;
		case KEY_CTRL_EXE:
			ClearBox(x, y) ;	/* 清除长方形输入框 */
			if (ipflag == TRUE){
				number = atof((char*) buf) ;
				if (number == 0) number = 0.000001 ;
			}
			else number = atof((char*) buf) ;	/* 转换为数字，用于计算 */
			return key ; break ;
		case KEY_CTRL_UP :
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; break ;
		case KEY_CTRL_DOWN :
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; break ; 
		case KEY_CTRL_EXIT:
		case KEY_CTRL_F1: case KEY_CTRL_F2: case KEY_CTRL_F3:
		case KEY_CTRL_F4: case KEY_CTRL_F5: case KEY_CTRL_F6:
			ClearBox(x, y) ;
			number = atof((char*) buf) ;	
			return key ; 
			break ;
		case KEY_CTRL_DEL:
			if (buf[0]) {
				PrintXY(x+58-pos*6,y+2, (unsigned char *)" ", 0) ;
				buf[--pos] = '\0' ;
			} 
			if (pos == 0) {
				PrintXY(x+6,y+2, (unsigned char *)"         ", 0) ;
				return key ;
			}
			break ;
		case KEY_CTRL_LEFT:
		case KEY_CTRL_RIGHT:
		case KEY_CTRL_SHIFT:
			return key ;
			break ;
		default:  break ;
		}
		
		if (pos < 9) {
			if (once) { PrintXY(x+2,y+2, (unsigned char *)"          ", 0) ;once = FALSE ;}
			if ((key >= KEY_CHAR_0 && key <= KEY_CHAR_9) || key == KEY_CHAR_DP) {/* Numbers and point */
				buf[pos] = (char)key ;	
				++pos ;
				ipflag = TRUE ;	//输入数字即有非0返回值
			}	
			else if (key == KEY_CHAR_MINUS) {
				buf[pos] = '-' ;
				++pos ;
			}		
		}
		PrintXY(x+58-pos*6,y+2, (unsigned char *)buf, 0) ;
	}
	
	return FALSE;
}

int BuildBox(int x, int y)
{
	DrawRect(x, y, 11, 62) ;
	return ;
}

int DrawRect(int x, int y, int h, int w)	/* height, width */
{
	Bdisp_DrawLineVRAM(x, y, x+w, y) ;
	Bdisp_DrawLineVRAM(x, y, x, y+h) ;
	Bdisp_DrawLineVRAM(x+w, y, x+w, y+h) ;
	Bdisp_DrawLineVRAM(x, y+h, x+w, y+h) ;	
}

int ClearBox(int x, int y)
{
	ClearRect(x, y, 11, 62) ;
	return ;
}

int ClearRect(int x, int y, int h, int w)
{
	Bdisp_ClearLineVRAM(x, y, x+w, y) ;	/* 清除长方形输入框 */
	Bdisp_ClearLineVRAM(x, y, x, y+h) ;
	Bdisp_ClearLineVRAM(x+w, y, x+w, y+h) ;
	Bdisp_ClearLineVRAM(x, y+h, x+w, y+h) ;	
	return ;
}

extern char ipstr[5] = {0, 0, 0, 0, 0} ;
int InputStr(int x, int y)
{
	unsigned int key ;
	BYTE pos = 0 ; 

	memset(ipstr,'\0', 5);  
	GetKey(&key) ;
	PrintXY(x, y, (unsigned char *)"    ", 0) ;
	do {
		switch (key) {
		case KEY_CTRL_EXE: 
			ipstr[pos] = '\0' ;
			return key ;
			break ;
		case KEY_CTRL_EXIT:
			return key ; 
			break ;
		case  KEY_CTRL_ALPHA:
			break ;
		case KEY_CTRL_DEL:
			if (pos)  ipstr[--pos] = '\0' ; 
			PrintXY(x, y, (unsigned char *)"    ", 0) ;
			break ;
		default: break ;
		}

		 /* Numbers and characters */
		if ( (pos < 4) &&
		     ((key >= KEY_CHAR_0 && key <= KEY_CHAR_9) || 
		      (key >= KEY_CHAR_A && key <= KEY_CHAR_Z)) ) {
			ipstr[pos] = (char)key ;
			++pos ;
		}
		PrintXY(x, y, (unsigned char *)ipstr, 0) ;
		GetKey(&key) ;
	} while (TRUE) ;
}


int fgetch(int fd)
{
  char buff;
  int Bflag =0 ;
  Bflag = Bfile_ReadFile(fd,&buff,1,-1);
  //if (Bflag <= 0) DebugS("test signal");
  return (Bflag <= 0)? EOF : buff ;
 }

char* Ugets(char *str, int size, int fd, int nl) {
  char *next;
  next = str;
  while(--size > 0) {
    switch (*next = fgetch(fd)) {
      case  EOF: *next = '\0';
                 if(next == str) return (NULL);
                 return (str);
      case '\n': *(next + nl) = '\0';
                 return (str);
      default: ++next;
      }
    }
  *next = '\0';
  return (str);
}

char* fgetst(char *str,int size,int fd) {
  return (Ugets(str, size, fd, 1));
 }


/*  读取文本至缓冲区  */
char Linebuffer[MAXLINE] ;


char* getline(int fbuf)
{
    char* str ;

    str = fgetst(Linebuffer,MAXLINE,fbuf) ;
    

    return  str ? str : NULL ;
}

char* getPOSline(int fbuf,DWORD pos)
{
    char* str ;

    if (Bfile_ReadFile(fbuf,&Linebuffer,MAXLINE, pos) >= 0)
	return (char*)&Linebuffer ;
    else return NULL ;
}

/*  修改指定位置的内容为指定字符串 */
int memodify(char* ptr,int pos, void* str)
{
	unsigned int pt = 0 ;
	int size ;

	size = strlen(str) ;
	memcpy(ptr+pos,str,size);
	return 0;
}

BYTE timeout = FALSE ;
void timeoutjump(void)
{
	KillTimer(ID_USER_TIMER1) ;
	timeout = TRUE ;
}
/*  角度 六十进制转十进制  */
double Sixty2ten(double sixty)
{
	return (25 * sixty - 6 * floor(sixty) - 10 * Cut_2(sixty)) / 9  ;
	/* old
	double K ;	//度分秒
	double L ;
	double M ;

	K = floor(sixty) ;
	L = floor((sixty - K + 0.0000000001)*100) ;
	M = (sixty - K - L / 100) * 10000 ;

	return (K+L/60+M/3600) ;
	*/
	
}
/*  角度 十进制转六十进制  */
double Ten2sixty(double ten)
{
	double X ;	//终点度	终点分	终点秒	终点小数秒
	double Y ;
	double Z ;
	double AA ;

	X = floor(ten) ;
	Y = floor((ten-X)*60) ;
	Z = floor( ( (ten-X)*60-Y+0.000000000001 )*60) ;
	AA = floor((((ten - X)*60-Y)*60-Z)*1000+0.5) ;

	return (X+Y/100+Z/10000+AA/10000000) ;
}

/* 测试是否有SD卡 */
int has_SDcard(void) 
{
	int flag = FALSE ;
	FONTCHARACTER SDtest[] = { '\\','\\','c','r','d','0','\\','S','D','.','f','l','g','\0'} ;
	if (Bfile_CreateFile(SDtest,1) == 0)
	{
		if(Bfile_DeleteFile(SDtest) != 0) DebugS("请手动删除SD.flg");
		flag = TRUE ;
	} 
	else flag = FALSE ;

	return flag ;
}

/*  以minisize显示水准测量中数字  */
static int Printsmall_x = 0 ;
static int Printsmall_y = 0 ;

int Printsmall(double printval,BOOL init)
{
	char ftoa[10] ;	/* 10 Byte print string buff (include \0) */
	memset(ftoa, '\0', 10) ;  	

	if (init == TRUE){
		sprintf(ftoa,"%.3f",printval) ;
		PrintMini(5,2,(const unsigned char*)ftoa,MINI_OVER);
		Printsmall_x = 5 + strlen(ftoa)*4;	Printsmall_y = 2 ;
	} else if (printval >= 0){
		sprintf(ftoa,"+%.3f",printval) ;
		if (Printsmall_x + strlen(ftoa)*4 >= 112 ) {
			if (Printsmall_y == 2+6*2) {PrintMini(108,Printsmall_y,(const unsigned char*)" [..]",MINI_OVER);}	//可以显示的最大行 
			Printsmall_x = 5 ;Printsmall_y = Printsmall_y + 6 ;	//断行 
		}	
		if (Printsmall_y <= 2+6*2 ) PrintMini(Printsmall_x,Printsmall_y,(const unsigned char*)ftoa,MINI_OVER);
		Printsmall_x = Printsmall_x + strlen(ftoa)*4;	
	} else if (printval < 0){
		sprintf(ftoa,"-%.3f",-printval) ;
		if (Printsmall_x + strlen(ftoa)*4 >= 112 ) {
			if (Printsmall_y == 2+6*2) {PrintMini(108,Printsmall_y,(const unsigned char*)" [..]",MINI_OVER);}	//可以显示的最大行 
			Printsmall_x = 5 ;Printsmall_y = Printsmall_y + 6 ;	//断行 
		}	
		if (Printsmall_y <= 2+6*2 ) PrintMini(Printsmall_x,Printsmall_y,(const unsigned char*)ftoa,MINI_OVER);
		Printsmall_x = Printsmall_x + strlen(ftoa)*4;
	}
	
}
