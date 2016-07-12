/*
**  Casio 9860g 汉字显示
**  Last update 2009.10.16
*/


# include "9860g.h"

char font_buf[200] ;
int fontptr ;

int Print_zh(char* input, BYTE x, BYTE y, BYTE type)
{
	DWORD fontpos = 0 ;
	char ASCIIshow[2] = {'\0','\0'};
	WORD HZshow ;
	char ch = '\0';
	unsigned char bitmap_ptr[] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
	/* 28字节点阵信息 */		   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
						   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
						   '\0', '\0', '\0', '\0', '\0'} ;
	unsigned int temp_step ;
	unsigned long int temp_bit ;

	GRAPHDATA font_data ;	/* Display structure */
	DISPGRAPH font_disp ;
	char* buffer = (char*)font_buf;

	strcpy((char*)font_buf, input) ;		/* Copy input to font buffer */

	/* 初始化结构体 */
	font_data.width = 14 ;		/* GRAPHDATA */
	font_data.height = 14 ;
	font_disp.x = x ;			/* DISPGRAPH  */
	font_disp.y = y ;
	font_disp.WriteKind = IMB_WRITEKIND_OVER ;		/* Overwrite (fill) */
	switch (type){
	case NORMAL:
		font_disp.WriteModify = IMB_WRITEMODIFY_NORMAL ; 	/* Normal */	
		break ;
	case REVERSE:  case VERT_REV:
		font_disp.WriteModify = IMB_WRITEMODIFY_REVERCE ; 	/* Revers(c)e */
		break ;
	case UNDER_LINE:	/* 下划线 */
		Bdisp_DrawLineVRAM(x-5, y+14, x+47, y+14) ;
		font_disp.WriteModify = IMB_WRITEMODIFY_NORMAL ; 	/* Normal */	
	default: break ;
	}
		
	
	while (*buffer != '\0') {
		if (*buffer & 0x80){
		/* 当前内码位置 = 索引位置 × 字体大小 */
		strncpy((char*)&HZshow,buffer,2);
		
		fontpos = GetPosWithMbcs(HZshow) * 28 ;
		if (Bfile_ReadFile(fontptr, bitmap_ptr, 28, fontpos) < 0)
		{
			OpenFont();
			Bfile_ReadFile(fontptr, bitmap_ptr, 28, fontpos) ;
		} ;	/* Read bitmap */
	
		font_data.pBitmap = bitmap_ptr ;	
		font_disp.GraphData = font_data ;

		//字模修正，将反显字右移一像素
		if (type == VERT_REV) {
			for (temp_step = 0 ; temp_step < 28 ; ++temp_step) {
				if ( (temp_step + 2) % 2 == 0 ) {
					temp_bit = bitmap_ptr[temp_step] ;
					temp_bit <<= 8 ;
				} else {
					temp_bit |= bitmap_ptr[temp_step] ;
					temp_bit >>= 1 ;
					bitmap_ptr[temp_step] = (unsigned char)temp_bit ;	
					bitmap_ptr[temp_step - 1] = (unsigned char)(temp_bit>>8) ;	
				}
			}
 		}

		Bdisp_WriteGraph_VRAM(&font_disp);
		
		if (type == VERT_REV || type == VERT)	font_disp.y += 14 ;
		else font_disp.x += 14 ;
		buffer += 2 ;	//汉字占两字节
		} 
		else{
		ASCIIshow[0]=*buffer;
		if (ASCIIshow[0] == '\n'){
			font_disp.x = x ;
			font_disp.y += 13 ;
		}
		else{
		PrintXY(font_disp.x,font_disp.y+4,(unsigned char*)ASCIIshow,(type)?1:0);
		if (type == VERT_REV || type == VERT)	font_disp.y += 7 ;
		else font_disp.x += 7 ;
		}
		++buffer;
		}
		//断行处理，仅处理横向	
		//if (font_disp.x+7>=127){
		//	font_disp.x = x ;
		//	font_disp.y += 14 ;
		//}
		if (font_disp.y+14>=63)	break ;	 //满屏则剩余不显示	
	}

	Bdisp_PutDisp_DD() ;   
	return TRUE ;
}

DWORD GetPosWithMbcs(WORD code)
{                      
	BYTE R = (code >> 8) & 0xFF;   	/* 区码 */
	BYTE C = code & 0xFF;   		/* 位码 */
	if ((R >= 0xA1 && R <= 0xFE) && (C >= 0xA1 && C <= 0xFE))
         	return (R - 0xa1)*94 + (C - 0xa1) ;
        	else  return 0 ;
}

int OpenFont(void)
{

	FONTCHARACTER fontfile[] = { '\\','\\','f','l','s','0','\\','F','O','N','T','.','b','i','n' ,'\0'} ;
	FONTCHARACTER fontfile2[] = { '\\','\\','c','r','d','0','\\','f','o','n','t','.','b','i','n' ,'\0'} ;

	fontptr = Bfile_OpenFile(fontfile2, _OPENMODE_READ) ;	
	if (fontptr < 0)
	{
		fontptr = Bfile_OpenFile(fontfile, _OPENMODE_READ) ;
		if (fontptr < 0) {
			locate(1,4) ;
    		Print((unsigned char*)"Cann't open fontfile.") ;
			return FALSE ;	/* error */
		}
	}
	return TRUE ;
}

void CloseFont(void)
{
	Bfile_CloseFile( fontptr ) ;
}


