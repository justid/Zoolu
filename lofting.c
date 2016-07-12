/*
	施工放样 Lofting
	Last update: 2010.5.04
*/


# include "9860g.h"




#pragma inline (TestQxzh)
extern int TestQxzh(void) ;
/*  曲线要素 :坐标，半径，前缓长，前切长，方位角，曲线偏向 , 曲线转角 & 
    桩号：直缓、缓圆，圆缓，缓直  */
double* qx_ZH = NULL ;
double* qx_HY = NULL ;
double* qx_YH = NULL ;
double* qx_HZ = NULL ;
double* qx_Stake = NULL ;
double* qx_X = NULL ;
double* qx_Y = NULL ;
double* qx_R = NULL ;
double* qx_Ls1 = NULL ;
double* qx_T1 = NULL ;
double* qx_angel = NULL ;
double* qx_zy = NULL ;
double* qx_ZJ = NULL ;
/*  记得释放分配的内存  */

double x = 0.0 ;
double y = 0.0 ;
BYTE qxys_flag = FALSE ;

static double qiexian_an = 0.0 ;/*  切线角  */
static int xl_pos = XD_ERROR ;

static double ipstake = 0.0 ;
/* 一般线型放样 curve data*/
int LoftingNor(void)
{
	
	unsigned int key = 0 ;
	unsigned int back_key ;
	BYTE exit_flag = FALSE ;
	
	BYTE xyrow = 0 ;
	double final_x =0.0 ;
	double final_y =0.0 ;
	double distance = 0.0 ;	/* 需求距离 */
	double bz_dist = 0.0 ;	/*  边桩距离及交角  */
	double S = 0.0 ;	/*  正算边长  */
	double a = 0.0 ;	/*  正算方位角  */
	double final_a = 0.0 ;
	const DISPBOX area = {65, 47, 125, 55} ;

	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */
	ipstake = 0 ; //放样里程归零
	Print_zh("　放样　", 5, 0, VERT_REV) ;

	/* Menu F1, F2, F3:... */
	PrintMini(5, 58, (unsigned char*)" XY ", MINI_REV) ; 
	PrintMini(24, 58, (unsigned char*)" GO ", MINI_REV) ; 
	PrintMini(42, 58, (unsigned char*)" PLD ", MINI_REV) ; 
	PrintMini(65, 58, (unsigned char*)" ZH ", MINI_REV) ; 
	PrintMini(82, 58, (unsigned char*)" DAT ", MINI_REV) ; 
	PrintMini(110, 58, (unsigned char*)" D ", MINI_REV) ; 

	while (exit_flag != TRUE)  {
		
		switch(key) {
		case KEY_CTRL_F1: default: 
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			
			Print_zh("放样点 ", 22, 6, 0) ;
			PrintXY(25, 25, (unsigned char*)"X :", 0) ;
			PrintXY(25, 39, (unsigned char*)"Y :", 0) ;

			if (xyrow > 1) { 	/* 判断是否进入下一个菜单 */
				key = KEY_CTRL_F2 ; 
				xyrow = 0 ; 
				break ;
			 }
			/* 显示当前值 */
			Print_C_Val(x, y) ;

			key = InputVal(50, 22+xyrow*14) ;
			if (number < 100000 && number > -100000)
				switch (xyrow) {
				case 0:   x = number ? number : x ; break ; 
				case 1:   y = number ? number : y ; break ; 
				default:  break ;
				}
			break ;
		case KEY_CTRL_F2:
			/* 检查数据 */
			if (x == 0.0 || y == 0.0 ) { 
				xyrow = 0 ;
				Warning("坐标值无效 ", 2) ;
				key = back_key ;
				break ;
			}
			/* 需计算final坐标 */
			final_x = x - total_set.stat_point.x  ;
			final_y = y - total_set.stat_point.y   ;

			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			


			angel = atan2(Cut_3(final_y), Cut_3(final_x)) - total_set.azimuth ;
	
			Print_Angel(65, 5) ;
			distance = sqrt(pow(final_y, 2) + pow(final_x, 2)) ;
			Print_Float(65, 19, distance) ;
			Print_zh("角度: ", 22, 2, 0) ;
			Print_zh("距离: ", 22, 15, 0) ;
			Print_zh("实测: ", 22, 28, 0) ;
			Print_zh("进退: ", 22, 41, 0) ;

			do {
				key = InputVal(58, 16+14) ;
				if (key == KEY_CTRL_EXE) {
					Bdisp_AreaClr_DDVRAM(&area) ;
					Print_Float(65, 47, distance - number) ;
				}
				if (key == KEY_CTRL_EXIT)	exit_flag = TRUE ;
			} while (  !MenuSwitch(key) && exit_flag != TRUE && key != KEY_CTRL_LEFT && key != KEY_CTRL_RIGHT) ;

			break ;
		case KEY_CTRL_F3:
			back_key = key ;
			
			
			if (pld_flag == FALSE) {
				Bdisp_AreaClr_DDVRAM(&clear_area) ;
				Print_zh("点文件未加载 ", 22, 6, 0) ;
				Print_zh("请到数据库界面 ", 22, 22, 0) ;
				Print_zh("载入csv文件 ", 22, 38, 0) ;
				GetKey(&key) ;
			} else {
				SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */
				if( InputPT_num(Bfile_OpenFile(pld_FileName,_OPENMODE_READ_SHARE), TRUE) 
					!= TRUE) 
					key = KEY_CTRL_F1 ;
				else key = KEY_CTRL_F2 ;
				RestoreDisp(SAVEDISP_PAGE2) ;
			}	
			break ;
		case KEY_CTRL_F4:
			//back_key = key ;
			SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */

			if (qxys_flag == TRUE  || xyf_flag == TRUE)	 {
				if ( InputStake(0) == TRUE) 
					InputPJ(InputAngle()) ;  // 输入偏距
				key =  back_key ;
				//key = InputStake() ? KEY_CTRL_F2 : KEY_CTRL_F1 ;
			}
			else  key = KEY_CTRL_F5 ;

			RestoreDisp(SAVEDISP_PAGE2) ;
			break ;
		case KEY_CTRL_F5:
			back_key = key ;
			SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */

			if (allset.xl_type == 1 )	qxys_flag = load_pqx_data(qxys_flag) ;
			else				xyf_flag =  load_xyf_data(xyf_flag) ;	

			RestoreDisp(SAVEDISP_PAGE2) ;
			key = KEY_CTRL_F1 ;
			break ;	
		case KEY_CTRL_F6:
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			
			Print_zh("坐标正算 ", 22, 6, 0) ;	
			Print_zh("夹角 ", 24, 21, 0) ;
			Print_zh("边长 ", 24, 35, 0) ;
			if (xyrow > 1) { 	/* 判断是否进入下一个菜单 */	
				if (bz_dist) { Warning("边距不为零 ", 2) ; key = KEY_CTRL_F6 ; }
				else if (a > 360.0) {Warning("角度过大 ", 2) ; key = KEY_CTRL_F6 ; }
				else {
					key = KEY_CTRL_F1 ; 
					final_a = (25 * a - 6 *(int)a - 10 * Cut_2(a)) / 9  ;
					final_a = final_a * PI / 180 + total_set.azimuth  ;
					x = Cut_3(total_set.stat_point.x + S * cos(final_a)) ;
					y = Cut_3(total_set.stat_point.y + S * sin(final_a)) ;
				}
				xyrow = 0 ; 
				break ;
			 }
			Print_C_Val(CA = a, CB = S) ;
			Print_CC_Val(108, 24, CB = a) ;
			key = InputVal(55, 22+xyrow*14) ;
			switch (xyrow) {
			case 0:   a = number ? number : a ; break ; 
			case 1:   S = number ? number : S ; break ; 
			default:  break ;
			}

			break ;
		case KEY_CTRL_LEFT:case KEY_CTRL_RIGHT:
			if (qxys_flag == TRUE || xyf_flag == TRUE){
				SaveDisp(SAVEDISP_PAGE2) ;		// Save screen 
					
				if (allset.xl_type == 1) {
					if (key == KEY_CTRL_LEFT) {
						if (ipstake - allset.stake_d >= 0) {
							ipstake -= allset.stake_d ;
						}else ipstake = 0 ;
					} else {
						if (ipstake + allset.stake_d <= qx_HZ[list_size - 2]) {
							ipstake += allset.stake_d ;
						} else ipstake = qx_HZ[list_size - 2] ;
					}
					number = ipstake ;
					Stake_JS() ;
					if (xl_pos == XD_DYZX)  
						qiexian_an -= PI ; //位于第一直线时需减去pi 
					x = x + PJ_dist * cos(qiexian_an + j_to_h) ;
					y = y + PJ_dist * sin(qiexian_an + j_to_h) ;
				}
				else {
					if (key == KEY_CTRL_LEFT) {
						if (ipstake - allset.stake_d >= 0) {
							ipstake -= allset.stake_d ;
						}else ipstake = 0 ;
					} else {
						if (ipstake + allset.stake_d <= xy_zd_stake[xyys_size-1]) {
							ipstake += allset.stake_d ;
						} else ipstake = xy_zd_stake[xyys_size-1] ;
					}
					number = ipstake ;
					Stake_XY() ;
					x = x + PJ_dist * cos(Sta.a + j_to_h) ;
					y = y + PJ_dist * sin(Sta.a + j_to_h) ;
				}
				PopUpWin(3) ;
				locatestr(12,22) ;
				printf("当前桩号%.3f",number ) ;
				Bdisp_PutDisp_DD() ;
				Sleep(1500) ;

				RestoreDisp(SAVEDISP_PAGE2) ;
			}
			key =  back_key ;
			break ;
		case KEY_CTRL_UP:  
			if (xyrow > 0)  --xyrow ;  
			key = back_key ;
			break ;
		case KEY_CTRL_DOWN: 
		case KEY_CTRL_EXE:
			++xyrow ; 
			key = back_key ;
			break ;
		case KEY_CTRL_DEL:
			key = back_key ;
			break ;
		case KEY_CTRL_EXIT :
			exit_flag = TRUE ;
			//FreeQxys() ;		
			break ;
		}
	}
	return ;          
}


/* 菜单切换 */
int MenuSwitch(unsigned int key)
{
	if (key >= KEY_CTRL_F1 && key <= KEY_CTRL_F6) {
		Bdisp_AreaClr_DDVRAM(&clear_area) ;
		return TRUE ;
	}
	else return FALSE ;
}

double* ReadFloat(BYTE list_num, BYTE list,  int list_size) ;

static int p_handle ;
int list_size ;

int  TestQxzh(void)
{
	return ( qx_ZH != NULL && qx_HY != NULL && qx_YH != NULL && qx_HZ != NULL
		   && qx_X != NULL && qx_Y != NULL && qx_R != NULL && qx_Ls1 != NULL
		   && qx_T1 != NULL && qx_angel != NULL && qx_zy != NULL && qx_ZJ != NULL ) ? TRUE : FALSE ;
}

int FreeQxys()
{
	if (qx_ZH != NULL)  { free(qx_ZH) ; qx_ZH = NULL ; }
	if (qx_HY != NULL)  { free(qx_HY) ; qx_HY = NULL ; }
	if (qx_YH != NULL)  { free(qx_YH) ; qx_YH = NULL ; }
	if (qx_HZ != NULL)  { free(qx_HZ) ; qx_HZ = NULL ; }
	if (qx_Stake != NULL)  { free(qx_Stake) ; qx_Stake = NULL ; }
	if (qx_X != NULL)  { free(qx_X) ; qx_X = NULL ; }
	if (qx_Y != NULL)  { free(qx_Y) ; qx_Y = NULL ; }
	if (qx_R != NULL)  { free(qx_R) ; qx_R = NULL ; }
	if (qx_Ls1 != NULL)  { free(qx_Ls1) ; qx_Ls1 = NULL ; }
	if (qx_T1 != NULL)  { free(qx_T1) ; qx_T1 = NULL ; }
	if (qx_angel != NULL)  { free(qx_angel) ; qx_angel = NULL ; }
	if (qx_zy != NULL)  { free(qx_zy) ; qx_zy = NULL ; }
	if (qx_ZJ != NULL)  { free(qx_ZJ) ; qx_ZJ = NULL ; }
	
	return ;
}

int LoadQxys(BYTE flag)
{
	unsigned int key ;
	
	PopUpWin(3) ;
	Print_zh("文件名： ", 20, 22, 0) ;
	PrintXY(80, 25, (unsigned char *)dbset.current_pq, 0) ;
	
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE) {
		if (ipstr[0] == 0 && dbset.current_pq[0] != 0)  strncpy(ipstr, dbset.current_pq, 5) ;
		FreeQxys() ;
		list_size = Init_Sht(22) ;	/*  包含起点数据  */
		if (list_size == -1) return FALSE ;
		if (list_size <= 2) { Warning("路线不完整 ", 3) ; return FALSE ;}
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
		
		if (TestQxzh())	{ Warning("读取成功 ", 1) ; return TRUE ; }
		else	  Warning("读取失败 ", 3) ;
	} 
	else if (key == KEY_CTRL_EXIT) return EXIT_F ;
	return FALSE ;
}


int Init_Sht(BYTE list)
{
	BYTE char_p ;
	DWORD check_sum ;
	DWORD check ;	
# ifdef PC_TEST
	FONTCHARACTER pfile[30] ;
	char fname_buf[30] ;

	memset(fname_buf,'\0', 30);  
	strcat(fname_buf, (char*)"\\\\crd0\\") ;	// Parent folder
	strcat(fname_buf, ipstr) ;			// Point name 
	for (char_p = 0 ; char_p <= strlen(fname_buf) ; ++char_p) 
		pfile[char_p] = (WORD)fname_buf[char_p] ;
	Bfile_CloseFile( p_handle ) ;
	p_handle = Bfile_OpenFile(pfile, _OPENMODE_READ) ;
	if (p_handle >= 0) {
		Bfile_ReadFile(p_handle, &check_sum, 4, 0x54) ;	/*  position 54h  */
		for (char_p = 0 ; char_p < list ; ++char_p) {	
			Bfile_ReadFile(p_handle, &check, 4, 0x5C+char_p*4) ;
			check_sum -= check ;
		}
		Bfile_ReadFile(p_handle, &check, 4, 0x5C + 4) ;		/*  以B列实际输入的数据行数为准  */
		if (check_sum == 0)  return (int)((check - 128) / 12) ;
		else { Warning("文件无效 ", 3) ; Bfile_CloseFile( p_handle ) ; return -1 ; }	
	} else	{DebugS("打开%s失败 ", ipstr) ;  return -1 ; }
	// { Warning("打开失败 ", 3) ;  return -1 ; }
# else 
	char flags_0 ;	/*  Used for main memory syscall   */
	int item_ptr ;
	int data_length;
	int result ; 

	result = MCS_SearchDirectoryItem( (unsigned char*)"@SSHEET", (unsigned char*)ipstr, &flags_0, &item_ptr, &p_handle, &data_length ) ;
	if (result == 0) {
		/*  
			Main memory内的SSHEET文件偏移比PC测试文件少76字节，
			也就是没有G1M文件头，也就是0x8偏移的表格大小, less 0x4C
		*/
		check_sum = *(DWORD*)((char*)p_handle + 0x8) ;
		//Print_Val(20,5,(int)check_sum,0) ;
		//Bdisp_PutDisp_DD();
		//Sleep(5000) ;	
		for (char_p = 0 ; char_p < list ; ++char_p) {
			check = *(DWORD*)((char*)p_handle + 0x10 + char_p*4) ;
			check_sum -= check ;
		}
		check = *(DWORD*)((char*)p_handle + 0x10 + 4) ;	/*  以B列实际输入的数据行数为准  */
		if (check_sum == 0)  return (int)((check - 128) / 12) ;
	} else {DebugS("打开%s失败 ", ipstr) ;  return -1 ; }
	// { Warning("打开失败 ", 3) ;  return -1 ; }
# endif		
}

/* 列号，总列数，行数 */
double* ReadFloat(BYTE list_num, BYTE list, int size)
{
	double* float_p ;
	BYTE step ;
	DWORD check ;
	DWORD pos = 0 ;
	BYTE buff[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;		/*  12字节/数据  */
	BYTE f_buff[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
	BYTE x_buff[3] = {0, 0, '\0'} ;
	BYTE ppos = 0 ;
	BYTE count ;
	BYTE bp = 2 ;	/* 浮点数指针，默认从第3字节读起 */
	
	
	float_p = (double*) calloc(size, sizeof(double)) ;
	if (float_p == NULL) { Warning("内存不足 ", 4) ; return NULL ; }
	for (step = 0 ;  step < ( list_num - 1 ) ; ++step) {
	# ifdef PC_TEST
		Bfile_ReadFile(p_handle, &check, 4, 0x5C+step*4) ;
	# else 
		check = *(DWORD*)((char*)p_handle + 0x10 + step*4) ;
	# endif
		pos += check ;
	}
	/*  128字节前置偏移量 + 偏移位置 + 文件头  */
# ifdef PC_TEST
	if (list > 9)		//大于九列则切换寻址模式至包含计算式的模式
		pos = 128 + pos + list * 8 + 0x5C ;
	else 
		pos = 128 + pos + list * 4 + 0x5C ;
# else 
	if (list > 9)		//大于九列则切换寻址模式至包含计算式的模式
		pos = 128 + pos + list * 8 + 0x10 ;
	else 
		pos = 128 + pos + list * 4 + 0x10 ;
# endif
	for (step = 0 ; step < size ; ++step) {
	# ifdef PC_TEST
		/* 12 Byte per data  */
		Bfile_ReadFile(p_handle, &buff, 12, pos + step * 12) ;	
	# else
		buff[0]= *(BYTE*)(p_handle + pos + step * 12 + 0) ;
		buff[1]= *(BYTE*)(p_handle + pos + step * 12 + 1) ;
		buff[2]= *(BYTE*)(p_handle + pos + step * 12 + 2) ;
		buff[3]= *(BYTE*)(p_handle + pos + step * 12 + 3) ;
		buff[4]= *(BYTE*)(p_handle + pos + step * 12 + 4) ;
		buff[5]= *(BYTE*)(p_handle + pos + step * 12 + 5) ;
		buff[6]= *(BYTE*)(p_handle + pos + step * 12 + 6) ;
		buff[7]= *(BYTE*)(p_handle + pos + step * 12 + 7) ;
		buff[8]= *(BYTE*)(p_handle + pos + step * 12 + 8) ;
		buff[9]= *(BYTE*)(p_handle + pos + step * 12 + 9) ;
		buff[10]= *(BYTE*)(p_handle + pos + step * 12 + 10) ;
		buff[11]= *(BYTE*)(p_handle + pos + step * 12 + 11) ;
	# endif
		/*  0X to double  */
		sprintf((char*)&x_buff, "%02X", buff[1]) ;
		ppos = x_buff[0] - '0' ;
		
		memset(f_buff, '\0', 9);  /*  bug修正，浮点缓冲区清零  */
		
		switch (buff[0]) {
		case 0x10:		/*  常规浮点数  */
			if (ppos < 5) {	/*  整数部分取5位  */
				f_buff[0] = x_buff[1] ;	
				f_buff[ppos + 1] = '.' ;		/*  Add '.'  */
				count = 1 ;
			}
			/*  整数部分为0的情况  */
			else if (ppos > 7) {
				f_buff[0] = '0' ;
				f_buff[1] = '.' ;
				f_buff[2] = x_buff[1] ;
				count = 3 ;
			}
			/* 大整数情况 */
			else if (ppos == 5){
				count = 0 ;
				f_buff[5] = '.' ;		/*  固定小数点（缩至5位整数）  */
			}
			else if (ppos == 6){		
				sprintf((char*)&x_buff, "%02X", buff[bp]) ;
				f_buff[0] = x_buff[1] ;	
				bp = 3;
				count = 1 ;
				f_buff[5] = '.' ;		/*  固定小数点（缩至5位整数）  */
			}
			else if (ppos == 7){
				count = 0 ;
				bp = 3 ;
				f_buff[5] = '.' ;		/*  固定小数点（缩至5位整数）  */
			}
			else {
				f_buff[ppos] = '.' ;		/*  Add '.'  */	
				count = 0 ;
			}
			break ;
		case 0x60:		/*  负数  */
			f_buff[0] = '-' ;
			f_buff[1] = x_buff[1] ;
			f_buff[ppos + 2] = '.' ;
			count = 2 ;
			break ;
		case 0x09:		/*  指数浮点数  */
			ppos = 9 - ppos ;
			f_buff[0] = '0' ;
			f_buff[1] = '.' ;
			count = ppos + 3 ;
			f_buff[ppos + 2] = x_buff[1] ;
			while (ppos != 0) {
				f_buff[ppos + 1] = '0' ;
				--ppos ;
			}
			break ;
		case 0x59:		/*  指数负数  */
			ppos = 9 - ppos ;
			f_buff[0] = '-' ;
			f_buff[1] = '.' ;
			count = ppos + 3 ;
			f_buff[ppos + 2] = x_buff[1] ;
			while (ppos != 0) {
				f_buff[ppos + 1] = '0' ;
				--ppos ;
			}
			break ;
		default:
			break ;
		}
		
		while (count < 9) {
			if (f_buff[count] == '.') ++count ;
			sprintf((char*)&x_buff, "%02X", buff[bp]) ;
			f_buff[count] = x_buff[0] ;
			++count ;
			if (f_buff[count] == '.') ++count ;
			f_buff[count] = x_buff[1] ;
			++count ;
			++bp ;
		}
		f_buff[9] = '\0' ;
		float_p[step] = (double)atof((char*)f_buff) ;
		bp = 2 ;  //恢复浮点数指针 指向第三字节
	}
	return float_p ;
}

extern int Zx_JS(double zd_x, double zd_y, double dx_angel, double zd_stake, double zh_stake, double ip_stake) ;
extern int No1_JS(double stake_X, double stake_Y, double dx_angel, double zh_stake, double ip_stake, double Ls1, double R, double ZY) ;
extern int Yuan_JS(double stake_X, double stake_Y, double dx_angel, double hy_stake, double ip_stake,  double zh_stake, double Ls1, double R, double ZY) ;
extern int No2_JS(double stake_X, double stake_Y, double dx_angel, double zh_stake, double hz_stake, double ip_stake, double Ls1, double R, double ZY, double ZJ) ;
extern int No2_ZX(double zd_x, double zd_y, double dx_angel, double hz_stake, double ip_stake, double T1, double ZY, double ZJ) ;


/*  弦长公式  */
double X_C(double M9, double Ls1, double R)
{
	double N9, O9 ;
	N9 = M9 - pow(M9, 5) / (40 * Ls1 * Ls1 * R * R) + pow(M9, 9) / (3456 * pow(Ls1, 4) * pow(R, 4)) ;
	O9 = pow(M9, 3) / (6 * R * Ls1) - pow(M9, 7) / (336 * pow(Ls1, 3) * pow(R, 3))
		 + pow(M9, 11) / (42240 * pow(Ls1, 5) * pow(R, 5)) ;
	return sqrt(N9 * N9 + O9 * O9) ;	
}
static double bz_corner = 0.0;

double InputAngle(void)
{
	unsigned int key ;
	BYTE exit_flag = FALSE ;
	

	if (allset.pj_mode == 2){
		PopUpWin(3) ;
		Print_zh("偏角 ", 18, 20, 0) ;
		while (exit_flag != TRUE) {
			Print_Float(58, 24, bz_corner) ;
			key = InputValP(48, 22) ;
			switch (key){
			case KEY_CTRL_EXE:
				bz_corner = number ? number : bz_corner ;
				exit_flag = TRUE ;
				break ;
			case KEY_CTRL_EXIT:
				exit_flag = TRUE ;
				break ;
			default:
				break; 
			}
		}
		return bz_corner ;
	}
	else return 0 ;
}

double PJ_dist = 0.0 ;
double j_to_h = 0.0 ;
int InputPJ(double bz_corner)
{
	unsigned int key ;
	BYTE exit_flag = FALSE ;
	


	PopUpWin(3) ;
	Print_zh("偏距 ", 18, 20, 0) ;
	while (exit_flag != TRUE) {
		Print_Float(65, 24, PJ_dist) ;
		key = InputValP(48, 22) ;
		switch (key){
		case KEY_CTRL_EXE:
			
			
			if (number == 0.00001 )  j_to_h = 0 ;
			PJ_dist = number ? number : PJ_dist ;

			//if (PJ_dist == 0 ) { exit_flag = TRUE ; break ;}
			if (allset.pj_mode == 1) {
				if (PJ_dist <0) 
					bz_corner = 270 ;
				else if (PJ_dist >0) ;
					bz_corner = 90 ;
			} 
			j_to_h = Sixty2ten(bz_corner) * PI /180 ;
			if (allset.xl_type == 1) {
				if (xl_pos == XD_DYZX)  
					qiexian_an -= PI ; //位于第一直线时需减去pi
				x = x + PJ_dist * cos(qiexian_an + j_to_h) ;
				y = y + PJ_dist * sin(qiexian_an + j_to_h) ;
			} else {
				x = x + PJ_dist * cos(Sta.a + j_to_h) ;
				y = y + PJ_dist * sin(Sta.a + j_to_h) ;
			}
			exit_flag = TRUE ;
			break ;
		case KEY_CTRL_EXIT:
			exit_flag = TRUE ;
			break ;
		default:
			break ;
		}
	}
	return TRUE ;
}

int InputStake(unsigned int entry)
{
	unsigned int key ;
	unsigned int any_key ;
	BYTE popup_flag = TRUE ;
	BYTE exit_flag = FALSE ;
	unsigned int step = 0 ;
	double temp_x = 0.0 ;
	double temp_y = 0.0 ;
	double LN1 = 0.0 ;
	double LN0 = 0.0 ;
	double A,E,D,D_back ;
	int fh ;
	int key2 ;

	PopUpWin(3) ;
	Print_zh("桩号 ", 18, 20, 0) ;
	while (exit_flag != TRUE) {
		if (entry == KEY_CTRL_F4 || entry == FS_POPUP) {
			key = KEY_CTRL_F4 ;
			popup_flag = TRUE ;
		}
		else if  (entry == FS_NOPOPUP) {
			key = KEY_CTRL_F4 ;
			popup_flag = FALSE ;
		}
		else {

			if (ipstake >= 10000) Print_Float(53, 24, ipstake) ;
			else if (ipstake >= 1000) Print_Float(58, 24, ipstake) ;
			else if (ipstake >= 100) Print_Float(63, 24, ipstake) ;
			else if (ipstake >= 10) Print_Float(68, 24, ipstake) ;
			else Print_Float(73, 24, ipstake) ;
			key = InputValP(48, 22) ;
		}
		switch (key) {
		case KEY_CTRL_EXE:
			ipstake = number ? number : ipstake;
			if (allset.xl_type == 1) {
				if (ipstake > qx_HZ[list_size - 2] || ipstake < qx_Stake[0] ) {
					Print_Float(68, 47, qx_HZ[list_size - 2]) ;
					Warning("超出线路 ", 2) ;
					return FALSE ;
				}
				number = ipstake ;
				Stake_JS() ;
				return TRUE ;
			} else {
				if (ipstake < xy_zd_stake[0] || ipstake > xy_zd_stake[xyys_size-1]) {
					Print_Float(68, 47, xy_zd_stake[xyys_size-1]) ;
					Warning("超出线路 ", 2) ;
					return FALSE ;
				}
				number = ipstake ;
				Stake_XY() ;
				return TRUE ;
			}
			break ;
		case KEY_CTRL_F4:
			temp_x = x ;
			temp_y = y ;
			D_back = D = 0.0 ;
			
			SetTimer(ID_USER_TIMER1,10000,timeoutjump) ;
			PrintXY(105, 14, (unsigned char*)"\xE6\xA6 ", 0) ;
			Bdisp_PutDisp_DD() ;   

			if (allset.xl_type == 1){	//交点法反算
			
			LN1 = qx_Stake[0] ;
			do {
				LN0 = fabs(LN1) ;
				number = LN0 ;
				Stake_JS() ;
				D_back = D ;
				D = sqrt(pow((temp_y - y), 2) + pow((temp_x - x), 2)) ;
				E = atan2((temp_y - y + 0.0000000001), (temp_x - x)) ;
				fh = (D > D_back) ? -1 : 1 ;
				A = E - qiexian_an ;

				while (step <= list_size - 1 && LN0 > qx_ZH[step])
					++step ;
				if (step > list_size - 1) {
					x = temp_x ;
					y = temp_y ;
					KillTimer(ID_USER_TIMER1) ;
					Warning("超出线路 ", 2) ;
					return FALSE ;
				}
				step = 0 ;

				LN1 = LN0 + ( fh * D * cos(A) ) ;

			} while (fabs(LN1 - LN0) > 0.0001 && timeout == FALSE) ;
			if (timeout == TRUE) {
				timeout = FALSE ;
				x = temp_x ;
				y = temp_y ;
				Warning("反算超时!",1) ;
				return FALSE ;
			}
			else KillTimer(ID_USER_TIMER1) ;
			
			while (fabs((temp_y - y) / cos(qiexian_an)) > (allset.width_max+0.001)) {
				//DebugS("%f",(temp_y - y) / cos(qiexian_an)) ;
				Warning("偏距超限 ", 2) ;
				PopUpWin(3) ;
				Print_zh("估值 ", 18, 20, 0) ;
				key2 = InputVal(48, 22) ;
				if (key2 == KEY_CTRL_EXE) {
				LN1 = number ;
				do {
					LN0 = fabs(LN1) ;
					number = LN0 ;
					Stake_JS() ;
					D_back = D ;
					D = sqrt(pow((temp_y - y), 2) + pow((temp_x - x), 2)) ;
					E = atan2((temp_y - y + 0.0000000001), (temp_x - x)) ;
					fh = (D > D_back) ? -1 : 1 ;
					A = E - qiexian_an ;

					while (step <= list_size - 1 && LN0 > qx_ZH[step])
						++step ;

					if (step > list_size - 1) {
						x = temp_x ;
						y = temp_y ;
						Warning("超出线路 ", 2) ;
						return FALSE ;
					}
					step = 0 ;

					LN1 = LN0 + ( fh * D * cos(A) ) ;
				} while (fabs(LN1 - LN0) > 0.0001) ;
				}else {
					x = temp_x ;
					y = temp_y ;
					return FALSE ;
				}
			}
			if (popup_flag == TRUE) {
				SaveDisp(SAVEDISP_PAGE3) ;	
				PopUpWin(4) ;
				Print_zh("里程桩号 ", 14, 11, 0) ;
				Print_zh("离中心桩 ", 14, 25, 0) ;
				Print_Float(68, 15, LN1) ;
				//第一直线段时取反偏距
				if (xl_pos == XD_DYZX) 
					Print_Float(70, 29, -(temp_y - y) / cos(qiexian_an)) ;
				else   
					Print_Float(70, 29, (temp_y - y) / cos(qiexian_an))  ;
				GetKey(&any_key) ;
				RestoreDisp(SAVEDISP_PAGE3) ;
			}
			fs_zh =  LN1 ;
			if (xl_pos == XD_DYZX) 	fs_pj = -(temp_y - y) / cos(qiexian_an) ;
			else 					fs_pj = (temp_y - y) / cos(qiexian_an) ;
			} 
else {	//线元法反算
LN1 = xy_zd_stake[0] ;
do {
	LN0 = fabs(LN1) ;
	number = LN0 ;
	Stake_XY() ;
	D_back = D ;
	D = sqrt(pow((temp_y - y), 2) + pow((temp_x - x), 2)) ;
	E = atan2((temp_y - y + 0.0000000001), (temp_x - x)) ;
	fh = (D > D_back) ? -1 : 1 ;
	A = E - Sta.a ;
	
	while (step < xyys_size  && LN0 >= xy_zd_stake[step]) 
		++step ;
	if (step > xyys_size) {
			x = temp_x ;
			y = temp_y ;
			KillTimer(ID_USER_TIMER1) ;
			Warning("超出线路 ", 2) ;
			return FALSE ;
	}
	step = 0 ;
	
	LN1 = LN0 + ( fh * D * cos(A) ) ;
} while (fabs(LN1 - LN0) > 0.0001 && timeout == FALSE) ;
if (timeout == TRUE) {
		timeout = FALSE ;
		x = temp_x ;
		y = temp_y ;
		Warning("反算超时!",1) ;
		return FALSE ;
}
else KillTimer(ID_USER_TIMER1) ;

while (fabs((temp_y - y) / cos(Sta.a)) > (allset.width_max+0.001)) {
		Warning("偏距超限 ", 2) ;
		PopUpWin(3) ;
		Print_zh("估值 ", 18, 20, 0) ;
		key2 = InputVal(48, 22) ;
		if (key2 == KEY_CTRL_EXE) {
		LN1 = number ;
		do {
		LN0 = fabs(LN1) ;
		number = LN0 ;
		Stake_XY() ;
		D_back = D ;
		D = sqrt(pow((temp_y - y), 2) + pow((temp_x - x), 2)) ;
		E = atan2((temp_y - y + 0.0000000001), (temp_x - x)) ;
		fh = (D > D_back) ? -1 : 1 ;
		A = E - Sta.a ;
	
		while (step < xyys_size && LN0 >= xy_zd_stake[step]) 
			++step ;
		if (step > xyys_size) {
			x = temp_x ;
			y = temp_y ;
			KillTimer(ID_USER_TIMER1) ;
			Warning("超出线路 ", 2) ;
			return FALSE ;
		}
		step = 0 ;
	
		LN1 = LN0 + ( fh * D * cos(A) ) ;
		} while (fabs(LN1 - LN0) > 0.0001) ;
		}else {
				x = temp_x ;
				y = temp_y ;
				return FALSE ;
		}
}
if (popup_flag == TRUE) {
	SaveDisp(SAVEDISP_PAGE3) ;	
	PopUpWin(4) ;
	Print_zh("里程桩号 ", 14, 11, 0) ;
	Print_zh("离中心桩 ", 14, 25, 0) ;
	Print_Float(68, 15, LN1) ;
	Print_Float(70, 29, (temp_y - y) / cos(Sta.a)) ;
	GetKey(&any_key) ;
	RestoreDisp(SAVEDISP_PAGE3) ;
}
fs_zh =  LN1 ;
fs_pj = (temp_y - y) / cos(Sta.a) ;			
}	//线元法反算END

			
			x = temp_x ;
			y = temp_y ;
			exit_flag = TRUE ;	
			break ;
		case KEY_CTRL_EXIT:
			exit_flag = TRUE ;
			break ;
		default: break ;
		}
	}
	return FSOK ;
}



//角度整理（值域控制在0-2PI之间) 
double Ang(double rad)
{
	if (rad < 0)
		return rad + 2 * PI ;
	else if (rad > 2)
		return rad - 2*PI ;
	else	return rad ;
}
struct XYstat Sta ;

int GetStaCoor(struct CurSec Sec)
{
/*	5节点
z1 = -0.90617984593866399280, w1 = 0.23692688505618908751
z2 = -0.53846931010568309104, w2= 0.47862867049936646804
z3 = 0.00000000000000000000,  w3= 0.56888888888888888889 
z4 = 0.53846931010568309104,  w4= 0.47862867049936646804
z5 = 0.90617984593866399280,  w5= 0.23692688505618908751
*/	
	//求积结点系数赋矩阵T
	double T[]= {-0.906179845938663,-0.538469310105683,0.00000000000000,0.538469310105683,0.906179845938664} ;		
	//求积系数赋矩阵Y
	double Y[] = {0.236926885056189,0.478628670499366,0.568888888888888,0.478628670499366,0.236926885056189 } ;

	double L, LL, Getai ;
	double S1, S2, a0, c0, c1 ;
	
	double Qmin,Q,Precision,Resultback, tp1 , tp2,plusResult,tpResult ;
	unsigned int Nstep,N,i ;

	L = Sta.Mile - Sec.START_Sta.Mile ;		//计算点到起始点的距离
	LL = Sec.END_Sta.Mile - Sec.START_Sta.Mile ;	//当前线元缓长
	
	if (fabs(LL) < 0.001)
		Sta = Sec.START_Sta ;
	else {
		Sta.Ro = Sec.START_Sta.Ro + (Sec.END_Sta.Ro - Sec.START_Sta.Ro) * L / LL ;
		Getai = Sec.START_Sta.a + (Sta.Ro + Sec.START_Sta.Ro) * L / 2 ;
		Sta.a = Ang(Getai) ;
        		S1 = 0;
        		S2 = 0;
        		a0 = Sec.START_Sta.a;
        		c0 = Sec.START_Sta.Ro;
       		c1 = Sec.END_Sta.Ro;

		Qmin = fabs(0.0001*L) ;	//分割极限

		Precision = 1 ;
		Resultback = 1.0e+35; //第一次算的结果无法比较来确定精度，赋予很大的数 
		Q = L ;	//初始化分割长度为L
		N = 1 ;	//变步长
		while(Precision > 0.0001 && fabs(Q) > Qmin){
			S1 = 0 ;
			for ( Nstep = 1 ; Nstep <= N ; ++Nstep){
				tp1 = (Nstep - 1)*Q ;
				tp2 = Nstep*Q ;
				plusResult = 0 ;
				for (i = 0  ; i < 5 ; ++i){
					tpResult = ((tp2-tp1)*T[i] + (tp2+tp1))/2 ;
					plusResult = plusResult + Y[i] * cos (a0+c0*tpResult +(c1-c0) * tpResult*tpResult/(2*LL)) ;
				}
				S1 += plusResult ;
			}
			S1 = S1 * Q / 2 ;
			Precision = fabs(S1 - Resultback) / ( 1 + fabs(S1)) ;
			Resultback = S1 ;
			++N ;
			Q = L / N ;
		}

		Precision = 1 ;
		Resultback = 1.0e+35; //第一次算的结果无法比较来确定精度，赋予很大的数 
		Q = L ;	//初始化分割长度为L
		N = 1 ;	//变步长
		while(Precision > 0.0001 && fabs(Q) > Qmin){
			S2 = 0 ;
			for ( Nstep = 1 ; Nstep <= N ; ++Nstep){
				tp1 = (Nstep - 1)*Q ;
				tp2 = Nstep*Q ;
				plusResult = 0 ;
				for (i = 0  ; i < 5 ; ++i){
					tpResult = ((tp2-tp1)*T[i] + (tp2+tp1))/2 ;
					plusResult = plusResult + Y[i] * sin (a0+c0*tpResult +(c1-c0) * tpResult*tpResult/(2*LL)) ;
				}
				S2 += plusResult ;
			}
			S2 = S2 * Q / 2 ;
			Precision = fabs(S2 - Resultback) / ( 1 + fabs(S2)) ;
			Resultback = S2 ;
			++N ;
			Q = L / N ;
		}		

		Sta.x = Sec.START_Sta.x + S1 ;
		Sta.y = Sec.START_Sta.y + S2 ;

	}
}



int Stake_XY(void)	/* 高斯-勒让德迭代求积 */
{
	unsigned int step = 0 ;	
	struct CurSec Sec_cu ;	

	while (step < xyys_size - 1 && number >= xy_zd_stake[step]) 
		++step ;

	Sec_cu.START_Sta.a = Sixty2ten(xy_zd_angle[step-1]) * PI / 180 ;
	Sec_cu.START_Sta.Ro = xy_zd_R[step-1] ? 1 / xy_zd_R[step-1] : 0  ;
	Sec_cu.END_Sta.Ro = xy_zd_R[step] ? 1 / xy_zd_R[step] : 0 ;

	Sec_cu.START_Sta.Mile = xy_zd_stake[step-1] ;
	Sec_cu.END_Sta.Mile = xy_zd_stake[step] ;
	
	Sec_cu.START_Sta.x = xy_zd_X [step-1] ;
	Sec_cu.START_Sta.y = xy_zd_Y [step-1] ;

	Sta.Mile = number ;
	GetStaCoor(Sec_cu) ;
	x = Sta.x ;
	y = Sta.y ;

	return ;
}
int Stake_JS(void)
{
	unsigned int step = 0 ;
	double zh_x, zh_y, hy_x, hy_y, hz_x, hz_y ;

	while (step < list_size - 1 && number > qx_ZH[step])
		++step ;
	if (step == 0) {
		Zx_JS(qx_X[1], qx_Y[1], qx_angel[0] - PI, qx_Stake[1], qx_ZH[0], number) ;
		//Warning("起始直线 ", 2) ;
	}
	else {	--step ;
		/*  四大桩号坐标计算  */
		zh_x = qx_X[step + 1] + (qx_Stake[step + 1] - qx_ZH[step]) * cos(qx_angel[step] - PI) ;
		zh_y = qx_Y[step + 1] + (qx_Stake[step + 1] - qx_ZH[step]) * sin(qx_angel[step] - PI) ;
		hy_x = zh_x + X_C( (qx_HY[step] - qx_ZH[step]), qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001 )
		   	* cos(qx_angel[step] + qx_zy[step] 
			* ((qx_HY[step] - qx_ZH[step]) * (qx_HY[step] - qx_ZH[step])
			 / 2 / (qx_Ls1[step]  + 0.0000000000001)/ (qx_R[step] + 0.0000000000001)) / 3) ;
		hy_y = zh_y + X_C( (qx_HY[step] - qx_ZH[step]), qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001 )
		    	* sin(qx_angel[step] + qx_zy[step] 
			* ((qx_HY[step] - qx_ZH[step]) * (qx_HY[step] - qx_ZH[step])
			 / 2 / (qx_Ls1[step] + 0.0000000000001) / (qx_R[step] + 0.0000000000001)) / 3) ;
		hz_x = qx_X[step + 1] + qx_T1[step] * cos(qx_angel[step] + qx_zy[step] * qx_ZJ[step]) ;
		hz_y = qx_Y[step + 1] + qx_T1[step] * sin(qx_angel[step] + qx_zy[step] * qx_ZJ[step]) ;	

		if (number < qx_HY[step]) {
			No1_JS(zh_x, zh_y, qx_angel[step], qx_ZH[step], number, qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step] ) ;
			//Warning("第一曲线 ", 2) ;
		}
		else if (number < qx_YH[step]) {
			Yuan_JS(hy_x, hy_y, qx_angel[step], qx_HY[step], number,  qx_ZH[step], qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step]) ;
			//Warning("圆曲线 ", 2) ;
		}
		else if (number <qx_HZ[step]) {
			No2_JS(hz_x, hz_y, qx_angel[step], qx_ZH[step], qx_HZ[step], number, qx_Ls1[step] + 0.0000000000001, qx_R[step] + 0.0000000000001, qx_zy[step],  qx_ZJ[step]) ;
			//Warning("第二曲线 ", 2) ;
		}
		else {
			No2_ZX(qx_X[step + 1], qx_Y[step + 1], qx_angel[step], qx_HZ[step], number, qx_T1[step], qx_zy[step],  qx_ZJ[step] ) ;
			//Warning("第二直线 ", 2) ;
		}
	}
	return ;
}
/*  
	直线段坐标计算
	输入参数依次为：转点X坐标，转点Y坐标，定向方位角，转点桩号，直缓桩号，放样桩号
*/
int Zx_JS(double zd_x, double zd_y, double dx_angel, double zd_stake, double zh_stake, double ip_stake)
{
	double stake_X ;
	double stake_Y ;

	stake_X = zd_x + (zd_stake - zh_stake) * cos(dx_angel) ;
	stake_Y = zd_y + (zd_stake - zh_stake) * sin(dx_angel) ;
	x = stake_X + (zh_stake - ip_stake) * cos(dx_angel) ;
	y = stake_Y + (zh_stake - ip_stake) * sin(dx_angel) ;

	//qiexian_an = dx_angel - PI ;
	qiexian_an = dx_angel ;
	xl_pos = XD_DYZX ;
	return ;
}


/*  
	第一缓和曲线坐标计算
	输入参数依次为：直缓坐标X，直缓坐标Y，定向方位角，直缓桩号，放样桩号，前缓长，半径，曲线偏向
*/
int No1_JS(double stake_X, double stake_Y, double dx_angel, double zh_stake, double ip_stake, double Ls1, double R, double ZY)
{
	double M9, Q9, P9, R9 ;

	M9 = ( (ip_stake - zh_stake) > Ls1 ) ? Ls1 : (ip_stake - zh_stake) ;
	P9 = X_C(M9, Ls1, R) ;
	Q9 = M9 * M9 / 2 / Ls1 / R ;
	R9 = dx_angel + ZY * Q9 / 3 ;

	x = stake_X + P9 * cos(R9) ;
	y = stake_Y + P9 * sin(R9) ;

	qiexian_an = dx_angel + ZY * Q9 ;
	xl_pos = XD_DYQX ;
	return ;
}

/*  
	圆曲线坐标计算 
	输入参数依次为：缓圆坐标X，缓圆坐标Y，定向方位角，缓圆桩号，放样桩号，直缓桩号，前缓长，半径，曲线偏向
*/
int Yuan_JS(double stake_X, double stake_Y, double dx_angel, double hy_stake, double ip_stake,  double zh_stake, double Ls1, double R, double ZY)
{
	double M9, Q9, P10, Q10, R10 ;
	
	M9 = ( (ip_stake - zh_stake) > Ls1 ) ? Ls1 : (ip_stake - zh_stake) ;
	Q9 = M9 * M9 / 2 / Ls1 / R ;
	Q10 = (ip_stake - hy_stake) / R / 2 ;
	P10 = 2 * sin(Q10) * R ;
	R10 = ZY * Q10 + dx_angel + ZY * Q9 ;
	
	x = stake_X + P10 * cos(R10) ;
	y = stake_Y + P10 * sin(R10) ;
	
	qiexian_an = dx_angel + ZY * (M9 * M9 / 2 / Ls1 / R) + ZY * 2 * Q10 ;
	xl_pos = XD_YQX ;
	return ;
}

/*
	第二缓和曲线坐标计算
	输入参数依次为：缓直坐标X，缓直坐标Y，定向方位角，直缓桩号，缓直桩号，放样桩号，前缓长，半径，曲线偏向，曲线偏角
*/
int No2_JS(double stake_X, double stake_Y, double dx_angel, double zh_stake, double hz_stake, double ip_stake, double Ls1, double R, double ZY, double ZJ)
{
	double P11, Q11, R11, M11 ;

	M11 = hz_stake - ip_stake ;
	Q11 = M11 * M11 / 2 / Ls1 / R ;
	P11 = X_C(M11, Ls1, R) ;
	R11 = dx_angel + ZY * ZJ - PI - ZY * Q11 / 3 ;

	x = stake_X + P11 * cos(R11) ;
	y = stake_Y + P11 * sin(R11) ;

	qiexian_an = dx_angel + ZY * ZJ - ZY * Q11 ;
	xl_pos = XD_DEQX ;
	return ;
}
/*
	第二直线段坐标计算
	输入参数依次为：转点X坐标，转点Y坐标，定向方位角，缓直桩号，放样桩号，前切长，半径，曲线偏向，曲线偏角
*/
int No2_ZX(double zd_x, double zd_y, double dx_angel, double hz_stake, double ip_stake, double T1, double ZY, double ZJ)
{
	double P12, R12 ;

	P12 = ip_stake - hz_stake + T1 ;
	R12 = dx_angel + ZY * ZJ ;

	x = zd_x + P12 * cos(R12) ;
	y = zd_y + P12 * sin(R12) ;

	qiexian_an = dx_angel + ZY * ZJ ;
	xl_pos = XD_DEZX ;
	return ;
}
