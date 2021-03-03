# include <iostream>
# include <sstream>
# include <stdlib.h>
# include <stdio.h>
# include <vector>
# include <string>
# include <ctype.h>
// Copyright © 2020 Yu-Kai,Wang All rights reserved
// PAL Project 1
using namespace std ;
static int uTestNum = 0 ;
bool gIsQuit = false ;     // if the first IDENT token is "quit" 
bool gIsComment = false ;  // if dectect "//" 
bool gDotFound = false ;
bool gIsCmdBegin = true ;
bool gIsUnRecon = false ;
bool gIsDIV = false ;
bool gIsIDENT2First = true ;
string gCurTokenName = "" ;

struct IdTable {
  string idName ;
  float idValue ;
  bool isFloat ;
};

enum BooleanOperator {
  BOOLOP = 1010,
  EQU = 11111,    // =
  NEQ = 22222,    // <>
  GTR = 33333,    // >
  LSS = 44444,    // <
  GEQ = 55555,    // >=
  LEQ = 66666     // <=
};
enum ExpOprator {
  ARITHEXP = 1234, // ALL
  ADD = 1122,      // +
  SUB = 2233,      // -
  ADDORSUB = 1133, // + or -
  MUL = 3344,      // *
  DIV = 4455,      // /
  MULORDIV = 3355, // * or /
  ASSIGN = 5566    // :=
};
enum Type {
  // char
  DIGIT = 10000,         // digit
  ALPH = 20000,          // letter
  UNDERLINE = 30000,     // _
  DOT = 40000,           // .
  COLON = 50000,         // :
  SIGN = 60000,          // - +
  LEFT_PAREN = 70000,    // (
  RIGHT_PAREN = 80000,   // )
  SEMICOLON = 90000,     // ;
  WSPACE = 100000,       // space, tab, enter
  UNRECOGNIZED = 99999,  // non of above

  // string
  NUM = 110000,    // 870921
  FLOAT = 120000,  // 87.0921
  IDENT = 130000,  // johnson_7543
  IDENT2 = 133333, 
  CMNT = 140000,   // //
  QUIT = 88888     // "quit"
};
enum ErrorNumber {
  ERROR_ONE = 1,
  SP_ERROR_ONE = 11,
  SSP_ERROR_ONE = 111,
  ERROR_TWO = 2,
  SP_ERROR_TWO = 22,
  SSP_ERROR_TWO = 222,
  SSSP_ERROR_TWO = 2222,
  ERROR_THREE = 3,
  ERROR_FOUR = 4      // division by zero
};

vector<IdTable>gVecOfTable ;  // where the ID store

bool G_isUnderLine( char ch ) {  // if the input char is '_'
  if ( ( int ) ch == 95 )
    return true;

  else
    return false;

}  // G_isUnderLine()

class Token {

public:
  bool IsReconized( char ch ) ;
  bool GetToken( int tokenType, string &tokenValue ) ;
  void PeekNextChar( int &peekType, int tokenType ) ;
  void GetNextChar( char &curChar ) ;
  void SetCharType( int &charType, char charValue, int tokenType ) ;
  void SetTokenType( int &tokenType, string &tokenValue ) ;
  void SkipUntilEnter() ;
};

bool Token::GetToken( int tokenType, string &tokenValue ) {
  bool completeToken = false ;  // if get the token completely
  bool isFirstChar = true ;
  bool isOnlyOneDot = true ;
  bool isSign = false ;
  bool isBool = false ;
  bool isLss = false ;
  bool isGtr = false ;
  bool isEqu = false ;
  gIsIDENT2First = true ;
  int peekType = 0 ;
  char getChar = '\0' ;
  string tempToken = "";
  gIsComment = false ;
  // cout << tokenType << " tokenTYPE" << endl ;
  while ( !completeToken ) {
    PeekNextChar( peekType, tokenType ) ;  // peek the next char
    while ( peekType == WSPACE ) {
      // skip white space (tab, space, enter) and peek again
      char temp = '\0' ;
      GetNextChar( temp ) ;        // read the white space and do nothing
      PeekNextChar( peekType, tokenType ) ;   // peek again
      if ( !IsReconized( cin.peek() ) )
        throw 1 ;
    }  // while ()

    if ( tokenType != ARITHEXP && tokenType != MULORDIV && isFirstChar && peekType == DIV ) {
      if ( tokenType != ASSIGN ) {  // Number of columns exceeds 110.
        GetNextChar( getChar ) ;
        if ( cin.peek() == '/' ) {               // then it is a comment line
          GetNextChar( getChar ) ;
          gIsComment = true ;                  // set the boolean
        }  // if ()

        else
          throw 22 ;
        // this is a fucking spaical ERROR since there is a '/' after '+' or somethings like that
      }  // if ()

      else {
        GetNextChar( getChar ) ;
        if ( cin.peek() == '/' ) {               // then it is a comment line
          GetNextChar( getChar ) ;
          SkipUntilEnter() ;
          while ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) {
            // skip white space (tab, space, enter) and peek again
            char temp = '\0' ;
            GetNextChar( temp ) ;        // read the white space and do nothing
          }  // while ()

        }  // if ()

        else {  // after IDENT, there is a DIV token but comment line
          gIsDIV = true ;
          return true ;
        }  // else
 
        PeekNextChar( peekType, tokenType ) ;
      }  // else

    } // if ()

    if ( tokenType != ASSIGN && peekType == COLON && isFirstChar ) {
      GetNextChar( getChar ) ;
      tempToken = tempToken + getChar ;
      isFirstChar = false ;
      if ( cin.peek() != '=' )
        throw 111 ;  // there is only : but =, error one found
      else
        throw 222 ;  // error two ":="
    }  // if ()

    if ( tokenType != IDENT && tokenType != IDENT2 && 
         tokenType != NUM && tokenType != SIGN && peekType == DOT && isFirstChar ) {
      GetNextChar( getChar ) ;
      tempToken = tempToken + getChar ;
      isFirstChar = false ;
      if ( !isdigit( cin.peek() ) )
        throw 11 ;  // there is only . , error one found
      else
        throw 2222 ;  // error two ".123"
    }  // if ()

    if ( tokenType == IDENT && !gIsComment ) {
      // if we need a IDENT, the next char can be a digit, alpha or underline
      if ( peekType == DIGIT || peekType == ALPH || peekType == UNDERLINE ) {
        if ( isFirstChar && ( peekType == DIGIT || peekType == UNDERLINE ) ) { 
          // the first char of IDENT must be an alpha
          if ( peekType == UNDERLINE )
            throw 1 ;
          return false ;                         // 1ab is not a legal IDENT   
        }  // if ()

        GetNextChar( getChar ) ;           // get the next char from input buffer
        // cout << getChar << " is an ID ELEMENT." << endl ;
        tempToken = tempToken + getChar ;  // char to string
        isFirstChar = false ;              // obviously, the next char will not be the first char
        gCurTokenName = tempToken ;
        if ( tempToken == "quit" && gIsCmdBegin 
             && ( !IsReconized( cin.peek() ) || isspace( cin.peek() ) ) ) {      
          // if the first IDENT is "quit" 
          // cout << "found QUIT" << endl ;
          gIsQuit = true ;                 // it will shut down the command
          completeToken = true ;
        }  // if ()
       
      }  // if ()

      else if ( isFirstChar ) 
        return false ;
      // since the next char does not match all of the above
      // it must be other types of the token in the grammar

      else {
        if ( tempToken == "quit" && gIsCmdBegin ) {       // if the first IDENT is "quit" 
          // cout << "found QUIT" << endl ;
          gIsQuit = true ;                 // it will shut down the command
        }  // if ()

        completeToken = true ;
      }  // else


      if ( !isFirstChar && ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) )
        completeToken = true ;  // use space to cut token

    }  // if ()

    else if ( tokenType == IDENT2 && !gIsComment ) {
      // if we need a IDENT, the next char can be a digit, alpha or underline
      if ( peekType == DIGIT || peekType == ALPH || peekType == UNDERLINE ) {
        if ( isFirstChar && ( peekType == DIGIT || peekType == UNDERLINE ) ) {
          // the first char of IDENT must be an alpha
          if ( peekType == UNDERLINE )
            throw 1 ;
          return false ;                         // 1ab is not a legal IDENT   
        }  // if ()

        if ( !IsReconized( cin.peek() ) ) {
          int size = gVecOfTable.size() ;
          bool found = false ;
          for ( int i = 0 ; i < size ; i++ ) {
            if ( tempToken == gVecOfTable[ i ].idName )
              found = true ;
          }  // for ()

          if ( !found ) {
            cout << "> Undefined identifier : '" << tempToken << "'" ;
            throw 3 ;
          }  // if ()

          else
            throw 1 ;

        }  // if ()

        GetNextChar( getChar ) ;           // get the next char from input buffer
        // cout << getChar << " is an ID ELEMENT." << endl ;
        tempToken = tempToken + getChar ;  // char to string
        isFirstChar = false ;              // obviously, the next char will not be the first char
        gIsIDENT2First = false ;
        if ( tempToken == "quit" && gIsCmdBegin ) {       // if the first IDENT is "quit" 
          // cout << "found QUIT" << endl ;
          gIsQuit = true ;                 // it will shut down the command
          completeToken = true ;           // since quit is a complete token
        }  // if ()

      }  // if ()

      else if ( isFirstChar )
        return false ;
      // since the next char does not match all of the above
      // it must be other types of the token in the grammar

      else
        completeToken = true ;

      if ( !isFirstChar && ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) )
        completeToken = true ;  // use space to cut token

    }  // else if ()

    else if ( tokenType == FLOAT && !gIsComment ) {  // not sure if it will work
      // if we need a FLOAT, the next char must be a digit since there is already has a dot
      if ( peekType == DIGIT ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        isOnlyOneDot = false ;
      }  // if ()

      else {
        if ( isOnlyOneDot )
          throw 11 ;  // find a fucking spacial error since there is only a dot as token
        completeToken = true ;
        // tokenType = NUM ;  //  return tokenTpye value back to NUM
      }  // else

      if ( !isFirstChar && ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) )
        completeToken = true ;  // use space to cut token
    }  // else if ()

    else if ( tokenType == SIGN && !gIsComment ) {
      if ( ( peekType == SUB || peekType == ADD ) ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // else if ()

      else
        return false ;

    }  // else if ()

    else if ( tokenType == NUM && !gIsComment ) {
      // if we need a NUM, the next char can be a digit or a dot
      if ( peekType == DIGIT ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        isFirstChar = false ;
        isOnlyOneDot = false ;
      }  // if ()

      else if ( peekType == DOT ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        if ( !isFirstChar )
          isOnlyOneDot = false ;
        isFirstChar = false ;
        gDotFound = true ;   // I don't like it
        tokenType = FLOAT ;  // if we get a dot, the next char must be a digit
      }  // else if ()

      else if ( isFirstChar )
        return false ;

      else if ( peekType == DIV && isSign ) {
        GetNextChar( getChar ) ;
        PeekNextChar( peekType, tokenType ) ;             // if there is another '/'
        if ( peekType == DIV ) {               // then it is a comment line
          GetNextChar( getChar ) ;
          isFirstChar = false ;
          gIsComment = true ;                  // set the boolean
        }  // if ()
        else
          throw 22 ;
        // this is a fucking spaical ERROR since there is a '/' after '+' or somethings like that
      }  // else if ()

      else
        completeToken = true ;

      if ( !isFirstChar && ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) ) {
        if ( isOnlyOneDot )
          throw 11 ;
        else
          completeToken = true ;  // use space to cut token
      }  // if ()

    }  // else if ()

    else if ( tokenType == ASSIGN && !gIsComment ) {
      // if we need an ASSIGN, the next char must be ':'
      if ( cin.peek() == '/' ) {
        GetNextChar( getChar ) ;
        if ( cin.peek() == '/' ) {               // then it is a comment line
          GetNextChar( getChar ) ;
          SkipUntilEnter() ;
          while ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) {
            // skip white space (tab, space, enter) and peek again
            char temp = '\0' ;
            GetNextChar( temp ) ;        // read the white space and do nothing
          }  // while ()

        }  // if ()

        else {  // after IDENT, there is a DIV token but comment line
          gIsDIV = true ;
          return true ;
        }  // else

        PeekNextChar( peekType, tokenType ) ;
      }  // if ()

      else if ( peekType == COLON && isFirstChar ) {        
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        isFirstChar = false ;
        if ( cin.peek() != '=' )
          throw 111 ;  // there is only : but =, error one found
      }  // if ()

      else if ( peekType == EQU && !isFirstChar ) {
        // after the colon, there must be '='
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // else if ()

      else if ( isFirstChar ) 
        return false ;
    }  // else if ()

    else if ( tokenType == LEFT_PAREN && !gIsComment ) {
      // if we need LEFT_PAREN, the next char must be '('
      if ( peekType == LEFT_PAREN ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else return false ;
    }  // else if ()

    else if ( tokenType == RIGHT_PAREN && !gIsComment ) {
      // if we need RIGHT_PAREN, the next char must be ')'
      if ( peekType == RIGHT_PAREN ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else return false ;
    }  // else if ()

    else if ( tokenType == ARITHEXP && !gIsComment ) {
      // if we need ARITHEEXP, the next char must be '+' or '-' or '*' or '/'
      if ( peekType == ADD ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else if ( peekType == SUB ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else if ( peekType == MUL ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else if ( peekType == DIV ) {
        GetNextChar( getChar ) ;
        if ( !IsReconized( cin.peek() ) ) {
          int size = gVecOfTable.size() ;
          bool found = false ;
          for ( int i = 0 ; i < size ; i++ ) {
            if ( gCurTokenName == gVecOfTable[ i ].idName )
              found = true ;
          }  // for ()

          if ( !found ) {
            cout << "> Undefined identifier : '" << gCurTokenName << "'" ;
            throw 3 ;
          }  // if ()

        }  // if ()

        PeekNextChar( peekType, tokenType ) ;
        if ( peekType == DIV )  // double '/' , comment line found
          gIsComment = true ;
        else {
          tempToken = tempToken + getChar ;
          completeToken = true ;
        }  // else
      }  // if ()

      else return false ;

      if ( !IsReconized( cin.peek() ) ) {
        int size = gVecOfTable.size() ;
        bool found = false ;
        for ( int i = 0 ; i < size ; i++ ) {
          if ( gCurTokenName == gVecOfTable[ i ].idName )
            found = true ;
        }  // for ()

        if ( !found ) {
          cout << "> Undefined identifier : '" << gCurTokenName << "'" ;
          throw 3 ;
        }  // if ()

      }  // if ()

    }  // else if ()

    else if ( tokenType == ADDORSUB && !gIsComment ) {
      // if we need ADDORSUB, the next char must be '+' or '-'
      if ( peekType == ADD ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()
      else if ( peekType == SUB ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else return false ;
    }  // else if ()

    else if ( tokenType == MULORDIV && !gIsComment ) {
      // if we need MULORDIV, the next char must be '*' or '/'
      if ( peekType == MUL ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else if ( peekType == DIV ) {
        GetNextChar( getChar ) ;
    

        PeekNextChar( peekType, tokenType ) ;
        if ( peekType == DIV )
          gIsComment = true ;  // double '/' , comment line found
        else {
          tempToken = tempToken + getChar ;
          completeToken = true ;
        }  // else

      }  // if ()

      else return false ;

    }  // else if ()

    else if ( tokenType == BOOLOP && !gIsComment ) {
      // if we need BOOLOP, the next char must be '=' or '>' or '<' 
      if ( peekType == LSS ) {
        if ( isFirstChar ) {
          GetNextChar( getChar ) ;
          tempToken = tempToken + getChar ;
          isFirstChar = false ;
          isLss = true ;
        }  // if ()

        else if ( !isFirstChar )
          completeToken = true ;  // a<< wii be undefined
        else
          return false ;
 
      }  // if ()

      else if ( peekType == GTR ) {
        if ( isLss && !isFirstChar ) {
          GetNextChar( getChar ) ;
          tempToken = tempToken + getChar ;
          completeToken = true ;
        }  // if ()

        else if ( isFirstChar ) {
          GetNextChar( getChar ) ;
          tempToken = tempToken + getChar ;
          isFirstChar = false ;
          isGtr = true ;
        }  // else if ()

        else if ( !isFirstChar )
          completeToken = true ;  // a>> wii be undefined
        else
          return false ;

      }  // else if ()

      else if ( peekType == EQU ) {
        if ( isLss || isGtr ) {
          GetNextChar( getChar ) ;
          tempToken = tempToken + getChar ;
          completeToken = true ;
        }  // if ()

        else if ( isFirstChar ) {
          GetNextChar( getChar ) ;
          tempToken = tempToken + getChar ;
          completeToken = true ;
        }  // else if ()

      }  // else if ()

      else if ( peekType == DIV ) {
        GetNextChar( getChar ) ;
        if ( cin.peek() == '/' ) {               // then it is a comment line
          GetNextChar( getChar ) ;
          isFirstChar = false ;
          gIsComment = true ;                  // set the boolean
          completeToken = true ;
        }  // if ()

        else {
          int size = gVecOfTable.size() ;
          bool found = false ;
          for ( int i = 0 ; i < size ; i++ ) {
            if ( gCurTokenName == gVecOfTable[ i ].idName )
              found = true ;
          }  // for ()

          if ( !found ) {
            cout << "> Undefined identifier : '" << gCurTokenName << "'" ;
            throw 3 ;
          }  // if ()

          else
            throw 22 ;
          // this is a fucking spaical ERROR since there is a '/' after '<' or somethings like that
        }  // else

      }  // else if ()

      else if ( !isFirstChar )
        completeToken = true ;

      else
        return false ;

      if ( !isFirstChar && ( cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\t' ) )
        completeToken = true ;  // use space to cut token

      if ( !IsReconized( cin.peek() ) ) {
        int size = gVecOfTable.size() ;
        bool found = false ;
        for ( int i = 0 ; i < size ; i++ ) {
          if ( gCurTokenName == gVecOfTable[ i ].idName )
            found = true ;
        }  // for ()

        if ( !found ) {
          cout << "> Undefined identifier : '" << gCurTokenName << "'" ;
          throw 3 ;
        }  // if ()

      }  // if ()

    }  // else if ()

    else if ( tokenType == SEMICOLON && !gIsComment ) {
      // if we need SEMICOLON, the next char must be ';'
      if ( peekType == SEMICOLON ) {
        GetNextChar( getChar ) ;
        tempToken = tempToken + getChar ;
        completeToken = true ;
      }  // if ()

      else return false ;
    }  // else if ()

    if ( gIsComment ) {
      SkipUntilEnter() ;  // skip until enter
      // cout << "SkipUntilEnter" << endl ;
      gIsComment = false ;

      if ( tokenType == IDENT && isFirstChar )
        completeToken = false ;

      else
        isFirstChar = true ;

    }  // if ()

  } // while ()

  tokenValue = tempToken ;
  return true ;

}  // Token::GetToken()

void Token::PeekNextChar( int &charType, int tokenType ) {
  char charValue = '\0' ;
  charValue = cin.peek() ;
  SetCharType( charType, charValue, tokenType ) ;
  // cout << "What I peek." << charValue << "-- " << charType << "-- " << tokenType << endl;

}  // Token::PeekNextChar()

void Token::GetNextChar( char &curChar ) {
  curChar = getchar() ;
  // cout << "What the fuck I have gotten. --------> "<< curChar << endl ;

}  // Token::GetNextChar()

bool Token::IsReconized( char ch ) {
  if ( ch != '(' && ch != ')' && ch != '_' && ch != '.' &&
       ch != ':' && ch != '=' && ch != '>' && ch != '<' && ch != '+' &&
       ch != '-' && ch != '*' && ch != '/' && ch != ';' ) {
    if ( !isdigit( ch ) && !isalpha( ch ) && !isspace( ch ) )
      return false ;
  }  // if ()

  return true ;
}  // Token::IsReconized()

void Token::SetCharType( int &CharType, char CharValue, int tokenType ) {
  if ( isdigit( CharValue ) ) CharType = DIGIT ;         // 1
  else if ( isalpha( CharValue ) ) CharType = ALPH ;     // a
  else if ( CharValue == '(' ) CharType = LEFT_PAREN ;   // (
  else if ( CharValue == ')' ) CharType = RIGHT_PAREN ;  // )
  else if ( CharValue == '_' ) CharType = UNDERLINE ;    // _
  else if ( CharValue == '.' ) CharType = DOT ;          // .
  else if ( CharValue == ':' ) CharType = COLON ;        // :
  else if ( CharValue == '=' ) CharType = EQU ;          // =
  else if ( CharValue == '>' ) CharType = GTR ;          // >
  else if ( CharValue == '<' ) CharType = LSS ;          // <
  else if ( CharValue == '+' ) CharType = ADD ;          // +
  else if ( CharValue == '-' ) CharType = SUB ;          // -
  else if ( CharValue == '*' ) CharType = MUL ;          // *
  else if ( CharValue == '/' ) CharType = DIV ;          // /
  else if ( CharValue == ';' ) CharType = SEMICOLON ;    // ;
  else if ( isspace( CharValue ) ) CharType = WSPACE ;   // space, tab, enter
  else {
    if ( tokenType == IDENT2 && !gIsIDENT2First )
      gIsUnRecon = true ;
    else
      throw 1 ;
  }  // else

}  // Token::SetCharType()

void Token::SetTokenType( int &tokenType, string &tokenValue ) {
  bool hasLetter = false ;  // if there is letter in the token
  if ( tokenValue == "quit" ) tokenType = QUIT ;       // quit
  else if ( tokenValue == "=" ) tokenType = EQU ;      // =
  else if ( tokenValue == "<>" ) tokenType = NEQ ;     // <>
  else if ( tokenValue == ">" ) tokenType = GTR ;      // >
  else if ( tokenValue == "<" ) tokenType = LSS ;      // <
  else if ( tokenValue == ">=" ) tokenType = GEQ ;     // >=
  else if ( tokenValue == "<=" ) tokenType = LEQ ;     // <=
  else if ( tokenValue == "+" ) tokenType = ADD ;      // +
  else if ( tokenValue == "-" ) tokenType = SUB ;      // -
  else if ( tokenValue == "*" ) tokenType = MUL ;      // *
  else if ( tokenValue == "/" ) tokenType = DIV ;      // /
  else if ( tokenValue == ":=" ) tokenType = ASSIGN ;  // :=
  else {
    int strSize = tokenValue.size() ;
    for ( int i = 0 ; i < strSize && !hasLetter ; i++ ) {
      if ( isdigit( tokenValue.at( i ) ) )
        tokenType = NUM ;                            // number      
      else {
        tokenType = IDENT ;                          // identifier
        hasLetter = true;
      }  // else

    }  // for ()

  }  // else

}  // Token::SetTokenType()

void Token::SkipUntilEnter() {
  char ch = cin.peek() ;
  while ( ch != '\n' ) {  // read until enter
    char space = getchar() ;
    ch = cin.peek() ;
  }  // while ()

  char enter = getchar() ;  // skip the enter

}  // Token::SkipUntilEnter()

class Pharsing {
private:
  int mIDType ;
  int mID2Type ;
  int mASSIGNType ;
  int mSEMICOLONType ;
  int mSIGNType ;
  int mNUMType ;
  int mLPARENType ;
  int mRPARENType ;
  int mARITHEXPType ;
  int mADDORSUBType ;
  int mMULORDIVType ;
  int mBOOLOPType ;

public:
  void Init() {
    mIDType = IDENT ;
    mID2Type = IDENT2 ;
    mASSIGNType = ASSIGN ;
    mSEMICOLONType = SEMICOLON ;
    mSIGNType = SIGN ;
    mNUMType = NUM ;
    mLPARENType = LEFT_PAREN ;
    mRPARENType = RIGHT_PAREN ;
    mARITHEXPType = ARITHEXP ;
    mADDORSUBType = ADDORSUB ;
    mMULORDIVType = MULORDIV ;
    mBOOLOPType = BOOLOP ;

  }  // Init()

  void IsDefined( string str ) {
    int size = gVecOfTable.size() ;
    for ( int i = 0 ; i < size ; i++ ) {
      if ( gVecOfTable[i].idName == str ) {
        return ;
      }  // if ()

    }  // for ()

    cout << "> Undefined identifier : '" << str << "'" ;
    throw 3 ;
  }  // IsDefined()

  void GetIdValue( IdTable &id, string str ) {
    int size = gVecOfTable.size() ;
    for ( int i = 0 ; i < size ; i++ ) {
      if ( gVecOfTable[i].idName == str ) {
        id.idValue = gVecOfTable[i].idValue ;
        id.idName = gVecOfTable[i].idName ;
        id.isFloat = gVecOfTable[i].isFloat ;
        return ;
      }  // if ()

    }  // for ()

  }  // GetIdValue()

  bool SetIdValue( string str, float &idValue, bool DotFound ) {
    int size = gVecOfTable.size() ;
    for ( int i = 0 ; i < size ; i++ ) {
      if ( gVecOfTable[i].idName == str ) {
        gVecOfTable[i].idValue = idValue ;
        gVecOfTable[i].isFloat = DotFound ;
        // cout << gVecOfTable[i].idValue << " update value" << endl ;
        return true ;
      }  // if ()

    }  // for ()

    return false ;
  }  // SetIdValue()

  bool IsFloat( float value ) {
    int temp = ( int ) value ;
    if ( value - temp == 0 )
      return false ;
    else 
      return true ;
  }  // IsFloat()

  void Command( bool &correct ) {
    string tokenValue = "" ;
    Token token ;

    string identName = "" ;
    float idValue = 0.0 ;
    float aValue = 0.0 ;
    bool aCorrect = false ;
    string bValue = "" ;
    bool bCorrect = false ;
    bool isbool = false ;

    if ( token.GetToken( mIDType, tokenValue ) ) {  // IDENT
      if ( gIsComment ) {
        correct = true ;
        return ;  // Comment found, shut down the current Command
      }  // if ()

      if ( gIsQuit ) {
        correct = true ;
        return ;
      }  // if ()

      gIsCmdBegin = false ;  // command begining over, no QUIT command
      // cout << tokenValue << " is an INENT in <Command>." << endl ;
      identName = tokenValue;
      tokenValue = "" ;

      if ( token.GetToken( mASSIGNType, tokenValue ) && !gIsDIV ) {  // case ':='<ArithExp>
        // cout << tokenValue << " is an ASSIGN in <Command>." << endl ;
        ArithExp( aCorrect, aValue ) ;  // go to <ArithExp> and set correct
        if ( aCorrect && token.GetToken( mSEMICOLONType, tokenValue ) ) { // find ';'
          // cout << tokenValue << " is an SEMICOLON in <Command>IDENT." << endl ;

          if ( gDotFound )
            printf( "> %1.3f\n", aValue );
          else {
            cout << "> " << ( int ) aValue << endl ;  // output answer
          }  // else

          if ( !SetIdValue( identName, aValue, gDotFound ) ) {
            // update the id value if it was exist, otherwise push back as new ident
            IdTable id ;
            id.idName = identName ;
            id.idValue = aValue ;
            if ( gDotFound )
              id.isFloat = true ;  
            else
              id.isFloat = false ;
            gVecOfTable.push_back( id ) ;
            // cout << gVecOfTable[0].idName << "  " << gVecOfTable[ 0 ].idValue << endl ;
          }  // if ()

          correct = true ;
        }  // if ()
        // if all of above are good, then this is a correct grammar
        else
          throw 2 ;

      }  // if ()

      else if ( gIsDIV ) {
        // case <IDlessArithExpOrBexp>    
        IDlessArithExpOrBexp_DIV( bCorrect, isbool, aValue, bValue, identName ) ;
        // go to <IDlessArithExpOrBexp> and set correct

        if ( token.GetToken( mSEMICOLONType, tokenValue ) && bCorrect ) {  // find ';'
          // cout << tokenValue << " is an SEMICOLON in <Command>IDless." << endl ;
          IdTable id ;
          IsDefined( identName ) ;  // check if it has been defined and set id value
          GetIdValue( id, identName ) ;
          if ( isbool )
            cout << "> " << bValue << endl ;  // output answer
          else {
            if ( id.isFloat || gDotFound )
              printf( "> %1.3f\n", aValue );
            else {
              printf( "> %1.0f\n", aValue );
              // cout << "> " << ( int ) aValue << endl ;  // output answer
            }  // else

          }  // else

          correct = true ;
        }  // if ()

        // if all of above are good, then this is a correct grammar
        else
          throw 2 ;

      }  // else if ()

      else {
        // case <IDlessArithExpOrBexp>    
        IDlessArithExpOrBexp( bCorrect, isbool, aValue, bValue, identName ) ;
        // go to <IDlessArithExpOrBexp> and set correct
        
        if ( token.GetToken( mSEMICOLONType, tokenValue ) && bCorrect ) {  // find ';'
          // cout << tokenValue << " is an SEMICOLON in <Command>IDless." << endl ;
          IdTable id ;
          IsDefined( identName ) ;  // check if it has been defined and set id value
          GetIdValue( id, identName ) ;
          if ( isbool )
            cout << "> " << bValue << endl ;  // output answer
          else {
            if ( id.isFloat || gDotFound )
              printf( "> %1.3f\n", aValue );
            else {
              printf( "> %1.0f\n", aValue );
              // cout << "> " << ( int ) aValue << endl ;  // output answer
            }  // else

          }  // else

          correct = true ;
        }  // if ()

        // if all of above are good, then this is a correct grammar
        else
          throw 2 ;

      }  // else

    }  // if ()

    else {  // case <NOT_ID_StartArithExpOrBexp>)
      gIsCmdBegin = false ;  // command begining over, no QUIT command
      NOT_ID_StartArithExpOrBexp( correct, isbool, bValue, aValue ) ;
      if ( correct && token.GetToken( mSEMICOLONType, tokenValue ) ) {  // find ';'
        // cout << tokenValue << " is an SEMICOLON in <Command>NOTID." << endl ;
        if ( isbool )
          cout << "> " << bValue << endl ;  // output answer
        else {
          if ( gDotFound )
            printf( "> %1.3f\n", aValue );
          else {
            printf( "> %1.0f\n", aValue );
            // cout << "> " << ( int ) aValue << endl ;  // output answer
          }  // else

        }  // else

        gDotFound = false ;
        correct = true ;
      }  // if ()

      // if all of above are good, then this is a correct grammar
      else
        throw 2 ;
    }  // else

  }  // Command()

  void IDlessArithExpOrBexp_DIV( bool &correct, bool &isbool, float &value,
                                 string &boolValue, string identName ) {
    // cout << "<IDlessArithExpOrBexp_DIV>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tempType = 0 ;
    Token token ;
    IdTable id ;

    bool doArith = false ;
    bool tfCorrect = false ;
    bool aCorrect = false ;
    bool bCorrect = false ;
    float tfValue = 0.0 ;
    float aValue = 0.0 ;

    string bValue = "" ;
    gCurTokenName = identName ;

    // cout << tokenValue << " is an ArithExp in <IDlessArithExpOrBexp." << endl ;
    IsDefined( identName ) ;  // check if it has been defined and set id value
    GetIdValue( id, identName ) ;
    aValue = id.idValue ;
    correct = true ;
    doArith = true ;

    Factor( tfCorrect, tfValue ) ;  // go to <Factor> and set correct
    if ( tfValue != 0 )
      aValue = aValue / tfValue ;
    else
      throw 4 ;

    while ( token.GetToken( mARITHEXPType, tokenValue ) ) {
    // case ArithExpOrBexp
      // cout << tokenValue << " is an ArithExp in <IDlessArithExpOrBexp." << endl ;
      token.SetTokenType( tempType, tokenValue ) ;
      tokenValue = "" ;
      if ( tempType == ADD || tempType == SUB ) {
        Term( tfCorrect, tfValue ) ;  // go to <Term> and set correct
        // cout << id.idValue << "idValue" << endl ;
        // cout << tfValue<< "tfVaule" << endl ;
        if ( tfCorrect && tempType == ADD )
          aValue = aValue + tfValue ;
        else if ( tfCorrect && tempType == SUB )
          aValue = aValue - tfValue ;
        else
          throw 2 ;
        // cout << aValue << " ADD or SUB answer" << endl ;
      }  // if ()

      else if ( tempType == MUL || tempType == DIV ) {
        Factor( tfCorrect, tfValue ) ;  // go to <Factor> and set correct
        // cout << id.idValue << "idValue" << endl ;
        // cout << tfValue << "tfVaule" << endl ;
        if ( tfCorrect && tempType == MUL )
          aValue = aValue * tfValue ;
        else if ( tfCorrect && tempType == DIV )
          if ( tfValue != 0 )
            aValue = aValue / tfValue ;
          else
            throw 4 ;
        else
          throw 2 ;
        // cout << aValue << "MUL or DIV answer" << endl ;
      }  // else if ()

      tokenValue = "" ;

    }  // while ()


    float answerValue = aValue ;  // a + 1 = 1 ;
    aValue = 0.0 ;
    BooleanExp( bCorrect, bValue ) ;  // go to <BooleanOperator> and set correct
    if ( bCorrect ) {
      isbool = true ;
      IsDefined( identName ) ;  // check if it has been defined and set id value
      GetIdValue( id, identName ) ;
      if ( !doArith )
        answerValue = id.idValue ;  // a = 1 ;

      token.SetTokenType( tempType, bValue ) ;
      ArithExp( aCorrect, aValue ) ;  // go to <ArithExp> and set correct
      if ( aCorrect ) {
        // cout << answerValue << endl;
        // cout << aValue << endl ;
        correct = true ;
        if ( tempType == EQU && answerValue == aValue )
          boolValue = "true" ;
        else if ( tempType == NEQ && answerValue != aValue )
          boolValue = "true" ;
        else if ( tempType == GTR && answerValue > aValue )
          boolValue = "true" ;
        else if ( tempType == GEQ && answerValue >= aValue )
          boolValue = "true" ;
        else if ( tempType == LSS && answerValue < aValue )
          boolValue = "true" ;
        else if ( tempType == LEQ && answerValue <= aValue )
          boolValue = "true" ;
        else
          boolValue = "false" ;
      }  // if ()

      else
        throw 2 ;

    }  // if ()

    else {
      IdTable temp ;
      boolValue = "false" ;
      if ( !doArith ) {
        // IsDefined( identName ) ;  // check if it has been defined and set id value
        GetIdValue( temp, identName ) ;
        value = temp.idValue ;
      }  // if ()

      else
        value = answerValue ;
      correct = true ;  // though there is no BooleanOperator, it's still correct
    }  // else ()

  }  // IDlessArithExpOrBexp_DIV()

  void IDlessArithExpOrBexp( bool &correct, bool &isbool, float &value, 
                             string &boolValue, string identName ) {
    // cout << "<IDlessArithExpOrBexp>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tempType = 0 ;
    Token token ;
    IdTable id ;

    bool firstTime = true ;
    bool doArith = false ;
    bool tfCorrect = false ;
    bool aCorrect = false ;
    bool bCorrect = false ;
    float tfValue = 0.0 ;
    float aValue = 0.0 ;
    string bValue = "" ;
    gCurTokenName = identName ;


    while ( token.GetToken( mARITHEXPType, tokenValue ) ) {  // case ArithExpOrBexp
      // cout << tokenValue << " is an ArithExp in <IDlessArithExpOrBexp." << endl ;
      if ( firstTime ) {
        firstTime = false ;
        IsDefined( identName ) ;  // check if it has been defined and set id value
        GetIdValue( id, identName ) ;
        aValue = id.idValue ;
      }  // if ()

      correct = true ;
      doArith = true ;
      token.SetTokenType( tempType, tokenValue ) ;
      tokenValue = "" ;
      if ( tempType == ADD || tempType == SUB ) {
        Term( tfCorrect, tfValue ) ;  // go to <Term> and set correct
        // cout << id.idValue << "idValue" << endl ;
        // cout << tfValue<< "tfVaule" << endl ;
        if ( tfCorrect && tempType == ADD ) 
          aValue = aValue + tfValue ;
        else if ( tfCorrect && tempType == SUB )
          aValue = aValue - tfValue ;
        else
          throw 2 ;     
        // cout << aValue << " ADD or SUB answer" << endl ;
      }  // if ()

      else if ( tempType == MUL || tempType == DIV ) {
        Factor( tfCorrect, tfValue ) ;  // go to <Factor> and set correct
        // cout << id.idValue << "idValue" << endl ;
        // cout << tfValue << "tfVaule" << endl ;
        if ( tfCorrect && tempType == MUL )
          aValue = aValue * tfValue ;
        else if ( tfCorrect && tempType == DIV )
          if ( tfValue != 0 )
            aValue = aValue / tfValue ;
          else
            throw 4 ;
        else
          throw 2 ;
        // cout << aValue << "MUL or DIV answer" << endl ;
      }  // else if ()
        
      tokenValue = "" ;
    }  // while ()

    float answerValue = aValue ;  // a + 1 = 1 ;
    aValue = 0.0 ;
    BooleanExp( bCorrect, bValue ) ;  // go to <BooleanOperator> and set correct
    if ( bCorrect ) {
      isbool = true ;
      IsDefined( identName ) ;  // check if it has been defined and set id value
      GetIdValue( id, identName ) ;
      if ( !doArith )
        answerValue = id.idValue ;  // a = 1 ;

      token.SetTokenType( tempType, bValue ) ;
      ArithExp( aCorrect, aValue ) ;  // go to <ArithExp> and set correct
      if ( aCorrect ) {
        // cout << answerValue << endl;
        // cout << aValue << endl ;
        correct = true ;
        if ( tempType == EQU && answerValue == aValue )
          boolValue = "true" ;
        else if ( tempType == NEQ && answerValue != aValue )
          boolValue = "true" ;
        else if ( tempType == GTR && answerValue > aValue )
          boolValue = "true" ;
        else if ( tempType == GEQ && answerValue >= aValue )
          boolValue = "true" ;
        else if ( tempType == LSS && answerValue < aValue )
          boolValue = "true" ;
        else if ( tempType == LEQ && answerValue <= aValue )
          boolValue = "true" ;
        else
          boolValue = "false" ;
      }  // if ()

      else
        throw 2 ;

    }  // if ()

    else {
      IdTable temp ;
      boolValue = "false" ;
      if ( !doArith ) {
        // IsDefined( identName ) ;  // check if it has been defined and set id value
        GetIdValue( temp, identName ) ;
        value = temp.idValue ;
      }  // if ()

      else
        value = answerValue ;
      correct = true ;  // though there is no BooleanOperator, it's still correct
    }  // else ()

  }  // IDlessArithExpOrBexp()

  void BooleanExp( bool &correct, string &value ) {
    // cout << "<BooleanOperator>" << endl ;  // testing grammar
    string tokenValue = "" ;
    Token token ;

    if ( token.GetToken( mBOOLOPType, tokenValue ) ) {  // find BooleanOperator
      // cout << tokenValue << " is an BooleanOperator." << endl ;
      value = tokenValue ;
      correct = true ;
    } // if ()

    else
      correct = false ;

  }  // BooleanExp()

  void NOT_ID_StartArithExpOrBexp( bool &correct, bool &isbool, string &boolValue, float &value ) {
    // cout << "<NOT_ID_StartArithExpOrBexp>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tempType = 0 ;
    Token token ;

    bool startAE_Correct = false ;
    bool aCorrect = false ;
    bool bCorrect = false ;
    float startAEValue = 0.0 ;
    float aValue = 0.0 ;
    string bValue = "" ;

    NOT_ID_StartArithExp( startAE_Correct, startAEValue ) ;
    if ( startAE_Correct ) {
      value = startAEValue ;
      BooleanExp( bCorrect, bValue ) ;  // go to <BooleanOperator> and set correct
      if ( bCorrect ) {
        isbool = bCorrect ;
        token.SetTokenType( tempType, bValue ) ;
        ArithExp( aCorrect, aValue ) ;  // go to <ArithExp> and set correct
        if ( aCorrect ) {
          // cout << startAEValue << endl;
          // cout << aValue << endl ;
          correct = true ;
          float larger = 0 ;
          float smaller = 0 ;
          if ( startAEValue > aValue ) {
            larger = startAEValue ;
            smaller = aValue ;
          }  // if ()
          else {
            larger = aValue ;
            smaller = startAEValue ;
          }  // else

          float diff = larger - smaller  ;

          if ( tempType == EQU && startAEValue == aValue )
            boolValue = "true" ;
          else if ( tempType == NEQ && startAEValue != aValue )
            boolValue = "true" ;
          else if ( tempType == GTR && startAEValue > aValue )
            boolValue = "true" ;
          else if ( tempType == GEQ && startAEValue >= aValue )
            boolValue = "true" ;
          else if ( tempType == LSS && startAEValue < aValue )
            boolValue = "true" ;
          else if ( tempType == LEQ && startAEValue <= aValue )
            boolValue = "true" ;
          else
            boolValue = "false" ;
        }  // if ()

        else
          throw 2 ;
      }  // if ()

      else {
        boolValue = "false" ;
        correct = true ;  // though there is no BooleanOperator, it's still correct
      }  // else 

    }  // if ()

  }  // NOT_ID_StartArithExpOrBexp()

  void NOT_ID_StartArithExp( bool &correct, float &value ) {
    // cout << "<NOT_ID_StartArithExp>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tokenType = 0 ;
    Token token ;

    bool tCorrect_1 = false ;
    bool tCorrect_2 = false ;
    float tValue_1 = 0.0 ;
    float tValue_2 = 0.0 ;

    NOT_ID_StartTerm( tCorrect_1, tValue_1 ) ;
    if ( tCorrect_1 ) {
      value = tValue_1 ;
      correct = true ;
      while ( token.GetToken( mADDORSUBType, tokenValue ) ) {  // case ArithExpOrBexp
        // cout << tokenValue << " is an ADDORSUB in <NOT_ID_StartArithExp>." << endl ;
        token.SetTokenType( tokenType, tokenValue ) ;  // set tokenValue Type
        tokenValue = "" ;
        Term( tCorrect_2, tValue_2 ) ;  // go to <Term> and set correct
        if ( tCorrect_2 ) {
          correct = true ;
          if ( tokenType == ADD )
            tValue_1 = tValue_1 + tValue_2 ;
          else if ( tokenType == SUB )
            tValue_1 = tValue_1 - tValue_2 ;
          value = tValue_1 ;  // final value
        }  // if ()

        else
          throw 2 ;
      }  // while ()

    }  // if ()

  }  // NOT_ID_StartArithExp()

  void NOT_ID_StartTerm( bool &correct, float &value ) {
    // cout << "<NOT_ID_StartTerm>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tokenType = 0 ;
    Token token ;

    bool fCorrect_1 = false ;
    bool fCorrect_2 = false ;
    float fValue_1 = 0.0 ;
    float fValue_2 = 0.0 ;

    NOT_ID_StartFactor( fCorrect_1, fValue_1 ) ;
    if ( fCorrect_1 ) {
      // cout << fValue_1 << " is fValue_1" << endl ;
      correct = true ;
      value = fValue_1 ;
      while ( token.GetToken( mMULORDIVType, tokenValue ) ) {  // case ArithExpOrBexp   
        // cout << tokenValue << " is an MULORDIV in <NOT_ID_StartTerm>." << endl ;
        token.SetTokenType( tokenType, tokenValue ) ;  // set tokenValue Type
        tokenValue = "" ;
        Factor( fCorrect_2, fValue_2 ) ;  // go to <Factor> and set correct
        if ( fCorrect_2 ) {
          correct = true ;
          if ( tokenType == MUL )
            fValue_1 = fValue_1 * fValue_2 ;
          else if ( tokenType == DIV ) {
            if ( fValue_2 != 0 )
              fValue_1 = fValue_1 / fValue_2 ;
            else
              throw 4 ;
          }  // else if ()

          value = fValue_1 ;
          // cout << fValue_2 << endl ;
          // cout << value << "MULLLLLLL" << endl ;
        }  // if ()

        else
          throw 2 ;
        tokenValue = "" ;
      }  // while ()

    }  // if ()

  }  // NOT_ID_StartTerm()

  void NOT_ID_StartFactor( bool &correct, float &value ) {
    // cout << "<NOT_ID_StartFactor>" << endl ;  // testing grammar
    string tokenValue = "" ;
    Token token ;
    float aValue = 0.0 ;
    bool aCorrect = false ;

    if ( token.GetToken( mSIGNType, tokenValue ) ) {  // [SIGN]
      // cout << tokenValue << " is a SIGN in <NOT_ID_StartFactor>." << endl ;
      string signStr = tokenValue ;  // find sign
      if ( token.GetToken( mNUMType, tokenValue ) ) {  // NUM
        // cout << tokenValue << " is a NUM in <NOT_ID_StartFactor>." << endl ;
        tokenValue = signStr + tokenValue ;  // sign + number
        value = ( float ) atof( tokenValue.c_str() ) ;  // string to int
        correct = true ;
      }  // if ()

      else
        throw 2 ;
    }  // if ()

    else if ( token.GetToken( mNUMType, tokenValue ) ) {  // NUM
      // cout << tokenValue << " is a NUM in <NOT_ID_StartFactor>." << endl ;
      value = ( float ) atof( tokenValue.c_str() ) ;  // string to int
      correct = true ;
    }  // else if ()

    else if ( token.GetToken( mLPARENType, tokenValue ) ) {  // '(' <ArithExp> ')' 
      // cout << tokenValue << " is a LEFT_PAREN in <NOT_ID_StartFactor>." << endl ;
      ArithExp( aCorrect, aValue ) ;  // go to <ArithExp>
      if ( aCorrect && token.GetToken( mRPARENType, tokenValue ) ) {  // find ')'
        // cout << tokenValue << " is a RIGHT_PAREN in <NOT_ID_StartFactor>." << endl ;
        value = aValue ;
        correct = true ;
      }  // if ()

      else
        throw 2 ;

    }  // else if ()
    
    else
      throw 2 ;

  }  // NOT_ID_StartFactor()

  void ArithExp( bool &correct, float &value ) {
    // cout << "<ArithExp>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tokenType = 0 ;
    Token token ;

    bool tCorrect_1 = false ;
    bool tCorrect_2 = false ;
    float tValue_1 = 0.0 ;
    float tValue_2 = 0.0 ;

    Term( tCorrect_1, tValue_1 ) ;
    if ( tCorrect_1 ) {
      value = tValue_1 ;
      correct = true ;
      while ( token.GetToken( mADDORSUBType, tokenValue ) ) {  // case ArithExpOrBexp
        // cout << tokenValue << " is an ADDORSUB in <ArithExp>." << endl ;
        token.SetTokenType( tokenType, tokenValue ) ;  // set tokenValue Type
        Term( tCorrect_2, tValue_2 ) ;  // go to <Term> and set correct
        if ( tCorrect_2 ) {
          correct = true ;
          if ( tokenType == ADD )
            tValue_1 = tValue_1 + tValue_2 ;
          else if ( tokenType == SUB )
            tValue_1 = tValue_1 - tValue_2 ;
          value = tValue_1 ;
          // cout << value << "do ADDDDD" << endl ;
        }  // if ()

        else
          throw 2 ;

        tokenValue = "" ;
      }  // while ()

    }  // if ()

    else {
      value = 0.0 ;
      correct = true ;
    }  // else

  }  // ArithExp()

  void Term( bool &correct, float &value ) {
    // cout << "<Term>" << endl ;  // testing grammar
    string tokenValue = "" ;
    int tokenType = 0 ;
    Token token ;

    bool fCorrect_1 = false ;
    bool fCorrect_2 = false ;
    float fValue_1 = 0.0 ;
    float fValue_2 = 0.0 ;

    Factor( fCorrect_1, fValue_1 ) ;
    if ( fCorrect_1 ) {
      value = fValue_1 ;
      correct = true ;
      while ( token.GetToken( mMULORDIVType, tokenValue ) ) {  // case ArithExpOrBexp
        // cout << tokenValue << " is an MULORDIV in <Term>." << endl ;
        token.SetTokenType( tokenType, tokenValue ) ;  // set tokenValue Type
        Factor( fCorrect_2, fValue_2 ) ;  // go to <Factor> and set correct
        if ( fCorrect_2 ) {
          correct = true ;
          if ( tokenType == MUL )
            fValue_1 = fValue_1 * fValue_2 ;
          else if ( tokenType == DIV ) {
            if ( fValue_2 != 0 )
              fValue_1 = fValue_1 / fValue_2 ;
            else
              throw 4 ;
          }  // else if ()

          value = fValue_1 ;
          // cout << value << "do MULLL" << endl ;
        }  // if ()

        else
          throw 2 ;
        tokenValue = "" ;
      }  // while ()

    }  // if ()

    else {
      value = 0.0 ;
      correct = true ;
    }  // else

  }  // Term()

  void Factor( bool &correct, float &value ) {
    // cout << "<Factor>" << endl ;  // testing grammar
    string tokenValue = "" ;
    Token token ;
    float aValue = 0.0 ;
    float idValue = 0.0 ;
    bool aCorrect = false ;

    if ( token.GetToken( mID2Type, tokenValue ) ) {  // IDENT2
      // cout << tokenValue << " is an INENT in <Factor>." << endl ;
      IdTable id ;
      IsDefined( tokenValue ) ;       // check if the id has been defined
      GetIdValue( id, tokenValue ) ;  // get the certian id value
      value = id.idValue ;
      if ( id.isFloat )
        gDotFound = true ;
      // cout << value << "get value" << endl ;
      correct = true ;
    }  // if ()

    else if ( token.GetToken( mSIGNType, tokenValue ) ) {
      // cout << tokenValue << " is a SIGN in <Factor>." << endl ;
      string signStr = tokenValue ;  // find sign
      if ( token.GetToken( mNUMType, tokenValue ) ) {  // [SIGN] NUM
        // cout << tokenValue << " is a NUM in <Factor>." << endl ;
        tokenValue = signStr + tokenValue ;  // sign + number
        value = ( float ) atof( tokenValue.c_str() ) ;  // string to int
        correct = true ;
      }  // else if ()

      else
        throw 2 ;
    }  // else if ()

    else if ( token.GetToken( mNUMType, tokenValue ) ) {  // NUM
      // cout << tokenValue << " is a NUM in <Factor>." << endl ;
      value = ( float ) atof( tokenValue.c_str() ) ;  // string to int
      correct = true ;
    }  // else if ()

    else if ( token.GetToken( mLPARENType, tokenValue ) ) {  // '(' <ArithExp> ')' 
      // cout << tokenValue << " is a LEFT_PAREN in <Factor>." << endl ;
      ArithExp( aCorrect, aValue ) ;  // go to <ArithExp>
      if ( aCorrect && token.GetToken( mRPARENType, tokenValue ) ) {  // find ')'
        // cout << tokenValue << " is a RIGHT_PAREN in <Factor>." << endl ;
        value = aValue ;
        correct = true ;
      } // if ()

      else
        throw 2 ;

    }  // else if ()

    else
      throw 2 ;

  }  // Factor()

};

int main() {
  cout << "Program starts..." << endl ;
  cin >> uTestNum ; // PAL has a test number in the benining of each test.
  // cin.get() ;       // read the '\n'
  // if ( uTestNum == 2 ) cout << "FCUKYOU" << endl ;
  int i = 0 ;
  while ( !gIsQuit && i < 100 ) {  // keep geting input until "quit"
    bool correct = false ;
    gIsComment = false ;
    gIsCmdBegin = true ;
    gDotFound = false ;
    gIsUnRecon = false ;
    gIsDIV = false ;
    gCurTokenName = "" ;
    i++;
    try {
      Pharsing pharsing ;
      pharsing.Init() ;
      pharsing.Command( correct ) ;
      if ( gIsQuit )
        cout << "> Program exits..." << endl ;  // QUIT command found    
    }
    catch ( int errorNum ) {
      if ( errorNum == ERROR_ONE ) {
        char errorChar = getchar() ;
        cout << "> Unrecognized token with first char : " << "'" << errorChar << "'" << endl;
      }  // if ()

      else if ( errorNum == SP_ERROR_ONE )
        cout << "> Unrecognized token with first char : '.'" << endl;
      else if ( errorNum == SSP_ERROR_ONE )
        cout << "> Unrecognized token with first char : ':'" << endl;
      else if ( errorNum == ERROR_TWO ) {
        bool dotFound = false ;
        bool numComplete = false ;
        string errorString = "" ;
        char errorChar = getchar() ;
        char peekChar = cin.peek() ;
        errorString = errorString + errorChar ;

        while ( errorChar == '.' && isdigit( peekChar ) ) {
          errorString = errorString + peekChar ;
          char trash = getchar() ;
          peekChar = cin.peek() ;
        }  // while ()

        while ( !numComplete && isdigit( errorChar ) && ( isdigit( peekChar ) || peekChar == '.' ) ) {
          if ( peekChar == '.' && dotFound )
            numComplete = true ;
          else if ( peekChar == '.' ) {
            dotFound = true ;
            errorString = errorString + peekChar ;
            char trash = getchar() ;
            peekChar = cin.peek() ;
          }  // else if ()

          else {
            errorString = errorString + peekChar ;
            char trash = getchar() ;
            peekChar = cin.peek() ;
          }  // else

        }  // while ()

        if ( isalpha( errorChar ) ) {
          while ( isalpha( peekChar ) || isdigit( peekChar ) || peekChar == '_' ) {
            errorString = errorString + peekChar ;
            char trash = getchar() ;
            peekChar = cin.peek() ;
          }  // while ()

        }  // if ()

        if ( ( errorChar == '>' || errorChar == '<' ) && peekChar == '=' ) {
          errorString = errorString + peekChar ;
          char trash = getchar() ;
          peekChar = cin.peek() ;
        }  // if ()

        if ( errorChar == '<' && peekChar == '>' ) {
          errorString = errorString + peekChar ;
          char trash = getchar() ;
          peekChar = cin.peek() ;
        }  // if ()

        if ( errorString == "_" )
          cout << "> Unrecognized token with first char : '_'" << endl ;
        else
          cout << "> Unexpected token : " << "'" << errorString << "'" << endl;
      }  // else if ()
   
      else if ( errorNum == SP_ERROR_TWO )
        cout << "> Unexpected token : '/'" << endl;
      else if ( errorNum == SSP_ERROR_TWO )
        cout << "> Unexpected token : ':='" << endl;
      else if ( errorNum == SSSP_ERROR_TWO ) {
        string errorString = "." ;
        char peekChar = cin.peek() ;
        while ( isdigit( peekChar ) ) {
          errorString = errorString + peekChar ;
          char trash = getchar() ;
          peekChar = cin.peek() ;
        }  // while ()

        cout << "> Unexpected token : " << "'" << errorString << "'" << endl;
      }  // else if ()

      else if ( errorNum == ERROR_THREE )
        cout << endl;
      else if ( errorNum == ERROR_FOUR )
        cout << "> Error" << endl;
      Token token ;
      token.SkipUntilEnter() ;
    }  // catch

  }  // while ()

}  // main()