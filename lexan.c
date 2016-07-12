/*
**  Lexical analyzer 词法分析器
**  Last update 2012.04.18
*/


# include "9860g.h"



struct tkbox statebox ; /*  token盒子  */
static int state,start ;



struct tkbox* lexan()
{
	char ch;

	state = start = 0 ;
    	while (ch = GETCH) {
		switch(state){
		case 0:
			/*  case 1, -9: 空白符  */
			if (isspace(ch) || ch == ',') state = -9 ;
			else state = nextry() ;
			break ;
		case -9:
			if ( isspace(ch) ) state = -9 ;
           		else { RETCH ; recover() ; }
           		break ;
		case 1:
            		/*  case 1,2: 分号注释  */
           		 if (ch == ';') state = 2 ;
            		else state = nextry() ;
            		break ;
		case 2:
           		if (ch == '\n') return get(TOKEN_COMMENT) ;
           		else state = 2 ;
            		break ;
		case 3:
			/*  case 3,4: [标题]  */
			if (ch == '[') state =4 ; 
			else state = nextry() ;
			break ;
		case 4:
			if (ch ==']')  return get (TOKEN_TITLE);  /* 去除]号 */ 
			else if (ch == '\n') return get(TOKEN_ERR) ;   /*  无法识别的词素  */
			else state = 4 ; 
			break ;
		case 5:
			/*  case 5,6: ID元素  */
			if ( isalnum(ch) ) state = 6 ;
			else state =  nextry();
			break ;
		case 6:
			if ( isalnum(ch) || ch == '_') state = 6 ;
			else if (ch == '.') { state = 5 ; state =  nextry();}	/*  有点号则为数字  */
			else {
               			RETCH ;
                			return get(TOKEN_ID) ;
            		}
			break ;
		case 7:
			/*  case 7: 等号   */
			//ch=GETCH;
			if (ch == '=')  return get(TOKEN_EQUAL);
			else state = nextry();
			break ;
		case 8:
			/*  case 8,9: 数字   */
			if (isdigit(ch) || ch == '-') state = 9 ;
			else state = nextry();
			break ;
		case 9:
			if (isdigit(ch)) state = 9 ;
			else if (ch == '.') state = 9 ;
			else {
				RETCH ;
				return get(TOKEN_NUM) ;
			}
			break ;
		case 10:
			/* case 10,11:文件路径  */
			if (ch == '\\') state = 11 ;
			else state = nextry();
			break ;
		case 11:
			if (ch == '\n' || ch == ';') {
				RETCH ;
				return get(TOKEN_FOLDER) ;
			}
           		else state = 11 ;
            		break ;
		default:
			break ;
		}
    	}
  	 return NULL ;
}

struct tkbox* lexan_num()
{
	char ch;

	state = start = 0 ;
    	while (ch = GETCH) {
		switch(state){
		case 0:
			/*  case 1, -9: 空白符  */
			if (isspace(ch) || ch == ',') state = -9 ;
			else state = nexnum() ;
			break ;
		case -9:
			if ( isspace(ch) ) state = -9 ;
           		else { RETCH ; recover() ; }
           		break ;
		case 8:
			/*  case 8,9: 数字   */
			if (isdigit(ch) || ch == '-') state = 9 ;
			else state = nexnum();
			break ;
		case 9:
			if (isdigit(ch)) state = 9 ;
			else if (ch == '.') state = 9 ;
			else {
				RETCH ;
				return get(TOKEN_NUM) ;
			}
			break ;
		default:
			break ;
		}
    	}
  	 return NULL ;
}

/*  进入下一个可能的状态  */
int nexnum()
{    
    switch (start){
        case 0:  start = 8 ; break ;
        case 8:  start = 999 ; break ;
        default: break ;
    }
    forward = begin ;
    return start ;
}
int nextry()
{    
    switch (start){
        case 0:  start = 1 ; break ;
        case 1:  start = 3 ; break ;
        case 3:  start = 5 ; break ;
        case 5:  start = 7 ; break ;
        case 7:  start = 8 ; break ;
	case 8:  start = 10 ; break ;
        case 10:  start = 999 ; break ;
        default: break ;
    }
    forward = begin ;
    return start ;
}

/*  恢复指针及状态  */
int recover()
{
    state = start = 0 ;
    begin = forward ;
}

char key[MAXLINE]  ;

/*  获取TOKEN  */
struct tkbox* get(int token)
{
    double value = 0.0 ;
    char s[MAXLINE] = "" ;
    char* str ;
    
    
    if (token == TOKEN_TITLE) {
	++begin ;	 	/* 去除[号 */
	--forward ;		/* 去除]号 */
    }
    if (token != TOKEN_COMMENT){
	strncpy(s, begin, forward - begin) ;  /*  复制词素  */
    	str = strcpy(key, s);
    }
           
    switch (token){
        case TOKEN_COMMENT:
            token = TOKEN_COMMENT ;
            value = 0 ;
	   str = NULL ;
            break ;
	case TOKEN_ERR:
	   token = TOKEN_ERR ;
            value = -1 ;
	   str = str ;
            break ;	
	case TOKEN_TITLE:  case TOKEN_ID:  
	case TOKEN_EQUAL:
	   token = token ;
	   value = 0 ;
	   str = str ;
	   break ;	
	case TOKEN_NUM:
	   token = token ;
	   value = atof(str) ;
	   str = str ;
	   break ;
        default:
            break ;
    }
    statebox.str = str ;
    statebox.token = token ;
    statebox.value = value ;
    recover() ;
    
    return &statebox ;
}
