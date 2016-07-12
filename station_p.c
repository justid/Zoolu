/*
	绘星 Station Point  测站点
	Last update: 2009.11.23
*/

# include "9860g.h"

/* 计算后交虚拟测站坐标 */
static S_POINT PTresult = {0.0, 0.0, 0.0} ;
int CalPTC(S_POINT PTA,double A_a,double A_S,double A_h,  S_POINT PTB,double B_a,double B_S,double B_h) 
{
	double alpha ;
	double sinalpha ;
	double SAB ;
	double sinBAP ;
	double sinABP ;
	double tanAB ;
	double angAP ;
	double deltaX ;
	double deltaY ;
	double dAP,dBP ;
	double hrms ;	//平面精度 
	double vrms ;	//高程精度 

	//测试用例 
/*	PTA.x = 76222.138 ;
	PTA.y = 90228.021 ;
	PTB.x = 76529.694 ;
	PTB.y = 90061.451 ;
	A_S = 241.232 ;
	B_a = 223.5359 ;
	B_S = 133.365 ;
	A_h = 30.01 ;
	B_h = 30 ;
*/

	alpha = Sixty2ten(B_a) ;

	sinalpha = sin(alpha * PI / 180) ;
	SAB = sqrt(pow(PTA.x-PTB.x, 2) + pow(PTA.y-PTB.y, 2)) ;
	if (SAB == 0)
	{
		SAB += 0.00000000000001 ;
	}

	sinBAP = B_S*sinalpha/SAB ;
	sinABP = A_S*sinalpha/SAB ;


	tanAB = atan2((PTB.y-PTA.y),(PTB.x-PTA.x+1.0e-8)) ;

	angAP = tanAB + asin(sinBAP); // / PI * 180.0;

	deltaX = A_S * cos(angAP) ;// * PI / 180) ;
	deltaY = A_S * sin(angAP) ;// * PI / 180) ;

	
	PTresult.x = PTA.x + deltaX ;
	PTresult.y = PTA.y + deltaY ;

	/*
	DebugS("%.3f\n%.3f\n%.3f",alpha,sinalpha,SAB) ;
	DebugS("%.3f\n%.3f",sinBAP,sinABP) ;
	DebugS("%.3f\n%.3f",tanAB,angAP) ;
	DebugS("%.3f\n%.3f",deltaX,deltaY) ;
	DebugS("X = %.3f\nY = %.3f",PTresult.x,PTresult.y) ;
	*/
	dAP = sqrt(pow(PTA.x-PTresult.x, 2) + pow(PTA.y-PTresult.y, 2)) ;
	dBP = sqrt(pow(PTB.x-PTresult.x, 2) + pow(PTB.y-PTresult.y, 2)) ;

	//DebugS("%.3f\n%.3f",dAP,dBP) ;
	hrms = (fabs(A_S - dAP) + fabs(B_S - dBP)) / 2 ;
	vrms = fabs((PTA.z - A_h) - (PTB.z - B_h)) ;

	DebugS("平面精度%.1fcm\n高程精度%.1fcm",hrms*100,vrms*100) ;
	return ((hrms < 0.05) && (vrms < 0.1)) ;
}

int SaveStation(void)
{
	
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READ_SHARE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"X1 = %9.3f",total_set.stat_point.x) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.x1, temp_buffer,LATER,TRUE) ;
	sprintf(temp_buffer,"Y1 = %9.3f",total_set.stat_point.y) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.y1, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"H1 = %9.3f",total_set.stat_point.z) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.h1, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"X2 = %9.3f",total_set.bak_point.x) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.x2, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"Y2 = %9.3f",total_set.bak_point.y) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.y2, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"H2 = %9.3f",total_set.bak_point.z) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.h2, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"StH = %9.3f",total_set.station_ht) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.sth, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"PrismH = %9.3f",total_set.prism_ht) ;
	confile_ptr = WriteINI(confile_ptr, total_set.fpos.prh, temp_buffer,NOW,FALSE) ;


	Bfile_CloseFile(confile_ptr) ;
	return 0;
}

extern S_POINT point = {0.0, 0.0, 0.0} ;
static BYTE row = 0 ;
extern const DISPBOX clear_area = {20, 0, 126, 55} ;

int SetStation() 
{
	BYTE done_flag = FALSE ;
	BYTE exit_flag = FALSE ;
	BYTE init_flag = TRUE ;
	BYTE houf_flag = FALSE ;
	unsigned int key = 0 ;
	unsigned int back_key = 0 ;
	/* 后交使用 */
	S_POINT PTA = {0.0, 0.0, 0.0} ;
	S_POINT PTB = {0.0, 0.0, 0.0} ;
	double A_a = 0.0 ;	/*  方位角  */
	double A_S = 0.0 ;	/*  边长  */
	double A_h = 0.0 ;	/*  高差  */
	double B_a = 0.0 ;	/*  方位角  */
	double B_S = 0.0 ;	/*  边长  */
	double B_h = 0.0 ;	/*  高差  */
	
	while (done_flag != TRUE && exit_flag != TRUE) {
		if (houf_flag == FALSE){
			row = 0 ;	
			key = 0 ;
			back_key = 0 ;
			Bdisp_AllClr_DDVRAM() ; 	/* clear screen */

			Print_zh("　设站　", 5, 0, VERT_REV) ;
			/* Menu F1:Station F2:Back F3:... */
			PrintMini(5, 58, (unsigned char*)" Stat", MINI_REV) ; 
			PrintMini(26, 58, (unsigned char*)" Bak", MINI_REV) ; 
			PrintMini(43, 58, (unsigned char*)" Set", MINI_REV) ; 
			PrintMini(63, 58, (unsigned char*)" Save", MINI_REV) ; 
			PrintMini(84, 58, (unsigned char*)" Load", MINI_REV) ; 
			PrintMini(105, 58, (unsigned char*)" OPTN", MINI_REV) ;	
			while (done_flag != TRUE && exit_flag != TRUE && houf_flag != TRUE ) {
			switch (key) {
			case KEY_CTRL_F1: default: 
				back_key = key ;
				if (init_flag == TRUE) {
					Bdisp_AreaClr_DDVRAM(&clear_area) ;
					point.x = total_set.stat_point.x ;
					point.y = total_set.stat_point.y ;
					point.z = total_set.stat_point.z ;
					PrintPt() ;
				}
				key = StatSet() ;
				if (key == TRUE) {
					if (!CheckPoint(point)) { row = 2 ; init_flag = FALSE ; break ;}
					else {
						key = KEY_CTRL_F2 ;
						/* Copy point to Total SET */
						total_set.stat_point.x = point.x ;
						total_set.stat_point.y = point.y ;
						total_set.stat_point.z = point.z ;
						
						row = 0 ;
						point.x = point.y = point.z = 0.0 ;				
					}
				} 
				init_flag = TRUE ;
				break ;
			case KEY_CTRL_F2: 
				back_key = key ;
				if (init_flag == TRUE) {
					Bdisp_AreaClr_DDVRAM(&clear_area) ;
					point.x = total_set.bak_point.x ;
					point.y = total_set.bak_point.y ;
					point.z = total_set.bak_point.z ;
					PrintPt() ;
				}
				key = BakSet() ; 
				if (key == TRUE) {
					if (!CheckPoint(point))  { key = KEY_CTRL_F2 ; init_flag = FALSE ; row = 2 ; break ; }	
					else {
						key = KEY_CTRL_F3 ;
						/* Copy point to Total SET */
						total_set.bak_point.x = point.x ;
						total_set.bak_point.y = point.y ;
						total_set.bak_point.z = point.z ;
						
						row = 0 ;
						point.x = point.y = point.z = 0.0 ;
					}
				} 
				init_flag = TRUE ;
				break ;
			case KEY_CTRL_F3: 
				back_key = key ;
				total_set.setangle = sqrt( pow((total_set.bak_point.x - total_set.stat_point.x),2) 
								+pow((total_set.bak_point.y - total_set.stat_point.y),2) ); 
				if (init_flag == TRUE) {
					Bdisp_AreaClr_DDVRAM(&clear_area) ;
					point.x = total_set.station_ht ;
					point.y = total_set.prism_ht ;
					point.z = total_set.setangle ;
					PrintPt() ;
				}
				if (total_set.stat_point.x == total_set.bak_point.x &&
					total_set.stat_point.y == total_set.bak_point.y) { 
					Warning("两点相同 ", 2) ; 
					key = KEY_CTRL_F2 ;
					row = 0 ; init_flag = TRUE ; 
					break ;
				}
				key = Set() ; 
				
				if (key == TRUE)  {
					total_set.station_ht = point.x ;
					total_set.prism_ht = point.y ;	
					total_set.setangle = point.z ;
					if ( SaveStation()  != -1 )	
						Warning("已设置测站 ", 2) ;
					else Warning("设置失败 ", 2) ;
					done_flag = TRUE ;
					row = 0 ;
					point.x = point.y = point.z = 0.0 ;
				}
				init_flag = TRUE ;	
				break ;
			case KEY_CTRL_F4: 
				if ( CheckPoint(point) ) 	SavePoint(point) ;
				key = back_key ;
				init_flag = FALSE ;
				break ;
			case KEY_CTRL_F5:
				if ( LoadPoint() ) {
					switch (back_key) {
					case KEY_CTRL_F1:
						total_set.stat_point.x = point.x ;
						total_set.stat_point.y = point.y ;
						total_set.stat_point.z = point.z ;
						break ;
					case KEY_CTRL_F2:
						total_set.bak_point.x = point.x ;
						total_set.bak_point.y = point.y ;
						total_set.bak_point.z = point.z ;
						break ;
					case KEY_CTRL_F3:
						total_set.station_ht = point.x ;
						total_set.prism_ht = point.y ;
						total_set.setangle = point.z ;
						break ;
					default:
						break ;
					}
					row = 2 ;  /*  读取点号后自动跳到第三行  */
				}
				key = back_key ;
				init_flag = FALSE ;
				break ;
			case KEY_CTRL_F6:case KEY_CTRL_OPTN:
				houf_flag = TRUE ;
				break ;	
			case KEY_CTRL_EXIT: 
				exit_flag = TRUE ;
				break ;	
			}
		}
		}
		if (houf_flag == TRUE){
			row = 0 ;	
			key = 0 ;
			back_key = 0 ;
			Bdisp_AllClr_DDVRAM() ; 	/* clear screen */

			Print_zh("　后交　", 5, 0, VERT_REV) ;
			/* Menu F1:Station F2:Back F3:... */
			PrintMini(5, 58, (unsigned char*)" PT1", MINI_REV) ; 
			PrintMini(24, 58, (unsigned char*)" HD1", MINI_REV) ; 
			PrintMini(43, 58, (unsigned char*)" PT2", MINI_REV) ; 
			PrintMini(63, 58, (unsigned char*)" HD2", MINI_REV) ; 
			PrintMini(84, 58, (unsigned char*)" Load", MINI_REV) ; 
			PrintMini(105, 58, (unsigned char*)" OPTN", MINI_REV) ;	
			while (done_flag != TRUE && exit_flag != TRUE && houf_flag != FALSE) {
			switch (key) {
			case KEY_CTRL_F1: default: 
				back_key = key ;
				if (init_flag == TRUE) {
					Bdisp_AreaClr_DDVRAM(&clear_area) ;
					point.x = PTA.x ;
					point.y = PTA.y ;
					point.z = PTA.z ;
					PrintPt() ;
				}
				key = A_backcross_pos() ;
				if (key == TRUE) {
					if (!CheckPoint(point)) { key = KEY_CTRL_F1 ; row = 2 ; break ;}
					else {
						key = KEY_CTRL_F2 ;
						/* Copy point to Total SET */
						PTA.x = point.x ;
						PTA.y = point.y ;
						PTA.z = point.z ;
						
						row = 0 ;
						point.x = point.y = point.z = 0.0 ;				
					}
				} 
				init_flag = TRUE ;
				break ;
			case KEY_CTRL_F2: 
				back_key = key ;
				Bdisp_AreaClr_DDVRAM(&clear_area) ;
				Print_zh("观测值A=>置零 ", 22, 0, 0) ;

				Print_zh("镜高 ", 22, 14, 0) ;	
				Print_zh("距离 ", 22, 29, 0) ;
				Print_zh("高差 ", 22, 43, 0) ;
				if (row > 2) { 	/* 判断是否进入下一个菜单 */
					key = KEY_CTRL_F3 ; 
					row = 0 ; 
					break ;
				}
				Print_C(113, 17, A_a, 3);
				Print_C(113, 31, A_S, 3);
				Print_C(113, 45, A_h, 3);	
			
				key = InputValP(55, 15+row*14) ;
				if (number < 100000){
					switch (row) {
						case 0:   A_a = (number > 0) ? number : A_a ;break ; 
						case 1:   A_S = (number > 0) ? number : A_S ; break ; 
						case 2:   A_h = number ? number : A_h   ; break ;
						default:  break ;
					}
				}
				break ;
			case KEY_CTRL_F3: 
				back_key = key ;
				if (init_flag == TRUE) {
					Bdisp_AreaClr_DDVRAM(&clear_area) ;
					point.x = PTB.x ;
					point.y = PTB.y ;
					point.z = PTB.z ;
					PrintPt() ;
				}
				
				key = B_backcross_pos() ; 
				if (key == TRUE) {
					if (!CheckPoint(point))  { key = KEY_CTRL_F3 ; row = 2 ; break ; }	
					else {
						key = KEY_CTRL_F4 ;
						/* Copy point to Total SET */
						PTB.x = point.x ;
						PTB.y = point.y ;
						PTB.z = point.z ;
						
						row = 0 ;
						point.x = point.y = point.z = 0.0 ;
					}
				} 
				init_flag = TRUE ;
				break ;
			case KEY_CTRL_F4: 
				back_key = key ;
				Bdisp_AreaClr_DDVRAM(&clear_area) ;
				Print_zh("观测值B ", 22, 0, 0) ;

				Print_zh("角度 ", 22, 14, 0) ;	
				Print_zh("距离 ", 22, 29, 0) ;
				Print_zh("高差 ", 22, 43, 0) ;
				if (row > 2) { 	/* 判断是否进入下一个菜单 */
					row = 0 ; 
					if (CalPTC(PTA,A_a,A_S,A_h, PTB,B_a,B_S,B_h))
					{
						PTresult.z = fabs((PTA.z - A_h) + (PTB.z - B_h)) / 2 - 1 + A_a;	//虚拟测站高程 

						total_set.stat_point.x = PTresult.x ;
						total_set.stat_point.y = PTresult.y ;
						total_set.stat_point.z = PTresult.z ;
						total_set.bak_point.x = PTA.x ;
						total_set.bak_point.y = PTA.y ;
						total_set.bak_point.z = PTA.z ;
						total_set.prism_ht = A_a ;	
						total_set.station_ht = 1.0 ; //虚拟测站仪高为固定值1m
						total_set.azimuth =  atan2 ( (total_set.bak_point.y - total_set.stat_point.y) 
	      				  , (total_set.bak_point.x - total_set.stat_point.x) ) ;	//计算置角度 

						if ( SaveStation()  != -1 )	
							Warning("已设置测站 ", 2) ;
						else Warning("设置失败 ", 2) ;
						done_flag = TRUE ;
						row = 0 ;
						point.x = point.y = point.z = 0.0 ;
					}
					break ;
				}
				Print_CC_Val(113, 17, CB = B_a) ;
				Print_C(113, 31, B_S, 3);
				Print_C(113, 45, B_h, 3);	
			
				key = InputVal(55, 15+row*14) ;

				if (number < 100000){
					switch (row) {
						case 0:   B_a = (number > 0 && number <= 360) ? number : B_a ; break ; 
						case 1:   B_S = (number > 0) ? number : B_S ; break ; 
						case 2:   B_h = number ? number : B_h   ; break ;
						default:  break ;
					}
				}
				break ;
			case KEY_CTRL_F5:
				if ( LoadPoint() ) {
					switch (back_key) {
					case KEY_CTRL_F1:
						PTA.x = point.x ;
						PTA.y = point.y ;
						PTA.z = point.z ;
						break ;
					case KEY_CTRL_F3:
						PTB.x = point.x ;
						PTB.y = point.y ;
						PTB.z = point.z ;
						break ;
					default:
						break ;
					}
					row = 2 ;  /*  读取点号后自动跳到第三行  */
				}
				key = back_key ;
				init_flag = FALSE ;
				break ;
			case KEY_CTRL_UP:  
				if (row > 0)  --row ;  
				key = back_key ;
				break ;
			case KEY_CTRL_DOWN: 
			case KEY_CTRL_EXE:
				++row ; 
				key = back_key ;
				break ;
			case KEY_CTRL_DEL:
				key = back_key ;
				break ;
			case KEY_CTRL_F6:case KEY_CTRL_OPTN:
				houf_flag = FALSE ;
				break ;	
			case KEY_CTRL_EXIT: 
				exit_flag = TRUE ;
				break ;	
			}
		}
		}
	}
	return done_flag ? TRUE : -1 ;
}

int StatSet() 
{
	Print_zh("测站点 ", 22, 2, 0) ;
	PrintXY(25, 18, (unsigned char*)"X :", 0) ;
	PrintXY(25, 32, (unsigned char*)"Y :", 0) ;
	PrintXY(25, 46, (unsigned char*)"Z :", 0) ;
	return PointSet() ;
}


int BakSet()
{
	Print_zh("后视点 ", 22, 2, 0) ;
	PrintXY(25, 18, (unsigned char*)"X :", 0) ;
	PrintXY(25, 32, (unsigned char*)"Y :", 0) ;
	PrintXY(25, 46, (unsigned char*)"Z :", 0) ;
	return PointSet() ;
}

int Set()
{	
	total_set.azimuth =  atan2 ( (total_set.bak_point.y - total_set.stat_point.y) 
	      				  , (total_set.bak_point.x - total_set.stat_point.x) ) ;
	angel = total_set.azimuth ;
	Print_Angel(39, 5) ;
	

	Print_zh("仪高 ", 22, 14, 0) ;
	Print_zh("镜高 ", 22, 28, 0) ;
	Print_zh("距离 ", 22, 42, 0) ;
	return PointSet() ;
}

static void copynum(BYTE row)
{
	switch (row) {
	case 0: if (number) point.x = number ; break ; 
	case 1: if (number) point.y = number ; break ; 
	case 2: if (number) point.z = number ; break ; 
	default: break ; 
	}
}

int PointSet() 
{
	int set ;

	while (row < 3 ) {
		set = InputVal(55, 15+row*14) ;
		copynum(row) ;
		switch (set) {
		case KEY_CTRL_UP:  
			if (row >= 1)  --row ;  break ;
		case KEY_CTRL_DOWN: case KEY_CTRL_EXE:
			++row ;  break ;
		case KEY_CTRL_DEL:
			switch (row) {
			case 0: point.x = 0.0 ; break ; 
			case 1: point.y = 0.0 ; break ; 
			case 2: point.z = 0.0 ; break ; 
			default: break ; 
			}  break ;
		case KEY_CTRL_EXIT: case KEY_CTRL_OPTN:  
		case KEY_CTRL_F1: case KEY_CTRL_F2: 
		case KEY_CTRL_F3: case KEY_CTRL_F4:
		case KEY_CTRL_F5: case KEY_CTRL_F6: 
			return set ; break ;
		default:  break ;
		}		
	} 
	return TRUE ;
}


int SavePoint(S_POINT point)
{
	unsigned int key ;
	int file_handle = 0 ;
	char wr_buf[29] ;  
	char d_buf[24] ;
	char* buf = d_buf ;
	BYTE step ;
	
	memset(wr_buf, 0, 29) ;  
	memset(d_buf, 0, 24) ;  
	SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */
	PopUpWin(3) ;
	Print_zh("输入点号: ", 15, 22, 0) ;
	key = InputStr(80, 25) ;
	if (key == KEY_CTRL_EXE && ipstr[0] == 0)
		key = KEY_CTRL_EXIT ;
	switch (key) {
	case KEY_CTRL_EXE:	
		file_handle = SeekPoint() ;
		if (file_handle) {	
			/* 点号(5字节)+浮点数(8字节)*3 */
			strncat(wr_buf, (char*)&ipstr, 5) ;
			*((double*)buf) = point.x ;  buf += sizeof(double) ;
			*((double*)buf) = point.y ;  buf += sizeof(double) ;
			*((double*)buf) = point.z ;
			for (step = 0 ; step < 24 ; ++step)
				wr_buf[step+5] = d_buf[step] ;
			if (Bfile_WriteFile(file_handle, wr_buf, 5+sizeof(double)*3) >= 0) 
				Warning("点号已保存 ", 2) ;
			else Warning("保存失败 ", 2) ;
			Bfile_CloseFile(file_handle) ;
		}
		break ;
	case KEY_CTRL_EXIT:
	default: break ;
	}
	
	RestoreDisp(SAVEDISP_PAGE2) ;
	Bdisp_PutDisp_DD() ;
	return key ;
}

int LoadPoint()
{
	int handle ;
	
	SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */			
	handle = SelectPoint() ;
	RestoreDisp(SAVEDISP_PAGE2) ;
	Bdisp_PutDisp_DD() ;
	if (handle >= 0) {
		Bfile_ReadFile(handle, &point.x, sizeof(double), 5) ; 
		Bfile_ReadFile(handle, &point.y, sizeof(double), 5+sizeof(double)) ; 
		Bfile_ReadFile(handle, &point.z, sizeof(double), 5+sizeof(double)*2) ; 
		Bfile_CloseFile(handle) ;
		PrintPt() ;
		return TRUE ;
	}
	return FALSE ;
}

int StationHelp()
{
	unsigned int key ;

	SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */
	Bdisp_AreaClr_DDVRAM(&clear_area) ;

	Print_zh("功能键控制 ", 30, 8, 0) ;
	Print_zh("多点号管理 ", 30, 22, 0) ;
	Print_zh("任意键退出 ", 30, 36, 0) ;
	GetKey(&key) ;
	RestoreDisp(SAVEDISP_PAGE2) ;
	Bdisp_PutDisp_DD() ;
	return ;
}


/* 后交设置 */

int A_backcross_pos() 
{
	Print_zh("后交点A ", 22, 2, 0) ;
	PrintXY(25, 18, (unsigned char*)"X :", 0) ;
	PrintXY(25, 32, (unsigned char*)"Y :", 0) ;
	PrintXY(25, 46, (unsigned char*)"Z :", 0) ;
	
	return PointSet() ;
}

int B_backcross_pos() 
{
	Print_zh("后交点B ", 22, 2, 0) ;
	PrintXY(25, 18, (unsigned char*)"X :", 0) ;
	PrintXY(25, 32, (unsigned char*)"Y :", 0) ;
	PrintXY(25, 46, (unsigned char*)"Z :", 0) ;
	
	return PointSet() ;
}
