# include <iostream>
# include <vector>
# include <stdio.h>
# include <string>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <exception>
# include <iomanip>

using namespace std;
static int utestnum = 0;
struct Token {
  string type;
  string name;
  float value;
  int line ;
  int column;
};

struct Id {
  string id_type;
  string name;
  int isarray;
  string index;
};

struct Function {
  vector<string> content;
  vector<Id> funcID;
  string name;
  string type;
};

vector<Token> gtoKen;
vector<Token> gBuffer;
vector<Id> gIdtable;
vector<Id> gTempIdtable;
vector<Function> gFunctable;

int gline = 0, gcolumn = 0;
int gisfunction = 0;

void Rest_of_declarators( Token & temp, string type, string id ) ;
void Definition( Token & temp ) ;
void Statement( Token & temp ) ;
void Function_definition_without_ID( Token & temp, string type, string id ) ;
void Function_definition_or_declarators( Token & temp, string type, string id ) ;
void Formal_parameter_list( Token & temp, string type, string id ) ;
void Compound_statement( Token & temp ) ;
void Declaration( Token & temp ) ;
void Expression( Token & temp ) ;
void Basic_expression( Token & temp ) ;
void Rest_of_Identifier_started_basic_exp( Token & temp ) ;
void Rest_of_PPMM_Identifier_started_basic_exp( Token & temp ) ;
void Signed_unary_exp( Token & temp ) ;
void Romce_and_romloe( Token & temp ) ;
void Actual_parameter_list( Token & temp ) ;
void Rest_of_maybe_logical_OR_exp( Token & temp ) ;
void Rest_of_maybe_logical_AND_exp( Token & temp ) ;
void Maybe_logical_AND_exp( Token & temp ) ;
void Maybe_bit_OR_exp( Token & temp ) ;
void Rest_of_maybe_bit_OR_exp( Token & temp ) ;
void Maybe_bit_ex_OR_exp( Token & temp ) ;
void Rest_of_maybe_bit_ex_OR_exp( Token & temp ) ;
void Maybe_bit_AND_exp( Token & temp ) ;
void Rest_of_maybe_bit_AND_exp( Token & temp ) ;
void Maybe_equality_exp( Token & temp ) ;
void Rest_of_maybe_equality_exp( Token & temp ) ;
void Maybe_relational_exp( Token & temp ) ;
void Rest_of_maybe_relational_exp( Token & temp ) ;
void Maybe_shift_exp( Token & temp ) ;
void Rest_of_maybe_shift_exp( Token & temp ) ;
void Maybe_additive_exp( Token & temp ) ;
void Rest_of_maybe_additive_exp( Token & temp ) ;
void Maybe_mult_exp( Token & temp ) ;
void Rest_of_maybe_mult_exp( Token & temp ) ;
void Unary_exp( Token & temp ) ;
void Unsigned_unary_exp( Token & temp ) ;

char Get_the_nextchar() { // get the next char
  char ch = getchar();
  gcolumn++;
  if ( ch == '\n' ) {
    gcolumn = 0;
    gline++;
  } // if

  return ch;
} // Get_the_nextchar()

char Get_the_nextnonwhite() {
  char ch = Get_the_nextchar();
  while ( ch == ' ' || ch == '\n' || ch == '\t' ) ch = Get_the_nextchar();
  char ph = cin.peek();
  while ( ch == '/' && ph == '/' ) {
    if ( ph == '/' )  while ( ch != '\n' ) ch = Get_the_nextchar();
    while ( ch == ' ' || ch == '\n' || ch == '\t' ) ch = Get_the_nextchar();
    char ph = cin.peek();
  } // while

  return ch;
} // Get_the_nextnonwhite()

bool BooleanOprator( string type ) {
  if ( type == "=" || type == "<>" || type == ">"
       || type == "<" || type == ">=" || type == "<=" )
    return true;
  return false;
} // BooleanOprator()

bool IsCase1( char ch ) {
  if ( ch == '*' || ch == '/' || ch == '=' || ch == '!' || ch == '%' ) // 只能接=
    return true;
  return false;
} // IsCase1()

bool IsCase2( char ch ) {   // 只能接自己
  if ( ch == '&' || ch == '|' ) return true;
  return false;
} // IsCase2()

bool IsCase3( char ch ) {
  if ( ch == '>' || ch == '<' || ch == '+' || ch == '-' )  // 都可
    return true;
  return false;
} // IsCase3()

bool Process_table4( char ch, char ph ) { // 可接
  if ( ( IsCase1( ch ) && ph == '=' )  || ( IsCase2( ch ) && ch == ph )
       || ( IsCase3( ch ) && ph == '=' ) || ( IsCase3( ch ) && ph == ch ) )
    return true;
  return false;
} // Process_table4()

bool IsTable4( char ch ) {
  if ( ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == ';'
       || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}'
       || ch == '<' || ch == '>' || ch == '|' || ch == '&' || ch == '=' || ch == ','
       || ch == '?' || ch == '!' || ch == '^' || ch == ':' ) return true;

  return false;
} // IsTable4()

void Error3( string name ) {
  gBuffer.clear();
  name =  " : undefined identifier : '" + name + "'" ;
  throw name;
} // Error3()

void Error2( string name ) {
  gBuffer.clear();

  name = " : unexpected token : '" + name + "'" ;
  throw name;
} // Error2()

void Error1( char ch ) {
  gBuffer.clear();
  string name ;
  name = name + ch;

  name = " : unrecognized token with first char : '" + name + "'" ;
  throw name;
} // Error1()

void Sort_id() {
  for ( int i = gIdtable.size() - 1 ; i > 0 ; --i ) {
    for ( int j = 0 ; j < i ; ++j ) {
      if ( gIdtable[j].name.compare( gIdtable[j+1].name ) > 0 ) {
        Id temp;
        temp.name = gIdtable[j].name;
        temp.id_type = gIdtable[j].id_type;
        temp.isarray = gIdtable[j].isarray;
        temp.index = gIdtable[j].index;
        gIdtable[j].name = gIdtable[j + 1].name;
        gIdtable[j].id_type = gIdtable[j + 1].id_type;
        gIdtable[j].isarray = gIdtable[j + 1].isarray;
        gIdtable[j].index = gIdtable[j + 1].index;
        gIdtable[j + 1].name = temp.name;
        gIdtable[j + 1].id_type = temp.id_type;
        gIdtable[j + 1].isarray = temp.isarray;
        gIdtable[j + 1].index = temp.index;
      } // if
    } // for
  }  // for
} // Sort_id()

void Sort_func() {
  for ( int i = gFunctable.size() - 1 ; i > 0 ; --i ) {
    for ( int j = 0 ; j < i ; ++j ) {
      if ( gFunctable[j].name.compare( gFunctable[j+1].name ) > 0 ) {
        Function temp;
        temp.name = gFunctable[j].name;
        temp.type = gFunctable[j].type;
        temp.funcID = gFunctable[j].funcID;
        temp.content = gFunctable[j].content;
        gFunctable[j].name = gFunctable[j + 1].name;
        gFunctable[j].type = gFunctable[j + 1].type;
        gFunctable[j].funcID = gFunctable[j + 1].funcID;
        gFunctable[j].content = gFunctable[j + 1].content;
        gFunctable[j + 1].name = temp.name;
        gFunctable[j + 1].type = temp.type;
        gFunctable[j + 1].funcID = temp.funcID;
        gFunctable[j + 1].content = temp.content;
      } // if
    } // for
  }  // for
} // Sort_func()

void SaveBuffer( Token temp ) {
  Token gtemp;
  gtemp.type = temp.type;
  gtemp.value = temp.value;
  gtemp.name = temp.name;
  gtemp.line = temp.line;
  gtemp.column = temp.column;
  gBuffer.push_back( gtemp );
} // SaveBuffer()

void SaveToken( string & name, string & type, int & line, int & column, Token & temp ) {
  Token gtemp;
  gtemp.column = column;
  gtemp.line = line;
  gtemp.name = name;
  gtemp.type = type;
  if ( isdigit( name[0] ) || name[0] == '.' ) gtemp.value = atof( name.c_str() );
  gtoKen.push_back( gtemp );
  temp.column = column;
  temp.line = line;
  temp.name = name;
  temp.type = type;
  temp.value =  gtemp.value ;
  type.clear();
  name.clear();
} // SaveToken()

void First_table4( char ch, Token & temp  ) {
  string  name;
  char ph = cin.peek();
  if ( IsTable4( ph ) ) {
    if ( Process_table4( ch, ph ) ) {
      name = name + ch;
      name = name + ph;
      SaveToken( name, name, gline, gcolumn, temp );
      ph = Get_the_nextnonwhite();
    } // if

    else {
      name = name + ch;
      SaveToken( name, name, gline, gcolumn, temp );
    } // else
  } // if

  else {
    name = name + ch;
    SaveToken( name, name, gline, gcolumn, temp );
  } // else
} // First_table4()

void First_num( char ch, Token & temp ) {
  string name;
  int dot = 0;
  if ( ch == '.' ) dot++;
  int line = gline, column = gcolumn;
  name = name + ch;
  char ph = cin.peek();
  string type = "Constant";
  while ( ph == '.' || isdigit( ph ) ) {
    if ( ph == '.' && dot == 1 ) {
      SaveToken( name, type, line, column, temp );
      return ;
    } // if

    if ( ph == '.' ) dot++;
    ch =  Get_the_nextnonwhite();
    name = name + ch;
    ph = cin.peek();
  } // while

  SaveToken( name, type, line, column, temp );
} // First_num()

void First_idt( char ch, Token & temp ) {
  string name, type;
  int line = gline, column = gcolumn;
  name = name + ch;
  char ph = cin.peek();
  while ( isalpha( ph ) || ph == '_' || isdigit( ph ) ) {
    ch =  Get_the_nextnonwhite();
    name = name + ch;
    ph = cin.peek();
  } // while()

  if ( name == "char" ) type = "char";
  else if ( name == "int" ) type = "int";
  else if ( name == "float" ) type = "float";
  else if ( name == "bool" ) type = "bool";
  else if ( name == "string" ) type = "string";
  else if ( name == "void" ) type = "void";
  else if ( name == "if" ) type = "if";
  else if ( name == "else" ) type = "else";
  else if ( name == "while" ) type = "while";
  else if ( name == "do" ) type = "do";
  else if ( name == "return" ) type = "return";
  else if ( name == "true" ) type = "Constant";
  else if ( name == "false" ) type = "Constant";
  else type = "Ident";
  SaveToken( name, type, line, column, temp );
} // First_idt()

void First_str( char ch, Token & temp ) {
  string name, type;
  int line = gline, column = gcolumn;
  name = name + ch;
  char ph = cin.peek();
  while ( ph != '"' ) {
    ch =  Get_the_nextchar();
    name = name + ch;
    ph = cin.peek();
  } // while()

  ch =  Get_the_nextnonwhite();
  name = name + ch;
  type = "Constant";
  SaveToken( name, type, line, column, temp );
} // First_str()

void First_ch( char ch, Token & temp ) {
  string name, type;
  int line = gline, column = gcolumn;
  name = name + ch;
  char ph = cin.peek();
  while ( ph != '\'' ) {
    ch =  Get_the_nextchar();
    name = name + ch;
    ph = cin.peek();
  } // while()

  ch =  Get_the_nextnonwhite();
  name = name + ch;
  type = "Constant";
  SaveToken( name, type, line, column, temp );
} // First_ch()

void Gettoken( Token & temp ) {
  char ch = Get_the_nextnonwhite();
  if ( ch == EOF ) temp.type = "EOF";
  else if ( IsTable4( ch ) ) First_table4( ch, temp ) ;
  else if ( isdigit( ch ) || ch == '.' ) First_num( ch, temp );
  else if ( isalpha( ch ) || ch == '_' ) First_idt( ch, temp );
  else if ( ch == '"' )  First_str( ch, temp );
  else if ( ch == '\'' ) First_ch( ch, temp );
  else Error1( ch );
} // Gettoken()

void Getthetruetoken( Token & temp ) {
  if ( gBuffer.size() == 1 ) {
    temp.type =  gBuffer[0].type;
    temp.value = gBuffer[0].value;
    temp.name = gBuffer[0].name;
    temp.line = gBuffer[0].line;
    temp.column = gBuffer[0].column;
    gBuffer.clear();
  } // if

  else Gettoken( temp );
} // Getthetruetoken()

bool Hasident(  string name ) {
  for ( int i = 0 ; i < gIdtable.size() ; i++ ) {
    if ( gIdtable[i].name == name  ) return true;
  } // for

  return false;
} // Hasident()

bool Hasfunc(  string name ) {
  for ( int i = 0 ; i < gFunctable.size() ; i++ ) {
    if ( gFunctable[i].name == name  ) return true;
  } // for

  return false;
} // Hasfunc()

bool Hasfucident(  string name ) {
  for ( int i = 0 ; i < gTempIdtable.size() ; i++ ) {
    if ( gTempIdtable[i].name == name  ) return true;
  } // for

  return false;
} // Hasfucident()

void Process_idtable() {
  for ( int i = 0 ; i < gTempIdtable.size() ; i++ ) {
    if ( Hasident( gTempIdtable[i].name ) ) {
      if ( i == 0 )
        cout << "> New definition of " << gTempIdtable[i].name << " entered ..." << endl;
      else
        cout << "New definition of " << gTempIdtable[i].name << " entered ..." << endl;
      for ( int j = 0 ; j < gIdtable.size() ; j++ )
        if ( gIdtable[j].name == gTempIdtable[i].name )
          gIdtable.erase( gIdtable.begin() + j );
      Id temp_id;
      temp_id.name = gTempIdtable[i].name;
      temp_id.id_type = gTempIdtable[0].id_type;
      temp_id.isarray = gTempIdtable[i].isarray;
      temp_id.index = gTempIdtable[i].index;
      gIdtable.push_back( temp_id );
    } // if

    else {
      if ( i == 0 )
        cout << "> Definition of " << gTempIdtable[i].name << " entered ..." << endl;
      else
        cout << "Definition of " << gTempIdtable[i].name << " entered ..." << endl;
      Id temp_id;
      temp_id.name = gTempIdtable[i].name;
      temp_id.id_type = gTempIdtable[0].id_type;
      temp_id.isarray = gTempIdtable[i].isarray;
      temp_id.index = gTempIdtable[i].index;
      gIdtable.push_back( temp_id );
    } // else
  } // for

  gTempIdtable.clear();
} // Process_idtable()

void Process_idtable_temp( string type, string id, string index, int isarray ) {
  Id temp_id;
  temp_id.name = id;
  temp_id.id_type = type;
  temp_id.index = index;
  temp_id.isarray = isarray;
  gTempIdtable.push_back( temp_id );
} // Process_idtable_temp()

void Process_functable( string type, string id ) {
  Function func;
  func.funcID =  gTempIdtable;
  func.name = id;
  func.type = type;
  for ( int i = 0; i < gtoKen.size() ; i++ ) {
    string str = gtoKen[i].name;
    func.content.push_back( str );
  } // for

  if ( Hasfunc( id ) ) {
    cout << "> New definition of " << id << "() entered ..." << endl;
    for ( int i = 0 ; i < gFunctable.size() ; i++ ) {
      if ( gFunctable[i].name == id )
        gFunctable.erase( gFunctable.begin() + i );
    } // for
  } // if

  else
    cout << "> Definition of " << id << "() entered ..." << endl;

  gFunctable.push_back( func );
  gTempIdtable.clear();
  Id temp_id;
  temp_id.name = id;
  temp_id.id_type = type;
  if ( Hasident( id ) ) {
    for ( int i = 0 ; i < gIdtable.size() ; i++ ) {
      if ( gIdtable[i].name == id )
        gIdtable.erase( gIdtable.begin() + i );
    } // for
  } // if

  gIdtable.push_back( temp_id );
} // Process_functable()

bool IsDone() {
  if ( gtoKen[0].name == "Done" && gtoKen[1].name == "(" && gtoKen[2].name == ")" && gtoKen[3].name == ";" )
    return true;
  return false;
} // IsDone()

bool IsListAllFunctions() {
  if ( gtoKen[0].name == "ListAllFunctions" && gtoKen[1].name == "("
       && gtoKen[2].name == ")" && gtoKen[3].name == ";" ) {
    Sort_func();
    cout << "> " ;
    for ( int i = 0; i < gFunctable.size() ; i ++ ) {

      cout << gFunctable[i].name << "()" << endl;
    } // for

    return true;
  } // if

  return false;
} // IsListAllFunctions()


bool IsListVariable() {
  if ( gtoKen[0].name == "ListVariable" && gtoKen[1].name == "("
       && gtoKen[2].type == "Constant" && gtoKen[3].name == ")"
       && gtoKen[4].name == ";" ) {
    string constant;
    for ( int i = 1 ; i < gtoKen[2].name.size() -1 ; i ++ )
      constant = constant + gtoKen[2].name[i];
    if ( Hasident( constant ) ) {
      cout << "> " ;
      for ( int i = 0 ; i < gIdtable.size() ; i ++ ) {
        if ( constant == gIdtable[i].name ) {
          cout << gIdtable[i].id_type << ' ' << gIdtable[i].name;
          if ( gIdtable[i].isarray == 1 ) cout << "[ " << gIdtable[i].index << " ] " << ';' << endl;

          else cout << " ;" << endl;
          return true;
        } // if
      } // for
    } // if

    else Error3( constant );
  } // if

  return false;
} // IsListVariable()

void Printfunc( int i ) {
  int space = 0;
  bool isfirst = false;
  for ( int j = 0 ; j < gFunctable[i].content.size() ; j ++ ) {
    if ( gFunctable[i].content[j] == ";" || gFunctable[i].content[j] == "{"
         || gFunctable[i].content[j] == "}"  ) {
      if ( gFunctable[i].content[j] == "{" ) space = space + 2;
      if ( gFunctable[i].content[j] == "}" ) {
        space = space - 2;
        for ( int k = 0 ; k < space ; k++ )
          cout << ' ' ;
      }  // if

      cout << gFunctable[i].content[j] << endl;
      isfirst = true;
    }  // if

    else {
      if ( isfirst ) {
        for ( int k = 0 ; k < space ; k++ )
          cout << ' ' ;
      } // if


      if ( Hasfunc( gFunctable[i].content[j] ) )
        cout << gFunctable[i].content[j] ;
      else if ( ( gFunctable[i].content[j + 1] == "[" || gFunctable[i].content[j + 1] == "++"
                  || gFunctable[i].content[j + 1] == "--" || gFunctable[i].content[j + 1] == "," )
                && ( j + 1 < gFunctable[i].content.size() ) )
        cout << gFunctable[i].content[j] ;
      else
        cout << gFunctable[i].content[j] << ' ' ;
      isfirst = false;
    } // else
  } // for
} // Printfunc()

bool IsListFunction() {
  if ( gtoKen[0].name == "ListFunction" && gtoKen[1].name == "("
       && gtoKen[2].type == "Constant" && gtoKen[3].name == ")"
       && gtoKen[4].name == ";" ) {
    string constant;
    for ( int i = 1 ; i < gtoKen[2].name.size() -1 ; i ++ )
      constant = constant + gtoKen[2].name[i];
    if ( Hasident( constant ) ) {
      cout << "> " ;
      for ( int i = 0 ; i < gFunctable.size() ; i ++ ) {
        if ( constant == gFunctable[i].name ) {
          Printfunc( i );
          return true;
        } // if
      } // for
    } // if

    else Error3( constant );
  } // if

  return false;
} // IsListFunction()

bool IsListAllVariables() {
  if ( gtoKen[0].name == "ListAllVariables" && gtoKen[1].name == "("
       && gtoKen[2].name == ")" && gtoKen[3].name == ";" ) {
    cout << "> " ;
    Sort_id();
    for ( int i = 0; i < gIdtable.size() ; i ++ ) {
      if ( gIdtable[i].name != "cout" && gIdtable[i].name != "cin" && gIdtable[i].name != "ListAllVariables"
           && gIdtable[i].name != "Done" && gIdtable[i].name != "ListAllFunctions"
           && gIdtable[i].name != "ListVariable" && gIdtable[i].name != "ListFunction" )
        cout << gIdtable[i].name << endl;
    } // for

    return true;
  } // if

  return false;
} // IsListAllVariables()

bool Istype_specifier( string type ) {
  if ( type == "int" || type == "void" || type == "char" || type == "bool"
       || type == "string" || type == "float" ) return true;
  return false;
} // Istype_specifier()

void User_input( Token & temp ) {
  cout << " Our-C running ..." << endl;
  int a;
  do {
    try {
      gtoKen.clear();
      Getthetruetoken( temp );
      if ( gline == 0 ) gline = 1;
      SaveBuffer( temp );
      gTempIdtable.clear();
      if ( Istype_specifier( temp.type ) ) Definition( temp );
      else  {
        Statement( temp );
        if ( IsListAllVariables() ) cout << "Statement executed ..." << endl;
        else if ( IsListVariable() ) cout << "Statement executed ..." << endl;
        else if ( IsListAllFunctions() ) cout << "Statement executed ..." << endl;
        else if ( IsListFunction() ) cout << "Statement executed ..." << endl;
        else if ( !IsDone() ) cout << "> Statement executed ..." << endl;
      } // else

      gline = 0;
      gisfunction = 0;
    }
    catch( string e ) {
      cout << '>' << " Line " << gline << e << endl;
      char ch = '\0';
      gline = 0;
      while ( ch != '\n' ) ch = Get_the_nextchar();
      gisfunction = 0;
    } // catch

  } while ( !IsDone() );

  cout << "> Our-C exited ..." << endl;
}  // User_input()

void Definition( Token & temp ) {
  Getthetruetoken( temp );
  string id, type;
  if ( temp.type == "void" ) {
    type = temp.type;
    Getthetruetoken( temp );
    id = temp.name;
    if ( temp.type != "Ident" ) Error2( temp.name );
    Function_definition_without_ID( temp, type, id );
  } // if

  else  {
    type = temp.type;
    Getthetruetoken( temp );
    id = temp.name;
    if ( temp.type == "Ident" ) Function_definition_or_declarators( temp, type, id );
    else Error2( temp.name );
  }  // else
} // Definition()

void Function_definition_or_declarators( Token & temp, string type, string id ) {
  Getthetruetoken( temp );
  SaveBuffer( temp );
  if ( temp.type == "(" ) Function_definition_without_ID( temp, type, id );
  else if ( temp.type == "[" || temp.type == ","
            || temp.type == ";" )  Rest_of_declarators( temp, type, id );
  else Error2( temp.name );
} // Function_definition_or_declarators()

void Rest_of_declarators( Token & temp, string type, string id ) {
  string index;
  int isarray = 0;
  Getthetruetoken( temp );
  if ( temp.type == "[" ) {
    Getthetruetoken( temp );
    if ( temp.type != "Constant" ) Error2( temp.name );
    index = temp.name;
    Getthetruetoken( temp );
    if ( temp.type != "]" ) Error2( temp.name );
    Getthetruetoken( temp );
    isarray = 1;
  }   // if

  Process_idtable_temp( type, id, index, isarray );
  index.clear();
  isarray = 0;

  while ( temp.type == "," ) {
    Getthetruetoken( temp );
    if ( temp.type != "Ident" ) Error2( temp.name );
    id = temp.name;
    Getthetruetoken( temp );
    if ( temp.type == "[" ) {
      Getthetruetoken( temp );
      if ( temp.type != "Constant" ) Error2( temp.name );
      index = temp.name;
      Getthetruetoken( temp );
      if ( temp.type != "]" ) Error2( temp.name );
      Getthetruetoken( temp );
      isarray = 1;
    }   // if

    Process_idtable_temp( type, id, index, isarray );
    index.clear();
    isarray = 0;
  } // while()

  if ( temp.type != ";" ) Error2( temp.name );
  if ( gisfunction == 0 ) Process_idtable();
} // Rest_of_declarators()

void Function_definition_without_ID( Token & temp, string type, string id ) {
  gisfunction = 1;
  Getthetruetoken( temp ); // 抓(
  string type_ = type;
  string id_ = id;
  if ( temp.type == "(" ) {
    Getthetruetoken( temp );
    if ( temp.type == ")" ) {
      Getthetruetoken( temp );
      Compound_statement( temp );
    } // if

    else if ( temp.type == "void" ) {
      Getthetruetoken( temp );
      if ( temp.type != ")" ) Error2( temp.name );
      Getthetruetoken( temp );
      Compound_statement( temp );
    } // if

    else if ( Istype_specifier( temp.type ) ) {
      Formal_parameter_list( temp, type, id );
      Getthetruetoken( temp );
      if ( temp.type != ")" ) Error2( temp.name );
      Getthetruetoken( temp );
      Compound_statement( temp );
    } // else if
  } // if

  else Error2( temp.name );
  Process_functable( type_, id_ );
} // Function_definition_without_ID()

void Formal_parameter_list( Token & temp, string type, string id ) {
  Getthetruetoken( temp );
  string index;
  int isarray = 0;
  if ( temp.type == "&" ) Getthetruetoken( temp );
  if ( temp.type != "Ident" ) Error2( temp.name );
  id = temp.name;
  Getthetruetoken( temp );
  if ( temp.type == "[" ) {
    Getthetruetoken( temp );
    index = temp.name;
    isarray = 1;
    if ( temp.type != "Constant" ) Error2( temp.name );
    Getthetruetoken( temp );
    if ( temp.type != "]" ) Error2( temp.name );
    Getthetruetoken( temp );
  }  // if

  Process_idtable_temp( type, id, index, isarray );
  index.clear();
  isarray = 0;
  while ( temp.type == "," ) {
    Getthetruetoken( temp );
    type = temp.name;
    if ( temp.type == "void" ) Error2( temp.type );
    else if ( Istype_specifier( temp.type ) ) {
      Getthetruetoken( temp );
      if ( temp.type == "&" )  Getthetruetoken( temp );
      if ( temp.type == "Ident" ) {
        id = temp.name;
        Getthetruetoken( temp );
      }  // if

      if ( temp.type == "[" ) {
        Getthetruetoken( temp );
        if ( temp.type != "Constant" ) Error2( temp.name );
        index = temp.type;
        isarray = 1;
        Getthetruetoken( temp );
        if ( temp.type != "]" ) Error2( temp.name );
        Getthetruetoken( temp );
      } // if
    }  // else if

    Process_idtable_temp( type, id, index, isarray );
    index.clear();
    isarray = 0;
  } // while

  SaveBuffer( temp );
} // Formal_parameter_list()

void Compound_statement( Token & temp ) {
  if ( temp.type != "{" ) Error2( temp.name );
  else {
    Getthetruetoken( temp );
    while ( temp.type != "}" ) {
      if ( temp.type == "void" ) Error2( temp.name );
      else if ( Istype_specifier( temp.type ) ) Declaration( temp );
      else {
        SaveBuffer( temp );
        Statement( temp );
      } // else

      Getthetruetoken( temp );
    } // while()
  } // else
} // Compound_statement()

void Declaration( Token & temp ) {
  string type = temp.name;
  Getthetruetoken( temp );
  string id = temp.name;
  if ( temp.type != "Ident" ) Error2( temp.name );
  else  Rest_of_declarators( temp, type, id );
} // Declaration()

void Statement( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == ";" ) return;
  else if ( temp.type == "return" ) {
    Getthetruetoken( temp );
    if ( temp.type == ";" ) return;
    else {
      SaveBuffer( temp );
      Expression( temp );
      Getthetruetoken( temp );
      if ( temp.type != ";" ) Error2( temp.name );
    } // else
  } // else if

  else if ( temp.type == "if" ) {
    Getthetruetoken( temp );
    if ( temp.type != "(" ) Error2( temp.name );
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
    Statement( temp );
    Getthetruetoken( temp );
    if ( temp.type == "else" ) Statement( temp );
    else SaveBuffer( temp );
  } // else if

  else if ( temp.type == "while" ) {
    Getthetruetoken( temp );
    if ( temp.type != "(" ) Error2( temp.name );
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
    Statement( temp );
  } // else if

  else if ( temp.type == "do" ) {
    Statement( temp );
    Getthetruetoken( temp );
    if ( temp.type != "while" ) Error2( temp.name );
    Getthetruetoken( temp );
    if ( temp.type != "(" ) Error2( temp.name );
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
    Getthetruetoken( temp );
    if ( temp.type != ";" ) Error2( temp.name );
  } // else if

  else if ( temp.type == "{" ) Compound_statement( temp );
  else {
    SaveBuffer( temp );
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ";" ) Error2( temp.name );
  } // else

} // Statement()

void Expression( Token & temp ) {
  Basic_expression( temp );
  Getthetruetoken( temp );
  while ( temp.type == "," ) {
    Basic_expression( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Expression()

void Basic_expression( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "Ident" ) {
    if ( gisfunction == 1  ) {
      if ( !Hasfucident( temp.name ) && !Hasident( temp.name ) ) Error3( temp.name );
    } // if

    else {
      if ( !Hasident( temp.name ) )
        Error3( temp.name );
    } // else

    Rest_of_Identifier_started_basic_exp( temp );
  } // if

  else if ( temp.type == "++" || temp.type == "--" ) {
    Getthetruetoken( temp );
    if ( temp.type != "Ident" ) Error2( temp.name );
    if ( gisfunction == 1  ) {
      if ( !Hasfucident( temp.name ) && !Hasident( temp.name ) ) Error3( temp.name );
    } // if

    else {
      if ( !Hasident( temp.name ) ) Error3( temp.name );
    } // else

    Rest_of_PPMM_Identifier_started_basic_exp( temp );
  } // else if
  else if ( temp.type == "+" || temp.type == "-" || temp.type == "!" ) {
    Getthetruetoken( temp );
    while ( temp.type == "+" || temp.type == "-" || temp.type == "!" ) Getthetruetoken( temp );
    SaveBuffer( temp );
    Signed_unary_exp( temp ); // 已經抓好了
    Romce_and_romloe( temp );
  } // else if

  else if ( temp.type == "Constant" ) Romce_and_romloe( temp );
  else if ( temp.type == "(" ) {
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
    Romce_and_romloe( temp );
  } // else if

  else Error2( temp.name );
} // Basic_expression()

void Rest_of_Identifier_started_basic_exp( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "(" ) {
    Getthetruetoken( temp );
    if ( temp.type != ")" ) {
      SaveBuffer( temp );
      Actual_parameter_list( temp );
      Getthetruetoken( temp );
      if ( temp.type != ")" ) Error2( temp.name );
    } // if

    Romce_and_romloe( temp );
  } // if

  else {
    if ( temp.type == "[" ) {
      Expression( temp );
      Getthetruetoken( temp );
      if ( temp.type != "]" ) Error2( temp.name );
      Getthetruetoken( temp );
    } // if

    if ( temp.type == "=" || temp.type == "+=" || temp.type == "-="
         || temp.type == "/=" || temp.type == "*=" || temp.type == "%=" ) Basic_expression( temp );

    else if ( temp.type == "++" || temp.type == "--" ) Romce_and_romloe( temp );
    else {
      SaveBuffer( temp );
      Romce_and_romloe( temp );
    } // else
  } // else
} // Rest_of_Identifier_started_basic_exp()

void Rest_of_PPMM_Identifier_started_basic_exp( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "[" ) {
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != "]" ) Error2( temp.name );
    Getthetruetoken( temp );
  } // if

  SaveBuffer( temp );
  Romce_and_romloe( temp );
} // Rest_of_PPMM_Identifier_started_basic_exp()

void Actual_parameter_list( Token & temp ) {
  Basic_expression( temp );
  Getthetruetoken( temp );
  while ( temp.type == "," ) {
    Basic_expression( temp );
    Getthetruetoken( temp );
  }  // while

  SaveBuffer( temp );
} // Actual_parameter_list()

void Romce_and_romloe( Token & temp ) {
  Rest_of_maybe_logical_OR_exp( temp );
  Getthetruetoken( temp );
  if ( temp.type == "?" ) {
    Basic_expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ":" ) Error2( temp.name );
    Basic_expression( temp );
  } // if

  else SaveBuffer( temp );
} // Romce_and_romloe()

void Rest_of_maybe_logical_OR_exp( Token & temp ) {
  Rest_of_maybe_logical_AND_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "||" ) {
    Maybe_logical_AND_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_logical_OR_exp()

void Maybe_logical_AND_exp( Token & temp ) {
  Maybe_bit_OR_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "&&" ) {
    Maybe_bit_OR_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_logical_AND_exp()

void Rest_of_maybe_logical_AND_exp( Token & temp ) {
  Rest_of_maybe_bit_OR_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "&&" ) {
    Maybe_bit_OR_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_logical_AND_exp()

void Maybe_bit_OR_exp( Token & temp ) {
  Maybe_bit_ex_OR_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "|" ) {
    Maybe_bit_ex_OR_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_bit_OR_exp()

void Rest_of_maybe_bit_OR_exp( Token & temp ) {
  Rest_of_maybe_bit_ex_OR_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "|" ) {
    Maybe_bit_ex_OR_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_bit_OR_exp()

void Maybe_bit_ex_OR_exp( Token & temp ) {
  Maybe_bit_AND_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "^" ) {
    Maybe_bit_AND_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_bit_ex_OR_exp()

void Rest_of_maybe_bit_ex_OR_exp( Token & temp ) {
  Rest_of_maybe_bit_AND_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "^" ) {
    Maybe_bit_AND_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_bit_ex_OR_exp()

void Maybe_bit_AND_exp( Token & temp ) {
  Maybe_equality_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "&" ) {
    Maybe_equality_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_bit_AND_exp()

void Rest_of_maybe_bit_AND_exp( Token & temp ) {
  Rest_of_maybe_equality_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "&" ) {
    Maybe_equality_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_bit_AND_exp()

void Maybe_equality_exp( Token & temp ) {
  Maybe_relational_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "==" || temp.type == "!=" ) {
    Maybe_relational_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_equality_exp()

void Rest_of_maybe_equality_exp( Token & temp ) {
  Rest_of_maybe_relational_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "==" || temp.type == "!=" ) {
    Maybe_relational_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_equality_exp()

void Maybe_relational_exp( Token & temp ) {
  Maybe_shift_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "<" || temp.type == ">" || temp.type == ">=" || temp.type == "<=" ) {
    Maybe_shift_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_relational_exp()

void Rest_of_maybe_relational_exp( Token & temp ) {
  Rest_of_maybe_shift_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "<" || temp.type == ">" || temp.type == ">=" || temp.type == "<=" ) {
    Maybe_shift_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_relational_exp()

void Maybe_shift_exp( Token & temp ) {
  Maybe_additive_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "<<" || temp.type == ">>" ) {
    Maybe_additive_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_shift_exp()

void Rest_of_maybe_shift_exp( Token & temp ) {
  Rest_of_maybe_additive_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "<<" || temp.type == ">>" ) {
    Maybe_additive_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_shift_exp()

void Maybe_additive_exp( Token & temp ) {
  Maybe_mult_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "+" || temp.type == "-" ) {
    Maybe_mult_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Maybe_additive_exp()

void Rest_of_maybe_additive_exp( Token & temp ) {
  Rest_of_maybe_mult_exp( temp );
  Getthetruetoken( temp );
  while ( temp.type == "+" || temp.type == "-" ) {
    Maybe_mult_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_additive_exp()

void Maybe_mult_exp( Token & temp ) {
  Unary_exp( temp );
  Rest_of_maybe_mult_exp( temp );
} // Maybe_mult_exp()

void Rest_of_maybe_mult_exp( Token & temp ) {
  Getthetruetoken( temp );
  while ( temp.type == "*" || temp.type == "/" || temp.type == "%" ) {
    Unary_exp( temp );
    Getthetruetoken( temp );
  } // while

  SaveBuffer( temp );
} // Rest_of_maybe_mult_exp()

void Unary_exp( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "+" || temp.type == "-" || temp.type == "!" ) {
    Getthetruetoken( temp );
    while ( temp.type == "+" || temp.type == "-" || temp.type == "!" )
      Getthetruetoken( temp );
    SaveBuffer( temp );
    Signed_unary_exp( temp );
  }  // if

  else if ( temp.type == "++" || temp.type == "--" ) {
    Getthetruetoken( temp );
    if ( temp.type != "Ident" ) Error2( temp.name );
    if ( gisfunction == 1  ) {
      if ( !Hasfucident( temp.name ) && !Hasident( temp.name ) ) Error3( temp.name );
    } // if

    else {
      if ( !Hasident( temp.name ) ) Error3( temp.name );
    } // else

    Getthetruetoken( temp );
    if ( temp.type == "[" ) {
      Expression( temp );
      Getthetruetoken( temp );
      if ( temp.type != "]" ) Error2( temp .name );
    } // if

    else SaveBuffer( temp );
  } // else if

  else {
    SaveBuffer( temp );
    Unsigned_unary_exp( temp );
  } // else
} // Unary_exp()

void Signed_unary_exp( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "Ident" ) {
    if ( gisfunction == 1  ) {
      if ( !Hasfucident( temp.name ) && !Hasident( temp.name ) ) Error3( temp.name );
    } // if

    else {
      if ( !Hasident( temp.name ) ) Error3( temp.name );
    } // else

    Getthetruetoken( temp );
    if ( temp.type == "(" ) {
      Getthetruetoken( temp );
      if ( temp.type != ")" ) {
        SaveBuffer( temp );
        Actual_parameter_list( temp );
        Getthetruetoken( temp );
        if ( temp.type != ")" ) Error2( temp.name );
      }   // if
    } // if

    else if ( temp.type == "[" ) {
      Expression( temp );
      Getthetruetoken( temp );
      if ( temp.type != "]" ) Error2( temp.name );
    } // else if

    else SaveBuffer( temp );
  } // if

  else if ( temp.type == "Constant" ) return;
  else if ( temp.type == "(" ) {
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
  } // else if

  else Error2( temp.name );
} // Signed_unary_exp()

void Unsigned_unary_exp( Token & temp ) {
  Getthetruetoken( temp );
  if ( temp.type == "Ident" ) {
    if ( gisfunction == 1  ) {
      if ( !Hasfucident( temp.name ) && !Hasident( temp.name ) ) Error3( temp.name );
    } // if

    else {
      if ( !Hasident( temp.name ) ) Error3( temp.name );
    } // else

    Getthetruetoken( temp );
    if ( temp.type == "(" ) {
      Getthetruetoken( temp );
      if ( temp.type != ")" ) {
        SaveBuffer( temp );
        Actual_parameter_list( temp );
        Getthetruetoken( temp );
        if ( temp.type != ")" ) Error2( temp.name );
      }   // if
    } // if

    else if ( temp.type == "[" ) {
      Expression( temp );
      Getthetruetoken( temp );
      if ( temp.type != "]" ) Error2( temp.name );
      Getthetruetoken( temp );
      if ( temp.type == "++" || temp.type == "--" ) return;
      else {
        SaveBuffer( temp );
        return;
      } // else
    } // else if

    else SaveBuffer( temp );
  } // if

  else if ( temp.type == "Constant" ) return;
  else if ( temp.type == "(" ) {
    Expression( temp );
    Getthetruetoken( temp );
    if ( temp.type != ")" ) Error2( temp.name );
  } // else if

  else Error2( temp.name );
} // Unsigned_unary_exp()

int main() {
  Token temp;
  Id id_temp;
  id_temp.name = "cout";
  gIdtable.push_back( id_temp );
  id_temp.name = "Done";
  gIdtable.push_back( id_temp );
  id_temp.name = "cin";
  gIdtable.push_back( id_temp );
  id_temp.name = "ListAllVariables";
  gIdtable.push_back( id_temp );
  id_temp.name = "ListAllFunctions";
  gIdtable.push_back( id_temp );
  id_temp.name = "ListVariable";
  gIdtable.push_back( id_temp );
  id_temp.name = "ListFunction";
  gIdtable.push_back( id_temp );
  int utestnum;
  cin >> utestnum;
  if ( temp.type != "EOF" ) User_input( temp );
} // main()
