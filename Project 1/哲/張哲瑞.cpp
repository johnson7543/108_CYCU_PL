# include <iostream>
# include <stdio.h>
# include <cstring>
# include <sstream>
# include <vector>
# include <iomanip>
# include <queue>
# include <stack>
# include <string>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <exception>


using namespace std;

int gline_num = 0 ; // 第一個為 line 0
int gcolumn_num = -1 ;  // 第一個為 column 0
int gEnd = 0 ; // EOF 的話改1

typedef enum Type {

  PLUS, MINUS, MUL, DIV, NUM, ID, SIGNNUM, LEFTPAR, RIGHTPAR,
  BOOLOP, ASSIGN, SEMICOLON, SIGNNUMWITHPOINT, NUMWITHPOINT // 這個是為了 有小數點的數字


}
T;

struct Token {

  string content ;
  T type ; // 用 enum 自訂義的類別

}; // Token()

vector <string> gPeekBuffer ; // 用來存偷看的 token
vector <Token> grammarcorrect ; // 文法正確就放進來

struct IDToken {

  string idname ;
  float idvalue ;
  T type ;
  bool isFloat ;

}; // IDToken()

vector <IDToken> gIDDefinedTable ; // 放入 assign 過的 ID 值
bool gForC2 ; // 看 Command 第二種情況 是 直接給值 ( true )  還是 true false ( false )
bool gIsFloat = false ; // 看輸出時需不需要變小數後三位的輸出法  預設為不用
static float uTolerance = 0.0001 ;



struct CharLocation {

  char tchar ;
  int line ;
  int column ;

};

vector <CharLocation> gbuffer ;

void Factor() ;
void Term() ;
void ArithExp() ;
void NOT_ID_StartFactor() ;
void NOT_ID_StartTerm() ;
bool BooleanOprator( string a ) ;
void IDlessArithExpOrBexp( string a ) ;
void Command() ;
void ReadtoEnd() ;

bool Realfloat( string a ) ;
bool IsAssign( string a ) ;
float GetIdAssignValue( string idname ) ;
float FactorCal() ;
float TermCal() ;
float ArithExpCal() ;
float NOT_ID_StartFactorCal() ;
float NOT_ID_StartTermCal() ;
float NOT_ID_StartArithExpCal() ;
float NOT_ID_StartArithExpOrBexpCal() ;
void CommandCal() ;

char F1() {

  char a ;
  a = getchar() ;

  gcolumn_num = gcolumn_num + 1 ;

  if ( a == '\n' ) {
    gline_num = gline_num + 1 ;
    gcolumn_num = -1 ; // 到下一行了 column 重計
  } // if()

  CharLocation temp ;
  temp.tchar = a ;
  temp.line = gline_num ;
  temp.column = gcolumn_num ;
  gbuffer.push_back( temp ) ;

  return a ;

} // F1()  讀取字元 並 記錄行列

char F2() {

  char first ;
  char peek ;
  first = F1() ;

  while ( first == ' ' || first == '\n' || first == '\t' || first == '/' ) {

    if ( first == '/' ) {
      peek = cin.peek() ;

      if ( peek == '/' ) { // 代表出現 comment "//"
        peek = F1() ;  // 先把第二個斜線讀掉
        first = F1() ;
        while ( first != '\n' ) {
          first = F1() ;
        } // while()  讀取至 換行 '\n' 為止
      } // if() 遇到第二個註解斜線 '/'

      else { // 代表只是除法
        return first ;
      } // else()

    } // if()  遇到第一個註解斜線 '/'

    else if ( first == ' ' || first == '\n' || first == '\t' ) {
      first = F1() ;
    } // else if() 是white-space 就去叫 F1() 讀取字元


  }  // while() white-space 就去叫 F1() 讀取字元

  return first ;
} // F2()  負責跳過comment 並 取得whitespace後的第一個字元

//  ///////////////////////////////////////////pal已修正風格檢查

string F3( char a ) {

  string temp = "" ;
  char tchar ;

  temp = temp + a ;

  if ( a == '+' || a == '-' || a == '*' || a == '/' || a == '=' || a == ';' || a == '(' || a == ')' ) {

    return temp ;

  } // if() 單一標點符號

  else if ( a == ':' ) {

    tchar = cin.peek() ;  // 先看 確定是要得再取

    if ( tchar == '=' ) {

      tchar = F1() ;
      temp = temp + tchar ;

    } // if()   是 ':=' 的情況

    else {

      string error = "" ;
      error = "Unrecognized token with first char : '" + temp + "'" ;
      ReadtoEnd() ;
      throw error ;

    } // else()

  } // else if()

  else if ( a == '>' ) {

    tchar = cin.peek() ;  // 先看 確定是要得再取

    if ( tchar == '=' ) {

      tchar = F1() ;
      temp = temp + tchar ;

    } // if()   是 '>=' 的情況

  } // else if()

  else if ( a == '<' ) {

    tchar = cin.peek() ;  // 先看 確定是要得再取

    if ( tchar == '=' ) {

      tchar = F1() ;
      temp = temp + tchar ;

    } // if()   是 '<=' 的情況

    else if ( tchar == '>' ) {

      tchar = F1() ;
      temp = temp + tchar ;

    } // else if() 是 '<>'的情況

  } // else if()



  return temp ;


} // F3()  負責標點符號

string F4( char a ) {

  string temp = "" ;
  char tchar ;

  temp = temp + a ;

  tchar = cin.peek() ;  // 先看 確定是要得再取

  while ( ( tchar >= 65 && tchar <= 90 ) || ( tchar >= 97 && tchar <= 122 )
          || ( tchar >= 48 && tchar <= 57 ) || tchar == '_' ) {

    tchar = F1() ;  // 確定是要的就用  F1() 去取字元
    temp  = temp + tchar ;
    tchar = cin.peek() ;

  } // while()  是 a - z  A - Z  0 - 9  '_'  ( ID 後面能接這些 )

  return temp ;

} // F4()  負責 Id

string F5( char a ) {

  string temp = "" ;
  char tchar ;
  int countpoint = 0 ; // ( a >= 48 && a <= 57 )

  temp = temp + a ;

  if ( a == '.' ) { // 為 .3 .03 .156 ......等情況

    tchar = cin.peek() ;

    while ( tchar >= 48 && tchar <= 57 ) {  // 假如 peek 到的 next char 為 0 - 9 之間

      tchar = F1() ; // 確定是要的就用  F1() 去取字元
      temp = temp + tchar ;
      tchar = cin.peek() ;

    } // while()
  } // if()

  else if (  a >= 48 && a <= 57 ) {

    tchar = cin.peek() ;

    while ( tchar >= 48 && tchar <= 57 ) {  // 假如 peek 到的 next char 為 0 - 9 之間

      tchar = F1() ; // 確定是要的就用  F1() 去取字元
      temp = temp + tchar ;
      tchar = cin.peek() ;

    } // while()

    if ( tchar == '.' ) {

      tchar = F1() ; // 確定是要的就用  F1() 去取字元
      temp = temp + tchar ;
      tchar = cin.peek() ;

      while ( tchar >= 48 && tchar <= 57 ) {  // 假如 peek 到的 next char 為 0 - 9 之間

        tchar = F1() ; // 確定是要的就用  F1() 去取字元
        temp = temp + tchar ;
        tchar = cin.peek() ;

      } // while()
    } // if()  為  1.022  213.066 ......等情況
  } // else if()




  return temp ;

} // F5()  負責 Num

void ReadtoEnd() {  // 讀掉一行
  char a ;
  a = F1() ;

  while ( a != '\n' ) {

    a = F1() ;

  } // while()

  // cout << "Reading to End " <<endl ;


} // ReadtoEnd()

string GetToken() {

  char target ;
  string ans ;
  target = F2() ;

  if ( ( target >= 65 && target <= 90 ) || ( target >= 97 && target <= 122 )  ) {
        // ID的情況 ( 在 A - Z a - z 之間 )

    ans = F4( target ) ;

  } // if()

  else if ( ( target >= 48 && target <= 57 ) || target == '.' ) { // NUM的情況 ( 在 0 - 9 之間 或 '.' )

    if ( target == '.' ) {

      char temp = cin.peek() ;

      if ( ! ( temp >= 48 && temp <= 57 ) ) { // 後面不是 0 - 9  單獨一個 點( '.' ) 的情況
        string ansss = "" ;
        ansss = ansss + target ;
        ans = "Unrecognized token with first char : '" + ansss + "'" ;
        ReadtoEnd() ;
        throw ans ; // ///////////
      } // if()

      else {
        ans = F5( target ) ;
      } // else()
    } // if()

    else {
      ans = F5( target ) ;
    } // else()

  } // else if()

  else if ( target == '+' || target == '-' || target == '*' || target == '/'
            || target == ':' || target == ';' || target == '<' || target == '>'
            || target == '=' || target == '(' || target == ')' ) {

    // 是標點符號的情況
    ans = F3( target ) ;

  } // else if()

  else if ( target == EOF ) {
    ans = target ;
    gEnd = 1 ; // 結尾
    string q = "Is EOF" ;
    throw q ;
    return ans ;
  } // else if()

  else {
    string anss = "" ;
    anss = anss + target ;
    ans = "Unrecognized token with first char : '" + anss + "'" ;
    ReadtoEnd() ;
    throw ans ;
  } // else()


  return ans ;
} // GetToken()

// ////////////////////////////////03/25 放入 pal

bool IsIdent( string a ) {

  for ( int i = 0 ; i < a.length() ; i++ ) {

    if ( i == 0 ) { // 檢查 ID 的開頭是否為 letter

      if ( ( a[i] >= 65 && a[i] <= 90 ) || ( a[i] >= 97 && a[i] <= 122 ) ) {

      } // if()
      else return false ; // else()

    } // if()

    else {

      if ( ( a[i] >= 65 && a[i] <= 90 ) || ( a[i] >= 97 && a[i] <= 122 )
           || ( a[i] >= 48 && a[i] <= 57 ) || a[i] == '_' )  {
      } // if()

      else return false ; // else()

    } // else() 從第二個開始

  } // for()

  return true ;


} // IsIdent()  看此 TOKEN( 字串 ) 是否為 ID

bool IsSign( string a ) {

  if ( a.length() == 1 && ( a[0] == '+' || a[0] == '-' ) ) {
    return true ;
  } // if()
  else return false ; // else()

} // IsSign()

bool IsNum( string a ) {

  for ( int i = 0 ; i < a.length() ; i++ ) {

    if ( ( a[i] >= 48 && a[i] <= 57 ) || a[i] == '.' )   {
    } // if()

    else return false ; // else()

  } // for()

  return true ;

} // IsNum()

// ///////////////////////////////////////////// start grammar //////////////////

string Peektoken() { // Peek 完會存到 buffer

  string a ;
  a = GetToken() ;  // 拿到一段正確的 token
  gPeekBuffer.push_back( a ) ;
  return a ;

} // Peektoken()

string RealGettoken() {

  string a ;
  a = gPeekBuffer[0] ;
  gPeekBuffer.clear() ;
  return a ;

} // RealGettoken()

bool BooleanOprator( string a ) {

  if ( a == "=" || a == "<>" || a == ">"
       || a == "<" || a == ">=" || a == "<=" ) {

    return true ;

  } // if()

  else return false ; // else()

} // BooleanOprator()

void Factor() { // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'

  string target ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    target = gPeekBuffer[0] ;
  } // if()

  else {
    target = Peektoken() ; // 先看不拿
  } // else()

  T define ;

  if ( IsIdent( target ) ) {
    if ( !IsAssign( target ) ) { // ID 沒被定義過的話
      ReadtoEnd() ;
      string error ;
      error = "Undefined identifier : '" + target + "'" ;
      throw error ;
    } // if()

    else {
      Token temp ;
      temp.content = target ;
      define = ID ;
      temp.type = define ;  // 重要
      grammarcorrect.push_back( temp ) ; // 確定正確就放入文法正確 vector
      gPeekBuffer.clear() ; // 清除暫存內東西
    } // else()
  }  // if()

  else if ( IsSign( target ) ) {

    string tempans ;
    tempans = tempans + target ; // 把正負先放好
    gPeekBuffer.clear() ; // 清除暫存內東西
    // cout << "123" << endl ;
    target = Peektoken() ; // 先看不拿

    if ( IsNum( target ) ) {

      tempans = tempans + target ;
      Token temp ;
      temp.content = tempans ;
      if ( Realfloat( target ) ) { // 要去定義是否確定有小數點
        define = SIGNNUMWITHPOINT ; // 是小數而且有正負號
      } // if()

      else {
        define = SIGNNUM ; // 不是小數但有正負號
      } // else)_

      temp.type = define ;  // 重要
      grammarcorrect.push_back( temp ) ; // 確定正確就放入文法正確 vector
      gPeekBuffer.clear() ; // 清除暫存內東西

    } // if()

    else {
      string temp ;
      temp = "Unexpected token : '" + target + "'" ;
      gPeekBuffer.clear() ; // 清除暫存內東西
      ReadtoEnd() ;
      grammarcorrect.clear() ; // 這行有錯就不用了
      throw temp ;

    } // else()

  } // else if ()

  else if ( IsNum( target ) ) {

    Token temp ;
    temp.content = target ;
    if ( Realfloat( target ) ) {
      define = NUMWITHPOINT ; // 是小數
    } // if()

    else {
      define = NUM ; // 是整數
    } // else()

    temp.type = define ;  // 重要
    grammarcorrect.push_back( temp ) ; // 確定正確就放入文法正確 vector
    gPeekBuffer.clear() ; // 清除暫存內東西

  } // else if()

  else if ( target == "(" ) {

    string tempans ;
    Token temp ;
    define = LEFTPAR ;
    temp.content = target ;
    temp.type = define ;
    grammarcorrect.push_back( temp ) ; // 把左括號放入正確文法
    gPeekBuffer.clear() ; // 清除暫存內東西

    ArithExp() ; // ////////////////////////////////////////

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      target = gPeekBuffer[0] ;
    } // if()

    else {
      target = Peektoken() ; // 先看不拿
    } // else()

    if ( target == ")" ) {
      Token temp2 ;
      define = RIGHTPAR ;
      temp2.content = target ;
      temp2.type = define ;
      grammarcorrect.push_back( temp2 ) ; // 把左括號放入正確文法
      gPeekBuffer.clear() ;
    } // if()

    else {
      string temp ;
      temp = "Unexpected token : '" + target + "'" ;
      gPeekBuffer.clear() ; // 清除暫存內東西
      ReadtoEnd() ;
      grammarcorrect.clear() ; // 這行有錯就不用了
      throw temp ;

    } // else()


  } // else if() ArithExp 寫好再補上

  else {

    string temp ;
    temp = "Unexpected token : '" + target + "'" ;
    gPeekBuffer.clear() ; // 清除暫存內東西
    ReadtoEnd() ;
    grammarcorrect.clear() ; // 這行有錯就不用了
    throw temp ;

  } // else()

} // Factor()

void Term() { // <Term>  ::= <Factor> { '*' <Factor> | '/' <Factor> }

  Factor() ;

  string temp ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    temp = gPeekBuffer[0] ;
  } // if()

  else {
    temp = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  while ( temp == "*" || temp == "/" ) { // 進迴圈代表確定要拿 真的從暫存中取走

    putin.content = temp ;
    if ( temp == "*" ) define = MUL ; // if()
    if ( temp == "/" ) define = DIV ; // if()
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    Factor() ;

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      temp = gPeekBuffer[0] ;
    } // if()
    else {
      temp = Peektoken() ; // 先看不拿
    } // else

  } // while()

} // Term()

void ArithExp() { // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }

  Term() ;

  string temp ;

  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    temp = gPeekBuffer[0] ;
  } // if()

  else {
    temp = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  while (  temp == "+" || temp == "-" ) {

    putin.content = temp ;
    if ( temp == "+" ) define = PLUS ; // if()
    if ( temp == "-" ) define = MINUS ; // if()
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    Term() ;
    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      temp = gPeekBuffer[0] ;
    } // if()

    else {
      temp = Peektoken() ; // 先看不拿
    } // else

  } // while()
} // ArithExp()

void NOT_ID_StartFactor() { // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'

  string target ;

  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    target = gPeekBuffer[0] ;
  } // if()

  else {
    target = Peektoken() ; // 先看不拿
  } // else()

  T define ;

  if ( IsSign( target ) ) {

    string tempans ;
    tempans = tempans + target ; // 把正負先放好
    gPeekBuffer.clear() ; // 清除暫存內東西

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      target = gPeekBuffer[0] ;
    } // if()

    else {
      target = Peektoken() ; // 先看不拿
    } // else

    if ( IsNum( target ) ) {

      if ( Realfloat( target ) ) {
        define = SIGNNUMWITHPOINT ;
      } // if

      else {
        define = SIGNNUM ;
      } // else()

      tempans = tempans + target ;
      Token temp ;
      temp.content = tempans ;
      temp.type = define ;  // 重要
      grammarcorrect.push_back( temp ) ; // 確定正確就放入文法正確 vector
      gPeekBuffer.clear() ; // 清除暫存內東西

    } // if()

    else {
      string temp ;
      temp = "Unexpected token : '" + target + "'" ;
      gPeekBuffer.clear() ; // 清除暫存內東西
      ReadtoEnd() ;
      grammarcorrect.clear() ; // 這行有錯就不用了
      throw temp ;

    } // else()

  } // if ()

  else if ( IsNum( target ) ) {

    Token temp ;
    temp.content = target ;
    if ( Realfloat( target ) ) {
      define = NUMWITHPOINT ;
    } // if

    else {
      define = NUM ;
    } // else()


    temp.type = define ;  // 重要
    grammarcorrect.push_back( temp ) ; // 確定正確就放入文法正確 vector
    gPeekBuffer.clear() ; // 清除暫存內東西

  } // else if()

  else if ( target == "(" ) {

    string tempans ;
    Token temp ;
    define = LEFTPAR ;
    temp.content = target ;
    temp.type = define ;
    grammarcorrect.push_back( temp ) ; // 把左括號放入正確文法
    gPeekBuffer.clear() ; // 清除暫存內東西
    ArithExp() ;

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      target = gPeekBuffer[0] ;
    } // if()

    else {
      target = Peektoken() ; // 先看不拿
    } // else

    if ( target == ")" ) {
      define = RIGHTPAR ;
      temp.content = target ;
      temp.type = define ;
      grammarcorrect.push_back( temp ) ;
      gPeekBuffer.clear() ;
    } // if()

    else {
      string temp ;
      temp = "Unexpected token : '" + target + "'" ;
      gPeekBuffer.clear() ; // 清除暫存內東西
      ReadtoEnd() ;
      grammarcorrect.clear() ; // 這行有錯就不用了
      throw temp ;

    } // else()
  } // else if() ArithExp

  else {

    string temp ;
    temp = "Unexpected token : '" + target + "'" ;
    gPeekBuffer.clear() ; // 清除暫存內東西
    ReadtoEnd() ;
    grammarcorrect.clear() ; // 這行有錯就不用了
    throw temp ;

  } // else()

} // NOT_ID_StartFactor()

void NOT_ID_StartTerm() { //  <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }

  NOT_ID_StartFactor() ;

  string temp ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    temp = gPeekBuffer[0] ;
  } // if()

  else {
    temp = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  while ( temp == "*" || temp == "/" ) { // 進迴圈代表確定要拿 真的從暫存中取走

    putin.content = temp ;
    if ( temp == "*" ) define = MUL ; // if()
    if ( temp == "/" ) define = DIV ; // if()
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    Factor() ;
    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      temp = gPeekBuffer[0] ;
    } // if()

    else {
      temp = Peektoken() ; // 先看不拿
    } // else

  } // while()

} // NOT_ID_StartTerm()

void NOT_ID_StartArithExp() { // <NOT_ID_StartArithExp> ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }

  NOT_ID_StartTerm() ;

  string temp ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    temp = gPeekBuffer[0] ;
  } // if()

  else {
    temp = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  while (  temp == "+" || temp == "-" ) {

    putin.content = temp ;
    if ( temp == "+" ) define = PLUS ; // if()
    if ( temp == "-" ) define = MINUS ; // if()
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    Term() ;
    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      temp = gPeekBuffer[0] ;
    } // if()

    else {
      temp = Peektoken() ; // 先看不拿
    } // else

  } // while()


} // NOT_ID_StartArithExp()

void NOT_ID_StartArithExpOrBexp() {

// <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]

  NOT_ID_StartArithExp() ;

  string target ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    target = gPeekBuffer[0] ;
  } // if()

  else {
    target = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  if ( BooleanOprator( target ) ) {
    putin.content = target ;
    define = BOOLOP ;
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    ArithExp() ;

  } // if()

} // NOT_ID_StartArithExpOrBexp()

void IDlessArithExpOrBexp( string a ) {

// <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term> | '*' <Factor> | '/' <Factor> }
// [ <BooleanOperator> <ArithExp> ]

  string temp ;

  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    temp = gPeekBuffer[0] ;

  } // if()

  else {
    temp = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  while ( temp == "+" || temp == "-" || temp == "*" || temp == "/" ) {

    if ( !IsAssign( a ) ) { // ID 沒被定義過的話
      ReadtoEnd() ;
      string error ;
      error = "Undefined identifier : '" + a + "'" ;
      throw error ;
    } // if()

    if ( temp == "+" ) {

      putin.content = temp ;
      define = PLUS ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      Term() ;

      if ( gPeekBuffer.size() != 0 ) { // buffer有東西
        temp = gPeekBuffer[0] ;
      } // if()

      else {
        temp = Peektoken() ; // 先看不拿
      } // else
    } // if()

    else if ( temp == "-" ) {

      putin.content = temp ;
      define = MINUS ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      Term() ;

      if ( gPeekBuffer.size() != 0 ) { // buffer有東西
        temp = gPeekBuffer[0] ;
      } // if()

      else {
        temp = Peektoken() ; // 先看不拿
      } // else
    } // else if()

    else if ( temp == "*" ) {
      putin.content = temp ;
      define = MUL ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      Factor() ;
      if ( gPeekBuffer.size() != 0 ) { // buffer有東西
        temp = gPeekBuffer[0] ;
      } // if()

      else {
        temp = Peektoken() ; // 先看不拿
      } // else
    } // else if()

    else if ( temp == "/" ) {

      putin.content = temp ;
      define = DIV ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      Factor() ;
      if ( gPeekBuffer.size() != 0 ) { // buffer有東西
        temp = gPeekBuffer[0] ;
      } // if()

      else {
        temp = Peektoken() ; // 先看不拿
      } // else
    } // else if()

  } // while()

  if ( BooleanOprator( temp ) ) {
    if ( !IsAssign( a ) ) { // ID 沒被定義過的話
      ReadtoEnd() ;
      string error ;
      error = "Undefined identifier : '" + a + "'" ;
      throw error ;
    } // if()

    putin.content = temp ;
    define = BOOLOP ;
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    ArithExp() ;

  } // if()

} // IDlessArithExpOrBexp()

void Command() {

// <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
// | <NOT_IDStartArithExpOrBexp> ';'
// | QUIT

  string target ;
  string target2 ;
  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    target = gPeekBuffer[0] ;
  } // if()

  else {
    target = Peektoken() ; // 先看不拿
  } // else

  Token putin ;
  T define ;

  if ( target == "quit" ) {

    string temp ;
    temp = "Program exits..." ;
    gEnd = 1 ;
    throw temp ;

  } // if()

  if ( IsIdent( target ) ) {
    target2 = target ;
    putin.content = target ;
    define = ID ;
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      target = gPeekBuffer[0] ;
    } // if()

    else {
      target = Peektoken() ; // 先看不拿
    } // else

    if ( target == ":=" ) { // ':=' <ArithExp>
      putin.content = target ;
      define = ASSIGN ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      ArithExp() ; // //////////////////////////////////////////////

    } // if()

    else {

      IDlessArithExpOrBexp( target2 ) ; // target2 是來存 第一個id  因為 target 去讀下一個東西了

    } // else()

    if ( gPeekBuffer.size() != 0 ) { // buffer有東西
      target = gPeekBuffer[0] ;
    } // if()

    else {
      target = Peektoken() ; // 先看不拿
    } // else

    if ( target == ";" ) { // 是分號就能結束了
      putin.content = target ;
      define = SEMICOLON ;
      putin.type = define ;
      grammarcorrect.push_back( putin ) ;
      gPeekBuffer.clear() ;
      return ;
    } // if()

    else {
      string temp ;
      temp = "Unexpected token : '" + target + "'" ;
      gPeekBuffer.clear() ; // 清除暫存內東西
      ReadtoEnd() ;
      grammarcorrect.clear() ; // 這行有錯就不用了
      throw temp ;
    } // else()
  } // if()

  NOT_ID_StartArithExpOrBexp() ;

  if ( gPeekBuffer.size() != 0 ) { // buffer有東西
    target = gPeekBuffer[0] ;
  } // if()

  else {
    target = Peektoken() ; // 先看不拿
  } // else()

  if ( target == ";" ) {
    putin.content = target ;
    define = SEMICOLON ;
    putin.type = define ;
    grammarcorrect.push_back( putin ) ;
    gPeekBuffer.clear() ;
    return ;
  } // if()

  else {
    string temp ;
    temp = "Unexpected token : '" + target + "'" ;
    gPeekBuffer.clear() ; // 清除暫存內東西
    ReadtoEnd() ;
    grammarcorrect.clear() ; // 這行有錯就不用了
    throw temp ;

  } // else()
} // Command()

// //////////////////////////////////////////////// Please remind to put struct and vector to top

// 計算從這裡開始

bool Realfloat( string a ) {

  for ( int i = 0 ; i < a.length() ; i++ ) {
    if ( a[i] == '.' ) {
      return true ;
    } // if()
  } // for()

  return false ;
} // Realfloat()

bool IsAssign( string a ) {

  for ( int i = 0 ; i < gIDDefinedTable.size() ; i++ ) {
    if ( a == gIDDefinedTable[i].idname ) return true ; // if()
  } // for()

  return false ;

} // IsAssign()

float GetIdAssignValue( string idname ) {

  for ( int i = 0 ; i < gIDDefinedTable.size() ; i++ ) {
    if ( idname == gIDDefinedTable[i].idname ) {
      gIsFloat = gIDDefinedTable[i].isFloat ;
      return gIDDefinedTable[i].idvalue ;
    } // if()
  } // for()

  return -1 ;
} // GetIdAssignValue()

float FactorCal() { // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'

  T define ;
  string temp ;

  temp = grammarcorrect[0].content ; // 拿正確文法的第一項
  define = grammarcorrect[0].type ;

  grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過

  if ( define == ID ) {

    if ( IsAssign( temp ) ) {
      float a ;
      a = GetIdAssignValue( temp ) ;
      return a ;
    } // if()

    else { // 此 ID 沒有被 Assign
      string error ;
      error = "Undefined identifier : '" + temp + "'" ;
      throw error ;
    } // else()

  } // if()

  else if ( define == NUM || define == NUMWITHPOINT ) {

    if ( define == NUMWITHPOINT ) {
      gIsFloat = true ;
    } // if()

    float a ;
    a = ( float ) atof( temp.c_str() ) ; //  將 string 轉 float
    return a ;
  } // else if()

  else if ( define == SIGNNUM || define == SIGNNUMWITHPOINT ) { // 是有正負號的數字

    if ( define == SIGNNUMWITHPOINT ) gIsFloat = true ; // if()

    if ( temp[0] == '+' ) {
      string change ;
      float a ;
      for ( int i = 1 ; i < temp.length() ; i ++ ) { // 把正號拿掉
        change = change + temp[i] ;
      } // for()

      a = ( float ) atof( change.c_str() ) ; //  將 string 轉 float
      return a ;

    } // if()

    else { // 是負號的情況
      string change ;
      float a ;
      for ( int i = 1 ; i < temp.length() ; i ++ ) { // 把正號拿掉
        change = change + temp[i] ;
      } // for()

      a = ( float ) atof( change.c_str() ) ; //  將 string 轉 float
      a = a * -1 ; // 將數字加上負號
      return a ;

    } // else()
  } // else if()

  else { // 因為確定文法正確 所以 一定是 ( 括號

    float ans ;
    ans = ArithExpCal() ;
    temp = grammarcorrect[0].content ;
    define = grammarcorrect[0].type ;
    grammarcorrect.erase( grammarcorrect.begin() ) ; // 把右括號取走
    return ans ;

  } // else()
} // FactorCal()

float TermCal() { // <Term>  ::= <Factor> { '*' <Factor> | '/' <Factor> }

  float first ;
  float second ;

  T define ;
  string temp ;

  first = FactorCal() ; // 要叫 Factor 取值

  temp = grammarcorrect[0].content ;
  define = grammarcorrect[0].type ;

  while ( temp == "*" || temp == "/" ) {

    if ( temp == "*" && define == MUL ) {
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
      second = FactorCal() ;
      first = first * second ; // 有遇到 * 就代表要做乘法運算
    } // if()

    else { // 是 / 的情況
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
      second = FactorCal() ;
      first = first / second ; // 有遇到 * 就代表要做乘法運算
    } // else()

    temp = grammarcorrect[0].content ; // 去看下一個是否還是 * 或 /
    define = grammarcorrect[0].type ;

  } // while()

  return first ;

} // TermCal()

float ArithExpCal() { // // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }

  float first ;
  float second ;

  T define ;
  string temp ;

  first = TermCal() ; // 要叫 Factor 取值

  temp = grammarcorrect[0].content ;
  define = grammarcorrect[0].type ;

  while ( define == PLUS || define == MINUS ) {

    if ( temp == "+" && define == PLUS ) {
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
      second = TermCal() ;
      first = first + second ; // 有遇到 * 就代表要做加法運算
    } // if()

    else {
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
      second = TermCal() ;
      first = first - second ; // 有遇到 * 就代表要做加法運算
    } // else()

    temp = grammarcorrect[0].content ;
    define = grammarcorrect[0].type ;

  } // while()

  return first ;

} // ArithExpCal()

float NOT_ID_StartFactorCal() { // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'

  string temp ;
  T define ;



  temp = grammarcorrect[0].content ;
  define = grammarcorrect[0].type ;
  grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過

  if ( define == SIGNNUM || define == SIGNNUMWITHPOINT ) {
    if ( define == SIGNNUMWITHPOINT ) {
      gIsFloat = true ;
    } // if()

    if ( temp[0] == '+' ) {
      string change ;
      float a ;
      for ( int i = 1 ; i < temp.length() ; i ++ ) { // 把正號拿掉
        change = change + temp[i] ;
      } // for()

      a = ( float ) atof( change.c_str() ) ; //  將 string 轉 float
      return a ;
    } // if()

    else {
      string change ;
      float a ;
      for ( int i = 1 ; i < temp.length() ; i ++ ) { // 把正號拿掉
        change = change + temp[i] ;
      } // for()

      a = ( float ) atof( change.c_str() ) ; //  將 string 轉 float
      a = a * -1 ; // 將數字加上負號
      return a ;
    } // else()
  } // if()

  else if ( define == NUM || define == NUMWITHPOINT ) {
    if ( define == NUMWITHPOINT ) {
      gIsFloat = true ;
    } // if()

    float a ;
    a = ( float ) atof( temp.c_str() ) ; //  將 string 轉 float
    return a ;

  } // else if()

  else {
    float ans ;
    ans = ArithExpCal() ;
    temp = grammarcorrect[0].content ;
    define = grammarcorrect[0].type ;
    grammarcorrect.erase( grammarcorrect.begin() ) ; // 把右括號取走
    return ans ;
  } // else()
} // NOT_ID_StartFactorCal()

float NOT_ID_StartTermCal() {
// <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }

  float first ;
  float second ;

  T define ;
  string temp ;

  first = NOT_ID_StartFactorCal() ;

  temp = grammarcorrect[0].content ; // 拿正確文法的第一項
  define = grammarcorrect[0].type ;

  while ( define == MUL || define == DIV ) {
    if ( define == MUL && temp == "*" ) {
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的乘號取出來刪掉， 代表用過
      second = FactorCal() ;
      first = first * second ;

    } // if()

    else { // 為 / 的情況
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的除號取出來刪掉， 代表用過
      second = FactorCal() ;
      first = first / second ;
    } // else

    temp = grammarcorrect[0].content ; // 拿正確文法的第一項
    define = grammarcorrect[0].type ;
  } // while()

  return first ;

} // NOT_ID_StartTermCal()

float NOT_ID_StartArithExpCal() {
// <NOT_ID_StartArithExp> ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }

  float first ;
  float second ;
  T define ;
  string temp ;

  first = NOT_ID_StartTermCal() ;

  temp = grammarcorrect[0].content ; // 拿正確文法的第一項
  define = grammarcorrect[0].type ;

  while ( define == PLUS || define == MINUS ) {
    if ( define == PLUS && temp == "+" ) {
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的乘號取出來刪掉， 代表用過
      second = TermCal() ;
      first = first + second ;

    } // if()

    else { // 為 / 的情況
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的除號取出來刪掉， 代表用過
      second = TermCal() ;
      first = first - second ;
    } // else

    temp = grammarcorrect[0].content ; // 拿正確文法的第一項
    define = grammarcorrect[0].type ;

  } // while()

  return first ;

} // NOT_ID_StartArithExpCal()

float NOT_ID_StartArithExpOrBexpCal() {
// <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]
// 進 if else 的話 就直接 throw 答案出來

  float first ;
  float second ;
  T define ;
  string temp ;

  first = NOT_ID_StartArithExpCal() ;
  // cout << "first" << first << endl ;


  temp = grammarcorrect[0].content ; // 拿正確文法的第一項
  define = grammarcorrect[0].type ;

  if ( define == BOOLOP ) { // '=' | '<>' | '>' | '<' | '>=' | '<=' 是這一排其中一個
    gForC2 = false ;
    grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
    second = ArithExpCal() ;
    // cout << "second"<< second << endl ;

    if ( temp == "=" ) {
      if ( first == second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // if()

    else if ( temp == "<>" ) {
      if ( first != second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // else if()

    else if ( temp == ">" ) {
      if ( first > second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // else if()

    else if ( temp == "<" ) {
      if ( first < second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // else if()

    else if ( temp == ">=" ) {
      if ( first >= second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // else if()

    else if ( temp == "<=" ) {
      if ( first <= second ) {
        cout << "> true" << endl ;
        return 0 ;
      } // if()

      else {
        cout << "> false" << endl ;
        return 0 ;
      } // else()
    } // else if()

  } // if()

  gForC2 = true ;
  return first ;

} // NOT_ID_StartArithExpOrBexpCal()

void CommandCal() {

// <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
// | <NOT_IDStartArithExpOrBexp> ';'
// | QUIT

  float first ;
  float second ;
  float third ;

  string firstID ;
  T firstdefine ;

  string temp ;
  T define ;

  IDToken tempIdToken ;

  firstID = grammarcorrect[0].content ; // 拿正確文法的第一項
  firstdefine = grammarcorrect[0].type ;

  if ( firstdefine == ID ) { // 確定第一項為 ID 很長 ....
    grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過

    temp = grammarcorrect[0].content ; // 拿正確文法的第一項
    define = grammarcorrect[0].type ;

    if ( temp == ":=" ) { // 代表是 Assign
      grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
      if ( IsAssign( firstID ) ) { // 假如這個 ID Assign 過 就要去替代原本的值
        float ans ;
        for ( int i = 0 ; i < gIDDefinedTable.size() ; i++ ) {
          if ( firstID == gIDDefinedTable[i].idname ) {
            gIDDefinedTable[i].idvalue = ArithExpCal() ;
            gIDDefinedTable[i].isFloat = gIsFloat ;
            ans = gIDDefinedTable[i].idvalue ;
          }  // if()
        } // for()

        if ( gIsFloat ) { // 印小數點
          printf( "> %1.3f\n", ans ) ;
        } // if()

        else { // 印整數
          printf( "> %1.0f\n", ans ) ;
        } // else()

        // cout << "> " << ans << endl ;
        return ;
      } // if()

      else {
        tempIdToken.idname = firstID ; // 把要 Assign 的放進去 ID Define vector
        tempIdToken.idvalue = ArithExpCal() ;
        tempIdToken.isFloat = gIsFloat ;
        gIDDefinedTable.push_back( tempIdToken ) ;

        if ( gIsFloat ) { // 印小數點
          printf( "> %1.3f\n", tempIdToken.idvalue ) ;
        } // if()

        else { // 印整數
          printf( "> %1.0f\n", tempIdToken.idvalue ) ;
        } // else()

        // cout << "> " << tempIdToken.idvalue << endl ;
        return ;
      } // else()
    } // if() 是 Assign 那條路

    else { // 是 <IDlessArithExpOrBexp> 那條路
    // <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term> | '*' <Factor> | '/' <Factor> }
    // [ <BooleanOperator> <ArithExp> ]

    // 要先檢查 是否 assign 過

      if ( !IsAssign( firstID ) ) { // 沒有 Assign 過
        string error ;
        error = "Undefined identifier : '" + firstID + "'" ;
        throw error ;
      } // if()

      else { // 有 Assign 過
        first = GetIdAssignValue( firstID ) ;

        while ( define == PLUS || define == MINUS || define == MUL || define == DIV ) {
          if ( define == PLUS && temp == "+" ) {
            grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
            second = TermCal() ;
            first = first + second ;
          } // if()

          else if ( define == MINUS && temp == "-" ) {
            grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
            second = TermCal() ;
            first = first - second ;
          } // else if()

          else if ( define == MUL && temp == "*" ) {
            grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
            second = TermCal() ;
            first = first * second ;
          } // else if()

          else {
            grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
            second = TermCal() ;
            if ( second == 0 ) {
              string temperror = "> Error" ;
              throw temperror ;
            } // if()

            else {
              first = first / second ;
            } // else()
          } // else()

          temp = grammarcorrect[0].content ; // 拿正確文法的第一項
          define = grammarcorrect[0].type ;

        } // while()

        if ( define == BOOLOP ) {
          grammarcorrect.erase( grammarcorrect.begin() ) ; // 把正確文法中的第一個取出來刪掉， 代表用過
          third = ArithExpCal() ;
          float big ;
          float small ;
          float toler ;

          if ( temp == "=" ) {

            if ( first = third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()
          } // if()

          else if ( temp == "<>" ) {

            if ( first != third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()
          } // else if()

          else if ( temp == ">" ) {

            if ( first > third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()

          } // else if() == ">"

          else if ( temp == "<" ) {

            if ( first < third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()

          } // else if()

          else if ( temp == ">=" ) {
            if ( first >= third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()
          } // else if()

          else if ( temp == "<=" ) {
            if ( first <= third ) {
              cout << "> true" << endl ;
              return ;
            } // if()

            else {
              cout << "> false" << endl ;
              return ;
            } // else()
          } // else if()
        } // if()

        // cout << "hi" << endl ;
        if ( gIsFloat ) { // 印小數點
          printf( "> %1.3f\n", first ) ;
        } // if()

        else { // 印整數
          printf( "> %1.0f\n", first ) ;
        } // else()
      } // else()
    } // else()
  } // if()

  else {
    float mmm ;
    mmm = NOT_ID_StartArithExpOrBexpCal() ;

    if ( gForC2 ) {
      if ( gIsFloat ) { // 印小數點
        printf( "> %1.3f\n", mmm ) ;
      } // if()

      else { // 印整數
        printf( "> %1.0f\n", mmm ) ;
      } // else()

      return ;
    } // if()

    else {
      return ;
    } // else()

  } // else()

} // CommandCal()

int main()
{
  int aaaa ;
  //cin >> aaaa ;

  while ( gEnd != 1 ) {
    try {

      Command() ;
      // cout << grammarcorrect.size() << endl ;
      // for ( int i = 0 ; i < grammarcorrect.size() ; i++ ) {
       // cout << grammarcorrect[i].content << endl ;
     // }
      CommandCal() ;
    } // try

    catch ( string a ) {

      cout << "> " << a << endl ;

    } // catch

    grammarcorrect.clear() ;
    gPeekBuffer.clear() ;
    gIsFloat = false ;

  } // while()



  return 0 ;
} // main()
