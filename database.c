/*
	数据库 Database
	Last update: 2012.5.4
*/

# include "9860g.h"

static jmp_buf env ;
int assume(int condition, int flag)
{
	if (condition == EXIT_F) longjmp(env,1) ;
	else return 0;
}

int Database(void)
{
	unsigned int key ;
	unsigned int page = 1 ;

do {
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */

	
	Print_zh("　数据　", 5, 2, VERT_REV) ;
	PrintXY(113, 5, (unsigned char*)"\xE6\x9C ", 0) ;
	PrintXY(113, 50, (unsigned char*)"\xE6\x9D ", 0) ;

	if (page == 1 ) {
		locatestr(28,2) ;	printf("[1]平曲线F1") ;
		locatestr(28,16) ;	printf("[2]竖曲线F2") ;
		locatestr(28,30) ;	printf("[3]控制点F3") ;
		locatestr(28,44) ;	printf("[4]批量点F4") ;
	}
	else if (page == 2 ) {
		locatestr(28,2) ;	printf("[5]线元法F5") ;
		locatestr(28,16) ;	printf("[6]路幅库F6") ;
		locatestr(28,30) ;	printf("[7]隧道项N7") ;
		locatestr(28,44) ;	printf("[8]锁定表N8 ") ;
	}
	GetKey(&key) ;
	switch (key) {
	case KEY_CTRL_F1:case KEY_CHAR_1:
		pqx_data() ;
		break ;
	case KEY_CTRL_F2:case KEY_CHAR_2:
		sqx_data() ;
		break ;
	case KEY_CTRL_F3:case KEY_CHAR_3:
		kzd_data();
		break ;
	case KEY_CTRL_F4:case KEY_CHAR_4:
		pld_data() ;
		break ;
	case KEY_CTRL_F5:case KEY_CHAR_5:
		xyf_data() ;
		break ;
	case KEY_CTRL_F6:case KEY_CHAR_6:
		LF_data() ;
		break ;
	case KEY_CHAR_7:
		SD_data() ;
		break ;
	case KEY_CHAR_8:
		Checksum() ;
		break ;
	case KEY_CTRL_UP:
		if (page == 2) page = 1;
		break ;
	case KEY_CTRL_DOWN:
		if (page == 1) page = 2 ;
		break ;
	default:
		break ;
	}
}while (key != KEY_CTRL_EXIT) ;
	return ;
}

int Lockfile(char* sht_name,BOOL update)
{
	char flags_0 ;	/*  Used for main memory syscall   */
	int item_ptr ;
	int data_length;
	int result ; 
	int handle ;
	int loghandle ;
	unsigned char MDcp[16];
	char hex_MDcp[16*2 + 1];
	int di ;
	unsigned char MD5result[16];
	int ccret ;

	result = MCS_SearchDirectoryItem( (unsigned char*)"@SSHEET", (unsigned char*)sht_name, &flags_0, &item_ptr, &handle, &data_length ) ;
	if (result == 0) {
		if (isLocked(sht_name)){
			loghandle = OpenMD5log(sht_name) ;
			if (loghandle >= 0){
				Bfile_ReadFile(loghandle,MDcp,16,0) ;
				Bfile_CloseFile(loghandle) ;

				for (di = 0; di < 16; ++di)
	   				 sprintf(hex_MDcp + di * 2, "%02x", MDcp[di]);

				ccret = MD5(handle,data_length,hex_MDcp,MD5result) ;
			} else ccret = MD5(handle,data_length,"",MD5result) ;
		} else ccret = MD5(handle,data_length,"",MD5result) ;
		if (ccret == 0 ) ;//do nothing
		else {
			
			if (update == TRUE) {
				if ( DebugS("是否锁定?\nF1-是  F6-否 ") == TRUE){
					if (CreateMD5log(sht_name) == 0){
						loghandle = OpenMD5log(sht_name) ;
						if (loghandle >= 0){
							Bfile_WriteFile(loghandle,MD5result,16) ;
							Bfile_CloseFile(loghandle) ;
						}
					} else DebugS("写入校验失败 ") ;
				}
			} else DebugS("表格%s\n发生变化! ",sht_name) ;
		}
		
	} else {DebugS("打开%s失败 ", sht_name) ;  return -1 ; }
	return result ;
}

int isLocked(char* sht_name)
{
	int test ;
	BOOL flag = FALSE ;

	
	test = OpenMD5log(sht_name) ;

	if (test >= 0){
		flag = TRUE ;
		Bfile_CloseFile(test);
	} else {
		flag = FALSE ;
	}
	return flag ;
}

int OpenMD5log(char* sht_name)
{
	BYTE char_p ;
	FONTCHARACTER pfile[30] ;
	char fname_buf[30] ;

	memset(fname_buf,'\0', 30);  
	# ifdef PC_TEST
	strcat(fname_buf, (char*)"\\\\crd0\\MD5\\") ;	// Parent folder
	# else 
	strcat(fname_buf, (char*)"\\\\fls0\\MD5\\") ;	// Parent folder
	# endif
	strcat(fname_buf, sht_name) ;	
	strcat(fname_buf, (char*)".MD5") ;		

	for (char_p = 0 ; char_p <= strlen(fname_buf) ; ++char_p) 
		pfile[char_p] = (WORD)fname_buf[char_p] ;	
	return Bfile_OpenFile(pfile, _OPENMODE_READ) ; 
}

int CreateMD5log(char* sht_name)
{
	BYTE char_p ;
	FONTCHARACTER pfile[30] ;
	char fname_buf[30] ;

	memset(fname_buf,'\0', 30);  
	# ifdef PC_TEST
	strcat(fname_buf, (char*)"\\\\crd0\\MD5\\") ;	// Parent folder
	# else 
	strcat(fname_buf, (char*)"\\\\fls0\\MD5\\") ;	// Parent folder
	# endif
	strcat(fname_buf, sht_name) ;	
	strcat(fname_buf, (char*)".MD5") ;		

	for (char_p = 0 ; char_p <= strlen(fname_buf) ; ++char_p) 
		pfile[char_p] = (WORD)fname_buf[char_p] ;	

	/* 重建文件 */
	Bfile_DeleteFile(pfile) ;
	return  Bfile_CreateFile(pfile, 16) ; 
}

/* S.H.I.E.L.D 表格文件完整性检查 */

int Checksum(void)
{
	unsigned int key = 0 ;

	do {
		Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
		Print_zh("锁定", 2, 0, VERT_REV) ;

		locatestr(25,1) ;
		if (qxys_flag) {
			if (isLocked(dbset.current_pq)) printf("F1-平曲线:%s*",dbset.current_pq);
			else printf("F1-平曲线:%s",dbset.current_pq);
		} else printf("F1-平曲线:空 ") ;
		locatestr(25,14) ;
		if (sqx_flag) {
			if (isLocked(dbset.current_sq)) printf("F2-竖曲线:%s*",dbset.current_sq);
			else printf("F2-竖曲线:%s",dbset.current_sq);
		} else printf("F2-竖曲线:空 ") ;	
		locatestr(25,27) ;
		if (xyf_flag) {
			if (isLocked(dbset.current_ys)) printf("F3-线元表:%s*",dbset.current_ys);
			else printf("F3-线元表:%s",dbset.current_ys);
		} else printf("F3-线元表:空 ") ;
		locatestr(25,40) ;
		if (sd_flag) {
			if (isLocked(dbset.current_sd)) printf("F4-隧道面:%s*",dbset.current_sd);
			else printf("F4-隧道面:%s",dbset.current_sd);
		} else printf("F4-隧道面:空 ") ;	

//		printf("F1-平曲线:%s%c\nF2-竖曲线:%s%c\nF3-线元表:%s%c\nF4-隧道面:%s%c ",
//		qxys_flag?dbset.current_pq:"无 ",isLocked(dbset.current_pq) ? '*' : ' ',
//		sqx_flag?dbset.current_sq:"无 ",isLocked(dbset.current_sq) ? '*' : ' ',
//		xyf_flag?dbset.current_ys:"无 ",isLocked(dbset.current_ys) ? '*' : ' ',
//		sd_flag?dbset.current_sd:"无 ",isLocked(dbset.current_sd) ? '*' : ' ') ;
 
		
		PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
		PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
		PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
		PrintMini(59, 58, (unsigned char*)" F4 ", MINI_REV) ; 
		PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			if (qxys_flag == TRUE) Lockfile(dbset.current_pq,TRUE);
			else Warning("无锁定目标 ",2) ;	
			break ;
		case KEY_CTRL_F2:
			if (sqx_flag == TRUE) Lockfile(dbset.current_sq,TRUE);
			else Warning("无锁定目标 ",2) ;	
			break ;
		case KEY_CTRL_F3:
			if (xyf_flag == TRUE) Lockfile(dbset.current_ys,TRUE);
			else Warning("无锁定目标 ",2) ;
			break ;
		case KEY_CTRL_F4:
			if (sd_flag == TRUE) Lockfile(dbset.current_sd,TRUE);
			else Warning("无锁定目标 ",2) ;
			break ;
		default:
			break ;
		}
	} while (key != KEY_CTRL_EXIT) ;
	return 0;
}


BYTE sd_flag = FALSE ;

int SD_data(void)
{
	unsigned int key = 0 ;

	do {
		Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
		Print_zh("　隧道　", 2, 0, VERT_REV) ;

		locatestr(22,22) ;
		printf("F1-载入 F2-校验 \nF3-查看 F4-帮助 ") ;
	
		PrintMini(112, 10, (unsigned char*)" AC", MINI_REV) ; 
		PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
		PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
		PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
		PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

		setjmp(env) ;
		locatestr(22,5) ;
		if (sd_flag == TRUE) 
			printf("使用中: %s",dbset.current_sd) ;
		else printf("使用中:  空 ") ;
		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			sd_flag = load_SD_data(sd_flag) ;
			break ;
		case KEY_CTRL_F2:
			if (sd_flag == TRUE) {
				yanzheng() ;
			}
			else DebugS("未载入断面文件 ") ;			
			break ;
		case KEY_CTRL_F3:
			if (sd_flag == TRUE) show_SD_data() ;
			else DebugS("未载入断面文件 ") ;
			break ;
		case KEY_CTRL_F4:
			DatabaseHelp(7) ;
			break ;
		case KEY_CTRL_AC :
			if (sd_flag == TRUE) {
				sd_flag = FALSE ;
				if ( Empty_SD() == 0) {FreeSD() ; Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	} while (key != KEY_CTRL_EXIT) ;
	return 0;
}

int Empty_SD(void)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"Current_SD = EMP") ;
	confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,NOW,TRUE) ;
	
	Bfile_CloseFile(confile_ptr) ;
	
	return 0 ;
}

#pragma inline (TestSD)
int  TestSD(void)
{
	return (SD_start_x == NULL || SD_start_y == NULL 
			  || SD_end_x == NULL || SD_end_y == NULL
		 	  || SD_circle_x == NULL || SD_circle_y == NULL
			  || SD_R == NULL) ? FALSE : TRUE ;
}

int FreeSD()
{
	if (SD_start_x != NULL)  { free(SD_start_x) ; SD_start_x = NULL ; }
	if (SD_start_y != NULL)  { free(SD_start_y) ; SD_start_y = NULL ; }
	if (SD_end_x != NULL)  { free(SD_end_x) ; SD_end_x = NULL ; }
	if (SD_end_y != NULL)  { free(SD_end_y) ; SD_end_y = NULL ; }	
	if (SD_circle_x != NULL)  { free(SD_circle_x) ; SD_circle_x = NULL ; }	
	if (SD_circle_y != NULL)  { free(SD_circle_y) ; SD_circle_y = NULL ; }	
	if (SD_R != NULL)  { free(SD_R) ; SD_R = NULL ; }		
	return ;
}

/* 隧道要素 */
int SD_size = 0;
double* SD_start_x = NULL ;	/*起点坐标X*/
double* SD_start_y = NULL ;	/*起点坐标Y*/
double* SD_end_x = NULL ;	/*终点坐标X*/
double* SD_end_y = NULL ;	/*终点坐标Y*/
double* SD_circle_x = NULL ;/*圆心X*/
double* SD_circle_y = NULL ;/*圆心Y*/
double* SD_R = NULL ;		/*半径*/

int LoadSD(BYTE flag)
{
	unsigned int key ;

	PopUpWin(3) ;
	Print_zh("文件名： ", 20, 22, 0) ;
	PrintXY(80, 25, (unsigned char *)dbset.current_sd, 0) ;
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE) {
		if (ipstr[0] == 0 && dbset.current_sd[0] != 0)  strncpy(ipstr, dbset.current_sd, 5) ;
		FreeSD() ;
		SD_size = Init_Sht(7) ;	/*  7列  */
		if (SD_size == -1) return FALSE ;
		/*
		数据格式：
		行1：线段的起点坐标X，Y，终点坐标X1,Y2，圆心的坐标X3,Y4，半径R
		行N：同上
		*/
		SD_start_x = ReadFloat(1,7,SD_size) ;
		SD_start_y = ReadFloat(2,7,SD_size) ;
		SD_end_x = ReadFloat(3,7,SD_size) ;
		SD_end_y = ReadFloat(4,7,SD_size) ;
		SD_circle_x = ReadFloat(5,7,SD_size) ;
		SD_circle_y = ReadFloat(6,7,SD_size) ;
		SD_R = ReadFloat(7,7,SD_size) ;
		
		if (TestSD() && yanzheng() == 0)	{ Warning("读取成功 ", 1) ; return TRUE ; }
		else	  Warning("读取失败 ", 3) ;
	} 
	else if (key == KEY_CTRL_EXIT) return EXIT_F ;

	return FALSE ;
}

int autoload_SD(BYTE flag)
{	

		strncpy(ipstr, dbset.current_sd, 5) ;
		FreeSD() ;
		SD_size = Init_Sht(7) ;	/*  7列  */
		if (SD_size == -1) return FALSE ;

		SD_start_x = ReadFloat(1,7,SD_size) ;
		SD_start_y = ReadFloat(2,7,SD_size) ;
		SD_end_x = ReadFloat(3,7,SD_size) ;
		SD_end_y = ReadFloat(4,7,SD_size) ;
		SD_circle_x = ReadFloat(5,7,SD_size) ;
		SD_circle_y = ReadFloat(6,7,SD_size) ;
		SD_R = ReadFloat(7,7,SD_size) ;

		if (TestSD())	{ sd_flag = TRUE ; return TRUE ; }
		else	  rtfail() ;

		return flag ;
}

int show_SD_data(void) 
{
	unsigned int step = 0 ;

	assume(DebugS("数据行数: %d",SD_size), EXIT_F) ;
	for (step = 0 ; step < SD_size ; ++step) {
		assume(DebugS("起点X=%.3f\n起点Y=%.3f",SD_start_x[step], SD_start_y[step]), EXIT_F) ;
		assume(DebugS("终点X=%.3f\n终点Y=%.3f",SD_end_x[step], SD_end_y[step]), EXIT_F) ;
		assume(DebugS("圆心X=%.3f\n圆心Y=%.3f",SD_circle_x[step], SD_circle_y[step]), EXIT_F) ;
		assume(DebugS("半径R=%.3f",SD_R[step]), EXIT_F) ;
	}

	return  ;
}

int load_SD_data(BYTE fflag)
{
	const DISPBOX clr = {20, 2, 120,54} ;
	unsigned int key = 0 ;
	BYTE flag = FALSE;
	char* backup_cur = NULL;


	Bdisp_AreaClr_DDVRAM(&clr) ;
	DrawRect(20,2,54,100);
	locatestr(22,2) ;
	printf("1.%4s 2.%4s\n3.%4s 4.%4s\n5.%4s 6.%4s\n7.%4s 8.%4s",dbset.sd1,dbset.sd2,dbset.sd3,dbset.sd4,dbset.sd5,dbset.sd6,dbset.sd7,dbset.sd8) ;

	GetKey(&key) ;
	backup_cur = dbset.current_sd ;
	switch (key)
	{
	case KEY_CHAR_1:
		dbset.current_sd = dbset.sd1 ;
		flag = LoadSD(flag) ;	
		break ;
	case KEY_CHAR_2:
		dbset.current_sd = dbset.sd2 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_3:
		dbset.current_sd = dbset.sd3 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_4:
		dbset.current_sd = dbset.sd4 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_5:
		dbset.current_sd = dbset.sd5 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_6:
		dbset.current_sd = dbset.sd6 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_7:
		dbset.current_sd = dbset.sd7 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CHAR_8:
		dbset.current_sd = dbset.sd8 ;
		flag = LoadSD(flag) ;
		break ;
	case KEY_CTRL_EXIT:
		dbset.current_sd = dbset.current_sd ;
		flag =  EXIT_F ;
	default:
		break ;
	}
	if (flag == TRUE) {
		Save_SD_data(key) ;
		return TRUE ;	
	} 
	else if (flag == EXIT_F) {
		dbset.current_sd = backup_cur ;
		return fflag ;
	}
	else return FALSE ;
}

int Save_SD_data(unsigned int key)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	switch (key)
	{
	case KEY_CHAR_1:
		strncpy(dbset.sd1,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD1") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD1 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd1, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_2:
		strncpy(dbset.sd2,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD2") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD2 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd2, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_3:
		strncpy(dbset.sd3,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD3") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD3 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd3, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_4:
		strncpy(dbset.sd4,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD4") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD4 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd4, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_5:
		strncpy(dbset.sd5,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD5") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD5 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd5, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_6:
		strncpy(dbset.sd6,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD6") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD6 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd6, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_7:
		strncpy(dbset.sd7,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD7") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD7 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd7, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_8:
		strncpy(dbset.sd8,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SD = SD8") ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.cusd, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SD8 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.sd_fpos.sd8, temp_buffer,NOW,FALSE) ;
		break ;
	default:
		break ;
	}
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

BYTE LF_flag = FALSE ;

int LF_data(void) 
{
	unsigned int key = 0 ;
	do
	{
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
	Print_zh("　路幅　", 2, 0, VERT_REV) ;

	locatestr(22,22) ;
	printf("F1-载入 F2-校验 \nF3-查看 F4-帮助 ") ;

	
	PrintMini(112, 10, (unsigned char*)" AC", MINI_REV) ; 
	PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
	PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
	PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
	PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 
	
	locatestr(22,5) ;
	if (LF_flag == TRUE) 
		printf("使用中: %s",dbset.current_hp) ;
	else 
		printf("使用中:  空 ") ;

		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			LF_flag = load_lf_data(LF_flag) ;
			break ;
		case KEY_CTRL_F2:
			if (LF_flag == TRUE) InputLFstake() ; 
			else DebugS("未载入数据 ") ;
			break ;
		case KEY_CTRL_F3:
			if (LF_flag == TRUE)  show_LF_data() ;
			else DebugS("未载入数据 ") ;
			break ;
		case KEY_CTRL_F4:
			DatabaseHelp(6) ;
			break ;
		case KEY_CTRL_AC :
			if (LF_flag == TRUE) {
				LF_flag = FALSE ;
				strncpy(ipstr, " EMP\0", 5) ;
				if ( Save_lf_data() == 0) { FreeLF() ; Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	}while (key != KEY_CTRL_EXIT) ;	

	return ;
}

int InputLFstake(void)
{
	unsigned int key ;
	BYTE exit_flag = FALSE ;
	unsigned int check_HP ;

	PopUpWin(3) ;
	Print_zh("桩号 ", 18, 20, 0) ;
	while (exit_flag != TRUE) {
		key = InputValP(48, 22) ;
		switch (key) {
		case KEY_CTRL_EXE:
			check_HP = LF_JS(number) ;
			DebugS("左幅宽 %.3f\n左横坡 %.2f%%",LF_L_Width[check_HP], LF_L_HP[check_HP]) ;
			DebugS("右幅宽 %.3f\n右横坡 %.2f%%",LF_R_Width[check_HP], LF_R_HP[check_HP]) ;	
			return TRUE ;
			break ;
		case KEY_CTRL_EXIT:
			exit_flag = TRUE ;
			break ;
		default:
			break ;
		}
	}
	return FALSE ;
}
	
unsigned int LF_JS(double stake)
{
	unsigned int step = 0 ;

	for (step = 0 ; step < LF_size ; ++step) {
		if (step == key_HP) continue ;
		else {
			if (stake >= LF_Start[step] && stake <= LF_End[step])
				return step ;
		}
	}
	return key_HP ;
}

int show_LF_data(void) 
{
	unsigned int step = 0 ;
	BYTE key_set = FALSE ;

	for (step = 0 ; step < LF_size ; ++step) {
		if (LF_Start[step] < 0 && LF_End[step] < 0 && key_set == FALSE ) {
			DebugS("通用设定 ") ;
			key_set = TRUE ;
		}
		else DebugS("起始 %.3f\n结束 %.3f",LF_Start[step], LF_End[step]) ;
		DebugS("左幅宽 %.3f\n左横坡 %.2f%%",LF_L_Width[step], LF_L_HP[step]) ;
		DebugS("右幅宽 %.3f\n右横坡 %.2f%%",LF_R_Width[step], LF_R_HP[step]) ;
	}

	return  ;
}

int LF_size = 0;
double* LF_L_Width = NULL ;
double* LF_L_HP = NULL ;
double* LF_R_Width = NULL ;
double* LF_R_HP = NULL ;
double* LF_Start = NULL ;
double* LF_End = NULL ;
unsigned int key_HP = 0 ;

int  TestLF(void)
{
	unsigned int step = 0 ;

	if  ( LF_L_Width != NULL && LF_L_HP != NULL && LF_R_Width != NULL && LF_R_HP != NULL
	      && LF_Start != NULL && LF_End != NULL)  {
		for (step = 0 ; step < LF_size ; ++step) {		//遍历得出通用路幅宽及横坡 
			if (LF_Start[step] < 0 && LF_End[step] < 0) {
				key_HP = step ;
				break ;
			}
		}
		return TRUE ;
	}
	else return FALSE ;
}

int load_lf_data(BYTE fflag)
{
	unsigned int key ;
	
	PopUpWin(3) ;
	Print_zh("文件名： ", 20, 22, 0) ;
	PrintXY(80, 25, (unsigned char *)dbset.current_hp, 0) ;
	
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE) {
		if (ipstr[0] == 0 && dbset.current_hp[0] != 0)  strncpy(ipstr, dbset.current_hp, 5) ;
		FreeLF() ;
		LF_size = Init_Sht(6) ;	//A~F列 
		if (LF_size == -1) return FALSE ;
		
		/* A~F列依次为 左宽，左横坡，右宽，右横坡，里程开始，里程结束（通用部分里程开始和结束输负数）*/
		LF_L_Width = ReadFloat(1, 6, LF_size) ;
		LF_L_HP = ReadFloat(2, 6, LF_size) ;
		LF_R_Width = ReadFloat(3, 6, LF_size) ;
		LF_R_HP = ReadFloat(4, 6, LF_size) ;
		LF_Start = ReadFloat(5, 6, LF_size) ;
		LF_End = ReadFloat(6, 6, LF_size) ;

		if (TestLF()) { 
			if ( Save_lf_data() == 0) {
				strncpy(dbset.current_hp, ipstr, 5) ;
				Warning("读取成功 ", 1) ; 
			}
			else Warning("请整理闪存 ", 1) ; 
			return TRUE ; 
		}
		else	  Warning("读取失败 ", 3) ;
	} 
	else if (key == KEY_CTRL_EXIT) return fflag ;
	return FALSE ;
}

int autoload_LF(BYTE flag)
{	

		strncpy(ipstr, dbset.current_hp, 5) ;
		FreeLF() ;
		LF_size = Init_Sht(6) ;	//A~F列 
		if (LF_size == -1) return FALSE ;
		
		LF_L_Width = ReadFloat(1, 6, LF_size) ;
		LF_L_HP = ReadFloat(2, 6, LF_size) ;
		LF_R_Width = ReadFloat(3, 6, LF_size) ;
		LF_R_HP = ReadFloat(4, 6, LF_size) ;
		LF_Start = ReadFloat(5, 6, LF_size) ;
		LF_End = ReadFloat(6, 6, LF_size) ;
		
		if (TestLF()) { LF_flag = TRUE ;return TRUE ; }
		else	  rtfail() ;
		return flag ;
}

int Save_lf_data(void)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"Current_HP = %4s",ipstr) ;
	confile_ptr = WriteINI(confile_ptr, dbset.hp_pos, temp_buffer,NOW,TRUE) ;
	
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

int FreeLF(void)
{
	if (LF_L_Width != NULL)  { free(LF_L_Width) ; LF_L_Width = NULL ; }
	if (LF_L_HP != NULL)  { free(LF_L_HP) ; LF_L_HP = NULL ; }
	if (LF_R_Width != NULL)  { free(LF_R_Width) ; LF_R_Width = NULL ; }
	if (LF_R_HP != NULL)  { free(LF_R_HP) ; LF_R_HP = NULL ; }
	if (LF_Start != NULL)  { free(LF_Start) ; LF_Start = NULL ; }
	if (LF_End != NULL)  { free(LF_End) ; LF_End = NULL ; }
	
	return ;
}

BYTE pld_flag = FALSE ;
char name[FILENAMEMAX] ;
FONTCHARACTER pld_FileName[50];
BYTE SDcard_F = FALSE ;
BYTE Fls_F = FALSE ;

int pld_data(void)
{
	char folder[FILENAMEMAX] = ""; 
	char filename_buf[50] ;

	unsigned int key = 0 ;
	int csvFile ;
	unsigned int csv_size = 0;

	do
	{	
		Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
		Print_zh("　批量　 ", 2, 0, VERT_REV) ;
		locatestr(22,22) ;
		printf("F1-载入 F2-校验  ") ;
		PrintMini(114, 1, (unsigned char*)" AC", MINI_REV) ;
		PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
		PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
		PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

		locatestr(22,5) ;
		if (pld_flag == TRUE)
			printf("使用中:%s",name) ;
		else
			printf("使用中:  空 ") ;
	
		GetKey(&key) ;
		switch (key){
		case KEY_CTRL_F1 :
			if (SelectRoot() == 0)
			{
			if(SelectFile( folder, name ) == 1){
				if( strlen(folder) == 0 ) {
					if (SDcard_F == TRUE)
					{
						sprintf( filename_buf, "\\\\crd0\\%s", name );
					}
					else if (Fls_F == TRUE)
					{
						sprintf( filename_buf, "\\\\fls0\\%s", name );
					}
				}
				else {
					if (SDcard_F == TRUE)
					{
						sprintf( filename_buf, "\\\\crd0\\%s\\%s", name );
					}
					else if (Fls_F == TRUE)
					{
						sprintf( filename_buf, "\\\\fls0\\%s\\%s", name );
					}
				}

				CharToFont(filename_buf,pld_FileName); 
				csvFile = Bfile_OpenFile(pld_FileName,_OPENMODE_READ_SHARE);
				if (csvFile >=0) {
					PopUpWin(3) ;
					Print_zh("索引建立中..", 28, 22, 0) ;
					//csv_size = Bfile_GetFileSize (csvFile);
					if (CreateIndex(csvFile) >=0){
						//Reopen the csv file ;
						Bfile_CloseFile(csvFile) ;
						csvFile = Bfile_OpenFile(pld_FileName,_OPENMODE_READ);
						
						if (BuildIndex(csvFile)==0) pld_flag = TRUE ;
					}
					else Warning("创建索引失败!",2) ;
					Bfile_CloseFile(csvFile) ;
				}
				else {Warning("打开文件失败",2) ; pld_flag = FALSE ;}
			}
			}
			break ;
		case KEY_CTRL_F2 : 
			if (pld_flag == TRUE) 	
				InputPT_num(Bfile_OpenFile(pld_FileName,_OPENMODE_READ_SHARE), FALSE) ;
					
			else DebugS("未加载csv文件 ") ;
			break ;
		case KEY_CTRL_AC :
			if (pld_flag == TRUE) {
				pld_flag = FALSE ;
				if ( Empty_pld() == 0) { Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	}while (key != KEY_CTRL_EXIT) ;
	return ;
}

int Empty_pld(void)
{
	int confile_ptr = -1 ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;
	
	WriteINI(confile_ptr, dbset.pld_pos, "Current_PLD = \\;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;",NOW,TRUE) ;
	Bfile_CloseFile(confile_ptr) ;

	return 0 ;
}
unsigned int ptlist_count ;
int InputPT_num(int csvFile, BOOL usePT)
{
FONTCHARACTER indexfile[] = { '\\','\\','f','l','s','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
FONTCHARACTER indexfile2[] = { '\\','\\','c','r','d','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;

	unsigned int key ;
	BYTE exit_flag = FALSE ;
	unsigned int PT = 1;
	int index_fptr = -1;
	DWORD index_pos = 0;

	PopUpWin(3) ;
	Print_zh("点号 ", 18, 20, 0) ;

	if (csvFile < 0) {
		Warning("打开文件失败",2) ;
		exit_flag = TRUE ;
	}

	while (exit_flag != TRUE) {
		locatestr(100,20) ; printf("%d",PT) ;
		key = InputValP(48, 22) ;
		switch (key) {
		case KEY_CTRL_EXE:
			PT = number ? number : PT ;
			if (PT < 1 || PT > ptlist_count) {
				DebugS("点号范围1~%d",ptlist_count) ;
				Bfile_CloseFile(csvFile) ;
				return FALSE ;
			}else {
				/*首先尝试SD卡，其次内置闪存*/
				index_fptr = Bfile_OpenFile(indexfile2, _OPENMODE_READWRITE_SHARE) ;
				if (index_fptr < 0)	index_fptr = Bfile_OpenFile(indexfile, _OPENMODE_READWRITE_SHARE) ;

				if (index_fptr >=0){
					Bfile_ReadFile(index_fptr, &index_pos, sizeof(DWORD), (PT - 1)*sizeof(DWORD)) ; 
					getPOSline(csvFile, index_pos) ;
					parseCSVfile((char*)Linebuffer, usePT) ;

					Bfile_CloseFile(index_fptr) ;	
					exit_flag = TRUE  ;	//Normal stop ;
				}
				else { DebugS("打开索引失败! ") ; exit_flag = TRUE ; }
			}
			exit_flag = TRUE ;
			break ;
		case KEY_CTRL_EXIT:
			//exit_flag = TRUE ;
			Bfile_CloseFile(csvFile) ;
			return FALSE ;
			break ;
		default: break ;
		}
	}
	Bfile_CloseFile(csvFile) ;
	return TRUE ;
}


int parseCSVfile(char* string,BOOL usePT)
{
	struct tkbox* info ;
	unsigned int step_count = 1 ;
	BOOL exit_flag = FALSE ;
	double check_x = 0 ;
	double check_y = 0 ;
	double check_cmt = 0 ; 	//备注

	begin = forward = string ;  /*  指向缓冲区  */
	while ((info = lexan_num()) && exit_flag != TRUE) {
		if (info->token == TOKEN_NUM){
			//数值大于10万切除大头
			if (info->value >= 100000) 
				info->value -= floor(floor(info->value)/100000) * 100000 ;
			switch(step_count){
			case 1:
				break ;
			case 2:
				if (usePT == TRUE)   x = info->value ;
				else  check_x = info->value ;
				break ;
			case 3:
				if (usePT == TRUE)   y = info->value ;
				else  check_y = info->value ;
				break ;
			case 4:
				break ;
			case 5:
				check_cmt = info->value ;
				if (usePT == TRUE)  DebugS("X= %.3f\nY= %.3f\n注:%.3f#", x , y,check_cmt) ;
				else DebugS("X= %.3f\nY= %.3f\n注:%.3f#", check_x , check_y,check_cmt) ;
				break ;
			default:
				break ;	
			}
			++step_count ;
		}
		else { DebugS("格式错误！\n存在非数字 ") ; exit_flag = TRUE; }
	}
	return ;
}
int BuildIndex(int file_pointer)
{
FONTCHARACTER indexfile[] = { '\\','\\','f','l','s','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
FONTCHARACTER indexfile2[] = { '\\','\\','c','r','d','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;

DWORD index_pos = 0;
int index_fptr = -1;
int confile_ptr = -1 ;
char temp_buffer[MAXLINE] ;
char temp_char[50] ;

	index_fptr = Bfile_OpenFile(indexfile2, _OPENMODE_READWRITE_SHARE) ;
	if (index_fptr < 0)
	{
		index_fptr = Bfile_OpenFile(indexfile, _OPENMODE_READWRITE_SHARE) ;
	}
	
	if (index_fptr >=0){
		while (getline(file_pointer)) {
			
			Bfile_WriteFile(index_fptr,&index_pos,sizeof(DWORD)) ;	
			index_pos += strlen(Linebuffer);
		} 
		Bfile_CloseFile(index_fptr) ;	

		confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
		if (confile_ptr < 0) return -1 ;
		sprintf(temp_buffer,"Current_PLD = %s;",FontToChar(pld_FileName,temp_char)) ;
		WriteINI(confile_ptr, dbset.pld_pos, (char*)temp_buffer,NOW,TRUE) ;
		Bfile_CloseFile(confile_ptr) ;

		return 0 ;	//Normal stop ;
	}
	else {
		DebugS("打开索引失败! ") ;
		return -1 ;
	}
}


int CreateIndex(int file_pointer)
{
FONTCHARACTER indexfile[] = { '\\','\\','f','l','s','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
FONTCHARACTER indexfile2[] = { '\\','\\','c','r','d','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
	int temp_flag ;

	ptlist_count = 0;

	while (getline(file_pointer))
		++ptlist_count ;
	// rebuild the index file 
	Bfile_DeleteFile(indexfile2) ;
	temp_flag = Bfile_CreateFile(indexfile2, ptlist_count * sizeof(DWORD)) ;
	if (temp_flag < 0 ){
		Bfile_DeleteFile(indexfile) ;
		temp_flag = Bfile_CreateFile(indexfile, ptlist_count * sizeof(DWORD)) ;
	}
	return temp_flag ;
}

int kzd_data(void)
{
	unsigned int key = 0 ;
	unsigned int back_key = 0 ;

	do {
		Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
		Print_zh("　控制　", 2, 0, VERT_REV) ;

		locatestr(22,22) ;
		printf("F1-输入 F2-查看 ") ;
	
		locatestr(22,5) ;
		printf("控制点库[POINT]") ;
	
		PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
		PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
		PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			back_key = key ;
			key = input_kzd() ;

			if (key == TRUE) {
				if (!CheckPoint(point)) { point.x = point.y = point.z = 0.0 ; }
				else {
					SavePoint(point) ;
					point.x = point.y = point.z = 0.0 ;				
				}	
			} else key = back_key ;
			break ;
		case KEY_CTRL_F2:
			show_kzd();
			break ;
		default:
			break ;
		}
	} while (key != KEY_CTRL_EXIT) ;
	return 0;	
}

int input_kzd(void)
{
	unsigned int temp_row = 0;
	int set ;

	Bdisp_AreaClr_DDVRAM(&clear_area) ;

	Print_zh("控制点 ", 22, 1, 0) ;
	PrintXY(25, 18, (unsigned char*)"X :", 0) ;
	PrintXY(25, 32, (unsigned char*)"Y :", 0) ;
	PrintXY(25, 46, (unsigned char*)"Z :", 0) ;
	point.x = point.y = point.z = 0.0 ;
	PrintPt() ;

	while (temp_row < 3 ) {
		set = InputVal(55, 15+temp_row*14) ;
		switch (temp_row) {
		case 0: if (number) point.x = number ; break ; 
		case 1: if (number) point.y = number ; break ; 
		case 2: if (number) point.z = number ; break ; 
		default: break ; 
		}
		switch (set) {
		case KEY_CTRL_UP:  
			if (temp_row >= 1)  --temp_row ;  break ;
		case KEY_CTRL_DOWN: case KEY_CTRL_EXE:
			++temp_row ;  break ;
		case KEY_CTRL_DEL:
			switch (temp_row) {
			case 0: point.x = 0.0 ; break ; 
			case 1: point.y = 0.0 ; break ; 
			case 2: point.z = 0.0 ; break ; 
			default: break ; 
			}  break ;
		case KEY_CTRL_EXIT:  
		case KEY_CTRL_F1: case KEY_CTRL_F2: 
		case KEY_CTRL_F3: case KEY_CTRL_F4:
		case KEY_CTRL_F5: case KEY_CTRL_F6: 
			return set ; break ;
		default:  break ;
		}		
	} 
	return TRUE ;
}


int show_kzd(void)
{
	unsigned int key = 0 ;
	int handle ;
	double kzd_x,kzd_y,kzd_z ;

	do
	{	
		handle = SelectPoint() ;
		
		if (handle >= 0) {
			Bfile_ReadFile(handle, &kzd_x, sizeof(double), 5) ; 
			Bfile_ReadFile(handle, &kzd_y, sizeof(double), 5+sizeof(double)) ; 
			Bfile_ReadFile(handle, &kzd_z, sizeof(double), 5+sizeof(double)*2) ; 
			Bfile_CloseFile(handle) ;
			Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
			//Print_zh("　控制　", 2, 0, VERT_REV) ;
			PrintXY(25, 10, (unsigned char*)"X :", 0) ;
			PrintXY(25, 24, (unsigned char*)"Y :", 0) ;
			PrintXY(25, 38, (unsigned char*)"Z :", 0) ;
			Print_C(108, 10, CB = kzd_x, 3) ;
			Print_C(108, 24, CA = kzd_y, 3);
			Print_C(108, 38, CB = kzd_z, 3);	
			
			PrintMini(105, 55, (unsigned char*)" EXE ", MINI_REV) ; 
			locatestr(25,48) ; printf("F2-放样 ") ; 

			GetKey(&key) ;
			if (key == KEY_CTRL_F2) {
				x = kzd_x ; y = kzd_y ;
				LoftingNor() ;
			}	
		}
		
	}while (handle >= 0) ;
	return ;
}

struct db_SET dbset = { NULL, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{0,0,0,0,0,0},
				NULL, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{0,0,0,0,0,0},
				0,
				NULL,
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 		
				{'\0','\0','\0','\0','\0'}, 	
				{'\0','\0','\0','\0','\0'}, 	
				{'\0','\0','\0','\0','\0'}, 
				{0,0,0,0,0,0,0,0,0},
				{'\0','\0','\0','\0','\0'}, 
				0,	
				NULL,
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 
				{'\0','\0','\0','\0','\0'}, 		
				{'\0','\0','\0','\0','\0'}, 	
				{'\0','\0','\0','\0','\0'}, 	
				{'\0','\0','\0','\0','\0'}, 
				{0,0,0,0,0,0,0,0,0}	
			         };

BYTE sqx_flag = FALSE ;


int sqx_data(void)
{
	unsigned int key = 0 ;
	



	do
	{
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
	Print_zh("　竖曲　", 2, 0, VERT_REV) ;

	locatestr(22,22) ;
	printf("F1-载入 F2-校验 \nF3-查看 F4-帮助 ") ;

	PrintMini(112, 10, (unsigned char*)" AC", MINI_REV) ;
	PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
	PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
	PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
	PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 
	
	setjmp(env) ;
	locatestr(22,5) ;
	if (sqx_flag == TRUE) 
		printf("使用中: %s",dbset.current_sq) ;
	else 
		printf("使用中:  空 ") ;

		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			sqx_flag = load_sqx_data(sqx_flag) ;
			break ;
		case KEY_CTRL_F2:
			if (sqx_flag == TRUE) {
				if (InputSQstake() == TRUE)
					DebugS("设计高程%.3f", DesignGC) ;
			}
			else DebugS("未载入竖曲线 ") ;
			break ;
		case KEY_CTRL_F3:
			if (sqx_flag == TRUE) {
			show_sqx_data() ;
			}
			else DebugS("未载入竖曲线 ") ;
			break ;
		case KEY_CTRL_F4:
			DatabaseHelp(2) ;
			break ;
		case KEY_CTRL_AC :
			if (sqx_flag == TRUE) {
				sqx_flag = FALSE ;
				if ( Empty_sqx() == 0) { FreeSqx() ; Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	}while (key != KEY_CTRL_EXIT) ;
	return ;
}

int Empty_sqx(void)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"Current_SQ = EMP") ;
	confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,NOW,TRUE) ;
	
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

double DesignGC = 0 ;
static double ipSQstake = 0.0 ;

int InputSQstake(void)
{
	unsigned int key ;
	BYTE exit_flag = FALSE ;

	PopUpWin(3) ;
	Print_zh("桩号 ", 18, 20, 0) ;
	while (exit_flag != TRUE) {
		if (ipSQstake >= 10000) Print_Float(53, 24, ipSQstake) ;
		else if (ipSQstake >= 1000) Print_Float(58, 24, ipSQstake) ;
		else if (ipSQstake >= 100) Print_Float(63, 24, ipSQstake) ;
		else if (ipSQstake >= 10) Print_Float(68, 24, ipSQstake) ;
		else Print_Float(73, 24, ipSQstake) ;
		key = InputValP(48, 22) ;
		switch (key) {
		case KEY_CTRL_EXE:
			ipSQstake = number ? number : ipSQstake;
			if (ipSQstake > sq_Stake[sqx_size - 1] || ipSQstake < sq_Stake[0]) {
				Print_Float(68, 47, sq_Stake[sqx_size - 1]) ;
				Warning("超出线路 ", 2) ;
				return FALSE ;
			}
			number = ipSQstake ;
			GC_JS(number) ;
			return TRUE ;
			break ;
		case KEY_CTRL_EXIT:
			exit_flag = TRUE ;
			break ;
		default: break ;
		}
	}
	return FALSE ;
}

int GC_JS(double ip) 
{
	unsigned int step ;
	double T, E, i1 ,i2 ; 	//切线，外偏距，斜坡率
	double fT,zT ;		//曲线起点 曲线终点
	double backT ;
	double AE4,AC4 ;

	backT = sq_Stake[0] ;
for (step = 1 ; step < sqx_size - 1 ; ++step){
	
	i1 = (sq_GC[step] - sq_GC[step - 1]) / (sq_Stake[step] - sq_Stake[step - 1] ) ;
	i2 = (sq_GC[step + 1] - sq_GC[step]) / (sq_Stake[step + 1] - sq_Stake[step]) ;

	T = fabs(i1 - i2) * (sq_R[step - 1] + 0.0000000000001) /2 ;
	E = T * T / (sq_R[step - 1]+0.0000000000001) / 2  ;
	fT = sq_Stake[step] - T ;
	zT = sq_Stake[step] + T ;

	if (ip >= backT && ip < zT){
		AE4 = (sq_Stake[step] > ip) ? -i1 : i2 ;
		AC4 = fabs(ip - sq_Stake[step]) ;
		if (sq_R[step-1] == 0 || AC4 > T) 	DesignGC = sq_GC[step] + AC4 * AE4 ;
		else 						DesignGC = sq_GC[step] + AC4 * AE4 - (i1 - i2) * (T - AC4)  * (T - AC4)/ ( 2 * sq_R[step-1] * fabs(i1 - i2) + 0.000000000000001);
		return TRUE ;
	}
	backT = zT ;
}	
	if (ip >= backT && ip <= sq_Stake[sqx_size - 1]) {
		step = sqx_size - 1 ;
		i1 = (sq_GC[step] - sq_GC[step - 1]) / (sq_Stake[step] - sq_Stake[step - 1]) ;
		//起点段
		AE4 = (sq_Stake[step] > ip) ? -i1 : i2 ;
		AC4 = fabs(ip - sq_Stake[step]) ;
		if (sq_R[step-1] == 0 || AC4 > T) 	DesignGC = sq_GC[step] + AC4 * AE4 ;
		else 						DesignGC = sq_GC[step] + AC4 * AE4 - (i1 - i2) * (T - AC4)  * (T - AC4)/ ( 2 * sq_R[step-1] * fabs(i1 - i2) + 0.000000000000001);
		return TRUE ;
	}
		;//终点段
	return ;
}

int show_sqx_data(void) 
{
	unsigned int step = 0 ;

	assume(DebugS("起点桩号 \n%.3f",sq_Stake[0]), EXIT_F) ;
	assume(DebugS("起点高程 \n%.3f",sq_GC[0]), EXIT_F) ;

	for (step = 1 ; step < sqx_size - 1; ++step){
		assume(DebugS("桩号 \n%.3f",sq_Stake[step]), EXIT_F) ;
		assume(DebugS("高程 \n%.3f",sq_GC[step]), EXIT_F) ;
		assume(DebugS("半径 \n%.3f",sq_R[step - 1]), EXIT_F) ;
	}
	assume(DebugS("终点桩号 \n%.3f",sq_Stake[sqx_size - 1]), EXIT_F) ;
	assume(DebugS("终点高程 \n%.3f",sq_GC[sqx_size - 1]), EXIT_F) ;


	return  ;
}

int load_sqx_data(BYTE fflag)
{
	const DISPBOX clr = {20, 6, 120,54} ;
	unsigned int key = 0 ;
	BYTE flag = FALSE;
	char* backup_cur ;


	Bdisp_AreaClr_DDVRAM(&clr) ;
	DrawRect(20,6,48,100);
	locatestr(22,8) ;
	printf("1.%4s 2.%4s\n3.%4s 4.%4s\n5.%4s 选编号 ",dbset.sq1,dbset.sq2,dbset.sq3,dbset.sq4,dbset.sq5) ;

	GetKey(&key) ;
	backup_cur = dbset.current_sq ;
	switch (key)
	{
	case KEY_CHAR_1:
		dbset.current_sq = dbset.sq1 ;
		flag = LoadSqx(flag) ;	
		break ;
	case KEY_CHAR_2:
		dbset.current_sq = dbset.sq2 ;
		flag = LoadSqx(flag) ;
		break ;
	case KEY_CHAR_3:
		dbset.current_sq = dbset.sq3 ;
		flag = LoadSqx(flag) ;
		break ;
	case KEY_CHAR_4:
		dbset.current_sq = dbset.sq4 ;
		flag = LoadSqx(flag) ;
		break ;
	case KEY_CHAR_5:
		dbset.current_sq = dbset.sq5 ;
		flag = LoadSqx(flag) ;
		break ;
	case KEY_CTRL_EXIT:
		flag =  EXIT_F ;
	default:
		break ;
	}
	if (flag == TRUE) {
		Save_sqx_data(key) ;
		return TRUE ;	
	}
	else if (flag == EXIT_F) {
		dbset.current_sq = backup_cur ;
		return fflag ;
	} 
	else return FALSE ;
}

int sqx_size = 0;
double* sq_Stake = NULL ;
double* sq_GC = NULL ;
double* sq_R = NULL ;

int LoadSqx(BYTE flag)
{
	unsigned int key ;
	
	PopUpWin(3) ;
	Print_zh("文件名： ", 20, 22, 0) ;
	PrintXY(80, 25, (unsigned char *)dbset.current_sq, 0) ;
	
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE) {
		if (ipstr[0] == 0 && dbset.current_sq[0] != 0)  strncpy(ipstr, dbset.current_sq, 5) ;
		FreeSqx() ;
		sqx_size = Init_Sht(3) ;	/*  包含起点数据  */
		if (sqx_size == -1) return FALSE ;
		//sqx_size += 2 ;	//包括起点及终点数据
		if (sqx_size <= 2) { Warning("路线不完整 ", 3) ; return FALSE ;}

		sq_Stake = ReadFloat(1, 3, sqx_size) ;
		sq_GC = ReadFloat(2, 3, sqx_size) ;
		sq_R = ReadFloat(3, 3, sqx_size - 2) ;	//起点及终点无半径	
		
		if (TestSqx())	{ Warning("读取成功 ", 1) ; return TRUE ; }
		else	  Warning("读取失败 ", 3) ;
	} 
	else if (key == KEY_CTRL_EXIT) return EXIT_F ;
	return FALSE ;
}
#pragma inline (TestSqx)
int  TestSqx(void)
{
	return ( sq_Stake != NULL && sq_GC != NULL && sq_R != NULL) ? TRUE : FALSE ;
}

int autoload_kzd()
{
		//补充计算此值
		total_set.azimuth =  atan2 ( (total_set.bak_point.y - total_set.stat_point.y) 
	      				  , (total_set.bak_point.x - total_set.stat_point.x) ) ;
}

int autoload_sqx(BYTE flag)
{	

		strncpy(ipstr, dbset.current_sq, 5) ;
		FreeSqx() ;
		sqx_size = Init_Sht(3) ;	/*  包含起点数据  */
		if (sqx_size == -1) return FALSE ;
		//sqx_size += 2 ;	//包括起点及终点数据
		if (sqx_size <= 2) { Warning("路线不完整 ", 3) ; return FALSE ;}

		sq_Stake = ReadFloat(1, 3, sqx_size) ;
		sq_GC = ReadFloat(2, 3, sqx_size) ;
		sq_R = ReadFloat(3, 3, sqx_size - 2) ;	//起点及终点无半径	

		
		if (TestSqx())	{  sqx_flag = TRUE ;return TRUE ; }
		else	  rtfail() ;
		return flag ;
}
int FreeSqx()
{
	if (sq_Stake != NULL)  { free(sq_Stake) ; sq_Stake = NULL ; }
	if (sq_GC != NULL)  { free(sq_GC) ; sq_GC = NULL ; }
	if (sq_R != NULL)  { free(sq_R) ; sq_R = NULL ; }
	
	return ;
}

int Save_sqx_data(unsigned int key)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	switch (key)
	{
	case KEY_CHAR_1:
		strncpy(dbset.sq1,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SQ = SQ1") ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SQ1 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.sq1, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_2:
		strncpy(dbset.sq2,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SQ = SQ2") ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SQ2 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.sq2, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_3:
		strncpy(dbset.sq3,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SQ = SQ3") ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SQ3 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.sq3, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_4:
		strncpy(dbset.sq4,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SQ = SQ4") ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SQ4 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.sq4, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_5:
		strncpy(dbset.sq5,ipstr, 5) ;
		sprintf(temp_buffer,"Current_SQ = SQ5") ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.cusq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"SQ5 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.s_fpos.sq5, temp_buffer,NOW,FALSE) ;
		break ;
	default:
		break ;
	}
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}






int pqx_data(void) 
{
	unsigned int key = 0 ;
	double backallset ;



	do
	{
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
	Print_zh("　平曲　", 2, 0, VERT_REV) ;

	locatestr(22,22) ;
	printf("F1-载入 F2-校验 \nF3-查看 F4-帮助 ") ;

	PrintMini(112, 10, (unsigned char*)" AC", MINI_REV) ; 
	PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
	PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
	PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
	PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 
	
	setjmp(env) ;
	locatestr(22,5) ;
	if (qxys_flag == TRUE) 
		printf("使用中: %s",dbset.current_pq) ;
	else 
		printf("使用中:  空 ") ;

		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			qxys_flag = load_pqx_data(qxys_flag) ;
			break ;
		case KEY_CTRL_F2:
			if (qxys_flag == TRUE) {
				backallset = allset.xl_type ;
				allset.xl_type = 1 ;
				if (InputStake(0) == TRUE) 
					DebugS("X = %.3f\nY = %.3f", x , y) ;
				allset.xl_type = backallset ;
			}
			else DebugS("未载入路线文件 ") ;
			break ;
		case KEY_CTRL_F3:
			if (qxys_flag == TRUE) {
			show_pqx_data() ;
			}
			else DebugS("未载入路线文件 ") ;
			break ;
		case KEY_CTRL_F4:
			DatabaseHelp(1) ;
			break ;
		case KEY_CTRL_AC :
			if (qxys_flag == TRUE) {
				qxys_flag = FALSE ;
				if ( Empty_pqx() == 0) {FreeQxys() ; Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	}while (key != KEY_CTRL_EXIT) ;
	return  ;
}

int Empty_pqx(void)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"Current_PQ = EMP") ;
	confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,NOW,TRUE) ;
	
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

int show_pqx_data(void) 
{
	unsigned int step = 0 ;
	double zh_x, zh_y, hy_x, hy_y, hz_x, hz_y ;
	double test ;
	
	//Zx_JS(qx_X[1], qx_Y[1], qx_angel[0] - PI, qx_Stake[1], qx_ZH[0], number) ;
	//转点X坐标，转点Y坐标，定向方位角，转点桩号，直缓桩号
	
	assume(DebugS("起点桩号 \n%.3f",qx_Stake[0]), EXIT_F) ;
	assume(DebugS("起点X=%.3f\n　　Y=%.3f",qx_X[0],qx_Y[0]), EXIT_F) ;

	for (step = 0 ; step < list_size - 1; ++step){
		assume(DebugS("交点 %d",step + 1), EXIT_F) ;
		/*  四大桩号坐标计算  */
		zh_x = qx_X[step + 1] + (qx_Stake[step + 1] - qx_ZH[step]) * cos(qx_angel[step] - PI) ;
		zh_y = qx_Y[step + 1] + (qx_Stake[step + 1] - qx_ZH[step]) * sin(qx_angel[step] - PI) ;
		hy_x = zh_x + X_C( (qx_HY[step] - qx_ZH[step]+ 0.0000000000001), qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001 )
		   	* cos(qx_angel[step] + qx_zy[step] 
			* ((qx_HY[step] - qx_ZH[step]) * (qx_HY[step] - qx_ZH[step])
			 / 2 / (qx_Ls1[step]  + 0.0000000000001)/ (qx_R[step] + 0.0000000000001)) / 3) ;
		hy_y = zh_y + X_C( (qx_HY[step] - qx_ZH[step]+ 0.0000000000001), qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001 )
		    	* sin(qx_angel[step] + qx_zy[step] 
			* ((qx_HY[step] - qx_ZH[step]) * (qx_HY[step] - qx_ZH[step])
			 / 2 / (qx_Ls1[step] + 0.0000000000001) / (qx_R[step] + 0.0000000000001)) / 3) ;
		hz_x = qx_X[step + 1] + qx_T1[step] * cos(qx_angel[step] + qx_zy[step] * qx_ZJ[step]) ;
		hz_y = qx_Y[step + 1] + qx_T1[step] * sin(qx_angel[step] + qx_zy[step] * qx_ZJ[step]) ;	


		assume(DebugS("前缓长 \n%g",qx_Ls1[step] + 0.0000000000001), EXIT_F) ;
		assume(DebugS("前切长 \n%g",qx_T1[step]), EXIT_F) ;	
		assume(DebugS("半径 \n%g",qx_R[step] + 0.0000000000001), EXIT_F) ;
		assume(DebugS("定向方位角 \n%g",qx_angel[step]), EXIT_F) ;
		assume(DebugS("曲线偏向 \n%g",qx_zy[step]), EXIT_F) ;

		//直缓坐标X，直缓坐标Y，定向方位角，直缓桩号，放样桩号，前缓长，半径，曲线偏向
		//No1_JS(zh_x, zh_y, qx_angel[step], qx_ZH[step], number, qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step] ) ;
		assume(DebugS("第一曲线 "), EXIT_F) ;
		assume(DebugS("直缓X=%.3f\n　　Y=%.3f",zh_x,zh_y), EXIT_F) ;
		assume(DebugS("直缓桩号 \n%g",qx_ZH[step]), EXIT_F) ;
		
		//Yuan_JS(hy_x, hy_y, qx_angel[step], qx_HY[step], number,  qx_ZH[step], qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step]) ;
		//缓圆坐标X，缓圆坐标Y，定向方位角，缓圆桩号，放样桩号，直缓桩号，前缓长，半径，曲线偏向
		assume(DebugS("圆曲线 "), EXIT_F) ;
		assume(DebugS("缓圆X=%.3f\n　　Y=%.3f",hy_x,hy_y), EXIT_F) ;
		assume(DebugS("缓圆桩号 \n%g",qx_HY[step]), EXIT_F) ;

		//No2_JS(hz_x, hz_y, qx_angel[step], qx_ZH[step], qx_HZ[step], number, qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step],  qx_ZJ[step]) ;
		//缓直坐标X，缓直坐标Y，定向方位角，直缓桩号，缓直桩号，放样桩号，前缓长，半径，曲线偏向，曲线偏角
		assume(DebugS("第二曲线 "), EXIT_F) ;
		assume(DebugS("缓直X=%.3f\n　　Y=%.3f",hz_x,hz_y), EXIT_F) ;
		assume(DebugS("缓直桩号 \n%g",qx_HZ[step]), EXIT_F) ;	
		assume(DebugS("曲线偏角 \n%g",qx_ZJ[step]), EXIT_F) ;

		//No2_ZX(qx_X[step + 1], qx_Y[step + 1], qx_angel[step], qx_HZ[step], number, qx_T1[step], qx_zy[step],  qx_ZJ[step] ) ;
		//转点X坐标，转点Y坐标，定向方位角，缓直桩号，放样桩号，前切长，半径，曲线偏向，曲线偏角
		assume(DebugS("第二直线 ") , EXIT_F);
		assume(DebugS("交点桩号 \n%.3f",qx_Stake[step + 1]), EXIT_F) ;
		assume(DebugS("转点X=%.3f\n　　Y=%.3f",qx_X[step + 1],qx_Y[step + 1]), EXIT_F) ;	
	}
	return ;
}

int rtfail(void) {
DebugS("读取数据库失败! ") ;
return FALSE ;
}

int autoload_pqx(BYTE flag)
{	

		strncpy(ipstr, dbset.current_pq, 5) ;
		FreeQxys() ;
		list_size = Init_Sht(22) ;	/*  包含起点数据  */
		if (list_size == -1) return rtfail() ;
		if (list_size <= 2) { Warning("路线不完整 ", 3) ; return rtfail() ;}
		qx_Stake = ReadFloat(2, 22, list_size) ;
		qx_X = ReadFloat(3, 22, list_size) ;
		qx_Y = ReadFloat(4, 22, list_size) ;
		qx_R = ReadFloat(5, 22, list_size - 2) ;	/*  减去起点及终点数据  */
		qx_Ls1 = ReadFloat(6, 22, list_size - 2) ;	
		qx_T1 = ReadFloat(12, 22, list_size - 2) ;
		qx_angel = ReadFloat(16, 22, list_size - 1) ;	/*  减去起点数据  */
		qx_zy = ReadFloat(17, 22, list_size - 1) ;	
		qx_ZJ = ReadFloat(18, 22, list_size - 1) ;	
		qx_ZH = ReadFloat(19, 22, list_size - 1) ;	
		qx_HY = ReadFloat(20, 22, list_size - 1) ;
		qx_YH = ReadFloat(21, 22, list_size - 1) ;
		qx_HZ = ReadFloat(22, 22, list_size - 1) ;
		
		if (TestQxzh())	{  qxys_flag = TRUE ;return TRUE ; }
		else	  rtfail() ;
		return flag ;
}

int load_pqx_data(BYTE fflag)
{
	const DISPBOX clr = {20, 6, 120,54} ;
	unsigned int key = 0 ;
	BYTE flag = FALSE;
	char* backup_cur ;

	Bdisp_AreaClr_DDVRAM(&clr) ;
	DrawRect(20,6,48,100);
	locatestr(22,8) ;
	printf("1.%4s 2.%4s\n3.%4s 4.%4s\n5.%4s 选编号 ",dbset.pq1,dbset.pq2,dbset.pq3,dbset.pq4,dbset.pq5) ;

	GetKey(&key) ;
	backup_cur = dbset.current_pq ;
	switch (key)
	{
	case KEY_CHAR_1:
		dbset.current_pq = dbset.pq1 ;
		flag = LoadQxys(flag) ;	
		break ;
	case KEY_CHAR_2:
		dbset.current_pq = dbset.pq2 ;
		flag = LoadQxys(flag) ;
		break ;
	case KEY_CHAR_3:
		dbset.current_pq = dbset.pq3 ;
		flag = LoadQxys(flag) ;
		break ;
	case KEY_CHAR_4:
		dbset.current_pq = dbset.pq4 ;
		flag = LoadQxys(flag) ;
		break ;
	case KEY_CHAR_5:
		dbset.current_pq = dbset.pq5 ;
		flag = LoadQxys(flag) ;
		break ;
	case KEY_CTRL_EXIT:
		dbset.current_pq = dbset.current_pq ;
		flag =  EXIT_F ;
	default:
		break ;
	}
	if (flag == TRUE) {
		Save_pqx_data(key) ;
		return TRUE ;	
	} 
	else if (flag == EXIT_F) {
		dbset.current_pq = backup_cur ;
		return fflag ;
	}
	else return FALSE ;
}

int Save_pqx_data(unsigned int key)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	switch (key)
	{
	case KEY_CHAR_1:
		strncpy(dbset.pq1,ipstr, 5) ;
		sprintf(temp_buffer,"Current_PQ = PQ1") ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"PQ1 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.pq1, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_2:
		strncpy(dbset.pq2,ipstr, 5) ;
		sprintf(temp_buffer,"Current_PQ = PQ2") ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"PQ2 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.pq2, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_3:
		strncpy(dbset.pq3,ipstr, 5) ;
		sprintf(temp_buffer,"Current_PQ = PQ3") ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"PQ3 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.pq3, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_4:
		strncpy(dbset.pq4,ipstr, 5) ;
		sprintf(temp_buffer,"Current_PQ = PQ4") ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"PQ4 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.pq4, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_5:
		strncpy(dbset.pq5,ipstr, 5) ;
		sprintf(temp_buffer,"Current_PQ = PQ5") ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.cupq, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"PQ5 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.fpos.pq5, temp_buffer,NOW,FALSE) ;
		break ;
	default:
		break ;
	}
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

BYTE xyf_flag = FALSE ;

int xyf_data(void)
{
	unsigned int key = 0 ;
	double backallset ;

	do {
		Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
		Print_zh("　线元　", 2, 0, VERT_REV) ;

		locatestr(22,22) ;
		printf("F1-载入 F2-校验 \nF3-查看 F4-帮助 ") ;
	
		PrintMini(112, 10, (unsigned char*)" AC", MINI_REV) ; 
		PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
		PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
		PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
		PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

		setjmp(env) ;
		locatestr(22,5) ;
		if (xyf_flag == TRUE) 
			printf("使用中: %s",dbset.current_ys) ;
		else 
		printf("使用中:  空 ") ;
		GetKey(&key) ;
		switch (key)
		{
		case KEY_CTRL_F1:
			xyf_flag = load_xyf_data(xyf_flag) ;
			break ;
		case KEY_CTRL_F2:
			if (xyf_flag == TRUE) {
				backallset = allset.xl_type ;
				allset.xl_type = 2 ;
				if (InputStake(0) == TRUE) 
					DebugS("X = %.3f\nY = %.3f", x , y) ;
				allset.xl_type = backallset ;
			}
			else DebugS("未载入路线文件 ") ;			
			break ;
		case KEY_CTRL_F3:
			if (xyf_flag == TRUE) show_xyf_data() ;
			else DebugS("未载入线元要素 ") ;
			break ;
		case KEY_CTRL_F4:
			DatabaseHelp(5) ;
			break ;
		case KEY_CTRL_AC :
			if (xyf_flag == TRUE) {
				xyf_flag = FALSE ;
				if ( Empty_xyf() == 0) {FreeXyys() ; Warning("重置成功!",1) ;}
				else   Warning("重置失败!",1) ;
			}
			break ;
		default:
			break ;
		}
	} while (key != KEY_CTRL_EXIT) ;
	return 0;
}

int Empty_xyf(void)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"Current_YS = EMP") ;
	confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,NOW,TRUE) ;
	
	Bfile_CloseFile(confile_ptr) ;
	
	return 0 ;
}

int load_xyf_data(BYTE fflag)
{
	const DISPBOX clr = {20, 2, 120,54} ;
	unsigned int key = 0 ;
	BYTE flag = FALSE;
	char* backup_cur ;

	Bdisp_AreaClr_DDVRAM(&clr) ;
	DrawRect(20,2,54,100);
	locatestr(22,2) ;
	printf("1.%4s 2.%4s\n3.%4s 4.%4s\n5.%4s 6.%4s\n7.%4s 8.%4s",dbset.ys1,dbset.ys2,dbset.ys3,dbset.ys4,dbset.ys5,dbset.ys6,dbset.ys7,dbset.ys8) ;

	GetKey(&key) ;
	backup_cur = dbset.current_ys ;

	switch (key)
	{
	case KEY_CHAR_1:
		dbset.current_ys = dbset.ys1 ;
		flag = LoadXyys(flag) ;	
		break ;
	case KEY_CHAR_2:
		dbset.current_ys = dbset.ys2 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_3:
		dbset.current_ys = dbset.ys3 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_4:
		dbset.current_ys = dbset.ys4 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_5:
		dbset.current_ys = dbset.ys5 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_6:
		dbset.current_ys = dbset.ys6 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_7:
		dbset.current_ys = dbset.ys7 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CHAR_8:
		dbset.current_ys = dbset.ys8 ;
		flag = LoadXyys(flag) ;
		break ;
	case KEY_CTRL_EXIT:
		dbset.current_ys = dbset.current_ys ;
		flag =  EXIT_F ;
	default:
		break ;
	}
	if (flag == TRUE) {
		Save_xyf_data(key) ;
		return TRUE ;	
	} 
	else if (flag == EXIT_F){
		dbset.current_ys = backup_cur ;
		return fflag ;
	} 
	else return FALSE ;
}


/* 线元要素 */
int xyys_size = 0;
double* xy_zd_stake = NULL ;	/*线元终点里程*/
double* xy_zd_R = NULL ;		/*线元终点半径*/
double* xy_zd_X = NULL ;
double* xy_zd_Y = NULL ;
double* xy_zd_angle = NULL ;

#pragma inline (TestXyys)
int  TestXyys(void)
{
	return ( xy_zd_stake != NULL && xy_zd_R != NULL && xy_zd_X != NULL && xy_zd_Y != NULL && xy_zd_angle != NULL) ? TRUE : FALSE ;
}

int LoadXyys(BYTE flag)
{
	unsigned int key ;
	double* xy_temp_X = NULL ;
	double* xy_temp_Y = NULL ;
	double* xy_temp_angle = NULL ;

	PopUpWin(3) ;
	Print_zh("文件名： ", 20, 22, 0) ;
	PrintXY(80, 25, (unsigned char *)dbset.current_ys, 0) ;
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE) {
		if (ipstr[0] == 0 && dbset.current_ys[0] != 0)  strncpy(ipstr, dbset.current_ys, 5) ;
		FreeXyys() ;
		xyys_size = Init_Sht(5) ;	/*  5列  */
		if (xyys_size == -1) return FALSE ;
		if (xyys_size <= 1) { Warning("路线不完整 ", 3) ; return FALSE ;}
		/*
		数据格式：
		行1：起始桩号，起始半径，起始X，起始Y,起始方位角
		行2：终点桩号，终点半径
		行N：终点桩号，终点半径
		*/
		xy_zd_stake = ReadFloat(1,5,xyys_size) ;
		xy_zd_R = ReadFloat(2,5,xyys_size) ;
		xy_temp_X = ReadFloat(3,5,1) ;		/*  起点数据只读一行 */
		xy_temp_Y = ReadFloat(4,5,1) ;
		xy_temp_angle = ReadFloat(5,5,1) ;
		
		xy_zd_X = (double*) calloc(xyys_size, sizeof(double)) ;
		xy_zd_Y = (double*) calloc(xyys_size, sizeof(double)) ;
		xy_zd_angle = (double*) calloc(xyys_size, sizeof(double)) ;

		if (xy_temp_X != NULL && xy_temp_Y  != NULL && xy_temp_angle != NULL
		&& xy_zd_X!= NULL &&  xy_zd_Y  != NULL && xy_zd_angle != NULL){
			xy_zd_X[0] = xy_temp_X[0] ;
			xy_zd_Y[0] = xy_temp_Y[0] ;
			xy_zd_angle[0] = xy_temp_angle[0] ;
		} else	{
		 	Warning("内存不足 ", 2) ;
			return FALSE ;
		}
		free(xy_temp_X) ;free(xy_temp_Y) ;free(xy_temp_angle) ;
		if (TestXyys())	{ Xy_cal() ; Warning("读取成功 ", 1) ; return TRUE ; }
		else	  Warning("读取失败 ", 3) ;
	} 
	else if (key == KEY_CTRL_EXIT) return EXIT_F ;

	return FALSE ;
}

int autoload_xyys(BYTE flag)
{	
		double* xy_temp_X = NULL ;
		double* xy_temp_Y = NULL ;
		double* xy_temp_angle = NULL ;

		strncpy(ipstr, dbset.current_ys, 5) ;
		FreeXyys() ;
		xyys_size = Init_Sht(5) ;	/*  5列  */
		if (xyys_size == -1) return FALSE ;
		if (xyys_size <= 1) { Warning("路线不完整 ", 3) ; return FALSE ;}

		xy_zd_stake = ReadFloat(1,5,xyys_size) ;
		xy_zd_R = ReadFloat(2,5,xyys_size) ;
		xy_temp_X = ReadFloat(3,5,1) ;		/*  起点数据只读一行 */
		xy_temp_Y = ReadFloat(4,5,1) ;
		xy_temp_angle = ReadFloat(5,5,1) ;
		
		xy_zd_X = (double*) calloc(xyys_size, sizeof(double)) ;
		xy_zd_Y = (double*) calloc(xyys_size, sizeof(double)) ;
		xy_zd_angle = (double*) calloc(xyys_size, sizeof(double)) ;

		if (xy_temp_X != NULL && xy_temp_Y  != NULL && xy_temp_angle != NULL
		&& xy_zd_X!= NULL &&  xy_zd_Y  != NULL && xy_zd_angle != NULL){
			xy_zd_X[0] = xy_temp_X[0] ;
			xy_zd_Y[0] = xy_temp_Y[0] ;
			xy_zd_angle[0] = xy_temp_angle[0] ;
		} else	{
		 	Warning("内存不足 ", 2) ;
			return FALSE ;
		}
		free(xy_temp_X) ;free(xy_temp_Y) ;free(xy_temp_angle) ;
		if (TestXyys())	{ Xy_cal() ; xyf_flag = TRUE  ; return TRUE ; }
		else	  rtfail() ;

		return flag ;
}

/* 线元要素计算补全 */
int Xy_cal(void)
{
	unsigned int  step ;
	struct CurSec Sec ;	
		
	for (step = 1 ; step < xyys_size ; ++step){
		Sec.START_Sta.a = Sixty2ten(xy_zd_angle[step-1]) * PI / 180 ;
		Sec.START_Sta.Ro = xy_zd_R[step-1] ? 1 / xy_zd_R[step-1] : 0  ;
		Sec.END_Sta.Ro = xy_zd_R[step] ? 1 / xy_zd_R[step] : 0 ;

		Sec.START_Sta.Mile = xy_zd_stake[step-1] ;
		Sec.END_Sta.Mile = xy_zd_stake[step] ;

		Sec.START_Sta.x = xy_zd_X [step-1] ;
		Sec.START_Sta.y = xy_zd_Y [step-1] ;

		Sta.Mile = Sec.END_Sta.Mile ;
		GetStaCoor(Sec) ;
		
		//ouput result 	
		xy_zd_X[step] = Sta.x ;
		xy_zd_Y[step] = Sta.y ;
		xy_zd_angle[step] = Ten2sixty(Sta.a*180/PI) ;
		if (xy_zd_angle[step] < 0 ) xy_zd_angle[step] += 360  ;
		if (xy_zd_angle[step] >= 360 ) xy_zd_angle[step] -= 360 ;
	}
	return ;
}

int FreeXyys()
{
	if (xy_zd_stake != NULL)  { free(xy_zd_stake) ; xy_zd_stake = NULL ; }
	if (xy_zd_R != NULL)  { free(xy_zd_R) ; xy_zd_R = NULL ; }
	if (xy_zd_X != NULL)  { free(xy_zd_X) ; xy_zd_X = NULL ; }
	if (xy_zd_Y != NULL)  { free(xy_zd_Y) ; xy_zd_Y = NULL ; }	
	if (xy_zd_angle != NULL)  { free(xy_zd_angle) ; xy_zd_angle = NULL ; }	
	return ;
}


int show_xyf_data(void)
{
	unsigned int step = 0 ;

	for (step = 0  ; step < xyys_size ; ++step)
	{
		assume(DebugS("桩号 %.3f\n半径 %g",xy_zd_stake[step], xy_zd_R[step]), EXIT_F) ;
		assume(DebugS("X=%.3f\nY=%.3f",xy_zd_X[step], xy_zd_Y[step]), EXIT_F) ;
		assume(DebugS("方位角\n%g",xy_zd_angle[step]), EXIT_F) ;	
	}
	return ;
}

int Save_xyf_data(unsigned int key)
{
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	switch (key)
	{
	case KEY_CHAR_1:
		strncpy(dbset.ys1,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS1") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS1 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys1, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_2:
		strncpy(dbset.ys2,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS2") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS2 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys2, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_3:
		strncpy(dbset.ys3,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS3") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS3 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys3, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_4:
		strncpy(dbset.ys4,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS4") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS4 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys4, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_5:
		strncpy(dbset.ys5,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS5") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS5 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys5, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_6:
		strncpy(dbset.ys6,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS6") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS6 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys6, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_7:
		strncpy(dbset.ys7,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS7") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS7 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys7, temp_buffer,NOW,FALSE) ;
		break ;
	case KEY_CHAR_8:
		strncpy(dbset.ys8,ipstr, 5) ;
		sprintf(temp_buffer,"Current_YS = YS8") ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.cuys, temp_buffer,LATER,TRUE) ;
		sprintf(temp_buffer,"YS8 = %4s",ipstr) ;
		confile_ptr = WriteINI(confile_ptr, dbset.ys_fpos.ys8, temp_buffer,NOW,FALSE) ;
		break ;
	default:
		break ;
	}
	Bfile_CloseFile(confile_ptr) ;
	return 0 ;
}

int DatabaseHelp(int helplist)
{
	unsigned int key ;

	SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	

	switch (helplist) {
	case 1:
		Print_zh("空,起始里程,X,Y", 5, 5, 0) ;
		Print_zh("2空,X,Y,R,Ls1,Ls2", 5, 19, 0) ;
		Print_zh("... ", 5, 30, 0) ;
		Print_zh("空,空,X,Y ", 5, 44, 0) ;
		break ;
	case 2:
		Print_zh("起点里程，高程 ", 5, 5, 0) ;
		Print_zh("里程1，高程，半径 ", 5, 19, 0) ;
		Print_zh("... ", 5, 30, 0) ;
		Print_zh("终点里程，高程 ", 5, 44, 0) ;
		break ;
	case 5:
		Print_zh("里程,R,X,Y,方位角 ", 5, 5, 0) ;
		Print_zh("里程1,半径R", 5, 19, 0) ;
		Print_zh("... ", 5, 30, 0) ;
		Print_zh("里程N,半径R", 5, 44, 0) ;
		break ;
	case 6:
		Print_zh("左宽,左横坡,\n右宽,右横坡,\n起始里程,结束里程", 5, 5, 0) ;
		Print_zh("...", 5, 44, 0) ;
		break ;
	case 7:
		Print_zh("起点X1,Y1,", 5, 5, 0) ;
		Print_zh("终点X2,Y2,", 5, 19, 0) ;
		Print_zh("圆心X3,Y3,半径R", 5, 33, 0) ;
		Print_zh("... ", 5, 44, 0) ;
		break ;
	default:
		break ;
	}

	GetKey(&key) ;
	RestoreDisp(SAVEDISP_PAGE2) ;
	Bdisp_PutDisp_DD() ;
	return ;
}
