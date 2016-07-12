/*
**  Lexical analyzer 词法分析器
**  Last update 2012.04.18
*/

# include "9860g.h"

char* begin ;
char* forward ; /*  无敌双指针  */



int parse(char* string)
{   
    struct tkbox* info ;
    BOOL stateE = FALSE ;
    
    stateK = stateV =0 ;
    begin = forward = string ;  /*  指向缓冲区  */
   
	while (info = lexan()) {
		switch (info->token){
			case TOKEN_COMMENT:
				//do nothing	
				break ;
			case TOKEN_ERR:
				//DebugS("错误: \n%s",info->str);
				break ;
			case TOKEN_TITLE:	
				if (strcmp(info->str,"RegCode") == 0)
					stateT = PROG_REGCD ;
				else if (strcmp(info->str,"Station") == 0)
					stateT = PROG_STATION ;
				else if (strcmp(info->str,"ProjectSet") == 0) 
					stateT = PROG_PROJSET ;
				else if (strcmp(info->str,"Database") == 0) 
					stateT = PROG_DATABASE ;
				//DebugS("标题:\n%s",info->str);
				break ;
			case TOKEN_ID:
				//为字符串重名设置优先级
				if (stateK == PROG_CUPQ || stateK == PROG_CUSQ
				 || stateK == PROG_CUYS  || stateK == PROG_CUHP
				 || stateK == PROG_CUSD) 
					{ stateV = PROG_STR ; break ;}

				if (strcmp(info->str,"SN") == 0) {stateK= PROG_SN ; break ;}
				if (strcmp(info->str,"X1") == 0) {stateK = PROG_X1 ; break ;}
				if (strcmp(info->str,"Y1") == 0) {stateK = PROG_Y1 ; break ;}
				if (strcmp(info->str,"H1") == 0) {stateK = PROG_H1 ; break ;}
				if (strcmp(info->str,"X2") == 0) {stateK = PROG_X2 ; break ;}
				if (strcmp(info->str,"Y2") == 0) {stateK = PROG_Y2 ; break ;}
				if (strcmp(info->str,"H2") == 0) {stateK = PROG_H2 ; break ;}
				if (strcmp(info->str,"StH") == 0) {stateK = PROG_STH ; break ;}
				if (strcmp(info->str,"PrismH") == 0) {stateK = PROG_PRH ; break ;}
				if (strcmp(info->str,"StakeD") == 0) {stateK = PROG_STAKED ; break ;}
				if (strcmp(info->str,"WitchMAX") == 0) {stateK = PROG_WDMAX ; break ;}
				if (strcmp(info->str,"XLTYPE") == 0) {stateK = PROG_XLTYPE ; break ;}
				if (strcmp(info->str,"PJMODE") == 0) {stateK = PROG_PJMODE ; break ;}
				if (strcmp(info->str,"Current_PQ") == 0) {stateK = PROG_CUPQ ; break ;}
				if (strcmp(info->str,"PQ1") == 0) {stateK = PROG_PQ1 ; break ;}
				if (strcmp(info->str,"PQ2") == 0) {stateK = PROG_PQ2 ; break ;}
				if (strcmp(info->str,"PQ3") == 0) {stateK = PROG_PQ3 ; break ;}
				if (strcmp(info->str,"PQ4") == 0) {stateK = PROG_PQ4 ; break ;}
				if (strcmp(info->str,"PQ5") == 0) {stateK = PROG_PQ5 ; break ;}
				if (strcmp(info->str,"Current_SQ") == 0) {stateK = PROG_CUSQ ; break ;}
				if (strcmp(info->str,"SQ1") == 0) {stateK = PROG_SQ1 ; break ;}
				if (strcmp(info->str,"SQ2") == 0) {stateK = PROG_SQ2 ; break ;}
				if (strcmp(info->str,"SQ3") == 0) {stateK = PROG_SQ3 ; break ;}
				if (strcmp(info->str,"SQ4") == 0) {stateK = PROG_SQ4 ; break ;}
				if (strcmp(info->str,"SQ5") == 0) {stateK = PROG_SQ5 ; break ;}
				if (strcmp(info->str,"Version") == 0) {stateK = PROG_VER ; break ;}
				if (strcmp(info->str,"Current_PLD") == 0) {stateK = PROG_CUPLD ; break ;}
				if (strcmp(info->str,"Current_YS") == 0) {stateK = PROG_CUYS ; break ;}
				if (strcmp(info->str,"YS1") == 0) {stateK = PROG_YS1 ; break ;}
				if (strcmp(info->str,"YS2") == 0) {stateK = PROG_YS2 ; break ;}
				if (strcmp(info->str,"YS3") == 0) {stateK = PROG_YS3 ; break ;}
				if (strcmp(info->str,"YS4") == 0) {stateK = PROG_YS4 ; break ;}
				if (strcmp(info->str,"YS5") == 0) {stateK = PROG_YS5 ; break ;}
				if (strcmp(info->str,"YS6") == 0) {stateK = PROG_YS6 ; break ;}
				if (strcmp(info->str,"YS7") == 0) {stateK = PROG_YS7 ; break ;}
				if (strcmp(info->str,"YS8") == 0) {stateK = PROG_YS8 ; break ;}
				if (strcmp(info->str,"Current_HP") == 0) {stateK = PROG_CUHP ; break ;}
				if (strcmp(info->str,"Current_SD") == 0) {stateK = PROG_CUSD ; break ;}
				if (strcmp(info->str,"SD1") == 0) {stateK = PROG_SD1 ; break ;}
				if (strcmp(info->str,"SD2") == 0) {stateK = PROG_SD2 ; break ;}
				if (strcmp(info->str,"SD3") == 0) {stateK = PROG_SD3 ; break ;}
				if (strcmp(info->str,"SD4") == 0) {stateK = PROG_SD4 ; break ;}
				if (strcmp(info->str,"SD5") == 0) {stateK = PROG_SD5 ; break ;}
				if (strcmp(info->str,"SD6") == 0) {stateK = PROG_SD6 ; break ;}
				if (strcmp(info->str,"SD7") == 0) {stateK = PROG_SD7 ; break ;}
				if (strcmp(info->str,"SD8") == 0) {stateK = PROG_SD8 ; break ;}
				else stateV = PROG_STR ;
				//DebugS("ID: \n%s",info->str);
				break ;
			case TOKEN_EQUAL:
				stateE = TRUE ;
				//DebugS("赋值等于: \n%s",info->str);
				break ;
			case TOKEN_NUM:
				stateV = PROG_NUM ;
				//DebugS("数字:\n%.12f", info->value);
				break ;
			case TOKEN_FOLDER:
				stateV = PROG_FOLDER ;
				break ;
			default:
				break ;
		}
		if ( (stateT == PROG_REGCD && stateE == TRUE &&stateV == PROG_STR)
		   || (stateT == PROG_REGCD && stateE == TRUE &&stateV == PROG_NUM)
		   ||(stateT == PROG_STATION && stateE == TRUE && stateV == PROG_NUM)
		   ||(stateT == PROG_PROJSET && stateE == TRUE && stateV == PROG_NUM) 
		   ||(stateT == PROG_DATABASE && stateE == TRUE &&stateV == PROG_STR)
		   ||(stateT == PROG_DATABASE && stateE == TRUE &&stateV == PROG_FOLDER)	 )
		{  stateE = FALSE ;   return stateK ; }	
	}

    
	return PROG_ERR ;
}



