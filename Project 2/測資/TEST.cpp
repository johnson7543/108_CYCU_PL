# include <iostream>
# include <sstream>
# include <stdlib.h>
# include <stdio.h>
# include <vector>
# include <string>
# include <ctype.h>

using namespace std ;
static int uTestNum = 0 ;
int gNewLine = 1 ;

typedef enum 
{
  IDENT,  // Identifier e.g. abc123_
  CONST,  // 35 , 35.67 , .35 , 35. , 'a' , "Hi, there" , true , false , 0023 
  INT, FLOAT, CHAR, BOOL, STRING,  // int float char bool string
  VOID, IF, ELSE, WHILE, DO, RETURN,  // viod if else while do return
  DELIMITER,  // gerneral delimiter(one char)
  GE, LE,          
  EQ, NEQ,                     
  AND, OR, PE, ME, TE, DE, RE, PP, MM, RS, LS,
  DONE
} 
EnumType ;

class TokenInfo {
public:
  string mStr ;
  int mTokenType ;
  int mRow ;
  bool mIsFloat ;
  TokenInfo() {
    mStr = "" ;
    mRow = 0 ;
    mIsFloat = false ;
  }  // TokenInfo()
};

class IdInfo {
public:
  string mName ;
  float mValue ;
  int mType ;
  int mSize ; // char chArray[30] ;
  bool mIsArray ;
  IdInfo() {
    mName = "" ;
    mValue = 0 ;
  } // IdInfo()
};

class FunctionInfo {
public:
  string mFunctionName ;
  vector<TokenInfo>mVecOfFunStatement ;
  FunctionInfo() {
    mFunctionName = "" ;
  } // FunctionInfo()
};

void Done() {
  throw DONE ;
}  // Done()

void Error_1( char unreconized_ch, int row ) {
  int errorRow = row - gNewLine + 1 ;
  gNewLine = row ;
  // if ( errorRow <= 0 )
  // errorRow = 0 ;
  cout << "> Line " << errorRow ;
  string str = " : Unrecognized token with first char : '" ;
  string throw_message = str + unreconized_ch + "'" ;
  // cout << endl << "error1:" << throw_message << endl ;
  throw throw_message ;
}  // Error_1()

void Error_2( string unexpected_token, int row ) { 
  int errorRow = row - gNewLine + 1 ;
  gNewLine = row ;
  cout << "> Line " << errorRow ;
  string str = " : unexpected token : '" ;
  string throw_message = str + unexpected_token + "'" ;
  // cout << endl << "error2:" << throw_message << endl ;
  throw throw_message ;
}  // Error_2()

void Error_3( string undefined_id, int row ) {
  int errorRow = row - gNewLine + 1 ;
  gNewLine = row ;
  cout << "> Line " << errorRow ;
  string str = " : undefined identifier : '" ;
  string throw_message = str + undefined_id + "'" ;
  throw throw_message ;
}  // Error_3()

class Execute {
public:
  string EnumToString( int num ) {
    if ( num == INT )
      return "int";
    if ( num == FLOAT )
      return "float";
    if ( num == CHAR )
      return "char";
    if ( num == BOOL )
      return "bool";
    if ( num == STRING )
      return "string";
    if ( num == VOID )
      return "void";
    return "" ;

  }  // EnumToString()

  void OutPutWhiteSpace( int num ) {
    for ( int i = 1 ; i <= num ; i++ ) {
      cout << " " ;
    }  // for ()
  }  // OutPutWhiteSpace()

  void ListAllVariables( vector<IdInfo>vec ) {
    cout << "> " ;
    for ( int i = 0 ; i < vec.size() ; i++ )
      cout << vec[i].mName << endl ;
    cout << "Statement executed ..." << endl ;

  }  // ListAllVariables()

  void ListVariable( vector<IdInfo>vec, string str ) {
    cout << "> " ;
    for ( int i = 0 ; i < vec.size() ; i++ ) {
      string temp = '"' + vec[i].mName + '"' ;
      if ( temp == str ) {
        if ( vec[i].mIsArray )
          cout << EnumToString( vec[i].mType ) << " " << vec[i].mName << "[ "
               << vec[i].mSize << " ]" << " ;" << endl ;
        else
          cout << EnumToString( vec[i].mType ) << " " << vec[i].mName << " ;" << endl ;
      }  // if ()
    }  // for ()

    cout << "Statement executed ..." << endl ;

  }  // ListVariable()

  void ListAllFunctions( vector<FunctionInfo>vec ) {
    cout << "> " ;
    for ( int i = 5 ; i < vec.size() ; i++ )
      cout << vec[i].mFunctionName << "()" << endl ;
    cout << "Statement executed ..." << endl ;

  }  // ListAllFunctions()

  void ListFunction( vector<FunctionInfo>vec, string str ) {
    int ws = 0 ;
    int ws_brace[10] ;
    int right_index = 0 ;
    int left_index = 0 ;
    cout << "> " ;
    for ( int i = 0 ; i < vec.size() ; i++ ) {
      string temp = '"' + vec[i].mFunctionName + '"' ;
      if ( temp == str ) {
        for ( int j = 0 ; j < vec[i].mVecOfFunStatement.size() ; j++ ) {
          if ( vec[i].mVecOfFunStatement[j].mStr == "{" ) {
            cout << "{" ;
            ws_brace[right_index] = ws ;
            left_index = right_index ;
            right_index++ ;
            ws = ws + 2 ;
            cout << endl ;
            if ( vec[i].mVecOfFunStatement[j+1].mStr != "}" )
              OutPutWhiteSpace( ws ) ;
          }  // if ()
          else if ( vec[i].mVecOfFunStatement[j].mStr == "}" ) {
            ws = ws_brace[left_index] ;
            left_index-- ;  // I know this is not save
            OutPutWhiteSpace( ws ) ;
            cout << "}" << endl ;
            if ( j != vec[i].mVecOfFunStatement.size() - 1 && vec[i].mVecOfFunStatement[j+1].mStr != "}" )
              OutPutWhiteSpace( ws ) ;
          }  // if ()
          else if ( vec[i].mVecOfFunStatement[j].mStr == ";" ) {
            cout << ";" << endl ;
            if ( vec[i].mVecOfFunStatement[j+1].mStr != "}"  )
              OutPutWhiteSpace( ws ) ;
          }  // else if ()
          else {
            cout << vec[i].mVecOfFunStatement[j].mStr ;
            if ( vec[i].mVecOfFunStatement[j].mTokenType == IDENT &&
                 ( vec[i].mVecOfFunStatement[j+1].mStr == "[" ||
                   vec[i].mVecOfFunStatement[j+1].mStr == "(" ||
                   vec[i].mVecOfFunStatement[j+1].mStr == "++" ||
                   vec[i].mVecOfFunStatement[j+1].mStr == "--" ) )
              cout << "" ;
            else if ( vec[i].mVecOfFunStatement[j+1].mStr == "," )
              cout << "" ;
            else
              cout << " " ;
          }  // else         
        }  // for ()
      }  // if ()
    }  // for ()

    cout << "Statement executed ..." << endl ;

  }  // ListFunction()
  
};

class Scanner {
private:
  char mCh ;  // current char
  int mRow ;  // row of the current char
  bool mStringMode ;
  vector<TokenInfo>mVecOfPeek ;  // put the complete token we might need

public:
  Scanner() {
    mCh = '\0' ;
    mRow = 0 ;
    mStringMode = false ;
    mVecOfPeek.clear() ;

  } // Scanner()

  void ClearPeek() {
    mVecOfPeek.clear() ;

  }  // ClearPeek()

  void GetNextChar() {  // use to get the next char and set mCh, mRow
    if ( cin.get( mCh ) ) {
      if ( mCh == '/' && !mStringMode ) {
        if ( cin.peek() == '/' ) {
          SkipUntilEnter() ;
          GetNextChar() ;  // do it again
        }  // if ()
      }  // if ()
      else if ( mCh == '\n' )
        mRow++ ;  // since there is an enter('\n'), going to next row
    }  // if ()
    else
      mCh = EOF ;  // no input exist, set EOF

  }  // GetNextChar()

  void GetNextChar_NonWhiteSpace() {  // get the next char which is not a white space or comment
    GetNextChar() ;
    while ( mCh == ' ' || mCh == '\n' || mCh == '\t' )
      GetNextChar() ;

  }  // GetNextChar_NonWhiteSpace()

  TokenInfo TrulyGetToken() {
    TokenInfo tempToken ;
    GetNextChar_NonWhiteSpace() ;
    if ( mCh == EOF ) {
      Done() ;
    } // if ()
    else if ( isalpha( mCh ) ) {
      tempToken = GetIdent_GrammarWord_ConstantTF() ;
      return tempToken ;
    } // if ()
    else if ( isdigit( mCh ) || ( mCh == '.' ) ) { // number or dot
      tempToken = GetConstant_Number() ;
      if ( tempToken.mStr == "." )
        Error_1( '.', mRow ) ;  // a dot is an unreconized token
      return tempToken ;
    } // else if ()
    else if ( mCh == 39 ) {
      tempToken = GetConstant_Char() ;
      return tempToken ;
    } // else if ()
    else if ( mCh == '"' ) {
      tempToken = GetConstant_String() ;
      return tempToken ;
    } // else if ()
    else if ( Isdelimiter( mCh ) ) {
      tempToken = GetDelimiter() ;
      return tempToken ;
    }  // else if ()
    else
      Error_1( mCh, mRow ) ;
    
    // cout << "char :::::: " << tempToken.mStr << endl ;
    return tempToken ;

  }  // TrulyGetToken()

  TokenInfo GetIdent_GrammarWord_ConstantTF() {
    string token_Str ;
    token_Str = token_Str + mCh ;
    char tempPeek ;
    tempPeek = cin.peek() ;
    while ( isalpha( tempPeek ) || isdigit( tempPeek ) || ( tempPeek == '_' ) ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempPeek = cin.peek() ;
    } // while()

    TokenInfo tempToken ;
    if ( token_Str == "int" )
      tempToken.mTokenType = INT ;
    else if ( token_Str == "float" )
      tempToken.mTokenType = FLOAT ;
    else if ( token_Str == "char" )
      tempToken.mTokenType = CHAR ;
    else if ( token_Str == "bool" )
      tempToken.mTokenType = BOOL ;
    else if ( token_Str == "string" )
      tempToken.mTokenType = STRING ;
    else if ( token_Str == "void" )
      tempToken.mTokenType = VOID ;
    else if ( token_Str == "if" )
      tempToken.mTokenType = IF ;
    else if ( token_Str == "else" )
      tempToken.mTokenType = ELSE ;
    else if ( token_Str == "while" )
      tempToken.mTokenType = WHILE ;
    else if ( token_Str == "do" )
      tempToken.mTokenType = DO ;
    else if ( token_Str == "return" )
      tempToken.mTokenType = RETURN ;
    else if ( token_Str == "true" || token_Str == "false" )
      tempToken.mTokenType = CONST ;
    else
      tempToken.mTokenType = IDENT ;

    tempToken.mStr = token_Str ;
    tempToken.mRow = mRow ;
    return tempToken ;

  }  // GetIdent_GrammarWord_ConstantTF()

  TokenInfo GetConstant_Number() {
    string token_Str ;
    token_Str = token_Str + mCh ;
    char tempPeek ;
    tempPeek = cin.peek() ;

    if ( mCh == '.' ) {  // case .1234
      while ( isdigit( tempPeek ) ) {
        GetNextChar() ;
        token_Str = token_Str + mCh ;
        tempPeek = cin.peek() ;
      } // while ()

      TokenInfo tempToken ;
      tempToken.mTokenType = CONST;
      tempToken.mStr = token_Str ;
      tempToken.mRow = mRow ;
      tempToken.mIsFloat = true ;  // since there is a '.'
      return tempToken ;
    }  // if ()

    else {  // case 1234 or 1.234 or 1.
      while ( isdigit( tempPeek ) ) {
        GetNextChar() ;
        token_Str = token_Str + mCh ;
        tempPeek = cin.peek() ;
      } // while ()

      if ( tempPeek == '.' ) {  // after digit, there might be a '.'
        GetNextChar() ;
        token_Str = token_Str + mCh ;
        tempPeek = cin.peek() ;
        while ( isdigit( tempPeek ) ) {  // keep getting digit to complete whole number
          GetNextChar() ;
          token_Str = token_Str + mCh ;
          tempPeek = cin.peek() ;
        } // while ()

        TokenInfo tempToken ;
        tempToken.mTokenType = CONST ;
        tempToken.mStr = token_Str ;
        tempToken.mRow = mRow ;
        tempToken.mIsFloat = true ;  // since there is a '.'
        return tempToken ;
      }  // if ()
    }  // else ()

    // here is a integer number
    TokenInfo tempToken ;
    tempToken.mTokenType = CONST ;
    tempToken.mStr = token_Str ;
    tempToken.mRow = mRow ;
    return tempToken ;

  }  // GetConstant_Number()

  TokenInfo GetConstant_Char() {
    string token_Str ;
    TokenInfo tempToken ;
    token_Str = token_Str + mCh ;
    char tempPeek ;
    tempPeek = cin.peek() ;
    while ( tempPeek != 39 ) {
      mStringMode = true ;
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempPeek = cin.peek() ;
    } // while ()

    GetNextChar() ;
    token_Str = token_Str + mCh ;
    tempToken.mTokenType = CONST ;
    tempToken.mStr = token_Str ;
    tempToken.mRow = mRow ;
    mStringMode = false ;
    return tempToken ;

  }  // GetConstant_Char()

  TokenInfo GetConstant_String() {
    string token_Str ;
    TokenInfo tempToken ;
    token_Str = token_Str + mCh ;
    char tempPeek ;
    tempPeek = cin.peek() ;
    while ( tempPeek != '"' ) {
      mStringMode = true ;
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempPeek = cin.peek() ;
    } // while ()

    GetNextChar() ;
    token_Str = token_Str + mCh ;
    tempToken.mTokenType = CONST ;
    tempToken.mStr = token_Str ;
    tempToken.mRow = mRow ;
    mStringMode = false ;
    return tempToken ;

  }  // GetConstant_String()

  TokenInfo GetDelimiter() {
    string token_Str ;
    token_Str = token_Str + mCh ;
    char tempPeek ;
    tempPeek = cin.peek() ;
    TokenInfo tempToken ;

    if ( mCh == '>' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = GE ;
      tempToken.mStr = token_Str ;
      tempToken.mRow = mRow ;
    }  // if ()
    else if ( mCh == '<' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = LE ;
      tempToken.mStr = token_Str ;
      tempToken.mRow = mRow ;
    }  // else if ()
    else if ( mCh == '=' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = EQ ;
    }  // else if ()
    else if ( mCh == '!' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = NEQ ;
    }  // else if ()
    else if ( mCh == '&' && tempPeek == '&' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = AND ;
    }  // else if ()
    else if ( mCh == '|' && tempPeek == '|' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = OR ;
    }  // else if ()
    else if ( mCh == '+' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = PE ;
    }  // else if ()
    else if ( mCh == '-' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = ME ;
    }  // else if ()
    else if ( mCh == '*' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = TE ;
    }  // else if ()
    else if ( mCh == '/' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = DE ;
    }  // else if ()
    else if ( mCh == '%' && tempPeek == '=' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = RE ;
    }  // else if ()
    else if ( mCh == '+' && tempPeek == '+' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = PP ;
    }  // else if ()
    else if ( mCh == '-' && tempPeek == '-' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = MM ;
    }  // else if ()
    else if ( mCh == '>' && tempPeek == '>' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = RS ;
    }  // else if ()
    else if ( mCh == '<' && tempPeek == '<' ) {
      GetNextChar() ;
      token_Str = token_Str + mCh ;
      tempToken.mTokenType = LS ;
    }  // else if ()
    else
      tempToken.mTokenType = DELIMITER ;

    tempToken.mStr = token_Str ;
    tempToken.mRow = mRow ;
    return tempToken ;

  }  // GetDelimiter()

  TokenInfo PeekToken() {
    TokenInfo mTempToken ;
    if ( !mVecOfPeek.empty() )
      mTempToken = mVecOfPeek.front() ;
    else { 
      mTempToken = TrulyGetToken() ;
      mVecOfPeek.push_back( mTempToken ) ;
    } // else

    return mTempToken ;

  }  // PeekToken()

  TokenInfo GetToken() {
    TokenInfo tempToken ;
    tempToken = mVecOfPeek.front() ;
    mVecOfPeek.erase( mVecOfPeek.begin() );  // pop front element
    // cout << "get token: " << tempToken.mStr << endl ;
    return tempToken ;

  }  // GetToken()

  void SkipUntilEnter() {
    while ( cin.peek() != '\n' ) {
      cin.get() ;
    }  // while ()
     
  }  // SkipUntilEnter()

  bool Isdelimiter( char ch ) {
    if ( ch == '(' || ch == ')' || ch == '[' || ch == ']' ||
         ch == '{' || ch == '}' || ch == '+' || ch == '-' ||
         ch == '*' || ch == '/' || ch == '%' || ch == '^' ||
         ch == '>' || ch == '<' || ch == '&' || ch == '|' ||
         ch == '=' || ch == '!' || ch == ';' || ch == ',' ||
         ch == '?' || ch == ':' ) return true ;    
    return false ;

  }  // Isdelimiter()

};

class Parser {
private:
  Scanner mScanner ;
  vector<TokenInfo> mVecOfSyntaxResult ;
  vector<IdInfo> mVecOfId ;
  vector<FunctionInfo> mVecOfFunction ;
  vector<IdInfo>mVecOfReDefId ;
  vector<IdInfo>mVecOfTempDefId ;  // trying IdInfo type 
  vector<IdInfo>mVecOfTempLocalDefId ;
  vector<string>mVecOfCinCout ;
  bool mIsCompound ;
  bool mIsFunction ;
  bool mIsReDefFunction ;
  bool mIsError ;
  string mCurrFunName ;
public:
  Parser() {
    mScanner = Scanner() ;
    int mCurrType = 0 ;
    int mCurrSize = 0 ;
    FunctionInfo temp ;
    temp.mFunctionName = "ListAllVariables" ;
    mVecOfFunction.push_back( temp ) ;
    temp.mFunctionName = "ListAllFunctions" ;
    mVecOfFunction.push_back( temp ) ;
    temp.mFunctionName = "ListVariable" ;
    mVecOfFunction.push_back( temp ) ;
    temp.mFunctionName = "ListFunction" ;
    mVecOfFunction.push_back( temp ) ;
    temp.mFunctionName = "Done" ;
    mVecOfFunction.push_back( temp ) ;
    mVecOfCinCout.push_back( "cin" ) ;
    mVecOfCinCout.push_back( "cout" ) ;

  } // Parser()
 
  void Bsort_Var() {
    bool pass = false ;
    IdInfo temp;
    while ( !pass ) {
      pass = true;
      for ( int i = 0 ; i < mVecOfId.size()-1 ; i++ ) {
        if ( mVecOfId[i].mName > mVecOfId[i+1].mName ) {
          pass = false;
          temp = mVecOfId[i];
          mVecOfId[i] = mVecOfId[i+1];
          mVecOfId[i+1] = temp;
        }  // if ()
      }  // for ()
    }  // while ()

  }  // Bsort_Var()

  void Bsort_Function() {
    bool pass = false ;
    FunctionInfo temp;
    while ( !pass ) {
      pass = true;
      for ( int i = 5 ; i < mVecOfFunction.size() - 1 ; i++ ) {
        if ( mVecOfFunction[i].mFunctionName > mVecOfFunction[i+1].mFunctionName ) {
          pass = false;
          temp = mVecOfFunction[i];
          mVecOfFunction[i] = mVecOfFunction[i+1];
          mVecOfFunction[i+1] = temp;
        }  // if ()
      }  // for ()
    }  // while ()

  }  // Bsort_Function()

  void Execute_FuncitonDef() {
    FunctionInfo function ;
    function.mFunctionName = mCurrFunName ;
    function.mVecOfFunStatement = mVecOfSyntaxResult ;
    if ( mIsReDefFunction ) {
      for ( int i = 0 ; i < mVecOfFunction.size() ; i++ ) {
        if ( mVecOfFunction[i].mFunctionName == mCurrFunName )
          mVecOfFunction[i].mVecOfFunStatement = mVecOfSyntaxResult ;
      }  // for ()
    }  // if ()
    else
      mVecOfFunction.push_back( function ) ;
    if ( mVecOfFunction.size() > 1 )
      Bsort_Function() ;

  }  // Execute_FuncitonDef()

  void Execute_definition() {
    IdInfo id ;
    for ( int i = 0 ; i < mVecOfTempDefId.size() ; i++ ) {
      id.mName = mVecOfTempDefId[i].mName ;
      id.mType = mVecOfTempDefId[i].mType ;
      id.mIsArray = mVecOfTempDefId[i].mIsArray ;
      id.mSize = mVecOfTempDefId[i].mSize ;
      if ( !IsDefined( id.mName ) )
        mVecOfId.push_back( id ) ;
    }  // for ()

    if ( mVecOfId.size() > 1 )
      Bsort_Var() ;
    ReDefinedId() ;

  }  // Execute_definition()

  void ReDefinedId() {
    IdInfo id ;
    FunctionInfo funciton ;
    for ( int i = 0 ; i < mVecOfReDefId.size() ; i++ )
      for ( int j = 0 ; j < mVecOfId.size() ; j++ )
        if ( mVecOfId[j].mName == mVecOfReDefId[i].mName ) {
          mVecOfId[j].mType = mVecOfReDefId[i].mType ;
          mVecOfId[j].mIsArray = mVecOfReDefId[i].mIsArray ;
          mVecOfId[j].mSize = mVecOfReDefId[i].mSize ;
        }  // if ()

  }  // ReDefinedId()

  bool IsDefinedStatement() {
    for ( int i = 0 ; i < mVecOfSyntaxResult.size() ; i++ ) {
      if ( mVecOfSyntaxResult[i].mTokenType == INT || mVecOfSyntaxResult[i].mTokenType == FLOAT ||
           mVecOfSyntaxResult[i].mTokenType == CHAR || mVecOfSyntaxResult[i].mTokenType == BOOL ||
           mVecOfSyntaxResult[i].mTokenType == STRING )
        return true ;
    } // for ()

    return false ;
  }  // IsDefinedStatement()

  bool IsDefinedFunction() {
    for ( int i = 0 ; i < mVecOfSyntaxResult.size() ; i++ ) {
      if ( mVecOfSyntaxResult[i].mTokenType == INT || mVecOfSyntaxResult[i].mTokenType == FLOAT ||
           mVecOfSyntaxResult[i].mTokenType == CHAR || mVecOfSyntaxResult[i].mTokenType == BOOL ||
           mVecOfSyntaxResult[i].mTokenType == STRING || mVecOfSyntaxResult[i].mTokenType == VOID )
        if ( mVecOfSyntaxResult[i+2].mStr == "(" )
          return true ;
    } // for ()

    return false ;
  }  // IsDefinedFunction()

  void Execute_statement() {
    Execute execute ;
    IdInfo id ;
    for ( int i = 0 ; i < mVecOfSyntaxResult.size() ; i++ ) {
      if ( mVecOfSyntaxResult[i].mStr == "Done" && mVecOfSyntaxResult[i+1].mStr == "(" && 
           mVecOfSyntaxResult.size() == 4 )
        Done() ;
      else if ( mVecOfSyntaxResult[i].mStr == "ListAllVariables" ) {
        execute.ListAllVariables( mVecOfId ) ;
        return ;
      }  // else if ()
      else if ( mVecOfSyntaxResult[i].mStr == "ListVariable" ) {
        execute.ListVariable( mVecOfId, mVecOfSyntaxResult[i+2].mStr ) ;
        return ;
      }  // else if ()
      else if ( mVecOfSyntaxResult[i].mStr == "ListAllFunctions" ) {
        execute.ListAllFunctions( mVecOfFunction ) ;
        return ;
      }  // else if ()
      else if ( mVecOfSyntaxResult[i].mStr == "ListFunction" ) {
        execute.ListFunction( mVecOfFunction, mVecOfSyntaxResult[i+2].mStr ) ;
        return ;
      }  // else if ()
      else if ( IsDefinedStatement() && !mIsCompound ) {  // int i = 10 ;
        cout << "> " ;
        for ( int i = 1 ; i < mVecOfSyntaxResult.size() ; i++ ) {
          if ( mVecOfSyntaxResult[i].mTokenType == IDENT ) {
            id.mName = mVecOfSyntaxResult[i].mStr ;
            if ( !IsReDefined( id.mName ) )
              cout << "Definition of " << id.mName << " entered ..." << endl ;
            else
              cout << "New definition of " << id.mName << " entered ..." << endl ;
          }  // if ()
        }  // for ()

        return ;
      }  // else if ()
      else if ( IsDefinedFunction() ) {  // int i = 10 ;
        Execute_FuncitonDef() ;
        if ( mIsReDefFunction )
          cout << "> New definition of " << mCurrFunName << "() entered ..." << endl ;
        else
          cout << "> Definition of " << mCurrFunName << "() entered ..." << endl ;
        return ;
      }  // else if ()
    }  // for ()

    cout << "> Statement executed ..." << endl ;

  }  // Execute_statement()

  bool IsReDefined( string str ) {
    for ( int i = 0 ; i < mVecOfReDefId.size() ; i++ )
      if ( str == mVecOfReDefId[i].mName )
        return true ;
    return false ;

  }  // IsReDefined()

  bool IsLocalDefined( string str ) {
    for ( int i = 0 ; i < mVecOfTempLocalDefId.size() ; i++ )
      if ( mVecOfTempLocalDefId[i].mName == str )
        return true;

    return false ;

  }  // IsLocalDefined()

  bool IsDefinedFunction( string str ) {
    for ( int j = 0 ; j < mVecOfFunction.size() ; j++ )
      if ( mVecOfFunction[j].mFunctionName == str )
        return true;
    return false ;
  }  // IsDefinedFunction()

  bool IsDefined( string str ) {
    for ( int i = 0 ; i < mVecOfId.size() ; i++ )
      if ( mVecOfId[i].mName == str )
        return true;
    for ( int k = 0 ; k < mVecOfCinCout.size() ; k++ )
      if ( mVecOfCinCout[k] == str )
        return true;
    return false ;

  }  // IsDefined()

  // above is for definition
  void Initial() {
    TokenInfo tempToken ;
    if ( !mIsError ) {
      gNewLine = mVecOfSyntaxResult.back().mRow ;  
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mRow != gNewLine )
        gNewLine++ ;
    }  // if

    mVecOfSyntaxResult.clear() ;
    mVecOfReDefId.clear() ;
    mVecOfTempLocalDefId.clear() ;
    mVecOfTempDefId.clear() ;
    mIsCompound = false ;
    mIsReDefFunction = false ;
    mIsFunction = false ;
    string mCurrFunName = "" ;
    mIsError = false ;

  }  // Initial()

  void SyntaxAnalysis() {
    do {
      try {
        User_input() ;
      } // try
      catch ( string ErrorMsg ) {
        cout << ErrorMsg << endl ;
        mIsError = true ;
        
        while ( cin.peek() != '\n' )
          cin.get() ;
        if ( cin.peek() == '\n' )
          cin.get() ;

        mScanner.ClearPeek() ;
        Initial() ;

      } // catch
      catch ( EnumType done ) {
        if ( done == DONE )
          return ;
      } // catch
    } while ( true ) ;

  } // SyntaxAnalysis()

  void User_input() {
    // cout << "<user_input>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( Definition() || Statement() ) { 
      Execute_statement() ;
      Initial() ;
    }  // if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above
    while ( Definition() || Statement() ) { 
      Execute_statement() ;
      Initial() ;
    }  // while ()

    // tempToken = mScanner.PeekToken() ;
    // Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above

  }  // User_input()

  bool Definition() {
    // cout << "<definition>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mTokenType == VOID ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == IDENT ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        TokenInfo temptempToken ;
        temptempToken = mScanner.PeekToken() ;
        mIsFunction = true ;  // this id is a funciton id    no used now
        mCurrFunName = tempToken.mStr ;
        if ( IsDefinedFunction( tempToken.mStr ) )
          mIsReDefFunction = true ;
        Function_definition_without_ID() ;
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT
    }  // if ()
    else if ( Type_specifier() ) {   
      tempToken = mScanner.GetToken() ;  // get the type e.g: int, char, string
      mVecOfSyntaxResult.push_back( tempToken ) ;
      IdInfo id ;
      id.mType = tempToken.mTokenType ;  // save the def id type
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == IDENT ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        id.mName = tempToken.mStr ;
        TokenInfo temptempToken ;  // save the def id name
        temptempToken = mScanner.PeekToken() ;
        if ( temptempToken.mStr == "[" )
          id.mIsArray = true ;  // set if the id is an array
        if ( temptempToken.mStr == "(" ) {
          mIsFunction = true ;  // this id is a funciton id
          mCurrFunName = tempToken.mStr ;
          if ( IsDefinedFunction( tempToken.mStr ) )
            mIsReDefFunction = true ;
        }  // if ()

        Function_definition_or_declarators( id ) ;
        if ( !mIsCompound )
          Execute_definition() ;  // after all of the declaration in the line, define those shit
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT
    }  // else if ()
    else
      return false ;

    return true ;

  }  // Definition()

  bool Type_specifier() {
    // cout << "<type_specifier>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mTokenType == INT || tempToken.mTokenType == CHAR || tempToken.mTokenType == FLOAT ||
         tempToken.mTokenType == STRING || tempToken.mTokenType == BOOL )
      return true ;
    else
      return false ;

  }  // Type_specifier()

  void Function_definition_or_declarators( IdInfo id ) {
    // cout << "<function_definition_or_declarators>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "(" )
      Function_definition_without_ID() ;
    else
      Rest_of_declarators( id ) ;

  }  // Function_definition_or_declarators()

  void Rest_of_declarators( IdInfo pre_id ) {
    // cout << "<rest_of_declarators>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "[" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == CONST ) {
        tempToken = mScanner.GetToken() ;
        pre_id.mSize = atoi( tempToken.mStr.c_str() ) ;  // [30]
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "]" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no CONST     
    }  // if ()

    if ( !IsLocalDefined( pre_id.mName ) && mIsCompound )
      mVecOfTempLocalDefId.push_back( pre_id ) ;  // local 
    else if ( !IsDefined( pre_id.mName ) )
      mVecOfTempDefId.push_back( pre_id ) ;
    else
      mVecOfReDefId.push_back( pre_id ) ;
    while ( tempToken.mStr == "," ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      IdInfo id ;
      if ( tempToken.mTokenType == IDENT ) {
        tempToken = mScanner.GetToken() ;
        id.mType = pre_id.mType ;
        id.mName = tempToken.mStr ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "[" ) {
          tempToken = mScanner.GetToken() ;
          id.mIsArray = true ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mTokenType == CONST ) {
            tempToken = mScanner.GetToken() ;
            id.mSize = atoi( tempToken.mStr.c_str() ) ;  // [30]
            mVecOfSyntaxResult.push_back( tempToken ) ;
            tempToken = mScanner.PeekToken() ;
            if ( tempToken.mStr == "]" ) {
              tempToken = mScanner.GetToken() ;
              mVecOfSyntaxResult.push_back( tempToken ) ;
              tempToken = mScanner.PeekToken() ;
            }  // if ()
            else
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no CONST 
        }  // if ()
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT  
      if ( !IsLocalDefined( id.mName ) && mIsCompound )
        mVecOfTempLocalDefId.push_back( id ) ;  // local
      else if ( !IsDefined( id.mName ) )
        mVecOfTempDefId.push_back( id ) ;
      else
        mVecOfReDefId.push_back( id ) ;
    }  // while ()

    if ( tempToken.mStr == ";" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
    }  // if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ';'  

  }  // Rest_of_declarators()

  // functon def
  void Function_definition_without_ID() {
    // cout << "<function_definition_without_ID>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "(" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == VOID ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
      }  // if ()
      else 
        Formal_parameter_list() ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == ")" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( !Compound_statement() )
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '{'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
    }  // if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '('

  }  // Function_definition_without_ID()

  // int abc ( int i )
  bool Formal_parameter_list() {
    // cout << "<formal_parameter_list>" << endl ;
    TokenInfo tempToken ;
    IdInfo id ;
    if ( Type_specifier() ) {
      tempToken = mScanner.GetToken() ;
      id.mType = tempToken.mTokenType ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "&" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
      }  // if ()

      if ( tempToken.mTokenType == IDENT ) {
        tempToken = mScanner.GetToken() ;
        id.mName = tempToken.mStr ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "[" ) {
          tempToken = mScanner.GetToken() ;
          id.mIsArray = true ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mTokenType == CONST ) {
            tempToken = mScanner.GetToken() ;
            id.mSize = atoi( tempToken.mStr.c_str() ) ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
            tempToken = mScanner.PeekToken() ;
            if ( tempToken.mStr == "]" ) {
              tempToken = mScanner.GetToken() ;
              mVecOfSyntaxResult.push_back( tempToken ) ;
              tempToken = mScanner.PeekToken() ;
            }  // if ()
            else
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no CONST  
        }  // if ()

        mVecOfTempLocalDefId.push_back( id ) ;  // int abc( int i )
        while ( tempToken.mStr == "," ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( !Formal_parameter_list() )
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no parameter  after ','
          tempToken = mScanner.PeekToken() ;
        }  // while ()
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT 
    }  // if ()
    else
      return false ;
    return true ;
   
  }  // Formal_parameter_list()

  bool Compound_statement() {
    // cout << "<compound_statement>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "{" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      mIsCompound = true ;
      while ( Declaration() || Statement() ) { }
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "}" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '}'
    }  // if ()
    else 
      return false ;

    return true ;

  }  // Compound_statement()

  bool Declaration() {
    // cout << "<declaration>" << endl ;
    TokenInfo tempToken ;
    IdInfo id ;
    if ( Type_specifier() ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      id.mType = tempToken.mTokenType ;  // save the def id type
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == IDENT ) {
        tempToken = mScanner.GetToken() ;
        id.mName = tempToken.mStr ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Rest_of_declarators( id ) ;     
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT
    }  // if ()   
    else
      return false ;
    return true ;

  }  // Declaration()

  bool Statement() {
    // cout << "<statement>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == ";" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
    }  // if ()
    else if ( Expression() ) {
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == ";" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;       
      }  // if () 
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ';'
    }  // else if ()
    else if ( tempToken.mTokenType == RETURN ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Expression() ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == ";" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ';'
    }  // else if ()
    else if ( tempToken.mTokenType == IF ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "(" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( Expression() ) {
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == ")" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
            tempToken = mScanner.PeekToken() ;
            if ( Statement() ) {
              tempToken = mScanner.PeekToken() ;
              if ( tempToken.mTokenType == ELSE ) {
                tempToken = mScanner.GetToken() ;
                mVecOfSyntaxResult.push_back( tempToken ) ;
                Statement() ;
              }  // if ()
            }  // if ()
            else
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no statement
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expresison in ()
      }  // if ()      
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '('
    }  // else if ()
    else if ( tempToken.mTokenType == WHILE ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "(" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( Expression() ) {
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == ")" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
            tempToken = mScanner.PeekToken() ;
            if ( !Statement() )
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no statement
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expresison in ()
      }  // if ()      
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '('
    }  // else if ()
    else if ( tempToken.mTokenType == DO ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Statement() ) {
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mTokenType == WHILE ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == "(" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
            tempToken = mScanner.PeekToken() ;
            if ( Expression() ) {
              tempToken = mScanner.PeekToken() ;
              if ( tempToken.mStr == ")" ) {
                tempToken = mScanner.GetToken() ;
                mVecOfSyntaxResult.push_back( tempToken ) ;
                tempToken = mScanner.PeekToken() ;
                if ( tempToken.mStr == ";" ) {
                  tempToken = mScanner.GetToken() ;
                  mVecOfSyntaxResult.push_back( tempToken ) ;
                }  // if ()
                else
                  Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ';'
              }  // if ()
              else
                Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
            }  // if ()
            else
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression in ()
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no '('
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no while after do-statement
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no statement in do-while
    }  // else if ()
    else if ( !Compound_statement() )
      return false ;

    return true ;

  }  // Statement()

  bool Expression() {
    // cout << "<expression>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( Basic_expression() ) {
      tempToken = mScanner.PeekToken() ;
      while ( tempToken.mStr == "," ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( !Basic_expression() )
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression after ','
        tempToken = mScanner.PeekToken() ;
      }  // while ()

      return true ;
    }  // if ()

    return false ;

  }  // Expression()

  bool Basic_expression() {
    // cout << "<basic_expression>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    // tempToken.mStr != mCurrFunName  
    if ( tempToken.mTokenType == IDENT ) {
      if ( !IsDefined( tempToken.mStr ) && !IsLocalDefined( tempToken.mStr ) &&
           !IsDefinedFunction( tempToken.mStr ) )
        Error_3( tempToken.mStr, tempToken.mRow ) ;
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Rest_of_Identifier_started_basic_exp() ;
    }  // if ()
    else if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == IDENT ) {
        if ( !IsDefined( tempToken.mStr ) && !IsLocalDefined( tempToken.mStr ) )
          Error_3( tempToken.mStr, tempToken.mRow ) ;
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Rest_of_PPMM_Identifier_started_basic_exp() ;
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT
    }  // else if ()
    else if ( Sign() ) {
      while ( Sign() ) { }
      Signed_unary_exp() ;
      Romce_and_romloe() ;
    }  // else if ()
    else if ( tempToken.mTokenType == CONST || tempToken.mStr == "(" ) {
      if ( tempToken.mTokenType == CONST ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
      }  // if ()
      else if ( tempToken.mStr == "(" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( Expression() ) {
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == ")" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression
      }  // if ()

      Romce_and_romloe() ;
    }  // else if ()
    else 
      return false ;

    return true ;

  }  // Basic_expression()

  void Rest_of_Identifier_started_basic_exp() {
    // cout << "<rest_of_Identifier_started_basic_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "[" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Expression() ) {  //  [ '[' expression ']' ] 
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "]" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;   // no Expression in []
      tempToken = mScanner.PeekToken() ;
      if ( Assignment_operator() ) {
        if ( !Basic_expression() ) {
          tempToken = mScanner.PeekToken() ;
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression after Assignment op
        }  // if ()
      }  // if ()
      else if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Romce_and_romloe() ;
      }  // else if ()
      else
        Romce_and_romloe() ;
    }  // if ()
    else if ( tempToken.mStr != "[" && tempToken.mStr != "(" ) {
      if ( Assignment_operator() ) {
        if ( !Basic_expression() ) {
          tempToken = mScanner.PeekToken() ;
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression after Assignment op
        }  // if ()
      }  // if ()
      else if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Romce_and_romloe() ;
      }  // else if ()
      else
        Romce_and_romloe() ;
    }  // else if ()
    else if ( tempToken.mStr == "(" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Actual_parameter_list() ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == ")" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Romce_and_romloe() ;
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
    }  // else if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above
  
  }  // Rest_of_Identifier_started_basic_exp()

  void Rest_of_PPMM_Identifier_started_basic_exp() {
    // cout << "<rest_of_PPMM_Identifier_started_basic_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "[" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Expression() ) {
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "]" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression in []
    }  // if ()

    Romce_and_romloe() ;

  }  // Rest_of_PPMM_Identifier_started_basic_exp()

  bool Sign() {
    // cout << "<sign>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "+" || tempToken.mStr == "-" || tempToken.mStr == "!" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      return true ;
    }  // if ()

    return false ;

  }  // Sign()

  // Error?
  void Actual_parameter_list() {
    // cout << "<actual_parameter_list>" << endl ;
    TokenInfo tempToken ;
    Basic_expression() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "," ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( !Basic_expression() ) 
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no expression after ','
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Actual_parameter_list()

  bool Assignment_operator() {
    // cout << "<assignment_operator>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "=" || tempToken.mTokenType == TE || tempToken.mTokenType == DE ||
         tempToken.mTokenType == RE || tempToken.mTokenType == PE || tempToken.mTokenType == ME ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      return true ;
    }  // if ()

    return false ;
  }  // Assignment_operator()

  // Error?
  void Romce_and_romloe() {
    // cout << "<romce_and_romloe>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_logical_OR_exp();
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mStr == "?" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Basic_expression() ) {
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == ":" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( !Basic_expression() )
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no expression after ':'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ':'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no expression after '?'
    }  // if ()

  }  // Romce_and_romloe()

  void Rest_of_maybe_logical_OR_exp() {
    // cout << "<rest_of_maybe_logical_OR_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_logical_AND_exp();
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == OR ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_logical_AND_exp();
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_logical_OR_exp()

  void Maybe_logical_AND_exp() {
    // cout << "<maybe_logical_AND_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_bit_OR_exp();
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == AND ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_OR_exp();
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_logical_AND_exp()

  void Rest_of_maybe_logical_AND_exp() {
    // cout << "<rest_of_maybe_logical_AND_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_bit_OR_exp();
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == AND ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_OR_exp();
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_logical_AND_exp()

  void Maybe_bit_OR_exp() {
    // cout << "<maybe_bit_OR_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_bit_ex_OR_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "|" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_AND_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_bit_OR_exp()

  void Rest_of_maybe_bit_OR_exp() {
    // cout << "<rest_of_maybe_bit_OR_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_bit_ex_OR_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "|" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_AND_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_bit_OR_exp()

  void Maybe_bit_ex_OR_exp() {
    // cout << "<maybe_bit_ex_OR_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_bit_AND_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "^" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_AND_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_bit_ex_OR_exp()

  void Rest_of_maybe_bit_ex_OR_exp() {
    // cout << "<rest_of_maybe_bit_ex_OR_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_bit_AND_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "^" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_bit_AND_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_bit_ex_OR_exp()

  void Maybe_bit_AND_exp() {
    // cout << "<maybe_bit_AND_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_equality_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "&" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_equality_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_bit_AND_exp()

  void Rest_of_maybe_bit_AND_exp() {
    // cout << "<rest_of_maybe_bit_AND_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_equality_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "&" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_equality_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_bit_AND_exp()

  void Maybe_equality_exp() {
    // cout << "<maybe_equality_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_relational_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == EQ || tempToken.mTokenType == NEQ ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_relational_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_equality_exp()

  void Rest_of_maybe_equality_exp() {
    // cout << "<rest_of_maybe_equality_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_relational_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == EQ || tempToken.mTokenType == NEQ ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_relational_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_equality_exp()

  void Maybe_relational_exp() {
    // cout << "<maybe_relational_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_shift_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "<" || tempToken.mStr == ">" ||
            tempToken.mTokenType == LE || tempToken.mTokenType == GE ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_shift_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_relational_exp()

  void Rest_of_maybe_relational_exp() {
    // cout << "<rest_of_maybe_relational_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_shift_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "<" || tempToken.mStr == ">" ||
            tempToken.mTokenType == LE || tempToken.mTokenType == GE ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_shift_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_relational_exp()

  void Maybe_shift_exp() {
    // cout << "<maybe_shift_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_additive_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == LS || tempToken.mTokenType == RS ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_additive_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_shift_exp()

  void Rest_of_maybe_shift_exp() {
    // cout << "<rest_of_maybe_shift_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_additive_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mTokenType == LS || tempToken.mTokenType == RS ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_additive_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_shift_exp()  

  void Maybe_additive_exp() {
    // cout << "<maybe_additive_exp>" << endl ;
    TokenInfo tempToken ;
    Maybe_mult_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "+" || tempToken.mStr == "-" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_mult_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Maybe_additive_exp()

  void Rest_of_maybe_additive_exp() {
    // cout << "<rest_of_maybe_additive_exp>" << endl ;
    TokenInfo tempToken ;
    Rest_of_maybe_mult_exp() ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "+" || tempToken.mStr == "-" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Maybe_mult_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_additive_exp()

  void Maybe_mult_exp() {
    // cout << "<maybe_mult_exp>" << endl ;
    Unary_exp() ;
    Rest_of_maybe_mult_exp() ;

  }  // Maybe_mult_exp()

  void Rest_of_maybe_mult_exp() {
    // cout << "<rest_of_maybe_mult_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    while ( tempToken.mStr == "*" || tempToken.mStr == "/" || tempToken.mStr == "%" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      Unary_exp() ;
      tempToken = mScanner.PeekToken() ;
    }  // while ()

  }  // Rest_of_maybe_mult_exp()

  void Unary_exp() {
    // cout << "<unary_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( Sign() ) {
      while ( Sign() ) { }
      Signed_unary_exp() ;
    }  // if ()
    else if ( tempToken.mTokenType != PP && tempToken.mTokenType != MM )
      Unsigned_unary_exp() ;
    else if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mTokenType == IDENT ) {
        if ( !IsDefined( tempToken.mStr ) && !IsLocalDefined( tempToken.mStr ) &&
             !IsDefinedFunction( tempToken.mStr ) )
          Error_3( tempToken.mStr, tempToken.mRow ) ;
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == "[" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
          tempToken = mScanner.PeekToken() ;
          if ( Expression() ) {  //  [ '[' expression ']' ] 
            tempToken = mScanner.PeekToken() ;
            if ( tempToken.mStr == "]" ) {
              tempToken = mScanner.GetToken() ;
              mVecOfSyntaxResult.push_back( tempToken ) ;
            }  // if ()
            else
              Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression in []
        }  // if ()
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no IDENT
    }  // else if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above

  }  // Unary_exp()

  void Signed_unary_exp() {
    // cout << "<signed_unary_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    if ( tempToken.mTokenType == IDENT ) {
      // tempToken.mStr != mCurrFunName 
      if ( !IsDefined( tempToken.mStr ) && !IsLocalDefined( tempToken.mStr ) &&
           !IsDefinedFunction( tempToken.mStr ) )
        Error_3( tempToken.mStr, tempToken.mRow ) ;
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "(" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Actual_parameter_list() ;  //  [ actual_parameter_list ] 
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == ")" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
      }  // if ()
      else if ( tempToken.mStr == "[" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( Expression() ) {  //  [ '[' expression ']' ] 
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == "]" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;   // no Expression in []
      }  // else if ()
    } // if ()
    else if ( tempToken.mTokenType == CONST ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
    }  // else if ()
    else if ( tempToken.mStr == "(" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Expression() ) {
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == ")" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression in ()
    }  // else if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above

  }  // Signed_unary_exp()

  void Unsigned_unary_exp() {
    // cout << "<unsigned_unary_exp>" << endl ;
    TokenInfo tempToken ;
    tempToken = mScanner.PeekToken() ;
    // tempToken.mStr != mCurrFunName 
    if ( tempToken.mTokenType == IDENT ) {
      if ( !IsDefined( tempToken.mStr ) && !IsLocalDefined( tempToken.mStr ) &&
           !IsDefinedFunction( tempToken.mStr ) )
        Error_3( tempToken.mStr, tempToken.mRow ) ;
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( tempToken.mStr == "(" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        Actual_parameter_list() ;  //  [ actual_parameter_list ] 
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == ")" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
      }  // if ()
      else if ( tempToken.mStr == "[" ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
        tempToken = mScanner.PeekToken() ;
        if ( Expression() ) {  //  [ '[' expression ']' ] 
          tempToken = mScanner.PeekToken() ;
          if ( tempToken.mStr == "]" ) {
            tempToken = mScanner.GetToken() ;
            mVecOfSyntaxResult.push_back( tempToken ) ;
          }  // if ()
          else
            Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ']'
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;   // no Expression in []
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
      }  // else if ()
      else if ( tempToken.mTokenType == PP || tempToken.mTokenType == MM ) {
        tempToken = mScanner.GetToken() ;
        mVecOfSyntaxResult.push_back( tempToken ) ;
      }  // else if ()
    } // if ()
    else if ( tempToken.mTokenType == CONST ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
    }  // else if ()
    else if ( tempToken.mStr == "(" ) {
      tempToken = mScanner.GetToken() ;
      mVecOfSyntaxResult.push_back( tempToken ) ;
      tempToken = mScanner.PeekToken() ;
      if ( Expression() ) {
        tempToken = mScanner.PeekToken() ;
        if ( tempToken.mStr == ")" ) {
          tempToken = mScanner.GetToken() ;
          mVecOfSyntaxResult.push_back( tempToken ) ;
        }  // if ()
        else
          Error_2( tempToken.mStr, tempToken.mRow ) ;  // no ')'
      }  // if ()
      else
        Error_2( tempToken.mStr, tempToken.mRow ) ;  // no Expression in ()
    }  // else if ()
    else
      Error_2( tempToken.mStr, tempToken.mRow ) ;  // not all of above

  }  // Unsigned_unary_exp()

};

int main() {
  cout << "Our-C running ..." << endl ;
  cin >> uTestNum ;
  Parser parser ;
  parser.SyntaxAnalysis() ;
  cout << "> Our-C exited ..." ;
}  // main()
