#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
using namespace std;
// ignore C++ 11 extension warning
#pragma GCC diagnostic ignored "-Wc++11-extensions"
// Global var

stringstream ss;
stringstream sst;
ifstream in;
ofstream out;
ofstream out2;
string LOCCTR = "INITIAL FLAG"; // if LOCCTR didn't change BY START OPCODE init LOCCTR to 0
string PRONAME;
string STARTADDR;
string PROLENGTH;
string LABEL , OPCODE , OPERAND;
string OBJADDR , ADDR , TS;
char _c[150];
int ct;
map<string , string> optab;
map<string , string> symtab;
map<string , string> objtab;
string TargetCodeName = "TargetCode/";
string ASMListingName = "asm_listing/";


// Basic Function

typedef struct
{
  string ADDR , LABEL , OPCODE , OPERAND , OBJCODE;
}ListOBJ;
vector<ListOBJ>BUFFER;
ListOBJ OBJ; 

void ssclean(stringstream &ss)
{
  ss.str("");
  ss.clear();
}

string hexADDdec(string num1 ,string num2)
{
  sst << std::hex << num1;
  int x , y = atoi(num2.c_str());
  sst >> x;
  ssclean(sst);
  sst << std::hex << (x+y);
  string ans;
  sst >> ans;
  ssclean(sst);
  for(char &c : ans) c = toupper(c);
  return ans;
}

string Format3Byte(string &str)
{
  for(int i = str.size() ; i < 6 ; i++)
    str.insert(str.begin(),'0');
  return str;
}

string hexADDdec(string num1 ,int num2)
{
  sst << std::hex << num1;
  int x , y = num2;
  sst >> x;
  ssclean(sst);
  sst << std::hex << (x+y);
  string ans;
  sst >> ans;
  ssclean(sst);
  for(char &c : ans) c = toupper(c);
  return ans;
}

string hexSUBhex(string num1 , string num2)
{
  int x , y;
  sst << std::hex << num1;
  sst >> x;
  ssclean(sst);
  sst << std::hex << num2;
  sst >> y;
  ssclean(sst);
  sst << std::hex << (x-y);
  string ans;
  sst >> ans;
  ssclean(sst);
  for(auto &c : ans) c = toupper(c);
  return ans;
}

void READOPTABLE()
{
  in.open("interTables/OPTABLE.txt",ios::in);
  ssclean(ss);
  string _INS , _OPCODE;
  while(in.getline(_c,150))
  {
    ss << _c;
    ss >> _INS >> _OPCODE;
    optab[_INS] = _OPCODE;
    ssclean(ss);
  }
  in.close();
}

void PASS1()
{
  string fileDIR = "SourceSIC/";
  int x;
  cout << "input test case : (1) TEST (2) COPY (3) SUM (4) COUNT\n";
  cin >> x;
  switch(x){
    case 1:
      fileDIR.append("COPY_Ver_TextBook.txt");   break;
    case 2:
      fileDIR.append("COPY.txt");   break;
    case 3:
      fileDIR.append("SUM.txt");    break;
    case 4:
      fileDIR.append("COUNT.txt");  break;
    default:
      fileDIR.append("COPY_Ver_TextBook.txt");   break;
  }
  in.open(fileDIR,ios::in);
  out.open("interTables/intermediate.txt",ios::out);
  while(in.getline(_c,150))
  {
    // comment
    if(_c[0] == '.')
      continue;
    ssclean(ss);
    ss << _c;
    ss >> LABEL >> OPCODE;
    if(ss.rdbuf()->in_avail())
      ss >> OPERAND;
    else
    {
      OPERAND = OPCODE;
      OPCODE = LABEL;
      LABEL = "";
    }
    ssclean(ss);

    if(OPERAND[0] == '.') // if misunderstand that comment to OPERAND
    {
      OPERAND = OPCODE;
      OPCODE = LABEL;
      LABEL = "";
    }

    if(OPCODE == "RSUB")
      LABEL = "" , OPERAND = "";

    if(OPCODE == "START")
    {
      STARTADDR = OPERAND;          // save #[OPERAND] as starting address
      LOCCTR = STARTADDR;           // initialize LOCCTR to starting address
      PRONAME = LABEL;
      out << LOCCTR << '\t' << LABEL << '\t' << OPCODE << '\t' << OPERAND << endl; // write line to intermediate file
      continue;                     // read next input line
    }

    if(LOCCTR == "INITIAL FLAG")    // else (no START OPCODE) init LOCCTR to 0
      LOCCTR = "0000";
    
    if(OPCODE != "END")
    {
      if(symtab.find(LABEL) != symtab.end() && LABEL != "") // if found LABEL in symtab
      {
        out << ". error flag !!! redefined the LABEL : " << LABEL << endl; // set error flag 
        continue;
      }
      else
        if(LABEL != "")
        symtab[LABEL] = LOCCTR;     // insert {LABEL,LOCCTR} into SYMTAB
      
      if(optab.find(OPCODE) != optab.end()) // search OPTABLE for OPCODE and if found
        LOCCTR = hexADDdec(LOCCTR,3);        // add 3 to LOCCTR
      else if(OPCODE == "WORD")
        LOCCTR = hexADDdec(LOCCTR,3);        // add 3 to LOCCTR
      else if(OPCODE == "RESW")
        LOCCTR = hexADDdec(LOCCTR, 3*atoi(OPERAND.c_str())); // add 3 * #[OPERAND] to LOCCTR
      else if(OPCODE == "RESB")
        LOCCTR = hexADDdec(LOCCTR, atoi(OPERAND.c_str()));   // add #[OPERAND] to LOCCTR
      else if(OPCODE == "BYTE")
      {
        if(OPERAND[0] == 'X')
        {
          int i = 0;
          bool flag = false;
          for(char c : OPERAND)     // find length of constant in bytes
          {
            if(flag && c != '\'')
              i++;
            if(c == '\'')
              flag = ~flag;
          }
          LOCCTR = hexADDdec(LOCCTR , i/2); // add length to LOCCTR (two hex number one BYTE)
        }
        
        if(OPERAND[0] == 'C')
        {
          int i = 0;
          bool flag = false;
          for(char c: OPERAND)
          {
            if(flag && c != '\'')
              i++;
            if(c == '\'')
              flag = ~flag;
          }
          LOCCTR = hexADDdec(LOCCTR,i);  // add length to LOCCTR (one char one BYTE)
        }
      }else{  // if neither found in OPTABLE nor been assembler directives
        out << ". error flag !!! (invalid operation code) : " << OPCODE << endl; // set error flag
        continue;
      }
    }
    
    // write line to intermediate file
    if(LABEL == "" && OPCODE != "END")
      out << LOCCTR << "\t\t\t" << OPCODE << '\t'<< OPERAND << endl;
    else if(LABEL != "" && OPCODE != "END")
      out << LOCCTR << '\t' << LABEL << '\t' << OPCODE << '\t' << OPERAND << endl;
    
    if(OPCODE == "END")
      out << "\t\t" << OPCODE << '\t' << OPERAND << endl;

  }
  PROLENGTH = hexSUBhex(LOCCTR , STARTADDR);      // save (LOCCTR - STARTADDR) as PROLENGTH
  out.close();
  in.close();
}

ListOBJ ListOBJCreate()
{
  ListOBJ ans;
  ans.LABEL = LABEL;
  ans.ADDR = ADDR;
  ans.OPERAND = OPERAND;
  ans.OPCODE = OPCODE;
  ans.OBJCODE = TS;
  return ans;
}

bool BUFFERFULL()
{
  ct = 0;
  for(ListOBJ x : BUFFER)
    ct += x.OBJCODE.size();
  return ct >= 60;
}

void WRITENEWTEXTLINE()
{
  if((OPCODE == "RESW" || OPCODE == "RESB") && BUFFER.size() == 0)
    return;
  ListOBJ temp = BUFFER.back();
  int tmp;
  if(ct > 60)
  {
    BUFFER.pop_back();
    tmp = temp.OBJCODE.size();
    ct -= tmp;
  }
  string TextLength = hexADDdec("0",ct/2);
  OBJADDR = "00"; OBJADDR.append(hexSUBhex(BUFFER.front().ADDR,"3"));
  if(TextLength.size() < 2) TextLength.insert(TextLength.begin(),'0');
  out << "T^" << OBJADDR << '^' << (TextLength);
  for(ListOBJ x : BUFFER)
    out << "^" << x.OBJCODE ;
  out << endl;
  BUFFER.clear();
  if( (ct+tmp > 60))
    BUFFER.push_back(temp);
}

string INDEXADDR(string X)
{
  string ans = X.substr(1,X.size()-1);
  int x;
  sst << std::hex << X[0];
  sst >> x;
  ssclean(sst);
  x += 8;
  string temp = hexADDdec("0",x);
  ans.insert(ans.begin(),temp[0]);
  return ans;
}

void PASS2()
{
  TargetCodeName.append(PRONAME);
  ASMListingName.append(PRONAME);
  in.open("interTables/intermediate.txt",ios::in);
  out.open(TargetCodeName,ios::out);
  out2.open(ASMListingName,ios::out);
  ssclean(ss);
  OBJADDR = STARTADDR; // init Text Record
  while(in.getline(_c,150))
  {
    if(_c[0] == '.')  // if is comment
      continue;

    ss << _c;
    ss >> ADDR >> LABEL >> OPCODE;
    if(ss.rdbuf()->in_avail())
      ss >> OPERAND;
    else
    {
      OPERAND = OPCODE;
      OPCODE = LABEL;
      LABEL = "";
    }
    ssclean(ss);
    
    if(OPERAND[0] == '.') // if misunderstand that comment to OPERAND
    {
      OPERAND = OPCODE;
      OPCODE = LABEL;
      LABEL = "";
    }
    // if END
    if(isalpha(ADDR[0]))
      OPERAND = OPCODE , OPCODE = "END" , ADDR = "" , LABEL = "";
    // if OPCODE == RESW || OPCODE == RESB write the new Line
    if(OPCODE == "RESW" || OPCODE == "RESB")
    {
      WRITENEWTEXTLINE();
      out2 << ADDR << right << setw(10) << LABEL << right << setw(10) << OPCODE << setw(10) << OPERAND << endl;
      continue;
    }
      
    if(OPCODE == "RSUB")
      OPERAND = "" , LABEL = "";
    
    // deal with index mode instruction
    if(OPERAND.find(',') != string::npos)
    {
      TS = optab[OPCODE];
      string temp;
      
      for(int i = 0 ; OPERAND[i] != ',' ; i++)
        temp.push_back(OPERAND[i]);
      temp = symtab[temp];
      TS.append(INDEXADDR(temp));
      goto write;
    }

    if(OPCODE == "START")
    {
      out2 << ADDR << right << setw(10) << LABEL << setw(10) << OPCODE << setw(10) << OPERAND << endl;                 // write the listing line
      out << "H^" << left << setw(6) << PRONAME << "^" << Format3Byte(STARTADDR) << "^" << Format3Byte(PROLENGTH) << endl;  // write Header first Text record
      continue;
    }
    
    if(OPCODE != "END")
    {
      if(optab.find(OPCODE) != optab.end())       // search OPTABLE for OPCODE
      {
        string TEMP;
        if(symtab.find(OPERAND) != symtab.end())  // search SYMTAB for OPERAND
        {
          TEMP = symtab[OPERAND]; // store sybol value as operand address
        }else
        {
          TEMP = "0000";          // store 0 as OPERAND address
        } // end if symbol
        TS = optab[OPCODE];
        TS.append(TEMP);
      }
      else if(OPCODE == "BYTE")   // if OPCODE == BYTE convert constant to obj code
      {
        TS.clear();
        bool flag = false;
        for(char c : OPERAND)
        {
          if(flag && c != '\'')
            TS.insert(TS.end(),c);
          if(c == '\'')
            flag = ~flag;
        }
        if(OPERAND[0] == 'C')
        {
          string trans;
          for(char c : TS) trans.append(hexADDdec("0",(int)c));
          TS = trans;
        }
        
      }
      else if(OPCODE == "WORD")     // if OPCODE == WORD convert constant to obj code
      {
        TS = hexADDdec("0",OPERAND);
        Format3Byte(TS);
      }

      write:                        // PUSH OBJ into BUFFER
      OBJ = ListOBJCreate();
      BUFFER.push_back(OBJ);
      if(BUFFERFULL())              // if BUFFER is full write new line
        WRITENEWTEXTLINE();
      out2 << OBJ.ADDR << right << setw(10) << OBJ.LABEL << setw(10) << OBJ.OPCODE << setw(10) << OBJ.OPERAND << setw(10) << OBJ.OBJCODE << endl;
    }else if(OPCODE == "END")      // END
    {
      if(!BUFFER.empty())           // write last Text record to obj program
        WRITENEWTEXTLINE();
      out << "E^" << Format3Byte(STARTADDR) << endl;  // write END record to obj program
      out2 << "  " << right << setw(11) << " " << setw(10) << OPCODE << setw(11) << OPERAND << endl;
    }

  }
  in.close();
  out.close();
  out2.close();
}

void WRITESYMTABLES()
{
  out.open("interTables/symtab.txt",ios::out);
  for(auto x : symtab)
    out << x.first << '\t' << x.second << endl;
  out.close();
}

void OUTPUT()
{
  cout << "---" << setw(3) << "" << "ASM Listing"  << setw(5) << "---" << endl;
  in.open(ASMListingName,ios::in);
  while (in.getline(_c , 150))
  {
    cout << _c << endl;
  }
  in.close();
  cout << "---" << setw(3) << ""  << "Target Code"  << setw(5) << "---" << endl;
  in.open(TargetCodeName,ios::in);
  while (in.getline(_c , 150))
  {
    cout << _c << endl;
  }
  in.close(); 
}

int main()
{
  READOPTABLE();
  PASS1();
  PASS2();
  WRITESYMTABLES();
  OUTPUT();
}
