/*
	绘星 Main file
	Last update: 2010.4.20
*/


# include "9860g.h"


int AddIn_main(int isAppli, unsigned short OptionNum)
{

	unsigned int key ;

	/* initialise */
	OpenFont() ;
	if (InitData() == FALSE) {CloseFont() ; return FALSE ;} 
		
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */
	/* Main menu */
	Print_zh("绘　星", 40, 2, UNDER_LINE) ;
	Print_zh("F1 工具 ", 10, 19, 0) ;
	Print_zh("F2 放样 ", 70, 19, 0) ;
	Print_zh("F3 测站 ", 10, 33, 0) ;
	Print_zh("F4 测点 ", 70, 33, 0) ;
	Print_zh("F5 数据 ", 10, 47, 0) ;
	Print_zh("F6 设置 ", 70, 47, 0) ;
	SaveDisp(SAVEDISP_PAGE1) ;		/* Save screen */

	while(TRUE) {
		GetKey(&key) ;
		switch (key) {
		case KEY_CTRL_F1:
			Tools_Menu() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_F2:
			LoftingNor() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_F3:
			SetStation() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_F4:
			Survey() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_F5:
			Database() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_F6:
			ProjSet() ;
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		case KEY_CTRL_EXE:
			RestoreDisp(SAVEDISP_PAGE1) ;
			break ;
		default:
			break ;
		}
	} 
	
	/* The end */
	//CloseStation() ;
	CloseFont() ;
	
	return TRUE ;
}



int InitData()
{
	# ifdef RELEASE
	FONTCHARACTER pt_folder[] = { '\\','\\','f','l','s','0','\\','P','O','I','N','T','\0'} ;
	# else
	FONTCHARACTER pt_folder[] = { '\\','\\','c','r','d','0','\\','P','O','I','N','T','\0'} ;
	# endif

	# ifdef RELEASE
	FONTCHARACTER md5_folder[] = { '\\','\\','f','l','s','0','\\','M','D','5','\0'} ;
	# else
	FONTCHARACTER md5_folder[] = { '\\','\\','c','r','d','0','\\','M','D','5','\0'} ;
	# endif

	unsigned int any_key ;
	
	Bfile_CreateDirectory(pt_folder) ;
	Bfile_CreateDirectory(md5_folder) ;

	while ( InitConfig() != 0 ) DebugS("按EXE重启程序 ")  ;
	autoload_kzd() ;
	if (dbset.current_pq != NULL)  {if (isLocked(dbset.current_pq)==TRUE){Lockfile(dbset.current_pq,FALSE);}autoload_pqx(FALSE) ;}
	if (dbset.current_sq != NULL)  {if (isLocked(dbset.current_sq)==TRUE){Lockfile(dbset.current_sq,FALSE);}autoload_sqx(FALSE) ;}
	if (dbset.current_ys != NULL)  {if (isLocked(dbset.current_ys)==TRUE){Lockfile(dbset.current_ys,FALSE);}autoload_xyys(FALSE) ;}
	if (dbset.current_hp[0] != 0)  autoload_LF(FALSE) ;
	if (dbset.current_sd != NULL)  {if (isLocked(dbset.current_sd)==TRUE){Lockfile(dbset.current_sd,FALSE);}autoload_SD(FALSE) ;}
	return TRUE;
}

void Error(char* tips)
{
	PopUpWin(3) ;
	Print_zh(tips, 22, 22, 0) ;
	return ;
}

void Warning(char* tips, int time)
{
	SaveDisp(SAVEDISP_PAGE3) ;	
	PopUpWin(3) ;
	Print_zh(tips, 28, 22, 0) ;
	Sleep(time*1000) ;
	RestoreDisp(SAVEDISP_PAGE3) ;
	return ;
}


int DebugS(const char *tips, ...)
{
	unsigned int key ;
	char printf_buf[256];
	va_list args;
	int printed;	

	va_start(args, tips);
	printed = vsprintf(printf_buf, tips, args);
	va_end(args);
	
	SaveDisp(SAVEDISP_PAGE3) ;	
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */
	PopUpWin(5) ;

	Print_zh(printf_buf, 15, 11, NORMAL);	
	PrintMini(94, 43, (unsigned char*)" EXE ", MINI_REV) ; 
	
	do {
		GetKey(&key);
	}while (key != KEY_CTRL_F1 && key != KEY_CTRL_F6 && key != KEY_CTRL_EXE && key != KEY_CTRL_EXIT) ;

	RestoreDisp(SAVEDISP_PAGE3) ;
	if (key == KEY_CTRL_EXIT) return EXIT_F ;
	else if  (key == KEY_CTRL_F1) return TRUE ;
	else if  (key == KEY_CTRL_F6) return FALSE ;
	else return FALSE ;
}


static int xx, yy ;
int locatestr(BYTE x_pos, BYTE y_pos)
{
	xx = x_pos;
	yy = y_pos ;
	return ;
}

int printf(const char *fmt, ...)
{
  char printf_buf[256];
  va_list args;
  int printed;	

  va_start(args, fmt);
  printed = vsprintf(printf_buf, fmt, args);
  va_end(args);

  Print_zh(printf_buf, xx, yy, NORMAL);	

  return printed;
}

int printfMINI(const char *fmt1, ...)
{
	/*char cpBuffer[STRING_BUFFER_SIZE];
	va_list pArgList;

	va_start(pArgList, cpFormat);
	vsprintf(cpBuffer, cpFormat, pArgList);
	va_end(pArgList);*/
  char printf_buf[256];
  va_list args1;
  int printed;	

  va_start(args1, fmt1);
  printed = vsprintf(printf_buf, fmt1, args1);
  va_end(args1);

  PrintMini(xx, yy, (unsigned char*)printf_buf, MINI_OVER);	

  return printed;
}



/*****************************************************************************
****************                                                         ****************
****************                          Notice!                     ****************
****************                                                          ****************
****************  Please do not change the following source.  *********
****************                                                          ****************
*****************************************************************************/


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

#pragma section _TOP

/* InitializeSystem */
 
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum) ;
}

#pragma section

