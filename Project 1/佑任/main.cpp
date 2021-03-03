# include <stdio.h>
# include <iostream> // CIN
# include <string> // STRING
# include <vector> // VECTOR
# include <stdlib.h> // atoi
# define TOLERANCE 0.0001

static int uTestNumber = 0 ; // 夏氏debug
using namespace std ;

// 各種標籤
// 0:INPUTEOF     1:ASSIGN:=    2:SIGN+-   3:NUM    4:IDENT
// 5:EQ=          6:SEMI;       7:ADD+     8:SUB-   9:MULT*
// 10:DIV/        11:BEQ布林=   12:LG<>    13:G>    14:L<
// 15:GE>=        16:LE<=       17:RP)     18:LP(   19:ADDORSIGN
//  20:SUBORSIGN  21:POSIVITE正 22:NEGATIVE負 23:QUIT 24:FLNUM小數
typedef enum
{
  INPUTEOF, ASSIGN, SIGN, NUM, IDENT, EQ, SEMI,
  ADD, SUB, MULT, DIV, BEQ, LG, G, L, GE, LE, RP, LP,
  ADDORSIGN, SUBORSIGN, POSIVITE, NEGATIVE, QUIT, FLNUM
}
TOKENTYPE ;

void ThrowEND( TOKENTYPE type ) {
  throw type ; // 丟出中斷訊息
} // ThrowEND()

void ThrowERROR() {
  string errorMSG = "Error" ;
  throw errorMSG ; // 丟出錯誤的訊息
} // ThrowERROR()

void ThrowERROR1( char ch ) {
  string str = "Unrecognized token with first char : '" ;
  string errorMSG = str + ch ;
  errorMSG = errorMSG + "'" ;
  throw errorMSG ; // 丟出錯誤的訊息
} // ThrowERROR1()

void ThrowERROR2( string str ) {
  string errorMSG = "Unexpected token : '" + str ;
  errorMSG = errorMSG + "'" ;
  throw errorMSG ; // 拋出錯誤例外ERROR2
} // ThrowERROR2()

void ThrowERROR3( string str ) {
  string errorMSG = "Undefined identifier : '" + str ;
  errorMSG = errorMSG + "'" ; // THROW出去的訊息
  throw errorMSG ; // 拋出錯誤例外ERROR2
} // ThrowERROR3()

class CharDetail { // 用來存放一個CHAR的結構
  public:
  char mCh  ; // 字元
  int mLineNumber ; // 行
  int mCloumnNumber ; // 列
  CharDetail() { // 建構子
    mCh = '\0' ;
    mLineNumber = 0 ;
    mCloumnNumber = 0 ;
  } // CharDetail()
}; // class CharDetail

class TokenDetail { // 用來存放一個TOKEN的結構
  public:
  TOKENTYPE mTokenType ; // 是甚麼類別的
  string mToken ; // Token是甚麼
  int mTokenLine ; // 此token是哪一行的
  TokenDetail() { // 建構子
    mTokenLine = 0 ;
  } // TokenDetail()
}; // class Token

class IDDetail { // 用來存放一個ID的結構
  public:
  string mIDName ; // 名子
  int mIDTokenLine ; // 行
  float mIDValue ; // ID所代表的值
  bool mIDisfloat ;  // 小數or整數
  IDDetail() { // 建構子
    mIDTokenLine = 0 ;
    mIDValue = 0 ;
  } // IDDetail()
}; // class IDDetail

class Scanner { // Class Scanner
  private:

  char mCh ; // 字元
  int mLineNumber ; // 字元的行
  int mCloumnNumber ; // 字元的列

  vector< CharDetail > mCharVector ;// 放一堆Char和其LineNumber和CloumnNumber口口口口口口口口口
  vector< TokenDetail > mPeekToken ; // 暫存一個偷看的token

  public:
  Scanner() { // Scanner建構子
    mLineNumber = 0 ;
    mCloumnNumber = 0 ;
  } // Scanner()

  void ClearToEnter() { // 將輸入讀到換行為止
    while ( mCh != '\n' ) GetTheNextChar() ; // 用GetTheNextChar()直接讀到換行為止
  } // ClearToEnter()

  bool IsDigit( char ch ) { // 是否為數字
    if ( ch >= 48 && ch <= 57 ) return true ;
    else return false ;
  } // IsDigit()

  bool IsLetter( char ch ) { // 是否為大小寫的字母
    if ( ( ch >= 65 && ch <= 90 ) || ( ch >= 97 && ch <= 122 ) ) return true ;
    else return false ;
  } // IsLetter()

  bool IsDelimiter( char ch ) { // 是否為符號
    if ( ch == ':' || ch == ';' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||  ch == '=' ||
         ch == '<' || ch == '>' || ch == '(' || ch == ')' ) return true ;
    else return false ;
  } // IsDelimiter()

  void ClearmPeekToken() { // 清除偷看的buffer
    mPeekToken.clear() ;
  } // ClearmPeekToken()

  TokenDetail TokenIsID() { // 去把ID剩下的部分讀近來
    // cout << "In TokenIsID()" << endl ; // debug用
    TokenDetail mTempToken ; // 暫存
    string token ; // token
    char mPeekch ; // 偷看的字元
    token = token + mCh ; // 串起來
    mPeekch = cin.peek() ; // 偷看下一個char
    while ( IsLetter( mPeekch ) || IsDigit( mPeekch  ) || ( mPeekch == '_' ) ) { // 只要符合ID都拿走且串起來
      GetTheNextChar() ; // 讀入剛剛偷看的那個
      token = token + mCh ; // 串起來
      mPeekch = cin.peek() ; // 偷看下一個char
    } // while()

    mTempToken.mTokenType = IDENT ; // 設定值
    mTempToken.mToken = token ; // 設定值
    mTempToken.mTokenLine = mLineNumber ; // 設定值
    return mTempToken ;
    // cout << "Out TokenIsID()" << endl ; // debug用
  } // TokenIsID()

  TokenDetail TokenIsNUM() { // 去把NUM剩下的部分讀近來(有可能是小數)
    TokenDetail mTempToken ; // 暫存
    // cout << "In TokenIsNUM()" << endl ; // debug用
    string token ;
    token = token + mCh ; // 串起來
    char mPeekch ; // 偷看的字元
    bool dot = false ; // 判斷有沒有小數點

    if ( mCh == '.' ) { // 點開頭的數
      dot = true ;
      mPeekch = cin.peek() ;
      while ( IsDigit( mPeekch ) ) { // 如果偷看的是數字
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mPeekch = cin.peek() ; // 繼續偷看
      } // while()
    } // if()
    else if ( IsDigit( mCh ) ) { // 數字開頭的數字
      mPeekch = cin.peek() ; // 偷看看
      while ( IsDigit( mPeekch ) ) {  // 如果偷看的是數字
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mPeekch = cin.peek() ; // 繼續偷看
      } // while()

      if ( mPeekch == '.' ) { // 如果偷看的是一個點
        dot = true ;
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mPeekch = cin.peek() ; // 繼續偷看
        while ( IsDigit( mPeekch ) ) {  // 如果又偷看到是數字
          GetTheNextChar() ; // 讀入剛剛偷看的那個
          token = token + mCh ; // 串起來
          mPeekch = cin.peek() ; // 繼續偷看
        } // while()
      } // if()
    } // else if()

    if ( dot == true ) {
      mTempToken.mTokenType = FLNUM ; // 設定值
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定值
    } // if()
    else if ( dot == false ) {
      mTempToken.mTokenType = NUM ; // 設定值
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定值
    } // else if()

    return mTempToken ;
  } // TokenIsNUM()

  TokenDetail TokenIsDelimiter() { // 做判斷且去把Delimiter剩下的部分讀近來
    TokenDetail mTempToken ; // 暫存
    // cout << "In TokenIsDelimiter()" << endl ; // debug用
    string token ;
    char mPeekch ; // 偷看的字元

    if ( mCh == ':' ) { // 如果是:
      token = token + mCh ; // 串起來
      mPeekch = cin.peek() ; // 偷看下一個char
      if ( mPeekch == '=' ) { // 如果是=
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mTempToken.mTokenType = ASSIGN ; // 設定值
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定值
      } // if()
      else { // 第一層ERROR:Unrecognized token with first char
        ThrowERROR1( mCh ) ;
      } // else
    } // if()
    else if ( mCh == '<' ) { // '<'有兩情況
      token = token + mCh ; // 串起來
      mPeekch = cin.peek() ; // 偷看下一個char
      if ( mPeekch == '>' ) {
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mTempToken.mTokenType = LG ; // 設定標籤
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定行
      } // if()
      else if ( mPeekch == '=' ) {
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mTempToken.mTokenType = LE ; // 設定標籤
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定行
      } // else if
      else {
        mTempToken.mTokenType = L ; // 設定標籤
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定行
      } // else
    } // else if()
    else if ( mCh == '>' ) { // '>'有兩情況
      token = token + mCh ; // 串起來
      mPeekch = cin.peek() ; // 偷看下一個char
      if ( mPeekch == '=' ) {
        GetTheNextChar() ; // 讀入剛剛偷看的那個
        token = token + mCh ; // 串起來
        mTempToken.mTokenType = GE ; // 設定標籤
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定行
      } // if()
      else {
        mTempToken.mTokenType = G ; // 設定標籤
        mTempToken.mToken = token ; // 設定值
        mTempToken.mTokenLine = mLineNumber ; // 設定行
      } // else
    } // else if()
    else if ( mCh == ';' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = SEMI ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '+' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = ADDORSIGN ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '-' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = SUBORSIGN ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '*' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = MULT ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '/' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = DIV ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '=' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = EQ ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == '(' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = LP ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()
    else if ( mCh == ')' ) {
      token = token + mCh ; // 串起來
      mTempToken.mTokenType = RP ; // 設定標籤
      mTempToken.mToken = token ; // 設定值
      mTempToken.mTokenLine = mLineNumber ; // 設定行
    } // else if()

    return mTempToken ;
    // cout << "Out TokenIsDelimiter()" << endl ; // debug用
  } // TokenIsDelimiter()

  // 呼叫這個時一定是從mPeekToken拿然後刪掉mPeekToke(代表我真得拿走偷看的了)
  TokenDetail GetToken() {
    // cout << "Into GetToken()" << endl ; // debug用
    TokenDetail mTempToken ; // 暫存
    mTempToken = mPeekToken.front() ; // 拿
    // cout << "[" << mTempToken.mToken << "]" << endl ;
    mPeekToken.clear() ; // 刪掉
    return mTempToken ;
    // cout << "Out GetToken()" << endl ; // debug用
  } // GetToken()

  // 偷看下一個token暫存起來(且略過ERRORTOKEN)
  TokenDetail PeekToken() {
    // cout << "Into PeekToken()" << endl ; // debug用
    if ( mPeekToken.empty() == false ) { // mPeekToken裡面有東西就回傳東西回去
      TokenDetail mTempToken ; // 暫存
      mTempToken = mPeekToken.front() ;
      // cout << "["<< mTempToken.mToken << "]" << endl ;
      return mTempToken ;
    } // if()
    else { // 如果空的就讀一個存到mPeekTokens並切依然回傳這個token資訊(裝被讀走了其實還沒，若是一個ERRORTOKEN就重新拿一次)
      TokenDetail mTempToken ; // 暫存
      mTempToken = TheRealGetToken() ; // 真的從INPUT讀掉字元
      mPeekToken.push_back( mTempToken ) ;
      // cout << "[" << mTempToken.mToken << "]" << endl ;
      // if ( mTempToken.mTokenType == INPUTEOF ) ThrowEND( QUIT ) ;
      return mTempToken ;
    } // else
    // cout << "Out PeekToken()" << endl ;
  } // PeekToken()

  // 從input讀入一個TOKEN進來
  TokenDetail TheRealGetToken() {
    // cout << "into TheRealGetToken()" << endl ; // debug用
    TokenDetail mTempToken ; //
    GetTheNextNonWhiteSpaceChar() ; // 讀下一個token的first char
    if ( mCh == EOF ) { // 若input沒了
      // cout << " INPUT EOF!!! " << endl ; // debug用
      ThrowEND( QUIT ) ;
      return mTempToken ;
    } // if()
    else if ( IsLetter( mCh ) ) { // 若ch是一個letter
      mTempToken = TokenIsID() ; // 呼叫TokenIsID()來找出整個TOKEN
      return mTempToken ;
    } // if()
    else if ( IsDigit( mCh ) || ( mCh == '.' ) ) { // 若ch是一個數字or'.'
      mTempToken = TokenIsNUM() ; // 呼叫TokenIsNUM()來找出整個TOKEN
      if ( mTempToken.mToken == "." ) ThrowERROR1( '.' ) ;
      return mTempToken ;
    } // else if()
    else if ( IsDelimiter( mCh ) ) { // 若CHAR是一個符號 TokenIsDelimiter()來找出整個TOKEN
      mTempToken = TokenIsDelimiter() ;
      return mTempToken ;
    } // else if()
    else {
      // cout << "throw" ;
      ThrowERROR1( mCh ) ; // 無法辨識的字元
    } // else

    return mTempToken ;
    // cout << "Unrecognized token with first char : " << "'" << mTokenValue[0] << "'" << endl ; // (結果)
    // cout << "out TheRealGetToken()" << endl ;// debug用
  } // TheRealGetToken()

  void GetTheNextChar() { // 追蹤紀錄所拿到的char的line number和column number
    CharDetail mTempChar ; // 暫時放一個得到的char的資訊
    // cout << "into GetTheNextChar()" << endl ; // debug用
    if ( cin.get( mCh ) ) { // 讀一個char
      mTempChar.mCh = mCh ; // char填入temp
      mTempChar.mLineNumber = mLineNumber ; // lineNumber填入temp
      mTempChar.mCloumnNumber = mCloumnNumber ; // CloumnNumber填入temp
      // cout << " CHAR: " << "[" << mTempChar.mCh << "]" << " lineNumber : " ; // debug用
      // cout << mTempChar.mLineNumber << " cloumnNumber : " << mTempChar.mCloumnNumber << endl ; // debug用
      mCharVector.push_back( mTempChar ) ; // 將char放入CharVector
      if ( mCh == '\n' ) { // 如果讀到一個'\n'
        mLineNumber = mLineNumber + 1 ; // 行數+1
        mCloumnNumber = 1 ; // 列從1開始23
      } // if()
      else { // 如果不是讀到一個'\n'
        mCloumnNumber = mCloumnNumber + 1 ; // 列+1
      } // else
    } // if()
    else { // 沒有input了
      mCh = EOF ; // 回報目前輸入是EOF
    } // else
    // cout << "out GetTheNextChar() ;" << endl ;// debug用
  } // GetTheNextChar()

  void GetTheNextNonWhiteSpaceChar() { // 此function要負責跳過comment和non-white-space char
    // cout << "into GetTheNextNonWhiteSpaceChar()" << endl ; // debug用
    GetTheNextChar() ; // 呼叫拿取一個char
    char mPeekch ; // 偷看的字元
    while ( mCh == ' ' || mCh == '\n' || mCh == '\t' || mCh == '/' ) { // 如果mCh是這四個其中一個
      if ( mCh == '/' ) { // 如果mCh是'/'
        mPeekch = cin.peek() ; // 偷看下一個
        if ( mPeekch == '/' ) { // 如果下一個也是/代表後都是註解
          GetTheNextChar() ;  // 先把剛剛的第二個斜線讀掉
          ClearToEnter() ; // 讀到'\n' 為止
        } // if() 遇到第二個註解斜線 '/
        else { // 只有一個/代表只是除法
          return ;
        } // else()
      } // if()
      else if ( mCh == ' ' || mCh == '\n' || mCh == '\t' ) { // 如果mCh是non-white-space
        GetTheNextChar() ;  // 要跳過這些字要往下讀
      } // else if
    } // while()
      // cout << "out GetTheNextNonWhiteSpaceChar() " << endl ; // debug用
  } // GetTheNextNonWhiteSpaceChar()

}; // Class Scanner

class Parser { // Class Parser
  private:
  Scanner mScanner ; // 此Parser的scanner

  vector < TokenDetail > mPassSyntaxVector ; // 暫存一條指令(通過文法的)(Token和類別和號碼 口口口口口口口口口)
  vector < IDDetail > mDefinedIDVector ; // 存放已經定義過的ID(名字，值，行)
  bool mIsfloat ;
  public:
  Parser( Scanner sc ) { // 建構子 (傳入在main宣告的scanner)
    mScanner = sc ; // 將scanner視為使用同一個
    mIsfloat = false ;
  } // Parser()

  void PushInTomDefinedIDVector( IDDetail mTempID ) { // 新增一個ID到mDefinedIDVector
    mDefinedIDVector.push_back( mTempID ) ; // 將一個新的ID推進去
  } // PushInTomDefinedIDVector()

  void SetTheIDValue( string IDname, float IDvalue ) { // 到mDefinedIDVector設定修改以定義過的ID值
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        mDefinedIDVector[i].mIDValue = IDvalue ; // 把ID的值更新
        return ;
      } // if()
    } // for
  } // SetTheIDValue()

  bool InThemDefinedIDVectorOrNot( string IDname ) { // 判斷此ID有沒有被定義過
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) return true ;
    } // for()

    return false ;
  } // InThemDefinedIDVectorOrNot()

  void GetTheIDValue( string IDname, float &IDvalue ) { // 如果有被定義，拿取已被定義過的ID的數值
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        IDvalue = mDefinedIDVector[i].mIDValue ;
        return ;
      } // if
    } // for()
  } // GetTheIDValue()

  void SetTheIDNumType( string IDname, bool isfloat ) { // 去設定此ID的數值是小數還是整數..................
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        mDefinedIDVector[i].mIDisfloat = isfloat ;
        return ;
      } // if
    } // for()
  } // SetTheIDNumType()

  void GetTheIDNumType( string IDname, bool &isfloat ) { // 取得ID的數值是小數還是整數
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        isfloat = mDefinedIDVector[i].mIDisfloat  ;
        // cout << isfloat << endl ;
        return ;
      } // if()
    } // for()
  } // GetTheIDNumType()
  // ------------------------------------------文法分析-------------------------
  void Factor() { // 文法<Factor>
    // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'
    TokenDetail mTempToken ; // 暫存
    mTempToken = mScanner.PeekToken() ; // 偷看下一個Token

    // 如果完全不屬於<Factor>想要的就直接回傳false
    if ( ( mTempToken.mTokenType != IDENT && mTempToken.mTokenType != ADDORSIGN &&
           mTempToken.mTokenType != SUBORSIGN && mTempToken.mTokenType != NUM &&
           mTempToken.mTokenType != LP && mTempToken.mTokenType != FLNUM ) ) {
      ThrowERROR2( mTempToken.mToken ) ;
    } // if()

    // 若是IDENT或者不是在<command>第一個拿到的"quit"那也是一個ID
    if ( mTempToken.mTokenType == IDENT  ) {
      if ( InThemDefinedIDVectorOrNot( mTempToken.mToken ) == false ) { // 如果沒有被定義
        ThrowERROR3( mTempToken.mToken ) ; // 丟出例外3
      } // if()
      // 到此為止，一定是ID且有定義
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個ID
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // if()

    // 如果是一個ADDORSIGN
    else if ( mTempToken.mTokenType == ADDORSIGN ) {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個(+)
      mTempToken.mTokenType = POSIVITE ; // 將ADDORSIGN改成POSIVITE，因為在這裡一定是正負
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      mTempToken = mScanner.PeekToken() ; // 偷看下一個Token
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // 如果下一個不是NUM也不是FLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 到此為止，下一個一定是NUM
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個NUM
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else if()

    // 如果是ASUBORSIGN
    else if ( mTempToken.mTokenType == SUBORSIGN ) {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個(-)
      mTempToken.mTokenType = NEGATIVE ; // 將SUBORSIGN改成NEGATIVE，因為在這裡一定是SIGN
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      mTempToken = mScanner.PeekToken() ; // 偷看下一個Token
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // 如果不是NUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 若到此，下一個一定是NUM或FLNUM
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個NUM
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else if()
    // 如果直接是NUMorFLNUM(可以不一定要有SIGN)
    else if ( mTempToken.mTokenType == NUM ||  mTempToken.mTokenType == FLNUM ) {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else if()
    // 若到此，下一個一定是左括號LP
    else {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個LP
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      ArithExp() ; // 呼叫ArithExp()
      mTempToken = mScanner.PeekToken() ; // 偷看下一個Token
      if ( mTempToken.mTokenType != RP ) { // 如果不是右括號RP
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 到此為止下一個一定是RP
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else
  } // Factor()

  void Term() { // 文法<Term>
    // <Term> ::= <Factor> { '*' <Factor> | '/' <Factor> }
    TokenDetail mTempToken ; // 暫存
    Factor() ; // 呼叫第一個Factor()
    // 到此為止第一個Factor一定正確

    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // 結束

    // 下一個是乘除就繼續做
    while ( mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {
      // 到此為止下一個一定是*或/
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      Factor() ; // 呼叫第二個Factor()
      // 到此為止Factor第二次一定正確
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // 結束
    } // while()
    // 到此為止都正確
    return ; // 結束

  } // Term()

  void ArithExp() { // 文法<ArithExp>
    // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }
    TokenDetail mTempToken ; // 暫存
    Term() ; // 呼叫Term()
    // 到此為止第一個<Term>一定正確
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // 結束
    // 只要下一個是加減就繼續
    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {

      // 到此為止下一個一定是ADDORSIGN or SUBORSIGN
      mTempToken = mScanner.GetToken() ;  // 拿取偷看的那個

      // 因為在此部分一定是單純的加減不是正負
      if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // 將ADDORSIGN改回ADD
      else mTempToken.mTokenType = SUB ; // 將SUBORSIGN改回SUB
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      Term() ; // 呼叫Term()
      // 到此為止Term第二次一定正確
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // 結束
    } // while() // 若沒有任何錯誤依然繼續做

    return ;
  } // ArithExp()

  void BooleanOperator( bool &Correct ) { // 文法<BooleanOperator>
    // <BooleanOprator> ::= '=' | '<>' | '>' | '<' | '>=' | '<='
    TokenDetail mTempToken ; // 暫存
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN

    // 如果不是任意一個布林符號
    if ( mTempToken.mTokenType != EQ && mTempToken.mTokenType != LG
         && mTempToken.mTokenType != G && mTempToken.mTokenType != L
         && mTempToken.mTokenType != GE && mTempToken.mTokenType != LE ) {
      Correct = false ;
      return ;
    } // if()
    // 到此為止下一個一定是個布林符號

    Correct = true ; // 是個布林林符號
    return ; // 結束
  } // BooleanOperator()

  void NOT_ID_StartFactor() { // 文法<NOT_ID_StartFactor>
    // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'
    TokenDetail mTempToken ; // 暫存
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN

    // 若不是<NOT_ID_StartFactor>所預期的type
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN
         && mTempToken.mTokenType != NUM && mTempToken.mTokenType != LP && mTempToken.mTokenType != FLNUM ) {
      ThrowERROR2( mTempToken.mToken  ) ;
    } // if()
    // 到次為止下一個一定是'ADDORSIGN' OR 'SUBORSIGN' OR 'NUM' OR '('
    if ( mTempToken.mTokenType == ADDORSIGN ) { // 若是ADDORSIGN
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mTempToken.mTokenType = POSIVITE ; // 將ADDORSIGN改成正，因為在此一定是個正號
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // 若不是NUM OR FLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 到此為止下一個一定是NUM
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // if()

    // 如果是一個SUBORSIGN
    else if ( mTempToken.mTokenType == SUBORSIGN ) {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mTempToken.mTokenType = NEGATIVE ; // 將SUBORSIGN改成負，因為在此一定是個負號
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // 若不是NUM OR FLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 到此為止下一個一定是NUM
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else if()
    // 若下一個直接是一個NUM OR FLNUM
    else if ( mTempToken.mTokenType == NUM || mTempToken.mTokenType == FLNUM ) {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else if()
    // 到此為止下一個一定是左括號LP
    else {
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個LP
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      ArithExp() ; // 呼叫ArithExp()
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != RP ) { // 如果不是右括號RP
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // 到此為止下一個一定是右括號
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else
  } // NOT_ID_StartFactor()

  void NOT_ID_StartTerm() { // 文法<NOT_ID_StartTerm>
    // <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }
    TokenDetail mTempToken ; // 暫存
    NOT_ID_StartFactor() ; // 呼叫NOT_ID_StartFactor()
    // 到此為止NOT_ID_StartFactor()正確

    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // 結束

    while ( mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {
      // 到此為止下一個一定是 '*' or '/'
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      Factor() ; // 呼叫Factor()
      // 到此為止Factor一定正確
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // 結束
    } // while()

    return ; // 結束
  } // NOT_ID_StartTerm()

  void NOT_ID_StartArithExp() { // 文法<NOT_ID_StartArithExp>
    // <NOT_ID_StartArithExp>  ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }
    TokenDetail mTempToken ; // 暫存
    NOT_ID_StartTerm() ; // 呼叫NOT_ID_StartTerm()
    // 到此為止NOT_ID_StartTerm()一定正確
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // 結束

    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {

      // 到此為止下一個一定是'ADDORSIGN' or 'SUBORSIGN'
      mTempToken = mScanner.GetToken() ;  // 拿取偷看的那個
      // 在此處一定是加減而不是正負
      if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // 將ADDORSIGN改回ADD
      else mTempToken.mTokenType = SUB ; // 將SUBORSIGN改回SUB

      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      Term() ; // 呼叫Term()
      // 到此為止Term一定正確

      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
      if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // 結束
    } // while()

    return ;
  } // NOT_ID_StartArithExp()

  void NOT_ID_StartArithExpOrBexp() { // 文法<NOT_ID_StartArithExpOrBexp>
    // <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]
    TokenDetail mTempToken ; // 暫存
    NOT_ID_StartArithExp() ; // 呼叫NOT_ID_StartArithExp()
    // 到此為止NOT_ID_StartArithExp()一定正確
    // 接著判斷<BooleanOperator>和<ArithExp>要同時正確或同時錯誤才可以!

    bool boolBooleanOperator = false ; // 用來判斷<BooleanOperator>是否正確
    BooleanOperator( boolBooleanOperator ) ;  // 呼叫BooleanOperator()
    if ( boolBooleanOperator  == true )  { // 布林正確的話
      mTempToken = mScanner.GetToken() ; // 拿走boolean
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      ArithExp() ; // 呼叫ArithExp()
      return ;
    } // if()
    else {  // 如果布林錯誤的話(因為布林可有可無)
      return ;
    } // else()那就是對的

  } // NOT_ID_StartArithExpOrBexp()

  void IDlessArithExpOrBexp( string idname ) { // 文法<IDlessArithExpOrBexp>
    // <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term>
    // | '*' <Factor> | '/' <Factor>
    // }
    // [ <BooleanOperator> <ArithExp> ]

    TokenDetail mTempToken ; // 暫存
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN


    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN
            || mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {

      if ( InThemDefinedIDVectorOrNot( idname ) == false ) { // 如果沒有被定義
        ThrowERROR3( idname ) ; // 丟出例外3
      } // if()

      if ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {
        mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
        // 在此部分一定是要改成加減
        if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // 將ADDORSIGN改回ADD
        else mTempToken.mTokenType = SUB ; // 將SUBORSIGN改回SUB

        mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
        // '+'，'-'就要判斷後面的<Term>
        Term() ; // 呼叫Term()
      } // if()
      else { // 如果是'*' or '/'
        mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
        mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
        // '*' or '/'就要判斷後面的<Factor>
        Factor() ; // 呼叫Factor()
      } // else
      // 到此為止不論'+'，'-'，'*'，'/'的<Term>or<Factor>都有正確
      mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    } // while()
    // 到此為止上面{}的一定正確
    // { }判斷結束後 要判斷<BooleanOperator>和<ArithExp>且要同時正確或同時錯誤才可以!
    bool boolBooleanOperator = false ; // 用來判斷<BooleanOperator>是否正確
    BooleanOperator( boolBooleanOperator ) ;  // 呼叫BooleanOperator()
    if ( boolBooleanOperator  == true )  { // 布林正確的話

      if ( InThemDefinedIDVectorOrNot( idname ) == false ) { // 如果沒有被定義
        ThrowERROR3( idname ) ; // 丟出例外3
      } // if()

      mTempToken = mScanner.GetToken() ; // 拿走boolean
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      ArithExp() ; // 呼叫ArithExp()
      return ;
    } // if()
    else {  // 如果布林錯誤的話(因為布林可有可無)
      return ;
    } // else() 那就是對的
  } // IDlessArithExpOrBexp()

  void Command() { // 文法<Command>
    // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
    // | <NOT_IDStartArithExpOrBexp> ';'
    // | QUIT
    TokenDetail mTempToken ; // 暫存
    mTempToken = mScanner.PeekToken() ; // 偷看下一個TOKEN
    if ( mTempToken.mTokenType == IDENT ) { // 如果是IDENT

      if ( mTempToken.mToken == "quit" ) ThrowEND( QUIT ) ;

      // 到此為止 一定是著正常的ID
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個ID
      string idname = mTempToken.mToken ; // ID的名字存起來
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      mTempToken =  mScanner.PeekToken() ; // 偷看下一個TOKEN

      if ( mTempToken.mTokenType == ASSIGN ) { // 判斷是不是':='
        mTempToken = mScanner.GetToken() ; // 拿取偷看的那個(:=)
        mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
        // 接著判斷<ArithExp>
        ArithExp() ; // 呼叫ArithExp()
        mTempToken = mScanner.PeekToken() ; // 偷看下一個token
        if ( mTempToken.mTokenType != SEMI ) { // 如果不是';'
          ThrowERROR2( mTempToken.mToken ) ; // 丟出例外2
        } // if
        // 到此為止下一個一定是';'
        mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
        mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
        return ; // 結束
      } // if()
      else { // 如果ID後面不是':='的情況 再來就要呼叫<IDlessArithExpOrBexp>

        IDlessArithExpOrBexp( idname ) ; // 呼叫IDlessArithExpOrBexp()
        // 若IDlessArithExpOrBexp正確
        mTempToken = mScanner.PeekToken() ; // 偷看下一個token
        if ( mTempToken.mTokenType != SEMI ) { // 如果不是';'
          ThrowERROR2( mTempToken.mToken ) ; // 丟出例外2
        } // if
        // 到此為止，下一個一定是';'
        mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
        mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
        return ; // 結束
      } // else
    } // if()
    else { // 上述兩個情況都不是的話 就執行<NOT_ID_StartArithExpOrBexp>
      NOT_ID_StartArithExpOrBexp() ; // 呼叫NOT_ID_StartArithExpOrBexp()
      mTempToken = mScanner.PeekToken() ; // 偷看下一個token
      if ( mTempToken.mTokenType != SEMI ) { // 如果不是';'
        ThrowERROR2( mTempToken.mToken ) ; // 丟出例外2
      } // if
      // 到此為止下一個一定是';'
      mTempToken = mScanner.GetToken() ; // 拿取偷看的那個
      mPassSyntaxVector.push_back( mTempToken ) ; // 符合文法就放入
      return ; // 結束
    } // else
  } // Command()
  // ------------------------------------------文法分析-------------------------
  // ------------------------------------------計算指令-------------------------
  // 計算是針對mPassSyntaxVector來進行操作
  void CalFactor( float &value ) { // 計算<Factor>
    // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'
    // cout << "In CalFactor()" << endl ; // debug用

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token

    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    // cout << mCalQueue.front().mTokenValue << endl ; // debug用
    mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token

    if ( mTempCalToken.mTokenType == IDENT ) { // 若是是一個IDENT
      if ( InThemDefinedIDVectorOrNot( mTempCalToken.mToken ) != true ) { // 如果沒有被定義
        ThrowERROR3( mTempCalToken.mToken ) ; // 丟出例外3
      } // if()
      else { // 有被被定義過了的
        GetTheIDValue( mTempCalToken.mToken, value ) ; // 取得此ID的值

        bool isfloat = false ;
        GetTheIDNumType( mTempCalToken.mToken, isfloat ) ;
        if ( isfloat == true ) {
          mIsfloat = true ;
        } // if()

        return ; // 結束
      } // else
    } // if()
    else if ( mTempCalToken.mTokenType == POSIVITE ) { // 若是是一個POSITIVE
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(NUM)or(FLNUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        return ; // 結束
      } // if()

      // 正的int數字
      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      return ; // 結束
    } // else if()
    else if ( mTempCalToken.mTokenType == NEGATIVE ) { // 若是是一個NEGATIVE
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(NUM)(FLNUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        value = value * -1 ;
        return ; // 結束
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      value = value * -1 ; // 負的數字
      return ; // 結束
    } // else if()
    else if ( mTempCalToken.mTokenType == NUM || mTempCalToken.mTokenType == FLNUM ) { // 若是一個NUM(FLNUM)
      // 正的數字

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        return ; // 結束
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      return ; // 結束
    } // else if()
    else if ( mTempCalToken.mTokenType == LP ) { // 若是一個 左括號LP
      float arithExpValue ;
      CalArithExp( arithExpValue ) ; // 呼叫CalArithExp()
      value = arithExpValue ;
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(右括號)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(右括號)
      return ;
    } // else if()
  } // CalFactor()

  void CalTerm( float &value ) { // 計算<Term>
    // cout << "In CalTerm()" << endl ; // debug用
    // <Term> ::= <Factor> { '*' <Factor> | '/' <Factor> }
    float cal1Factor1Value  ; // 用來接收第一個Factor所回傳的值
    CalFactor( cal1Factor1Value ) ; // 呼叫CalFactor()

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token

    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token

    // cout << mTempCalToken.mTokenValue << endl ; // debug用
    // 如果是* or /的話
    while ( mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
      if (  mTempCalToken.mTokenType == MULT ) { // 是個乘法
        float cal2FactorValue ; // 用來接收第二個Factor所回傳的值
        CalFactor( cal2FactorValue ) ; // 呼叫CalFactor()
        cal1Factor1Value = cal1Factor1Value * cal2FactorValue ;
      } // if()
      else { // 除法
        float cal2FactorValue ; // 用來接收第2個Factor所回傳的值
        CalFactor( cal2FactorValue ) ; // 呼叫CalFactor()
        if ( cal2FactorValue == 0 ) {
          ThrowERROR() ; // 拋出例外
        } // if()
        else cal1Factor1Value = cal1Factor1Value / cal2FactorValue ; // else
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // 看vctor裡面的第一個Token
    } // while()

    value = cal1Factor1Value ; // 將回傳值設定
    return ;
  } // CalTerm()

  void CalArithExp( float &value ) { // 計算<ArithExp>
    // cout << "In CalArithExp()" << endl ; // debug用
    // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }

    float calTerm1Value ; // 用來接收第一個Term所回傳的值
    CalTerm( calTerm1Value ) ; // 馬上呼叫CalTerm()

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    // 如果是+-的話
    while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
      if ( mTempCalToken.mTokenType == ADD ) { // 加法
        float calTerm2Value ; // 用來接收第2個Term所回傳的值
        CalTerm( calTerm2Value ) ; // 馬上呼叫CalTerm()
        calTerm1Value = calTerm1Value + calTerm2Value ;
      } // if()
      else { // 減法
        float calTerm2Value ; // 用來接收第2個Term所回傳的值
        CalTerm( calTerm2Value ) ; // 馬上呼叫CalTerm()
        calTerm1Value = calTerm1Value - calTerm2Value ;
      } // else

      mTempCalToken = mPassSyntaxVector.front() ; // 拿vctor裡面的第一個Token
    } // while()

    value = calTerm1Value ; // 設定回傳值
    return ;
  } // CalArithExp()

  void CalBooleanOperator( bool &CalCorrect ) { // 計算<BooleanOperator>
    // cout << "In CalBooleanOperator()" << endl ; // debug用

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    if ( mTempCalToken.mTokenType != EQ && mTempCalToken.mTokenType != LG
         && mTempCalToken.mTokenType != G && mTempCalToken.mTokenType != L
         && mTempCalToken.mTokenType != GE && mTempCalToken.mTokenType != LE ) {
      CalCorrect = false  ; // 設定回傳值
      return ;
    } // if()

    CalCorrect = true  ;
    return ;
  } // CalBooleanOperator()

  void CalNOT_ID_StartFactor( float &value ) { // 計算<NOT_ID_StartFactor>
    // cout << "In CalNOT_ID_StartFactor()" << endl ; // debug用
    // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    // cout << mCalQueue.front().mTokenValue << endl ; // debug用
    if ( mTempCalToken.mTokenType == POSIVITE ) { // 若是是一個POSITIVE
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是POSITIVE)
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(是NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是NUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        return ;
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      return ; // 結束
    } // if()
    else if ( mTempCalToken.mTokenType == NEGATIVE ) { // 若是是一個NEGATIVE
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是NEGATIVE)
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(是NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是NUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        value = value * -1 ;
        return ; // 結束
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      value = value * -1 ;
      return ; // 結束
    } // else if()
    else if ( mTempCalToken.mTokenType == NUM || mTempCalToken.mTokenType == FLNUM ) { // 若是一個NUM
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是NUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
        return ; // 結束
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  將 string 轉 float
      return ;
    } // else if()
    else if ( mTempCalToken.mTokenType == LP ) { // 若是一個 左括號LP
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是LP)
      float arithExpValue ;
      CalArithExp( arithExpValue ) ; // 呼叫CalArithExp()
      value = arithExpValue ;
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(右括號)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(是RP)
      return ;
    } // else if
    // 如果什麼都沒進去
  } // CalNOT_ID_StartFactor()

  void CalNOT_ID_StartTerm( float &value ) { // 計算<NOT_ID_StartTerm>
    // cout << "In CalNOT_ID_StartTerm()" << endl ; // debug用
    // <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }

    float calNOT_ID_StartFactorValue ; // 用來接收第一個NOT_ID_StartFactor()所回傳的值
    CalNOT_ID_StartFactor( calNOT_ID_StartFactorValue ) ; // 呼叫NOT_ID_StartFactor()

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    // cout << mTempCalToken.mTokenValue << endl ;
    // 如果是* or /的話
    while ( mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
      if (  mTempCalToken.mTokenType == MULT ) { // 乘法
        float calFactorValue ; // 用來接收CalFactor的值
        CalFactor( calFactorValue ) ; // 呼叫CalFactor
        calNOT_ID_StartFactorValue = calNOT_ID_StartFactorValue * calFactorValue ;
      } // if()
      else { // 除法
        float calFactorValue ; // 用來接收CalFactor的值
        CalFactor( calFactorValue ) ; // 呼叫CalFactor
        if ( calFactorValue == 0 ) {
          // cout << "throw error from CalNOT_ID_StartTerm " << endl ; // debug用
          ThrowERROR() ; // 拋出例外
        } // if()

        calNOT_ID_StartFactorValue = calNOT_ID_StartFactorValue / calFactorValue ;
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    } // while()

    value = calNOT_ID_StartFactorValue ; // 設定回傳值
    return ;
  } // CalNOT_ID_StartTerm()

  void CalNOT_ID_StartArithExp( float &value ) { // 計算<NOT_ID_StartArithExp>
    // cout << "In CalNOT_ID_StartArithExp()" << endl ; // debug用
    // <NOT_ID_StartArithExp>  ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }

    float calNOT_ID_StartTermValue ; // 用來接收第一個CalNOT_ID_StartTerm()所回傳的值
    CalNOT_ID_StartTerm( calNOT_ID_StartTermValue ) ; // 馬上呼叫CalNOT_ID_StartTerm()

    if ( mPassSyntaxVector.empty() ) { // 如果空了就不用繼續
      value = calNOT_ID_StartTermValue ;
      return ;
    }  // if()

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    // 如果是+-
    while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
      if (  mTempCalToken.mTokenType == ADD ) { // 加法
        float calTermValue ; // 用來接收第一個CalTerm()所回傳的值
        CalTerm( calTermValue ) ; // 馬上呼叫CalTerm()
        calNOT_ID_StartTermValue = calNOT_ID_StartTermValue + calTermValue ;
      } // if()
      else { // 減法
        float calTermValue ; // 用來接收第一個CalTerm()所回傳的值
        CalTerm( calTermValue ) ; // 馬上呼叫CalTerm()
        calNOT_ID_StartTermValue = calNOT_ID_StartTermValue - calTermValue ;
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token
    } // while()

    value = calNOT_ID_StartTermValue ; // 設定回傳值
    return ;

  } // CalNOT_ID_StartArithExp()

  void CalNOT_ID_StartArithExpOrBexp( float &value ) { // 計算<NOT_ID_StartArithExpOrBexp>
    // cout << "In CalNOT_ID_StartArithExpOrBexp()" << endl ; // debug用
    // <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]

    float calNOT_ID_StartArithExpValue ; // 用來接收NOT_ID_StartArithExp()的值
    CalNOT_ID_StartArithExp( calNOT_ID_StartArithExpValue ) ; // 呼叫NOT_ID_StartArithExp()
    // 到此為止 我至少有<NOT_ID_StartArithExp>的值

    bool nextIsBool = false ; // 判斷有沒有需要走[ <BooleanOperator> <ArithExp> ]
    CalBooleanOperator( nextIsBool ) ; // 看看下一個是不是BooleanOperator如果是那一定也有ArithExp

    if ( nextIsBool != true ) {
      value  = calNOT_ID_StartArithExpValue ;
      if ( mIsfloat == true ) printf( "> %1.3f\n", value ) ;
      else {
        int answer ;
        answer = value ;
        cout << "> " << answer << endl ;
        // printf( "> %d\n", value ) ;
      } // else
      // 輸出答案為一個值因為[]沒有東西要做(結果)
      return ;
    } // if()
    else { // 是BooleanOperator那一定也有ArithExp
      TokenDetail mTempCalToken ; // 暫存一個要運算的Token
      mTempCalToken = mPassSyntaxVector[0] ; // 拿vctor裡面的第一個Token(布林)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token(布林)
      TOKENTYPE nowBoolType = mTempCalToken.mTokenType ; // 將目前的布林記錄下來

      float calArithExpValue ; // 用來接收CalArithExp()的值
      CalArithExp( calArithExpValue ) ; // 呼叫CalArithExp(

      if ( nowBoolType == EQ ) { // 判斷是否等於要使用TOLERANCE
        if ( calNOT_ID_StartArithExpValue == calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // if()
      else if ( nowBoolType == LG ) { // 也就是問相不相等
        if ( calNOT_ID_StartArithExpValue != calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == G ) { // 要大超過TOLERANCE
        if ( calNOT_ID_StartArithExpValue > calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if()
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == L ) { // 要小超過TOLERANCE
        if ( calNOT_ID_StartArithExpValue < calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if()
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == GE ) { // 是相等或者大於
        if ( calNOT_ID_StartArithExpValue >= calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == LE ) { // 是相等或者小於
        if ( calNOT_ID_StartArithExpValue <= calArithExpValue ) {
          cout << "> true" << endl ; // (結果)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (結果)
          return ;
        } // else
      } // else if()
    } // else()
  } // CalNOT_ID_StartArithExpOrBexp()

  void CalIDlessArithExpOrBexp( string IDname, float &value ) { // 計算<IDlessArithExpOrBexp>
    // cout << "In CalIDlessArithExpOrBexp()" << endl ; // debug用
    // <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term>
    // | '*' <Factor> | '/' <Factor>
    // }
    // [ <BooleanOperator> <ArithExp> ]

    if ( InThemDefinedIDVectorOrNot( IDname ) != true )  { // 先看看從command來的此ID是否已被定義;
      ThrowERROR3( IDname ) ;
    } // if()
    else { // ID有被定義
      float answer ;
      GetTheIDValue( IDname, answer ) ; // 拿取此ID的值
      TokenDetail mTempCalToken ; // 暫存一個要運算的Token

      bool isfloat = false ;
      GetTheIDNumType( IDname, isfloat ) ;
      if ( isfloat == true ) {
        mIsfloat = true ;
      } // if()

      mTempCalToken = mPassSyntaxVector[0] ; // 看Vector裡面的第一個Token
      while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB
              || mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
        if (  mTempCalToken.mTokenType == ADD ) { // 加法
          float calTermValue ; // 用來存放CalTerm()的值
          CalTerm( calTermValue ) ; // 呼叫CalTerm()
          answer = answer + calTermValue ;
        } // if()
        else if ( mTempCalToken.mTokenType == SUB ) { // 減法
          float calTermValue ; // 用來存放CalTerm()的值
          CalTerm( calTermValue ) ; // 呼叫CalTerm()
          answer = answer - calTermValue ;
        } // else if
        else if (  mTempCalToken.mTokenType == MULT ) { // 乘法
          float calFactorValue ; // 用來接收CalFactor的值
          CalFactor( calFactorValue ) ; // 呼叫CalFactor
          answer = answer * calFactorValue ;
        } // else if()
        else { // 除法
          float calFactorValue ; // 用來接收CalFactor的值
          CalFactor( calFactorValue ) ; // 呼叫CalFactor
          if ( calFactorValue == 0 ) {
            ThrowERROR() ;
          } // if

          answer = answer / calFactorValue ;
        } // else

        mTempCalToken = mPassSyntaxVector[0] ; // 看Vector裡面的第一個Token

        // mTempCalToken = mCalQueue.front() ; // 繼續看QUEUE裡面的第一個Token
      } // while()

      value = answer ; // 設定結果值

      bool nextIsBool = false ; // 判斷要不要做後面的[ <BooleanOperator> <ArithExp> ]
      CalBooleanOperator( nextIsBool ) ; // 看看下一個是不是BooleanOperator如果是那一定也有ArithExp

      if ( nextIsBool != true ) { // 若下一個東西不是bool
        if ( mIsfloat == true ) printf( "> %1.3f\n", value ) ;
        else {
          int answer ;
          answer = value ;
          cout << "> " << answer << endl ;
          // printf( "> %d\n", value ) ;
        } // else
        // 輸出答案為一個值因為[]沒有東西要做(結果)
        return ;
      } // if()
      else { // 是BooleanOperator那一定也有ArithExp
        TokenDetail mTempCalToken ; // 暫存一個要運算的Token
        mTempCalToken = mPassSyntaxVector[0] ; // 看QUEUE裡面的第一個Token(布林)
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vctor裡面的第一個Token
        TOKENTYPE nowBoolType = mTempCalToken.mTokenType ; // 將目前的布林記錄下來

        float calArithExpValue ; // 用來存放CalArithExp的值
        CalArithExp( calArithExpValue ) ; // 呼叫CalArithExp()

        if ( nowBoolType == EQ ) { // 判斷是否等於要使用TOLERANCE
          if ( value == calArithExpValue ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // if()
        else if ( nowBoolType == LG ) { // 也就是問相不相等
          if ( value != calArithExpValue ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == G ) { // 若大-小超過TOLERANCE則視為大於
          if ( value > calArithExpValue  ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if()
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == L ) { // 若小和大的差距超過TOLERANCE則視為小於
          if ( value < calArithExpValue ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if()
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == GE ) { // 大於或等於
          if ( value >= calArithExpValue ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == LE ) { // 小於或等於
          if ( value <= calArithExpValue ) {
            cout << "> true" << endl ; // (結果)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (結果)
            return ;
          } // else
        } // else if()
      } // else()
    } // else
  } // CalIDlessArithExpOrBexp()

  void CalCommand( float &value ) { // 計算<CalCommand>呼叫所有function都會主動完成事情
    // cout << "In CalCommand()" << endl ; // debug用
    // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
    // | <NOT_IDStartArithExpOrBexp> ';'
    // | QUIT

    TokenDetail mTempCalToken ; // 暫存一個要運算的Token
    mTempCalToken = mPassSyntaxVector.front() ; // 看vector裡面地一個token
    // 如果是ID
    if ( mTempCalToken.mTokenType == IDENT ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vector裡面地一個token

      IDDetail mTempID ; // 暫存一個ID
      mTempID.mIDName = mTempCalToken.mToken ; // ID名字等於token的值
      mTempID.mIDTokenLine = mTempCalToken.mTokenLine ; // ID的LINE等於token的LINE

      mTempCalToken = mPassSyntaxVector.front() ; // 看vector裡面地一個token
      if ( mTempCalToken.mTokenType == ASSIGN ) { // 如果是:=
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vector裡面地一個token(:=)

        float calArithExpValue ; // 呼叫CalArithExp來取得數值
        CalArithExp( calArithExpValue ) ; // 呼叫CalArithExp來取得數值
        mTempID.mIDisfloat = mIsfloat ;
        // 接著看看有沒有這個ID存在
        if ( InThemDefinedIDVectorOrNot( mTempID.mIDName ) == true ) { // 判斷有沒有此ID
          SetTheIDValue( mTempID.mIDName, calArithExpValue ) ; // 去更新此ID的值
          SetTheIDNumType( mTempID.mIDName, mIsfloat ) ; // 更新此ID的NUMTYPE
          if ( mIsfloat == true ) printf( "> %1.3f\n", calArithExpValue ) ;
          else {
            int answer ;
            answer = calArithExpValue ;
            cout << "> " << answer << endl ;
            // printf( "> %d\n", calArithExpValue ) ;
          } // else
        } // if()
        else { // 如果是一個新的ID此指令為新增一個新的ID
          mTempID.mIDValue = calArithExpValue ; // 設定ID的值
          // cout << mTempID.misfloat << endl ;
          PushInTomDefinedIDVector( mTempID ) ; // 去VECTOR新增定義這個ID
          if ( mIsfloat == true ) printf( "> %1.3f\n", calArithExpValue ) ;
          else {
            int answer ;
            answer = calArithExpValue ;
            cout << "> " << answer << endl ;
            // printf( "> %d\n", calArithExpValue ) ;
          } // else
        } // else

        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vector裡面地一個token(;)
        return ; // 結束
      } // if()
      else { // ID後面不是:=
        // 丟ID名字給<IDlessArithExpOrBexp>
        CalIDlessArithExpOrBexp( mTempID.mIDName, value ) ; // 已在CalIDlessArithExpOrBexp()做完cout
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vector裡面地一個token(;)
        return ;
      } // else
    } // if()
    else { // 呼叫 <NOT_IDStartArithExpOrBexp>
      CalNOT_ID_StartArithExpOrBexp( value ) ; // 輸出已在CalNOT_ID_StartArithExpOrBexp()做完
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // 刪掉vector裡面地一個token(;)
      return ;
    } // else
  } // CalCommand()

  void SyntaxAnalysis() { // 文法分析
    // cout << "into SyntaxAnalysis()" << endl ; // debug用

    do { // 若Token沒了
      try {
        TokenDetail mTempToken ; // 暫存
        Command() ; // 呼叫Command()
        float value ;
        CalCommand( value ) ;
      } // try
      catch ( string ErrorMsg ) { // 所有錯誤!
        cout << "> " << ErrorMsg << endl ;
        mScanner.ClearToEnter() ; // 讀到換行為止，還沒輸入的都要整行捨棄
      } // catch
      catch ( TOKENTYPE type ) { // 中斷指令
        mIsfloat = false ; // 重製
        if ( type == QUIT ) return ;
      } // catch

      mScanner.ClearmPeekToken() ;
      mPassSyntaxVector.clear() ;
      mIsfloat = false ; // 重製

    } while ( true ) ;
  } // SyntaxAnalysis()

}; // class Parser

int main()
{
  cout << "Program starts..." << endl ;
  cin >> uTestNumber ; // 測試數據題目

  Scanner scanner ;
  Parser parser( scanner ) ;
  // parser.Debug2() ;
  parser.SyntaxAnalysis() ;
  // if ( uTestNumber == 2 ) cout << "YA" << endl ;
  // system("pause");
  cout << "> " << "Program exits..." << endl ;
  // return 0 ;
} // main()


