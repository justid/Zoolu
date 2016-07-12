/*
	测量点 Survey
	Last update: 2011.2.07
*/

# include "9860g.h"

static char sd_fname[5] = { 0,0,0,0,0 };
static unsigned int file_count = 0;

int File_SAVE(void)
{
	unsigned int key, key2 ;
	FONTCHARACTER folder_crd[50];
	char folder_name[50];
	BYTE exit_flag = FALSE ;

	Bdisp_AreaClr_DDVRAM(&clear_area) ;
	locatestr(23,22) ;  printf("F5-新建 ") ;
	
	do
	{
		locatestr(23,5) ;	printf("文件夹:%s(SD)",sd_fname) ;
		locatestr(23,39) ;  printf("当前数据[%d]条 ", file_count) ;
		GetKey(&key) ;
		switch (key) {
		case KEY_CTRL_F5 :
			SaveDisp(SAVEDISP_PAGE2) ;		/* Save screen */
			PopUpWin(3) ;
			Print_zh("数据名: [    ]", 15, 21, 0) ;
			key2 = InputStr(80, 25) ;
			if (ipstr[0] != 0) {
				sprintf(folder_name,"\\\\crd0\\%s",ipstr);
				strcpy(sd_fname,ipstr) ;
				CharToFont(folder_name, folder_crd) ;
				Bfile_CreateDirectory(folder_crd) ;
			}
			RestoreDisp(SAVEDISP_PAGE2) ;
			break ;
		default:
			exit_flag = TRUE ;
			break ;
		}
		if (sd_fname[0] != 0)
		{
			sprintf(folder_name,"\\\\crd0\\%s",sd_fname);
			file_count = Count_file_num(folder_name) ;
		}
	}
	while (exit_flag != TRUE);
	return key;
}


int Survey(void)
{
	unsigned int key = 0 ;
	unsigned int back_key ;
	BYTE exit_flag = FALSE ;
	BYTE xyrow = 0 ;

	double a = 0.0 ;	/*  方位角  */
	double S = 0.0 ;	/*  边长  */
	double h = 0.0 ;	/*  高差  */
	S_POINT final_p = {0.0, 0.0, 0.0} ;
	double final_a = 0.0 ;

	double back_x=0 ;
	double back_y=0 ;
	double back_z=0 ;
	
	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */	
	Print_zh("　测点　", 5, 0, VERT_REV) ;
	/* Menu F1, F2, F3:... */
	PrintMini(5, 58, (unsigned char*)" F1 ", MINI_REV) ; 
	PrintMini(24, 58, (unsigned char*)" GO ", MINI_REV) ; 
	//PrintMini(42, 58, (unsigned char*)" GO ", MINI_REV) ; 
	PrintMini(56, 58, (unsigned char*)" Save ", MINI_REV) ; 
	PrintMini(83, 58, (unsigned char*)" FILE ", MINI_REV) ; 
	
	PrintMini(110, 58, (unsigned char*)" ? ", MINI_REV) ; 
	
	while (exit_flag != TRUE)  {
		switch(key) {
		case KEY_CTRL_F1: default: 
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			
			Print_zh("角度 ", 24, 6, 0) ;	
			Print_zh("距离 ", 24, 21, 0) ;
			Print_zh("高差 ", 24, 35, 0) ;
			if (xyrow > 2) { 	/* 判断是否进入下一个菜单 */
				key = KEY_CTRL_F2 ; 
				xyrow = 0 ; 
				break ;
			 }
			Print_CC_Val(108, 11, CB = a) ;
			Print_C_Val(CA = S, CB = h) ;
			
			key = InputVal(55, 8+xyrow*14) ;
			if (number < 100000){
			switch (xyrow) {
			case 0:   a = (number > 0 && number <= 360) ? number : a ; break ; 
			case 1:   S = (number > 0) ? number : S ; break ; 
			case 2:   h = number ? number : h ;  ; break ;
			default:  break ;
			}
			}
			break ;
		case KEY_CTRL_F2:
			/* 检查数据 */
			if (a > 360.0) {
				xyrow = 0 ;
				Warning("角度过大\0", 2) ;
				key = back_key ;
				break ;
			}
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			
			PrintXY(25, 10, (unsigned char*)"X :", 0) ;
			PrintXY(25, 24, (unsigned char*)"Y :", 0) ;
			PrintXY(25, 38, (unsigned char*)"Z :", 0) ;
			if (sd_fname[0] != 0)
			{
				Print_zh("保\n存 ", 114, 8, 0) ;
				PrintMini(110, 39, (unsigned char*)" EXE ", MINI_REV) ;
				locatestr(127 - ((int)log10(file_count) + 1)*5-15,43) ;  printf("[%d]", (file_count == 0) ? file_count : file_count-1) ;
			}
			
			
			final_a = (25 * a - 6 *(int)a - 10 * Cut_2(a)) / 9 * PI / 180 + total_set.azimuth ;
			final_p.x = Cut_3(total_set.stat_point.x + S * cos(final_a)) ;
			final_p.y = Cut_3(total_set.stat_point.y+ S * sin(final_a)) ;
			final_p.z = total_set.stat_point.z + total_set.station_ht + h - total_set.prism_ht ;

			Print_C(108, 10, CB = final_p.x, 3) ;
			Print_C(108, 24, CA = final_p.y, 3);
			Print_C(108, 38, CB = final_p.z, 3);				
			
			GetKey(&key) ;
			if (sd_fname[0] != 0 && key == KEY_CTRL_EXE && 
				(final_p.x != back_x || final_p.y != back_y || final_p.z != back_z))	//不与上一点相同 
			{
				if (Save2SD(final_p.x,final_p.y,final_p.z)!= 0)
					Warning("保存失败",2) ;
				else {
					back_x = final_p.x ;
					back_y = final_p.y ;
					back_z = final_p.z ;
				}
			}
			xyrow = 0;
			if  (key == KEY_CTRL_EXIT)	exit_flag = TRUE ;
			break ;
		case KEY_CTRL_F4:
			if ( CheckPoint(final_p) ) 	SavePoint(final_p) ; 
			key = back_key ;
			break ;
		case KEY_CTRL_F3:  
			key = back_key ;
			break ;
		case KEY_CTRL_F5:
			if (has_SDcard() == TRUE)
			{
				key = File_SAVE() ;
			}
			else {
				Warning("无SD卡 ",2) ;
				key = back_key ;
			}
			break ;	
		case KEY_CTRL_F6:
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			locatestr(22,6) ;
			printf("F4保存为控制点 \nF5保存到SD卡 ") ;
			GetKey(&key) ;
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
			
			break ;
		}
	}
	return ;
}

int Save2SD(double s_x, double s_y,double s_z)
{
	char folder_name[50];
	char temp_str_buff[45];
	FONTCHARACTER folder_crd[50];
	BYTE flag = -1 ;
	int temp_handle ;

	sprintf(folder_name,"\\\\crd0\\%s\\%u.txt",sd_fname,file_count);
	CharToFont(folder_name, folder_crd) ;

	if (Bfile_CreateFile(folder_crd,100) == 0)
	{
		sprintf(temp_str_buff,"%d,00000000,%.3f,%.3f,%.3f\r\n",file_count,s_y,s_x,s_z) ;
		temp_handle = Bfile_OpenFile(folder_crd,_OPENMODE_READWRITE) ;
		if (temp_handle >= 0)
		{
			if (Bfile_WriteFile(temp_handle,temp_str_buff,strlen(temp_str_buff)) >= 0)
			{
				//成功保存后更新数据条目 
				sprintf(folder_name,"\\\\crd0\\%s",sd_fname);
				file_count = Count_file_num(folder_name) ;
				flag = 0 ;
			} 
			Bfile_CloseFile(temp_handle) ;
		}
	} 

	return flag ;
}



int Count_file_num(char * folder)
{
	FONTCHARACTER find_path[50];
	FONTCHARACTER find_name[50];
	FILE_INFO file_info;
	int find_h;
	int size = 0;

	int iError=0;
	int iFirst=0;

	//	Get File count	
	strncat( folder,"\\*.txt", 6) ;
	CharToFont( folder, find_path);
	do
	{
	  if(iFirst==0) {
        iError=Bfile_FindFirst(find_path,&find_h,find_name,&file_info);
        iFirst=1;
      } 
	  else {
        iError=Bfile_FindNext(find_h,find_name,&file_info);
      }
	  if(iError == 0)	 ++size ;
	}
	while (iError == 0);
	Bfile_FindClose(find_h);

	return size;
}