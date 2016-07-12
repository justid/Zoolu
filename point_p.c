/*
	绘星 Manage Point file
	Last update: 2009.11.25
*/


# include "9860g.h"


BYTE CheckPoint(S_POINT point)
{
	if (point.x != 0 && point.y !=0 && point.z != 0)
		return TRUE ;
	else {
		Warning("数据不完全 ", 1) ; 
		return FALSE ;
	}
}

int SeekPoint()
{
	FONTCHARACTER pfile[30] ;
	char fname_buf[30] ;
	BYTE char_p ;
	int pdata ;

	memset(fname_buf,'\0',30);  
	# ifdef RELEASE
	strcat(fname_buf, (char*)"\\\\fls0\\POINT\\") ;
	# else
	strcat(fname_buf, (char*)"\\\\crd0\\POINT\\") ;	/* Parent folder */
	# endif
	strcat(fname_buf, (char*)ipstr) ;	/* Point name */
	strcat(fname_buf, (char*)".P") ;
	/* Char to FONTCHARACTER */
	for (char_p = 0 ; char_p <= strlen(fname_buf) ; ++char_p) {
		pfile[char_p] = (WORD)fname_buf[char_p] ;
	}
	/* 重建控制点文件 */
	Bfile_DeleteFile(pfile) ;
	Bfile_CreateFile(pfile, 29) ;
	pdata = Bfile_OpenFile(pfile, _OPENMODE_WRITE) ;	
	return pdata ;
}


int SelectPoint()
{
	unsigned int key ;
	int ff_handle = -1 ;		/* File handle return */
	BYTE e_flag = FALSE ; /* Exit flag */
	BYTE c_flag = FALSE ; /* Choose flag */
	# ifdef RELEASE
	FONTCHARACTER s_file[] = {'\\', '\\', 'f', 'l', 's', '0', '\\', 'P','O','I','N','T', '\\', '*', '*', '*', '*', '.' ,'P' ,'\0'} ;	
	# else
	FONTCHARACTER s_file[] = {'\\', '\\', 'c', 'r', 'd', '0', '\\', 'P','O','I','N','T', '\\', '*', '*', '*', '*', '.' ,'P' ,'\0'} ;	
	# endif
	char search_c[5] = { 0, 0, 0, 0, 0 } ;
	BYTE c_step = 0 ;

	PopUpWin(6) ;
	Print_zh("点号： ", 17, 6, 0) ; 
	DrawRect(60, 6, 10, 49) ;
	
	GetList(s_file, THIS) ;
	memset(search_c,'\0',5) ;  
	do {
		GetKey(&key) ;
		switch (key) {	
		case KEY_CTRL_EXE:
			ClearRect(60, 6, 10, 49) ;
			DrawRect(14, 20, 35, 95) ;
			PrintXY(110, 22, (unsigned char*)"\xE6\x9C ", 0) ;
			PrintXY(110, 49, (unsigned char*)"\xE6\x9D ", 0) ;
			c_flag =TRUE ;
			break ;
		case  KEY_CTRL_ALPHA:
			break ;
		case KEY_CTRL_DEL:
			PrintXY(70, 8, (unsigned char *)"    ", 0) ;
			memset(search_c,'\0',5) ;  
			c_step = 0 ;
			s_file[13] = s_file[14] = s_file[15] = s_file[16] = '*' ;
			break ;
		case KEY_CTRL_EXIT:
			e_flag = TRUE ;
			break ;	
		default: break ;
		}
		if (c_flag == TRUE)
			switch (key) {
			case KEY_CTRL_UP: 
				GetList(s_file, LAST) ;
				break ;
			case KEY_CTRL_DOWN:
				GetList(s_file, NEXT) ;
				break ;
			case KEY_CHAR_1: case KEY_CHAR_2: case KEY_CHAR_3: 
			case KEY_CHAR_4: case KEY_CHAR_5: case KEY_CHAR_6: 
				if (page.total >= key - 0x30) 
					switch (key - 0x30) {
					case 1: ff_handle = ReadPoint(page.o1) ; break ;
					case 2: ff_handle = ReadPoint(page.o2) ; break ;
					case 3: ff_handle = ReadPoint(page.o3) ; break ;
					case 4: ff_handle = ReadPoint(page.o4) ; break ;
					case 5: ff_handle = ReadPoint(page.o5) ; break ;
					case 6: ff_handle = ReadPoint(page.o6) ; break ;
					default: break ;
					}
				else Warning("无效的选择 ", 1) ;
				break ;
			}
		else if ( c_step < 4 && 
				( (key >= KEY_CHAR_0 && key <= KEY_CHAR_9)
		     		|| (key >= KEY_CHAR_A && key <= KEY_CHAR_Z) )  
			) {
			s_file[13+c_step] = (char)key ;
			search_c[c_step] = (char)key ;
			PrintXY(70, 8, (unsigned char *)search_c, 0) ;
			++c_step ;
			GetList(s_file, THIS) ;
		}
	} while (ff_handle < 0 && e_flag != TRUE) ;
	Bfile_FindClose(s_handle) ;
	return ff_handle ;
}

static L_PAGE page ;
static int back_handle = 0 ;
static int s_handle ;

int GetList(FONTCHARACTER* file, BYTE direct)
{	
	/* Search need */
	FONTCHARACTER f_file[30] ;
	int* handle_p = &s_handle ;
	FILE_INFO fileinfo ;

	char name_c[5] ;
	char print_f[30] ;
	const char li[] = {'1', '2', '3', '4', '5', '6'} ;
	BYTE num = 0 ;
	BYTE char_p = 0 ;
	DISPBOX pArea = {15, 21, 108, 53} ;

	switch (direct) {
	case THIS: case LAST:
		Bfile_FindClose(s_handle) ;	/* Check this out , for search handle */
		if (Bfile_FindFirst(file, handle_p, f_file, &fileinfo) != 0)
			return ;
		else Bdisp_AreaClr_DDVRAM(&pArea) ;
		break ;
	case NEXT:
		if (!back_handle)  return ;
		s_handle = back_handle ;
		if (Bfile_FindNext(s_handle, f_file, &fileinfo) != 0) 
			return ;
		else Bdisp_AreaClr_DDVRAM(&pArea) ;
		break ;
	default:
		break ;
	}
	do {
		/* FONTCHARACTER to Char */
		char_p = 0 ;
		memset(print_f,'\0',30);  
		memset(name_c,'\0',5);  
		while ((BYTE)f_file[char_p] != '.') {
			name_c[char_p] = (BYTE)f_file[char_p] ;
			++char_p ;
		}
		print_f[0] = li[num] ;
		strcat(print_f, (char*)"->") ;
		strncat(print_f, name_c, 5) ;
		switch (num) {
		case 0: strncpy(page.o1, name_c, 5) ; PrintXY(17, 25, (unsigned char*)print_f, 0) ; break ;
		case 1: strncpy(page.o2, name_c, 5) ; PrintXY(65, 25, (unsigned char*)print_f, 0) ; break ;
		case 2: strncpy(page.o3, name_c, 5) ; PrintXY(17, 35, (unsigned char*)print_f, 0) ; break ;
		case 3: strncpy(page.o4, name_c, 5) ; PrintXY(65, 35, (unsigned char*)print_f, 0) ; break ;
		case 4: strncpy(page.o5, name_c, 5) ; PrintXY(17, 45, (unsigned char*)print_f, 0) ; break ;
		case 5: strncpy(page.o6, name_c, 5) ; PrintXY(65, 45, (unsigned char*)print_f, 0) ; break ;
		default: break ;
		}		
		++num ;
		
		if (num == 6) { 
			back_handle = s_handle ; 
			break ; 
		}			
	}while (Bfile_FindNext(s_handle, f_file, &fileinfo) == 0)  ;
	page.total = num ;

	return ;
}

int ReadPoint(char* str)
{
	# ifdef RELEASE
	FONTCHARACTER p_base[] = {'\\', '\\', 'f', 'l', 's', '0', '\\', 'P','O','I','N','T', '\\', 0, 0, 0, 0, 0 ,0 ,0} ;	
	# else
	FONTCHARACTER p_base[] = {'\\', '\\', 'c', 'r', 'd', '0', '\\', 'P','O','I','N','T', '\\', 0, 0, 0, 0, 0 ,0 ,0} ;	
	# endif
	FONTCHARACTER p_extern[] = { '.' ,'P' ,'\0' } ;
	int p_handle ;
	BYTE step ;
	for (step = 0 ; step < strlen(str) ; ++step)
		p_base[13+step] = str[step] ;
	p_base[13+step] = p_extern[0] ;
	p_base[14+step] = p_extern[1] ;
	p_base[15+step] = p_extern[2] ;
	p_handle = Bfile_OpenFile(p_base, _OPENMODE_READ) ;	
	if (p_handle < 0)   Warning("读取点号失败 ", 1) ;
	return p_handle  ;
}

int PrintPt()
{
	char ftoa[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;	/* 11 Byte string buff (include \0) */

	sprintf(ftoa, "%.3f", point.x) ;
	PrintXY(59, 17, (unsigned char *)"        ", 0) ;
	PrintXY(113 - strlen(ftoa) * 6, 17, (unsigned char *)ftoa, 0) ;

	memset(ftoa, '\0', 11) ;  
	sprintf(ftoa, "%.3f", point.y) ;
	PrintXY(59, 31, (unsigned char *)"        ", 0) ;
	PrintXY(113 - strlen(ftoa) * 6, 31, (unsigned char *)ftoa, 0) ;

	memset(ftoa, '\0', 11) ;  
	sprintf(ftoa, "%.3f", point.z) ;
	PrintXY(59, 45, (unsigned char *)"        ", 0) ;	
	PrintXY(113 - strlen(ftoa) * 6, 45, (unsigned char *)ftoa, 0) ;
	return ;
}

