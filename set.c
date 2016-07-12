/*
	绘星设置模块
	Last update: 2011.2.07
*/

# include "9860g.h"

extern struct tag_TOTAL total_set = {  {0.0, 0.0, 0.0},
						    {0.0, 0.0, 0.0},
						     0.0, 0.0, 0.0, 0.0,
						    {0,0,0,0,0,0,0,0} } ;
extern struct tag_SET allset = { 0.0, 0.0, 0.0, 0.0, {0,0,0,0} };



static char m_id[9] ;	/*  8 bytes starting at 0x8000FFD0   */	

int Get_MID()
{
	DWORD id_pointer = 0x8000FFD0 ;		
	char temp_step ;
	
	//memset(m_id, 0, 9) ;  
	for (temp_step = 0 ; temp_step < 8 ; ++temp_step)
		m_id[temp_step] = *(char*)(id_pointer + temp_step) ;
	# ifdef PC_TEST
		strcpy((char*)&m_id, "12345678") ;
	# endif
	return ;
}

static char regstr[9] = {0, 0, 0, 0, 0, 0, 0, 0, '\0'} ;

int Input_Regcode()
{
	unsigned int key ;
	
	BYTE pos = 0 ; 
	BYTE str_flag = FALSE ;
	int x = 58 ;
	int y = 26 ;
	int ptr = -1 ;

	PopUpWin(3) ;
	locatestr(15,22);
	printf("机器码%s",(unsigned char*)m_id) ;

	GetKey(&key) ;
	Bdisp_AllClr_DDVRAM() ;
	PopUpWin(3) ;
	printf("注册码 ") ;
	DrawRect(56,23,12,53) ;
	while(str_flag != TRUE)
	{
		GetKey(&key) ;
		switch (key) {
		case KEY_CTRL_EXE: 
			regstr[pos] = '\0' ;
			str_flag =TRUE ;
			break ;
		case  KEY_CTRL_ALPHA:
			break ;
		case KEY_CTRL_DEL:
			if (pos)  regstr[--pos] = '\0' ; 
			PrintXY(x, y, (unsigned char *)"        ", 0) ;
			break ;
		default: break ;
		}
		/* Numbers and characters */
		if ( (pos < 8) &&
		     ((key >= KEY_CHAR_0 && key <= KEY_CHAR_9) || 
		      (key >= KEY_CHAR_A && key <= KEY_CHAR_Z)) ) {
			regstr[pos] = (char)key ;
			++pos ;
		}
		PrintXY(x, y, (unsigned char *)regstr, 0) ;
	}
	

	return ;
}




BOOL CheckReg(char* reg_code)
{
	char mix_code[9] = {0, 0, 0, 0, 0, 0, 0, 0, '\0'} ;
	char final_code[9] = {0, 0, 0, 0, 0, 0, 0, 0, '\0'} ;
	BYTE step ;
	
	
	for (step = 0 ; step < 8 ; ++step)
		mix_code[step] = m_id[7 - step] ^ (0x6E + step * 2) ;
	sprintf(final_code, "%02X%02X%02X%02X", mix_code[0],  mix_code[3],  mix_code[5],  mix_code[6]) ;
	if ( strcmp(reg_code, final_code) == 0) 
		return TRUE ;
	else {
		DebugS("可免费使用\n无功能限制 ") ;
		//Warning("注册码错误!",2);
		memset(regstr,'\0', 8);  
		Input_Regcode() ; 
		if ( strcmp(regstr, final_code) == 0) {
			Warning("注册成功!",2);
			return TRUE ;
		}
		else	DebugS("+Q群106061748\n获取注册支持 ") ;
		return FALSE ;
	}
}

# ifdef RELEASE
FONTCHARACTER configfile[] = { '\\','\\','f','l','s','0','\\','C','O','N','F','I','G','.','i','n','i' ,'\0'} ;
# else
FONTCHARACTER configfile[] = { '\\','\\','c','r','d','0','\\','C','O','N','F','I','G','.','i','n','i' ,'\0'} ;
# endif

unsigned int lineno ;
int fpos ;  //文件指针记录
int stateT,stateK,stateV ;	//解析状态，T标题，K关键字，V值


int InitConfig()
{
# ifdef RELEASE
FONTCHARACTER indexfile[] = { '\\','\\','f','l','s','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
# else
FONTCHARACTER indexfile[] = { '\\','\\','c','r','d','0','\\','I','N','D','E','X','.','d','a','t' ,'\0'} ;
# endif
	int indexf_ptr = -1 ;
	int confile_ptr = -1 ;
	unsigned int prog = 0 ;
	int error_code = 0 ;
	double ini_version ;
	unsigned int ini_fpos ;
	
	
	//Get_MID() ;CheckReg("ABCDEF");
	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READWRITE) ;
	if (confile_ptr < 0) {
		Warning("无配置文件 ", 3) ;
		return -1 ;
	} else {
		// DO NOTHING NOW
	}

	

	fpos = 0 ;
	stateT = 0 ;
	stateK = 0 ;
	stateV = 0 ;
	
	
	while (error_code == 0  && getline(confile_ptr) ) {
		prog = parse((char*)Linebuffer) ;
		switch (prog){
		case PROG_VER:
			ini_version = statebox.value ;
			ini_fpos = fpos ;
			break ;
		case PROG_SN:
			Get_MID() ;
			strcpy(regstr,statebox.str) ;
			if (CheckReg(regstr) == TRUE)  {
				if (strcmp(regstr,statebox.str) != 0 ){
					//定位到数值位置并写入 
					sprintf(Linebuffer,"SN = %s\r\n",regstr) ;
					confile_ptr = WriteINI(confile_ptr,fpos,Linebuffer,NOW,TRUE) ;	
					if (confile_ptr < 0 )  error_code = -1 ;
				}
			}
			break ;
		case PROG_X1:
			total_set.stat_point.x = statebox.value ;
			total_set.fpos.x1 = fpos ;
			break ;
		case PROG_Y1:
			total_set.stat_point.y = statebox.value ; 
			total_set.fpos.y1 = fpos ;
			break ;
		case PROG_H1:
			total_set.stat_point.z = statebox.value ; 
			total_set.fpos.h1 = fpos ;
			break ;
		case PROG_X2:
			total_set.bak_point.x = statebox.value ; 
			total_set.fpos.x2 = fpos ;
			break ;
		case PROG_Y2:
			total_set.bak_point.y = statebox.value ; 
			total_set.fpos.y2 = fpos ;
			break ;
		case PROG_H2:
			total_set.bak_point.z = statebox.value ; 
			total_set.fpos.h2 = fpos ;
			break ;
		case PROG_STH:
			total_set.station_ht = statebox.value ; 
		        total_set.fpos.sth = fpos ;
			break ;
		case PROG_PRH:
			total_set.prism_ht = statebox.value ; 
			total_set.fpos.prh = fpos ;
			break ;
		case PROG_STAKED:
			allset.stake_d = statebox.value ; 
			allset.fpos.sk = fpos ;
			break ;
		case PROG_WDMAX:
			allset.width_max = statebox.value ; 
			allset.fpos.wd = fpos ;
			break ;
		case PROG_XLTYPE:
			allset.xl_type = statebox.value ; 
			allset.fpos.xl = fpos ;
			break; 
		case PROG_PJMODE:
			allset.pj_mode = statebox.value ; 
			allset.fpos.pj = fpos ;
			break ;
		case PROG_CUPQ:	//平曲
			if (strcmp(statebox.str,"PQ1") == 0)  dbset.current_pq = dbset.pq1 ;
			else if (strcmp(statebox.str,"PQ2") == 0) dbset.current_pq = dbset.pq2 ;
			else if (strcmp(statebox.str,"PQ3") == 0) dbset.current_pq = dbset.pq3 ;
			else if (strcmp(statebox.str,"PQ4") == 0) dbset.current_pq = dbset.pq4 ;
			else if (strcmp(statebox.str,"PQ5") == 0) dbset.current_pq = dbset.pq5 ;
			else if (strcmp(statebox.str,"EMP") == 0) dbset.current_pq = NULL ;
			else error_code = -1 ;
			
			dbset.fpos.cupq = fpos ;
			break ;
		case PROG_PQ1:
			strcpy(dbset.pq1,statebox.str) ;
			dbset.fpos.pq1 = fpos ;
			break ;
		case PROG_PQ2:
			strcpy(dbset.pq2,statebox.str) ;
			dbset.fpos.pq2 = fpos ;
			break ;
		case PROG_PQ3:
			strcpy(dbset.pq3,statebox.str) ;
			dbset.fpos.pq3 = fpos ;
			break ;
		case PROG_PQ4:
			strcpy(dbset.pq4,statebox.str) ;
			dbset.fpos.pq4 = fpos ;
			break ;
		case PROG_PQ5:
			strcpy(dbset.pq5,statebox.str) ;
			dbset.fpos.pq5 = fpos ;
			break ;
		case PROG_CUSQ:	//竖曲
			if (strcmp(statebox.str,"SQ1") == 0)  dbset.current_sq = dbset.sq1 ;
			else if (strcmp(statebox.str,"SQ2") == 0) dbset.current_sq = dbset.sq2 ;
			else if (strcmp(statebox.str,"SQ3") == 0) dbset.current_sq = dbset.sq3 ;
			else if (strcmp(statebox.str,"SQ4") == 0) dbset.current_sq = dbset.sq4 ;
			else if (strcmp(statebox.str,"SQ5") == 0) dbset.current_sq = dbset.sq5 ;
			else if (strcmp(statebox.str,"EMP") == 0) dbset.current_sq = NULL ;
			else error_code = -1 ;
			
			dbset.s_fpos.cusq = fpos ;
			break ;
		case PROG_SQ1:
			strcpy(dbset.sq1,statebox.str) ;
			dbset.s_fpos.sq1 = fpos ;
			break ;
		case PROG_SQ2:
			strcpy(dbset.sq2,statebox.str) ;
			dbset.s_fpos.sq2 = fpos ;
			break ;
		case PROG_SQ3:
			strcpy(dbset.sq3,statebox.str) ;
			dbset.s_fpos.sq3 = fpos ;
			break ;
		case PROG_SQ4:
			strcpy(dbset.sq4,statebox.str) ;
			dbset.s_fpos.sq4 = fpos ;
			break ;
		case PROG_SQ5:
			strcpy(dbset.sq5,statebox.str) ;
			dbset.s_fpos.sq5 = fpos ;
			break ;
		case PROG_CUPLD:
			if (strncmp(statebox.str,"\\\\",2) == 0) {
				CharToFont(statebox.str,pld_FileName) ;
				pld_flag = TRUE ;
				//从索引文件中获取数据行数 
				indexf_ptr = Bfile_OpenFile(indexfile, _OPENMODE_READ_SHARE) ;
				ptlist_count = Bfile_GetFileSize(indexf_ptr) /  sizeof(DWORD) ;
				Bfile_CloseFile(indexf_ptr) ;
				
				//从路径中提取文件名 
				GetNamefromStr(name,statebox.str) ;
				
				//DebugS("%s",statebox.str) ;
			} else pld_flag = FALSE ;
			dbset.pld_pos = fpos ;
			break ;			
		case PROG_CUYS:	//线元法
			if (strcmp(statebox.str,"YS1") == 0)  dbset.current_ys = dbset.ys1 ;
			else if (strcmp(statebox.str,"YS2") == 0)  dbset.current_ys = dbset.ys2 ;
			else if (strcmp(statebox.str,"YS3") == 0)  dbset.current_ys = dbset.ys3 ;
			else if (strcmp(statebox.str,"YS4") == 0)  dbset.current_ys = dbset.ys4 ;
			else if (strcmp(statebox.str,"YS5") == 0)  dbset.current_ys = dbset.ys5 ;
			else if (strcmp(statebox.str,"YS6") == 0)  dbset.current_ys = dbset.ys6 ;
			else if (strcmp(statebox.str,"YS7") == 0)  dbset.current_ys = dbset.ys7 ;
			else if (strcmp(statebox.str,"YS8") == 0)  dbset.current_ys = dbset.ys8 ;
			else if (strcmp(statebox.str,"EMP") == 0) dbset.current_ys = NULL ;
			else error_code = -1 ;
			
			dbset.ys_fpos.cuys = fpos ;
			break ;
		case PROG_YS1:
			strcpy(dbset.ys1,statebox.str) ;
			dbset.ys_fpos.ys1 = fpos ;
			break ;
		case PROG_YS2:
			strcpy(dbset.ys2,statebox.str) ;
			dbset.ys_fpos.ys2 = fpos ;
			break ;
		case PROG_YS3:
			strcpy(dbset.ys3,statebox.str) ;
			dbset.ys_fpos.ys3 = fpos ;
			break ;
		case PROG_YS4:
			strcpy(dbset.ys4,statebox.str) ;
			dbset.ys_fpos.ys4 = fpos ;
			break ;
		case PROG_YS5:
			strcpy(dbset.ys5,statebox.str) ;
			dbset.ys_fpos.ys5 = fpos ;
			break ;
		case PROG_YS6:
			strcpy(dbset.ys6,statebox.str) ;
			dbset.ys_fpos.ys6 = fpos ;
			break ;
		case PROG_YS7:
			strcpy(dbset.ys7,statebox.str) ;
			dbset.ys_fpos.ys7 = fpos ;
			break ;
		case PROG_YS8:
			strcpy(dbset.ys8,statebox.str) ;
			dbset.ys_fpos.ys8 = fpos ;
			break ;
		case PROG_CUHP:
			if (strcmp(statebox.str,"EMP") == 0)  ;
			else strcpy(dbset.current_hp,statebox.str) ;
			dbset.hp_pos = fpos ;
			break ;
		case PROG_CUSD:
			if (strcmp(statebox.str,"SD1") == 0)  dbset.current_sd = dbset.sd1 ;
			else if (strcmp(statebox.str,"SD2") == 0)  dbset.current_sd = dbset.sd2 ;
			else if (strcmp(statebox.str,"SD3") == 0)  dbset.current_sd = dbset.sd3 ;
			else if (strcmp(statebox.str,"SD4") == 0)  dbset.current_sd = dbset.sd4 ;
			else if (strcmp(statebox.str,"SD5") == 0)  dbset.current_sd = dbset.sd5 ;
			else if (strcmp(statebox.str,"SD6") == 0)  dbset.current_sd = dbset.sd6 ;
			else if (strcmp(statebox.str,"SD7") == 0)  dbset.current_sd = dbset.sd7 ;
			else if (strcmp(statebox.str,"SD8") == 0)  dbset.current_sd = dbset.sd8 ;
			else if (strcmp(statebox.str,"EMP") == 0) dbset.current_sd = NULL ;
			else error_code = -1 ;
			dbset.sd_fpos.cusd = fpos ;
			break ;
		case PROG_SD1:
			strcpy(dbset.sd1,statebox.str) ;
			dbset.sd_fpos.sd1 = fpos ;
			break ;
		case PROG_SD2:
			strcpy(dbset.sd2,statebox.str) ;
			dbset.sd_fpos.sd2 = fpos ;
			break ;
		case PROG_SD3:
			strcpy(dbset.sd3,statebox.str) ;
			dbset.sd_fpos.sd3 = fpos ;
			break ;
		case PROG_SD4:
			strcpy(dbset.sd4,statebox.str) ;
			dbset.sd_fpos.sd4 = fpos ;
			break ;
		case PROG_SD5:
			strcpy(dbset.sd5,statebox.str) ;
			dbset.sd_fpos.sd5 = fpos ;
			break ;
		case PROG_SD6:
			strcpy(dbset.sd6,statebox.str) ;
			dbset.sd_fpos.sd6 = fpos ;
			break ;
		case PROG_SD7:
			strcpy(dbset.sd7,statebox.str) ;
			dbset.sd_fpos.sd7 = fpos ;
			break ;
		case PROG_SD8:
			strcpy(dbset.sd8,statebox.str) ;
			dbset.sd_fpos.sd8 = fpos ;
			break ;
		case PROG_ERR:
			break ;
		default:
			break ;
		}		
		fpos += strlen(Linebuffer);
	}
	if (ini_version == 1.11) {
		Warning("当前ver1.11",2) ;
		DebugS("按EXE确认升级\nini配置文件 ") ;
		if ( upgradeINI(1.11,confile_ptr,ini_fpos) == TRUE ) Warning("升级成功!",2) ;
		else Warning("升级失败!",2) ;
		error_code = -1 ;		//不管升级成功还是失败 均返回程序开始
	}
	else if (ini_version == 1.13) {
		Warning("当前ver1.13",2) ;
		DebugS("按EXE确认升级\nini配置文件 ") ;
		if ( upgradeINI(1.13,confile_ptr,ini_fpos) == TRUE ) Warning("升级成功!",2) ;
		else Warning("升级失败!",2) ;
		error_code = -1 ;		//不管升级成功还是失败 均返回程序开始
	}
	else if (ini_version == 1.17){
		Warning("当前ver1.17",2) ;
		DebugS("按EXE确认升级\nini配置文件 ") ;
		if ( upgradeINI(1.17,confile_ptr,ini_fpos) == TRUE ) Warning("升级成功!",2) ;
		else Warning("升级失败!",2) ;
		error_code = -1 ;		//不管升级成功还是失败 均返回程序开始
	}
	else if (ini_version == 1.171) {
		Warning("当前ver1.171",2) ; 
		DebugS("按EXE确认升级\nini配置文件 ") ;
		if ( upgradeINI(1.171,confile_ptr,ini_fpos) == TRUE ) Warning("升级成功!",2) ;
		else Warning("升级失败!",2) ;
		error_code = -1 ;		//不管升级成功还是失败 均返回程序开始
	}
	else if (ini_version == 1.172) {
		Warning("当前ver1.172",2) ; 
		DebugS("按EXE确认升级\nini配置文件 ") ;
		if ( upgradeINI(1.172,confile_ptr,ini_fpos) == TRUE ) Warning("升级成功!",2) ;
		else Warning("升级失败!",2) ;
		error_code = -1 ;		//不管升级成功还是失败 均返回程序开始
	}
	else if (ini_version == 1.19) ;//Warning("当前ver1.19",2) ; 
	else Warning("ini版本错误 ",2) ;
	
	Bfile_CloseFile(confile_ptr) ;
	
	return  error_code;
}

int GetNamefromStr(name,str)
{
	char* beg  = NULL ;
	char* end  = NULL ;

	beg = strrchr(statebox.str,'\\') ;
	end = strrchr(statebox.str,'v') ;
	if (beg == NULL || end == NULL) 
		return -1 ;
	else 
		memcpy((void*)name,(char*)(beg + 1),end - beg ) ;
	return 0;
}

int upgradeINI(double version, int file_ptr, int pos)
{
void* rebuff_ptr = NULL;
void* buff_prev = NULL;
void* buff_next = NULL;

int size ;
int bytesfree ;
char* Addto ;
BYTE insert_flag = FALSE ;
char Addto113[] = "Current_PLD = \\;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\r\n\r\n" ;
char Addto117[] = "Current_YS = EMP ;\r\nYS1 =  XYF ;\r\nYS2 = BBBB ;\r\nYS3 = CCCC ;\r\nYS4 = DDDD ;\r\nYS5 = EEEE ;\r\nYS6 = FFFF ;\r\nYS7 = GGGG ;\r\nYS8 = HHHH ;\r\n\r\n" ;
char Addto1171[] = "XLTYPE =     1.000 ;\r\nPJMODE =     1.000 ;\r\n" ;
char Addto1172[] = "Current_HP =  EMP ;" ;
char Addto119[] = "\r\n\r\nCurrent_SD = EMP ;\r\nSD1 = AAAA ;\r\nSD2 = BBBB ;\r\nSD3 = CCCC ;\r\nSD4 = DDDD ;\r\nSD5 = EEEE ;\r\nSD6 = FFFF ;\r\nSD7 = GGGG ;\r\nSD8 = HHHH ;" ;

Warning("升级中..",2) ;
file_ptr = Reopen(file_ptr) ;

if (version == 1.11){
	Addto = Addto113 ;
	WriteINI(file_ptr, pos, "Version =     1.130 ;",NOW,TRUE) ;
} else if (version == 1.13){
	Addto = Addto117 ;
	WriteINI(file_ptr, pos, "Version =     1.170 ;",NOW,TRUE) ;	
} else if (version == 1.17){
	Addto = Addto1171 ;
	WriteINI(file_ptr, pos, "Version =     1.171 ;",NOW,TRUE) ;
	insert_flag = TRUE ;	
} else if (version == 1.171){
	Addto = Addto1172 ;
	WriteINI(file_ptr, pos, "Version =     1.172 ;",NOW,TRUE) ;
} else if (version == 1.172){
	Addto = Addto119 ;
	WriteINI(file_ptr, pos, "Version =     1.190 ;",NOW,TRUE) ;
} else {
	Warning("错误的版本号!",2) ;
	return FALSE ;
}

//增加ini文件大小
size = Bfile_GetFileSize(file_ptr) ;
rebuff_ptr = calloc( size+ strlen(Addto),sizeof(char)) ;
if (insert_flag == TRUE){
		buff_prev = calloc( allset.fpos.sk,sizeof(char)) ;	
		buff_next = calloc( size-allset.fpos.sk,sizeof(char)) ;
		if (buff_prev == NULL || buff_next == NULL){
			if (rebuff_ptr != NULL) free(rebuff_ptr) ;
			DebugS("申请内存失败!") ;
			return FALSE ;
		}
}
else if (rebuff_ptr == NULL){
	DebugS("申请内存失败!") ;
	return FALSE ;
}
if (insert_flag == TRUE){
	Bfile_ReadFile(file_ptr,buff_prev,allset.fpos.sk ,-1) ;	//读取前半部分
	Bfile_ReadFile(file_ptr,buff_next,size-allset.fpos.sk ,-1) ;	//读取后半部分	
}
else Bfile_ReadFile(file_ptr,rebuff_ptr,size ,-1) ;

Bfile_CloseFile(file_ptr) ;
Bfile_DeleteFile(configfile) ;


if (Bfile_CreateFile(configfile, size+strlen(Addto)) < 0) {
	if (rebuff_ptr != NULL)	free(rebuff_ptr) ;
	if (buff_prev != NULL)	free(buff_prev) ;
	if (buff_next != NULL)	free(buff_next) ;
	DebugS("创建文件失败\n请重新上传 ") ;
	return FALSE ;
}
file_ptr = Reopen(file_ptr) ;
if (insert_flag == TRUE) {
	strncat(rebuff_ptr,buff_prev,strlen(buff_prev)) ;		//拼接
	strncat(rebuff_ptr,Addto,strlen(Addto)) ;	
	strncat(rebuff_ptr,buff_next,strlen(buff_next)) ;		
}
else strncat(rebuff_ptr,Addto,strlen(Addto)) ;
Bfile_WriteFile(file_ptr,rebuff_ptr,size+strlen(Addto)) ;
		
if (rebuff_ptr != NULL)	free(rebuff_ptr) ;
if (buff_prev != NULL)	free(buff_prev) ;
if (buff_next != NULL)	free(buff_next) ;

Bfile_CloseFile(file_ptr) ;

return TRUE ;	
}



int Reopen(int file_ptr)
{
	Bfile_CloseFile(file_ptr) ;
	return Bfile_OpenFile(configfile, _OPENMODE_READWRITE_SHARE) ;
}

static void* write_ptr ;
static int filesize ;
int WriteINI(int file_ptr,int pos,char* Write,BYTE delay,BOOL init)
{
	int bytesfree ;

	if  (init ==  TRUE) {
		filesize = Bfile_GetFileSize(file_ptr) ;
		if (filesize < 0){
			DebugS("获取文件大小\n失败! ") ;
			return -1 ;
		}
	
		write_ptr = malloc( filesize ) ;
		if (write_ptr == NULL){
			DebugS("内存分配失败! ") ;
			return -1 ;
		}
		//Reopen and read the whole file
		file_ptr = Reopen(file_ptr) ;
		Bfile_ReadFile(file_ptr,write_ptr,filesize,-1) ;

		//Delete the file
		Bfile_CloseFile(file_ptr) ;
		//9750下无法使用Bfile_GetMediaFree，获取的闪存剩余字节总为0
		Bfile_DeleteFile(configfile) ;
	}
	
	
	memodify(write_ptr, pos, Write) ;


	if (delay == NOW) {
		//Recreate the file
		if (Bfile_CreateFile(configfile, filesize) < 0) {
			DebugS("创建文件失败\n请重新上传 ") ;
			return -1 ;
		}
		file_ptr = Reopen(file_ptr) ;
		Bfile_WriteFile(file_ptr,write_ptr,filesize) ;
		if (write_ptr != NULL) free(write_ptr) ;
	}
	return Reopen(file_ptr) ;
}

int ProjSet()
{
	unsigned int key = 0 ;
	unsigned int back_key = 0 ;
	BYTE row = 0 ;
	BYTE exit_flag = FALSE ;

	Bdisp_AllClr_DDVRAM() ; 	/* clear screen */
	Print_zh("　设置　", 2, 0, VERT_REV) ;
	PrintMini(2, 58, (unsigned char*)" F1 ", MINI_REV) ; 
	PrintMini(21, 58, (unsigned char*)" F2 ", MINI_REV) ; 
	//PrintMini(40, 58, (unsigned char*)" F3 ", MINI_REV) ; 
	PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 

	while (exit_flag != TRUE) {	
		switch (key) {
		case KEY_CTRL_F1: default: 
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			locatestr(27,2) ;printf("放样:增减桩距 ") ;
			locatestr(67,16) ;printf("%.3f", allset.stake_d) ;	
			locatestr(27,29) ;printf("反算:最大路宽 ") ;
			locatestr(67,43) ;printf("%.3f", allset.width_max) ;
			PrintXY(120, 6, (unsigned char*)"\xE6\x9C ", 0) ;
			PrintXY(120, 49, (unsigned char*)"\xE6\x9D ", 0) ;
			if (row > 1) { 	/* 判断是否进入下一个菜单 */
				key = KEY_CTRL_F2 ; 
				row = 0 ; 
				break ;
			 }
			
			key = InputVal(53, 17+row*27) ;
			switch (row) {
			case 0: allset.stake_d = number ? number : allset.stake_d ;break ;
			case 1: allset.width_max = number ? number : allset.width_max ;break ;
			default:  break ;
			}
			break ;
		case KEY_CTRL_F2:
			back_key = key ;
			Bdisp_AreaClr_DDVRAM(&clear_area) ;
			locatestr(27,2) ;printf("1=交点 2=线元 ") ;
			locatestr(77,16) ;printf("%.3f", allset.xl_type) ;	
			locatestr(27,29) ;printf("1=正交 2=任意 ") ;
			locatestr(77,43) ;printf("%.3f", allset.pj_mode) ;
			PrintXY(120, 6, (unsigned char*)"\xE6\x9C ", 0) ;
			PrintXY(120, 46, (unsigned char*)"\xE6\x9D ", 0) ;
			if (row > 1) { 	/* 判断是否进入下一个菜单 */
				if ( SaveProj()  != -1 )	Warning("设置成功 ", 2) ; 
				else  Warning("设置失败 ", 2) ;
				exit_flag = TRUE ;
				break;
			 }
			key = InputVal(53, 17+row*27) ;
			switch (row) {
			case 0:   allset.xl_type = (number==1 || number==2) ? number : allset.xl_type ;break ;
			case 1:   allset.pj_mode = (number==1 || number==2) ? number : allset.pj_mode ;break ;
			default:  break ;
			}
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
		case KEY_CTRL_EXIT :
			exit_flag = TRUE ;
			break ;
		}
	}
}

int SaveProj(void)
{
	
	int confile_ptr = -1 ;
	char temp_buffer[MAXLINE] ;

	confile_ptr = Bfile_OpenFile(configfile, _OPENMODE_READWRITE) ;
	if (confile_ptr < 0) return -1 ;

	sprintf(temp_buffer,"StakeD = %9.3f",allset.stake_d) ;
	confile_ptr = WriteINI(confile_ptr, allset.fpos.sk, temp_buffer,LATER,TRUE) ;
	sprintf(temp_buffer,"WitchMAX = %9.3f",allset.width_max) ;
	confile_ptr = WriteINI(confile_ptr, allset.fpos.wd, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"XLTYPE = %9.3f",floor(allset.xl_type)) ;
	confile_ptr = WriteINI(confile_ptr, allset.fpos.xl, temp_buffer,LATER,FALSE) ;
	sprintf(temp_buffer,"PJMODE = %9.3f",floor(allset.pj_mode)) ;
	confile_ptr = WriteINI(confile_ptr, allset.fpos.pj, temp_buffer,NOW,FALSE) ;


	Bfile_CloseFile(confile_ptr) ;
	return 0;
}
