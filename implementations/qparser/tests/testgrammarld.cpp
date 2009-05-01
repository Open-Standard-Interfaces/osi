//////////////////////////////////////////////////////////////////////////////
//
//    TESTGRAMMARLD.CPP
//
//    Copyright © 2009, Rehno Lindeque. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
/*                               DOCUMENTATION                              */
/*
    UNIT TEST:
      Test the LD grammar functionality
 */

/*                                 INCLUDES                                 */
/*// OpenParser
#include <osix/parser/parser.hpp>
#ifdef _DEBUG
  #include <osix/parser/parserdbg.hpp>
#endif

// BaseParser
#include <baseparser/baseparser.h>

// STL
//#include <algorithm>

// QParser
#define QPARSER_TEST_GRAMMARLD // Turn on unit testing output in the library        
#include "../src/token.h"
#include "../src/grammar.h"
#include "../src/grammarld.h"*/
#include "../src/api.h"
using namespace QParser;

// QParser unit tests
#include "testcommon.h"

/*                                  ALIASES                                 */
//typedef BuilderLD::ActionRow ActionRow;
//typedef BuilderLD::PivotSet PivotSet;
typedef ParserLD::ParseTokens ParseTokens;

/*                                 TEST DATA                                */
// Lexical tokens to use
ParseToken x = 0, y = 0, z = 0, w = 0;

/*                                   TESTS                                  */
// Build a left-recursive grammar with unbounded look-ahead (found at around page 23 of the draft)
void BuildTestGrammar1(ParserLD& parser, Lexer& lexer, GrammarLD& grammar)
{
  // Add lexical tokens
  x = lexer.CharToken("x", 'x');
  y = lexer.CharToken("y", 'y');
  z = lexer.CharToken("z", 'z');
  w = lexer.CharToken("w", 'w');
  lexer.Build(QParser::Lexer::TOKENTYPE_LEX_WORD);
  
  cout << "Terminals:" << endl;
  cout << "  x = " << x << endl;
  cout << "  y = " << y << endl;
  cout << "  z = " << z << endl;
  cout << "  w = " << w << endl;

  // Add productions
  // 1.A -> x
  grammar.BeginProduction("A");
    grammar.ProductionToken("x");
  grammar.EndProduction();
  
  // 2.B -> x
  grammar.BeginProduction("B");
    grammar.ProductionToken("x");
  grammar.EndProduction();
  
  // 3.C -> y
  grammar.BeginProduction("C");
    grammar.ProductionToken("y");
  grammar.EndProduction();
  
  // 4.D -> AC
  grammar.BeginProduction("D");
    grammar.ProductionToken("A");
    grammar.ProductionToken("C");
  grammar.EndProduction();
  
  // 5.D -> DAC
  grammar.BeginProduction("D");
    grammar.ProductionToken("D");
    grammar.ProductionToken("A");
    grammar.ProductionToken("C");
  grammar.EndProduction();
  
  // 6.E -> BC
  grammar.BeginProduction("E");
    grammar.ProductionToken("B");
    grammar.ProductionToken("C");
  grammar.EndProduction();
  
  // 7.E -> EBC
  grammar.BeginProduction("E");
    grammar.ProductionToken("E");
    grammar.ProductionToken("B");
    grammar.ProductionToken("C");
  grammar.EndProduction();
  
  // 8.S -> Dz
  grammar.BeginProduction("S");
    grammar.ProductionToken("D");
    grammar.ProductionToken("z");
  grammar.EndProduction();
  
  // 9.S -> Ew
  grammar.BeginProduction("S");
    grammar.ProductionToken("E");
    grammar.ProductionToken("w");
  grammar.EndProduction();
  
  /* 
  // s(x), r(i), s(y), r(3), r(i), p{x > 1, z > 2, w > 3}, g{3 > 5}, rp(4), rp(1), r(8), accept
  ActionRow& row0 = builder.AddActionRow();
  row0.AddActionShift(x);
  row0.AddActionReduce(TOKEN_SPECIAL_IGNORE);
  row0.AddActionShift(y);
  row0.AddActionReduce(rule3);
  row0.AddActionReduce(TOKEN_SPECIAL_IGNORE);
  PivotSet& pivot0 = row0.AddActionPivot();
  row0.AddActionGoto(3, 5);
  row0.AddActionReducePrev(rule4);
  row0.AddActionReducePrev(rule1);
  row0.AddActionReduce(rule8);
  row0.AddActionAccept();

  // r(i), s(y), r(3), r(i), p{x > 1, z > 2, w > 3}, g{3 > 4}, rp(5), rp(1), return
  ActionRow& row1 = pivot0.AddPivot(x);
  row1.AddActionReduce(TOKEN_SPECIAL_IGNORE);
  row1.AddActionShift(y);
  row1.AddActionReduce(rule3);
  row1.AddActionReduce(TOKEN_SPECIAL_IGNORE);
  PivotSet& pivot1 = row1.AddActionPivot();
  pivot1.AddPivot(x, 1);
  pivot1.AddPivot(z, 2);
  pivot1.AddPivot(w, 3);
  row1.AddActionGoto(3, 4);
  row1.AddActionReducePrev(rule5);
  row1.AddActionReducePrev(rule1);
  row1.AddActionReturn();

  // return
  ActionRow& row2 = pivot0.AddPivot(z);
  row2.AddActionReturn();
  
  // return
  ActionRow& row3 = pivot0.AddPivot(w);
  row3.AddActionReturn();
  
  // rp(7), rp(2), return
  ActionRow& row4 = builder.AddActionRow();
  row4.AddActionReducePrev(rule7);
  row4.AddActionReducePrev(rule2);
  row4.AddActionReturn();
  
  // rp(6), rp(2), r(9), accept
  ActionRow& row5 = builder.AddActionRow();
  row5.AddActionReducePrev(rule6);
  row5.AddActionReducePrev(rule2);
  row5.AddActionReduce(rule9);
  row5.AddActionAccept();*/
}

void PackParseResult(ParseResult& result, ParseToken* streamBegin, ParseToken* streamEnd)
{
  // Allocate the lex stream
  delete[] result.lexStream.data;
  result.lexStream.length = streamEnd-streamBegin;
  result.lexStream.elementSize = sizeof(ParseMatch);
  result.lexStream.data = new ParseMatch[result.lexStream.length];
  
  // Assign each match a token and an offset and length of 0.
  for(uint c = 0; c < result.lexStream.length; ++c)
  {
    result.lexStream.data[c].token = streamBegin[c];
    result.lexStream.data[c].offset = 0;
    result.lexStream.data[c].length = 0;
  }
}

void PrintRules(const ParseTokens& rules)
{
  cout << "Rules: ";
  for(ParseTokens::const_iterator i = rules.begin(); i != rules.end(); ++i)
    if (*i == TOKEN_SPECIAL_IGNORE)
      cout << "ignore ";
    else
      cout << *i << ' ';
  cout << endl;
}

class TestParserLD : public ParserLD
{
public:
  BuilderLD& TEST_GetBuilder() { return builder; }
  void TEST_RecognitionPass(ParseResult& parseResult, ParseTokens& rules) { RecognitionPass(parseResult, rules); }
};

bool TestGrammar1()
{
  TestParserLD parser;
  
  //// Build the parse table
  GrammarLD grammar(parser.GetTokenRegistry());
  Lexer lexer(parser.GetTokenRegistry());
  BuildTestGrammar1(parser, lexer, grammar);
  parser.ConstructParser(&grammar);
  
  // Print out the parse table
  
  
  //// Construct some test input (lexical) streams along with their expected results (rules)  
  // Stream 1: xyxyxyz (Correct input)
  ParseToken lexStream1[] = { x,y,x,y,x,y,z };
  ParseToken correctOutput1[] = { 0,2,3,0,2,4,0,2,4,7 };
  
  // Stream 2: xyxyxyz (Correct input)
  ParseToken lexStream2[] = { x,y,x,y,x,y,w };
  ParseToken correctOutput2[] = { 1,2,5,1,2,6,1,2,6,8 };
  
  // Stream 3: xyz (Correct input)
  ParseToken lexStream3[] = { x,y,w };
  ParseToken correctOutput3[] = { 1,2,5,8 };
    
  //// Test the recognition pass
  ParseResult parseResult;  // The parse result
  ParseTokens rules;        // Rules output by the recognition pass
  
  PackParseResult(parseResult, lexStream1, lexStream1 + sizeof(lexStream1)/sizeof(ParseToken));
  /*parser.TEST_RecognitionPass(parseResult, rules);
  PrintRules(rules);
  for(uint c = 0; c < rules.size(); ++c)
  {
    if(rules[c] != correctOutput1[c])
    {
      cout << "Error: rule does not match the expected outcome" << endl;  
      return false;
    }
  }*/
  
  /*PackParseResult(parseResult, lexStream2, lexStream2 + sizeof(lexStream2)/sizeof(ParseToken));
  parser.TEST_RecognitionPass(parseResult, rules);
  PrintRules(rules);
  for(uint c = 0; c < rules.size(); ++c)
  {
    if(rules[c] != correctOutput2[c])
    {
      cout << "Error: rule does not match the expected outcome" << endl;  
      return false;
    }
  }
  
  PackParseResult(parseResult, lexStream3, lexStream3 + sizeof(lexStream3)/sizeof(ParseToken));
  parser.TEST_RecognitionPass(parseResult, rules);
  PrintRules(rules);
  for(uint c = 0; c < rules.size(); ++c)
  {
    if(rules[c] != correctOutput3[c])
    {
      cout << "Error: rule does not match the expected outcome" << endl;  
      return false;
    }
  }*/
    
  return true;
}

/*                                ENTRY POINT                               */
int main()
{
  cout << "-----------------------------------" << endl
       << "Testing GrammarLD: " << endl;
  cout.flush();
  if (TestGrammar1())  
  {
    cout << "SUCCESS" << endl;
    cout.flush();
    return 0;
  }
  else
  {
    return 1; // Test case failed
  }
}
