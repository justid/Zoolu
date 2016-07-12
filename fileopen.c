/*
	闪存文件选择界面
	复制自开源程序malical的File.c文件
*/

# include "9860g.h"

void DrawPicture (int x,int y,int width,int height,unsigned char * pGraph)
{
	DISPGRAPH dg;
	dg.x = x;
	dg.y = y;
	dg.GraphData.width = width;
	dg.GraphData.height = height;
	dg.GraphData.pBitmap = pGraph;
	dg.WriteModify = IMB_WRITEMODIFY_NORMAL;
	dg.WriteKind = IMB_WRITEKIND_OVER;
	Bdisp_WriteGraph_VRAM(&dg);
}

FONTCHARACTER * CharToFont(const char *cFileName,FONTCHARACTER *fFileName)
{
	int i,len = strlen(cFileName);
	for(i=0; i<len ;++i)
		fFileName[i] = cFileName[i];
	fFileName[i]=0;
	return fFileName;
}

char * FontToChar(const FONTCHARACTER *fFileName,char *cFileName)
{
	int i = 0;
	while((cFileName[i]=fFileName[i])!=0)++i;
	return cFileName;
}

int SelectRoot(void)
{
	unsigned int key ;
	int ret = 0 ;

	SaveDisp(SAVEDISP_PAGE3) ;	
	PopUpWin(4) ;
	Print_zh("1.内置闪存 ", 14, 11, 0) ;
	Print_zh("2.SD卡 ", 14, 25, 0) ;

	GetKey(&key) ;
	switch (key)
	{
	case KEY_CHAR_1: 
		Fls_F = TRUE ; SDcard_F = FALSE ;
		break ;
	case KEY_CHAR_2:
		SDcard_F = TRUE ; Fls_F = FALSE ;
		break ;
	default:
		ret = 1 ;
		break ;
	}
	RestoreDisp(SAVEDISP_PAGE3) ;
	return ret ;
}

FONTCHARACTER *FilePath(char *sFolder, FONTCHARACTER *sFont)
{
	char path[50];

	if (Fls_F == TRUE){
		if( strlen(sFolder)==0 )	sprintf(path,"\\\\fls0\\*.*" );
		else sprintf(path,"\\\\fls0\\%s\\*.*",sFolder);
	}
	else if (SDcard_F == TRUE){
		if( strlen(sFolder)==0 )	sprintf(path,"\\\\crd0\\*.*" );
		else sprintf(path,"\\\\crd0\\%s\\*.*",sFolder);
	}

	//Convert to FONTCHARACTER
	CharToFont(path,sFont);
	return(sFont);
}

int PrintfRev(const char *cpFormat, ...)
{
	char cpBuffer[STRING_BUFFER_SIZE];
	va_list pArgList;

	va_start(pArgList, cpFormat);
	vsprintf(cpBuffer, cpFormat, pArgList);
	va_end(pArgList);
	PrintRev((unsigned char*)cpBuffer);
	return 0;
}

int IsXXXFile( char* tailname, FONTCHARACTER *find_name )
{
	char str[13];
	FontToChar(find_name,str);
	return (strcmp(str + strlen(str) - 4, tailname) == 0);
}

int IsCsvFile( FONTCHARACTER *find_name )
{
	return IsXXXFile(".csv",find_name);
}


int filecmp( const void *p1, const void *p2 )
{
	Files *f1 = (Files *)p1;
	Files *f2 = (Files *)p2;

	if( f1->filesize == -1 && f2->filesize == -1 )
		return strcmp( f1->filename + 1, f2->filename + 1);
	else if( f1->filesize == -1 )
		return -1;
	else if( f2->filesize == -1 )
		return 1;
	else
		return strcmp( f1->filename, f2->filename );
}

int pPrintf(char *cpFormat, ...)
{
	char cpBuffer[STRING_BUFFER_SIZE];
	va_list pArgList;

	va_start(pArgList, cpFormat);
	vsprintf(cpBuffer, cpFormat, pArgList);
	va_end(pArgList);
	Print((unsigned char*)cpBuffer);
	return 0;
}


Files *files;
int index = 0;

//return 0  or 1 
// -1 no file , 0 complate
int SelectFile (char *folder, char *name)
{
	int size;

	while( 1 ){
		size = ReadFile( folder );
		qsort( files, size, sizeof(Files), filecmp );

		Explorer( size, folder );

		if( index == size ){			//return to root
			if (folder[0] == '\0'){		//根目录下按exit返回上级目录
				index = 0 ;
				free( files );
				return 0 ;
			}
			else{
				folder[0] = '\0';
				index = 0;
			}
		}
		else if( files[index].filesize == -1 ){				//folder
			strcpy( folder,files[index].filename );
			index = 0;
		}
		else{										//file
			strcpy( name,files[index].filename );
			free( files );
			return 1;
		}
	}
}

int Explorer( int size, char *folder )
{
	int top, redraw;
	int i;
	unsigned int key;
	
	redraw = 1;
	top = index;
	
	while(1)
	{
		if( redraw )
		{
			Bdisp_AllClr_VRAM();
			//DrawPicture( 1, 56, 61, 8, graph_bar );
			PrintMini(1, 58, (unsigned char*)" EXE ", MINI_REV) ; 
			PrintMini(105, 58, (unsigned char*)" EXIT ", MINI_REV) ; 
			locate(1, 1);Print((unsigned char*)"File List  [        ]");
			locate(13, 1);Print( strlen(folder) ? (unsigned char*)folder : (unsigned char*)"Root");
			if( size < 1 ){
				locate( 8, 4 );
				Print( (unsigned char*)"No Data" );
			}
			else{			
				if( top > index )
					top = index;
				if( index > top + N_LINE - 1 )
					top = index - N_LINE + 1;
				if( top < 0 )
					top = 0;
	
				for(i = 0;i < N_LINE && i + top < size; ++i ){
					locate( 1, i + 2 );
					if( files[i + top].filesize == -1 )
						pPrintf( " [%s]", files[i + top].filename );
					else
						pPrintf( " %-12s:%6u ", files[i + top].filename, files[i + top].filesize );
				}
				Bdisp_AreaReverseVRAM( 0, (index-top+1)*8 , 127, (index-top+2)*8-1 );
				if( top > 0 )
					PrintXY( 120, 8, (unsigned char*)"\xE6\x92", top == index );
				if( top + N_LINE < size  )
					PrintXY( 120, N_LINE*8, (unsigned char*)"\xE6\x93" , top + N_LINE - 1 == index );
			}
			redraw = 0;
		}
		GetKey(&key);
		if( key==KEY_CTRL_UP ){
			if( --index < 0 )
				index = size - 1;
			redraw = 1;
		}
		else if( key==KEY_CTRL_DOWN ){
			if( ++index > size - 1 )
				index = 0;
			redraw = 1;
		}
		else if( key==KEY_CTRL_EXE || key == KEY_CTRL_F1)
			break;
		else if( key == KEY_CTRL_F2 ){
			//Help();
			redraw = 1;
		}
		else if( key == KEY_CTRL_F3 ){
			//About();
			redraw = 1;
		}		
		else if( key==KEY_CTRL_EXIT){
			index = size;
			break;
		}
	}
}


int ReadFile( char *folder )
{
	char str[FILENAMEMAX];
	FONTCHARACTER find_path[50];
	FONTCHARACTER find_name[50];
	FILE_INFO file_info;
	int find_h;
	int size, i;

	int iError=0;
	int iFirst=0;

	size = 0;
	FilePath( folder, find_path);

	//	Get File count	
	do
	{
	  if(iFirst==0) {
        iError=Bfile_FindFirst(find_path,&find_h,find_name,&file_info);
        iFirst=1;
      } 
	  else {
        iError=Bfile_FindNext(find_h,find_name,&file_info);
      }
	  if( iError == 0 && ( file_info.type == DT_DIRECTORY ||  IsCsvFile( find_name ) ) )
		++size ;
	}
	while (iError == 0);
	Bfile_FindClose(find_h);
	//	Get File name	
	files = (Files *)calloc(1,size*FILENAMEMAX);
	i = 0;
	iError=0;
	iFirst=0;
	do
	{
	  if(iFirst==0) {
        iError=Bfile_FindFirst(find_path,&find_h,find_name,&file_info);
        iFirst=1;
      } 
	  else {
        iError=Bfile_FindNext(find_h,find_name,&file_info);
      }
	  if( iError == 0 && ( file_info.type == DT_DIRECTORY ||  IsCsvFile( find_name ) ) ){
			FontToChar(find_name,str);
			strncpy( files[i].filename, str, FILENAMEMAX);
			files[i].filesize = (file_info.type == DT_DIRECTORY ? -1 : file_info.dsize);
			++i;
		}
	} while (iError == 0);

	Bfile_FindClose(find_h);
	return size;
}



