/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprNode.h"
#include "SeExprFunc.h"
#include "TypeBuilder.h"
#include "TypeTester.h"
#include "TypeIterator.h"

/**
   @file TypeTester.cpp
*/

void
TypeTesterExpr::test(const std::string & expr,
                     SeExprType expected_result,
                     SeExprType actual_result,
                     const std::string & givenString,
                     int verbosity_level) {
    bool       error      = false;
    bool       parseError = false;
    SeExprType returned   = SeExprType::ErrorType_varying();

    setExpr(expr);
    setReturnType(expected_result);

    if(verbosity_level >= 3)
        std::cout << "\tChecking expression:\t" << expr                     << std::endl
                  << "\t\tGiven:        "       << givenString              << std::endl
                  << "\t\tAgainst type: "       << actual_result.toString() << " ";

    if(isValid()) {
        if(actual_result != returnType())
            error = true;
    } else {
        parseError = true;
        if(actual_result.isValid())
            error = true;
    }

    if(!parseError) returned = returnType();

    if(error) {
        std::cerr << "Failed check." << std::endl;
        if     (verbosity_level == 1)
            std::cerr << "\t\tExpression: "  << expr                     << std::endl
                      << "\t\tExpected:   "  << actual_result.toString() << std::endl
                      << "\t\tReceived:   "  << returned     .toString() << std::endl;
        else if(verbosity_level == 2)
            std::cerr << "\t\tExpression: "  << expr                     << std::endl
                      << "\t\tGiven:      "  << givenString              << std::endl
                      << "\t\tExpected:   "  << actual_result.toString() << std::endl
                      << "\t\tReceived:   "  << returned     .toString() << std::endl;
        else if(verbosity_level >= 3)
            std::cerr << "\t\tExpected:   "  << actual_result.toString() << std::endl
                      << "\t\tReceived:   "  << returned     .toString() << std::endl;
    }
    else if(verbosity_level >= 3)
        std::cout << "Check passed!" << std::endl;
};

SeExprType
identity(const SeExprType & type)
{
    return type;
};

SeExprType
numeric(const SeExprType & type)
{
    if(type.isUnderNumeric()) return type;
    else                      return SeExprType::ErrorType_varying();
};

SeExprType
numericToScalar(const SeExprType & type)
{
    if(type.isUnderNumeric()) { SeExprType t = SeExprType::FP1Type_varying(); t.becomeLifetime(type); return t; }
    else                      return SeExprType::ErrorType_varying();
};

SeExprType
numericToScalar(const SeExprType & first,
                const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric())   { SeExprType t = SeExprType::FP1Type_varying(); t.becomeLifetime(first, second); return t; }
    else                          return SeExprType::ErrorType_varying();
};

SeExprType
generalComparison(const SeExprType & first,
                  const SeExprType & second)
{
    if(first .isUnderValue() &&
       second.isUnderValue() &&
       first.isa(second))       { SeExprType t = SeExprType::FP1Type_varying(); t.becomeLifetime(first, second); return t; }
    else                        return SeExprType::ErrorType_varying();
};

SeExprType
numericComparison(const SeExprType & first,
                  const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric() &&
       first.isa(second))         { SeExprType t = SeExprType::FP1Type_varying(); t.becomeLifetime(first, second); return t; }
    else                          return SeExprType::ErrorType_varying();
};

SeExprType
numericToNumeric(const SeExprType & first,
                 const SeExprType & second)
{
    //if first and second are both numeric and first 'isa' second:
    //   both first and second are FP1,
    //   first is FP1 and second is FPN,
    //   first is FPN and second is FP1, or
    //   both first and second are FPN (and the same N)
    if(first .isUnderNumeric() &&
       second.isUnderNumeric() &&
       first.isa(second))         { SeExprType t = SeExprType::FPNType_varying(first.isFP1() ?
                                                                               second.dim()  :
                                                                               first .dim()   ); t.becomeLifetime(first, second); return t; }
    else                          return SeExprType::ErrorType_varying();
};

SeExprType
numericTo2Vector(const SeExprType & first,
                 const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric())   { SeExprType t = SeExprType::FPNType_varying(2); t.becomeLifetime(first, second); return t; }
    else                          return SeExprType::ErrorType_varying();
};

SeExprType
numericTo3Vector(const SeExprType & first,
                 const SeExprType & second,
                 const SeExprType & third)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric() &&
       third .isUnderNumeric())   { SeExprType t = SeExprType::FPNType_varying(3); t.becomeLifetime(first, second, third); return t; }
    else                          return SeExprType::ErrorType_varying();
};

SeExprType
conditional(const SeExprType & first,
            const SeExprType & second,
            const SeExprType & third)
{
    if(first .isUnderNumeric() &&
       second.isUnderValue  () &&
       third .isUnderValue  () &&
       second == third)           { SeExprType t = SeExprType(second); t.becomeLifetime(first, second, third); return t; }
    else                          return SeExprType::ErrorType_varying();
};

void
TypeTesterExpr::testSingle(const std::string & expr, SingleWholeTypeIterator::ProcType proc, int verbosity_level) {
    SeExprType result;
    SingleWholeTypeIterator iter("v", proc, this);

    if(verbosity_level >= 1)
        std::cout << "Checking expression: " << expr << std::endl;

    int remaining = iter.start();
    test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);

    while(remaining) {
        remaining = iter.next();
        test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);
    };
};

void
TypeTesterExpr::testDouble(const std::string & expr, DoubleWholeTypeIterator::ProcType proc, int verbosity_level) {
    SeExprType result;
    DoubleWholeTypeIterator iter("x", "y", proc, this);

    if(verbosity_level >= 1)
        std::cout << "Checking expression: " << expr << std::endl;

    int remaining = iter.start();
    test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);

    while(remaining) {
        remaining = iter.next();
        test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);
    };
};

void
TypeTesterExpr::testTriple(const std::string & expr, TripleWholeTypeIterator::ProcType proc, int verbosity_level) {
    SeExprType result;
    TripleWholeTypeIterator iter("x", "y", "z", proc, this);

    if(verbosity_level >= 1)
        std::cout << "Checking expression: " << expr << std::endl;

    int remaining = iter.start();
    test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);

    while(remaining) {
        remaining = iter.next();
        test(expr, iter.result(), iter.result(), iter.givenString(), verbosity_level);
    };
};

void
testOne(const std::string & str, TypeTesterExpr & expr, SingleWholeTypeIterator::ProcType proc, int verbosity_level) {
    expr.testSingle(str, proc, verbosity_level);
};

void
testTwo(const std::string & str, TypeTesterExpr & expr, DoubleWholeTypeIterator::ProcType proc, int verbosity_level) {
    expr.testDouble(str, proc, verbosity_level);
};

void
testThree(const std::string & str, TypeTesterExpr & expr, TripleWholeTypeIterator::ProcType proc, int verbosity_level) {
    expr.testTriple(str, proc, verbosity_level);
};

int main(int argc,char *argv[])
{
    bool           end = false;
    int            verbosity_level = 0;
    TypeTesterExpr expr;
    std::string    str;
    bool           givenTest       = false;

    for(int i = 0; i < argc && !end; i++) {
        if(argv[i][0] == '-')
            switch(argv[i][1]) {
            case 'v':
                if(argv[i][2] == '=') {
                    switch(argv[i][3]) {
                    case '0':
                        verbosity_level = 0;
                        break;
                    case '1':
                        verbosity_level = 1;
                        break;
                    case '2':
                        verbosity_level = 2;
                        break;
                    case '3':
                        verbosity_level = 3;
                        break;
                    default:
                        break;
                    };
                };
                break;
            case 't':
                givenTest = true;
                if(argc > i + 1) {
                    str = argv[i + 1];
                    end = true;
                };
                break;
            default:
                break;
            };
    };

    if(givenTest) {
        expr.setExpr(str);
        if(expr.isValid()) {
            if(verbosity_level >= 3)
                std::cout << "Test passed!" << std::endl;
        } else {
            std::cerr << "Test failed!" << std::endl;
            if(verbosity_level >= 2)
                std::cerr << expr.parseError() << std::endl;
        }
    } else {
        testOne("$a = $v; $a", expr, identity,        verbosity_level);
        testOne("[$v]",        expr, numericToScalar, verbosity_level);
        testOne("-$v",         expr, numeric,         verbosity_level);
        testOne("!$v",         expr, numeric,         verbosity_level);
        testOne("~$v",         expr, numeric,         verbosity_level);

        testTwo("$x && $y", expr, numericToScalar,   verbosity_level);
        testTwo("$x || $y", expr, numericToScalar,   verbosity_level);
        testTwo("$x[$y]",   expr, numericToScalar,   verbosity_level);
        testTwo("$x == $y", expr, generalComparison, verbosity_level);
        testTwo("$x != $y", expr, generalComparison, verbosity_level);
        testTwo("$x <  $y", expr, numericComparison, verbosity_level);
        testTwo("$x >  $y", expr, numericComparison, verbosity_level);
        testTwo("$x <= $y", expr, numericComparison, verbosity_level);
        testTwo("$x >= $y", expr, numericComparison, verbosity_level);
        testTwo("$x + $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("$x - $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("$x * $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("$x / $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("$x % $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("$x ^ $y",  expr, numericToNumeric,  verbosity_level);
        testTwo("[$x, $y]", expr, numericTo2Vector,  verbosity_level);

        testThree("[$x, $y, $z]",                             expr, numericTo3Vector,  verbosity_level);
        testThree("$x ? $y : $z",                             expr, conditional,       verbosity_level);
        testThree("if($x) { $a = $y; } else { $a = $z; } $a", expr, conditional,       verbosity_level);

        //function tests:
        if(verbosity_level >= 1)
            std::cout << "Checking function expressions." << std::endl;
        std::string compress = "func(0,0,0)";
        expr.test(compress, SeExprType::FP1Type_constant(),  SeExprType::FP1Type_constant(),  compress, verbosity_level);
        compress = "func(0,0)";
        expr.test(compress, SeExprType::FP1Type_constant(),  SeExprType::ErrorType_varying(), compress, verbosity_level);
        compress = "func(0,0,0,0)";
        expr.test(compress, SeExprType::FP1Type_constant(),  SeExprType::ErrorType_varying(), compress, verbosity_level);
        compress = "compress([1,2],0,0)";
        expr.test(compress, SeExprType::FPNType_constant(2), SeExprType::FPNType_constant(2), compress, verbosity_level);
        compress = "compress(0,[1,2],0)";
        expr.test(compress, SeExprType::FPNType_constant(2), SeExprType::FPNType_constant(2), compress, verbosity_level);
        compress = "compress(0,0,[1,2,3])";
        expr.test(compress, SeExprType::FPNType_constant(3), SeExprType::FPNType_constant(3), compress, verbosity_level);
        compress = "compress(0,[1,2],[3,2,1])";
        expr.test(compress, SeExprType::FPNType_constant(2), SeExprType::ErrorType_varying(), compress, verbosity_level);

        //local function tests:
        if(verbosity_level >= 1)
            std::cout << "Checking local function definitions." << std::endl;
        std::string funcdef = "def foo() { $a = 4; $a } 4";
        expr.test(funcdef, SeExprType::FP1Type_constant(),  SeExprType::FP1Type_constant(),  funcdef, verbosity_level);
    };

    return 0;
}
