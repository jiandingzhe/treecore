/*
   ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

   ==============================================================================
 */

#include "treecore/Expression.h"

#include "treecore/RefCountObject.h"
#include "treecore/Logger.h"
#include "treecore/ScopedPointer.h"
#include "treecore/StringRef.h"

namespace treecore {

class Expression::Term: public RefCountObject
{
public:
    Term() {}
    virtual ~Term() {}

    virtual Type getType() const noexcept = 0;
    virtual Term* clone() const = 0;
    virtual Term* resolve( const Scope&, int recursionDepth ) = 0;
    virtual String toString() const = 0;
    virtual double toDouble() const { return 0; }
    virtual int getInputIndexFor( const Term* ) const { return -1; }
    virtual int getOperatorPrecedence() const { return 0; }
    virtual int getNumInputs() const { return 0; }
    virtual Term* getInput( int ) const { return nullptr; }
    virtual Term* negated();

    virtual Term* createTermToEvaluateInput( const Scope&, const Term* /*inputTerm*/,
                                             double /*overallTarget*/, Term* /*topLevelTerm*/ ) const
    {
        treecore_assert_false;
        return RefCountHolder<Term>();
    }

    virtual String getName() const
    {
        treecore_assert_false; // You shouldn't call this for an expression that's not actually a function!
        return String();
    }

    virtual void renameSymbol( const Symbol& oldSymbol, const String& newName, const Scope& scope, int recursionDepth )
    {
        for (int i = getNumInputs(); --i >= 0; )
            getInput( i )->renameSymbol( oldSymbol, newName, scope, recursionDepth );
    }

    class SymbolVisitor
    {
public:
        virtual ~SymbolVisitor() {}
        virtual void useSymbol( const Symbol& ) = 0;
    };

    virtual void visitAllSymbols( SymbolVisitor& visitor, const Scope& scope, int recursionDepth )
    {
        for (int i = getNumInputs(); --i >= 0; )
            getInput( i )->visitAllSymbols( visitor, scope, recursionDepth );
    }

private:
    TREECORE_DECLARE_NON_COPYABLE( Term )
};

//==============================================================================
struct Expression::Helpers
{
    typedef RefCountHolder<Term> TermPtr;

    static void checkRecursionDepth( const int depth )
    {
        if (depth > 256)
            throw EvaluationError( "Recursive symbol references" );
    }

    friend class Expression::Term;

    //==============================================================================
    /** An exception that can be thrown by Expression::evaluate(). */
    class EvaluationError: public std::exception
    {
public:
        EvaluationError ( const String& desc ): description( desc )
        {
            TREECORE_DBG( "Expression::EvaluationError: " + description );
        }

        String description;
    };

    //==============================================================================
    class Constant: public Term
    {
public:
        Constant ( const double val, const bool resolutionTarget )
            : value( val ), isResolutionTarget( resolutionTarget ) {}

        Type getType() const noexcept                { return constantType; }
        Term* clone() const { return new Constant( value, isResolutionTarget ); }
        Term* resolve( const Scope&, int )          { return this; }
        double toDouble() const { return value; }
        Term* negated()                            { return new Constant( -value, isResolutionTarget ); }

        String toString() const
        {
            String s( value );
            if (isResolutionTarget)
                s = "@" + s;

            return s;
        }

        double value;
        bool isResolutionTarget;
    };

    //==============================================================================
    class BinaryTerm: public Term
    {
public:
        BinaryTerm ( Term* const l, Term* const r ): left( l ), right( r )
        {
            treecore_assert( l != nullptr && r != nullptr );
        }

        int getInputIndexFor( const Term* possibleInput ) const
        {
            return possibleInput == left ? 0 : (possibleInput == right ? 1 : -1);
        }

        Type getType() const noexcept       { return operatorType; }
        int getNumInputs() const { return 2; }
        Term* getInput( int index ) const { return index == 0 ? left.get() : (index == 1 ? right.get() : 0); }

        virtual double performFunction( double left, double right ) const = 0;
        virtual void writeOperator( String& dest ) const = 0;

        Term* resolve( const Scope& scope, int recursionDepth )
        {
            return new Constant( performFunction( left->resolve( scope, recursionDepth )->toDouble(),
                                                  right->resolve( scope, recursionDepth )->toDouble() ), false );
        }

        String toString() const
        {
            String s;

            const int ourPrecendence = getOperatorPrecedence();
            if (left->getOperatorPrecedence() > ourPrecendence)
                s << '(' << left->toString() << ')';
            else
                s = left->toString();

            writeOperator( s );

            if (right->getOperatorPrecedence() >= ourPrecendence)
                s << '(' << right->toString() << ')';
            else
                s << right->toString();

            return s;
        }

protected:
        const TermPtr left, right;

        Term* createDestinationTerm( const Scope& scope, const Term* input, double overallTarget, Term* topLevelTerm ) const
        {
            treecore_assert( input == left || input == right );
            if (input != left && input != right)
                return TermPtr();

            if ( const Term* const dest = findDestinationFor( topLevelTerm, this ) )
                return dest->createTermToEvaluateInput( scope, this, overallTarget, topLevelTerm );

            return new Constant( overallTarget, false );
        }
    };

    //==============================================================================
    class SymbolTerm: public Term
    {
public:
        explicit SymbolTerm ( const String& sym ): symbol( sym ) {}

        Term* resolve( const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );
            return scope.getSymbolValue( symbol ).term->resolve( scope, recursionDepth + 1 );
        }

        Type getType() const noexcept   { return symbolType; }
        Term* clone() const { return new SymbolTerm( symbol ); }
        String toString() const { return symbol; }
        String getName() const { return symbol; }

        void visitAllSymbols( SymbolVisitor& visitor, const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );
            visitor.useSymbol( Symbol( scope.getScopeUID(), symbol ) );
            scope.getSymbolValue( symbol ).term->visitAllSymbols( visitor, scope, recursionDepth + 1 );
        }

        void renameSymbol( const Symbol& oldSymbol, const String& newName, const Scope& scope, int /*recursionDepth*/ )
        {
            if (oldSymbol.symbolName == symbol && scope.getScopeUID() == oldSymbol.scopeUID)
                symbol = newName;
        }

        String symbol;
    };

    //==============================================================================
    class Function: public Term
    {
public:
        explicit Function ( const String& name ): functionName( name ) {}

        Function ( const String& name, const Array<Expression>& params )
            : functionName( name ), parameters( params )
        {}

        Type getType() const noexcept   { return functionType; }
        Term* clone() const { return new Function( functionName, parameters ); }
        int getNumInputs() const { return parameters.size(); }
        Term* getInput( int i ) const { return parameters[i].term.get(); }
        String getName() const { return functionName; }

        Term* resolve( const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );
            double result = 0;
            const int numParams = parameters.size();
            if (numParams > 0)
            {
                HeapBlock<double> params( (size_t) numParams );
                for (int i = 0; i < numParams; ++i)
                    params[i] = parameters[i].term->resolve( scope, recursionDepth + 1 )->toDouble();

                result = scope.evaluateFunction( functionName, params, numParams );
            }
            else
            {
                result = scope.evaluateFunction( functionName, nullptr, 0 );
            }

            return new Constant( result, false );
        }

        int getInputIndexFor( const Term* possibleInput ) const
        {
            for (int i = 0; i < parameters.size(); ++i)
                if (parameters[i].term == possibleInput)
                    return i;

            return -1;
        }

        String toString() const
        {
            if (parameters.size() == 0)
                return functionName + "()";

            String s( functionName + " (" );

            for (int i = 0; i < parameters.size(); ++i)
            {
                s << parameters[i].term->toString();

                if (i < parameters.size() - 1)
                    s << ", ";
            }

            s << ')';
            return s;
        }

        const String functionName;
        Array<Expression> parameters;
    };

    //==============================================================================
    class DotOperator: public BinaryTerm
    {
public:
        DotOperator ( SymbolTerm* const l, Term* const r ): BinaryTerm( l, r ) {}

        Term* resolve( const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );

            EvaluationVisitor visitor( right, recursionDepth + 1 );
            scope.visitRelativeScope( getSymbol()->symbol, visitor );
            return visitor.output;
        }

        Term* clone() const { return new DotOperator( getSymbol(), right.get() ); }
        String getName() const { return "."; }
        int getOperatorPrecedence() const { return 1; }
        void writeOperator( String& dest ) const { dest << '.'; }
        double performFunction( double, double ) const { return 0.0; }

        void visitAllSymbols( SymbolVisitor& visitor, const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );
            visitor.useSymbol( Symbol( scope.getScopeUID(), getSymbol()->symbol ) );

            SymbolVisitingVisitor v( right, visitor, recursionDepth + 1 );

            try
            {
                scope.visitRelativeScope( getSymbol()->symbol, v );
            }
            catch(...) {}
        }

        void renameSymbol( const Symbol& oldSymbol, const String& newName, const Scope& scope, int recursionDepth )
        {
            checkRecursionDepth( recursionDepth );
            getSymbol()->renameSymbol( oldSymbol, newName, scope, recursionDepth );

            SymbolRenamingVisitor visitor( right, oldSymbol, newName, recursionDepth + 1 );

            try
            {
                scope.visitRelativeScope( getSymbol()->symbol, visitor );
            }
            catch(...) {}
        }

private:
        //==============================================================================
        class EvaluationVisitor: public Scope::Visitor
        {
public:
            EvaluationVisitor ( const TermPtr& t, const int recursion )
                : input( t ), output( t ), recursionCount( recursion ) {}

            void visit( const Scope& scope )   { output = input->resolve( scope, recursionCount ); }

            const TermPtr input;
            TermPtr output;
            const int recursionCount;

private:
            TREECORE_DECLARE_NON_COPYABLE( EvaluationVisitor )
        };

        class SymbolVisitingVisitor: public Scope::Visitor
        {
public:
            SymbolVisitingVisitor ( const TermPtr& t, SymbolVisitor& v, const int recursion )
                : input( t ), visitor( v ), recursionCount( recursion ) {}

            void visit( const Scope& scope )   { input->visitAllSymbols( visitor, scope, recursionCount ); }

private:
            const TermPtr input;
            SymbolVisitor& visitor;
            const int recursionCount;

            TREECORE_DECLARE_NON_COPYABLE( SymbolVisitingVisitor )
        };

        class SymbolRenamingVisitor: public Scope::Visitor
        {
public:
            SymbolRenamingVisitor ( const TermPtr& t, const Expression::Symbol& symbol_, const String& newName_, const int recursionCount_ )
                : input( t ), symbol( symbol_ ), newName( newName_ ), recursionCount( recursionCount_ )  {}

            void visit( const Scope& scope )   { input->renameSymbol( symbol, newName, scope, recursionCount ); }

private:
            const TermPtr input;
            const Symbol& symbol;
            const String newName;
            const int recursionCount;

            TREECORE_DECLARE_NON_COPYABLE( SymbolRenamingVisitor )
        };

        SymbolTerm* getSymbol() const { return static_cast<SymbolTerm*>( left.get() ); }

        TREECORE_DECLARE_NON_COPYABLE( DotOperator )
    };

    //==============================================================================
    class Negate: public Term
    {
public:
        explicit Negate ( const TermPtr& t ): input( t )
        {
            treecore_assert( t != nullptr );
        }

        Type getType() const noexcept                           { return operatorType; }
        int getInputIndexFor( const Term* possibleInput ) const { return possibleInput == input ? 0 : -1; }
        int getNumInputs() const { return 1; }
        Term* getInput( int index ) const { return index == 0 ? input.get() : nullptr; }
        Term* clone() const { return new Negate( input->clone() ); }

        Term* resolve( const Scope& scope, int recursionDepth )
        {
            return new Constant( -input->resolve( scope, recursionDepth )->toDouble(), false );
        }

        String getName() const { return "-"; }
        Term* negated()               { return input; }

        Term* createTermToEvaluateInput( const Scope& scope, const Term* t, double overallTarget, Term* topLevelTerm ) const
        {
            (void) t;
            treecore_assert( t == input );

            const Term* const dest = findDestinationFor( topLevelTerm, this );

            return new Negate( dest == nullptr ? new Constant( overallTarget, false )
                               : dest->createTermToEvaluateInput( scope, this, overallTarget, topLevelTerm ) );
        }

        String toString() const
        {
            if (input->getOperatorPrecedence() > 0)
                return "-(" + input->toString() + ")";

            return "-" + input->toString();
        }

private:
        const TermPtr input;
    };

    //==============================================================================
    class Add: public BinaryTerm
    {
public:
        Add ( Term* const l, Term* const r ): BinaryTerm( l, r ) {}

        Term* clone() const { return new Add( left->clone(), right->clone() ); }
        double performFunction( double lhs, double rhs ) const { return lhs + rhs; }
        int getOperatorPrecedence() const { return 3; }
        String getName() const { return "+"; }
        void writeOperator( String& dest ) const { dest << " + "; }

        Term* createTermToEvaluateInput( const Scope& scope, const Term* input, double overallTarget, Term* topLevelTerm ) const
        {
            const TermPtr newDest( createDestinationTerm( scope, input, overallTarget, topLevelTerm ) );
            if (newDest == nullptr)
                return TermPtr();

            return new Subtract( newDest.get(), (input == left ? right : left)->clone() );
        }

private:
        TREECORE_DECLARE_NON_COPYABLE( Add )
    };

    //==============================================================================
    class Subtract: public BinaryTerm
    {
public:
        Subtract ( Term* const l, Term* const r ): BinaryTerm( l, r ) {}

        Term* clone() const { return new Subtract( left->clone(), right->clone() ); }
        double performFunction( double lhs, double rhs ) const { return lhs - rhs; }
        int getOperatorPrecedence() const { return 3; }
        String getName() const { return "-"; }
        void writeOperator( String& dest ) const { dest << " - "; }

        Term* createTermToEvaluateInput( const Scope& scope, const Term* input, double overallTarget, Term* topLevelTerm ) const
        {
            const TermPtr newDest( createDestinationTerm( scope, input, overallTarget, topLevelTerm ) );
            if (newDest == nullptr)
                return nullptr;

            if (input == left)
                return new Add( newDest.get(), right->clone() );

            return new Subtract( left->clone(), newDest.get() );
        }

private:
        TREECORE_DECLARE_NON_COPYABLE( Subtract )
    };

    //==============================================================================
    class Multiply: public BinaryTerm
    {
public:
        Multiply ( Term* const l, Term* const r ): BinaryTerm( l, r ) {}

        Term* clone() const { return new Multiply( left->clone(), right->clone() ); }
        double performFunction( double lhs, double rhs ) const { return lhs * rhs; }
        String getName() const { return "*"; }
        void writeOperator( String& dest ) const { dest << " * "; }
        int getOperatorPrecedence() const { return 2; }

        Term* createTermToEvaluateInput( const Scope& scope, const Term* input, double overallTarget, Term* topLevelTerm ) const
        {
            const TermPtr newDest( createDestinationTerm( scope, input, overallTarget, topLevelTerm ) );
            if (newDest == nullptr)
                return nullptr;

            return new Divide( newDest.get(), (input == left ? right : left)->clone() );
        }

private:
        TREECORE_DECLARE_NON_COPYABLE( Multiply )
    };

    //==============================================================================
    class Divide: public BinaryTerm
    {
public:
        Divide ( Term* const l, Term* const r ): BinaryTerm( l, r ) {}

        Term* clone() const { return new Divide( left->clone(), right->clone() ); }
        double performFunction( double lhs, double rhs ) const { return lhs / rhs; }
        String getName() const { return "/"; }
        void writeOperator( String& dest ) const { dest << " / "; }
        int getOperatorPrecedence() const { return 2; }

        Term* createTermToEvaluateInput( const Scope& scope, const Term* input, double overallTarget, Term* topLevelTerm ) const
        {
            const TermPtr newDest( createDestinationTerm( scope, input, overallTarget, topLevelTerm ) );
            if (newDest == nullptr)
                return nullptr;

            if (input == left)
                return new Multiply( newDest.get(), right->clone() );

            return new Divide( left->clone(), newDest.get() );
        }

private:
        TREECORE_DECLARE_NON_COPYABLE( Divide )
    };

    //==============================================================================
    static Term* findDestinationFor( Term* const topLevel, const Term* const inputTerm )
    {
        const int inputIndex = topLevel->getInputIndexFor( inputTerm );
        if (inputIndex >= 0)
            return topLevel;

        for (int i = topLevel->getNumInputs(); --i >= 0; )
        {
            Term* const t = findDestinationFor( topLevel->getInput( i ), inputTerm );

            if (t != nullptr)
                return t;
        }

        return nullptr;
    }

    static Constant* findTermToAdjust( Term* const term, const bool mustBeFlagged )
    {
        treecore_assert( term != nullptr );

        if (term->getType() == constantType)
        {
            Constant* const c = static_cast<Constant*>(term);
            if (c->isResolutionTarget || !mustBeFlagged)
                return c;
        }

        if (term->getType() == functionType)
            return nullptr;

        const int numIns = term->getNumInputs();

        for (int i = 0; i < numIns; ++i)
        {
            Term* const input = term->getInput( i );

            if (input->getType() == constantType)
            {
                Constant* const c = static_cast<Constant*>(input);

                if (c->isResolutionTarget || !mustBeFlagged)
                    return c;
            }
        }

        for (int i = 0; i < numIns; ++i)
        {
            Constant* const c = findTermToAdjust( term->getInput( i ), mustBeFlagged );
            if (c != nullptr)
                return c;
        }

        return nullptr;
    }

    static bool containsAnySymbols( const Term* const t )
    {
        if (t->getType() == Expression::symbolType)
            return true;

        for (int i = t->getNumInputs(); --i >= 0; )
            if ( containsAnySymbols( t->getInput( i ) ) )
                return true;

        return false;
    }

    //==============================================================================
    class SymbolCheckVisitor: public Term::SymbolVisitor
    {
public:
        SymbolCheckVisitor ( const Symbol& symbol_ ): wasFound( false ), symbol( symbol_ ) {}
        void useSymbol( const Symbol& s )    { wasFound = wasFound || s == symbol; }

        bool wasFound;

private:
        const Symbol& symbol;

        TREECORE_DECLARE_NON_COPYABLE( SymbolCheckVisitor )
    };

    //==============================================================================
    class SymbolListVisitor: public Term::SymbolVisitor
    {
public:
        SymbolListVisitor ( Array<Symbol>& list_ ): list( list_ ) {}
        void useSymbol( const Symbol& s )    { list.addIfNotAlreadyThere( s ); }

private:
        Array<Symbol>& list;

        TREECORE_DECLARE_NON_COPYABLE( SymbolListVisitor )
    };

    //==============================================================================
    class Parser
    {
public:
        //==============================================================================
        Parser ( String::CharPointerType& stringToParse )
            : text( stringToParse )
        {}

        TermPtr readUpToComma()
        {
            if ( text.isEmpty() )
                return new Constant( 0.0, false );

            TermPtr e = readExpression();

            if ( e == nullptr || ( ( !readOperator( "," ) ) && !text.isEmpty() ) )
            {
                throw ParseError( "Syntax error: \"" + String( text ) + "\"" );
            }

            return e;
        }

private:
        String::CharPointerType& text;

        //==============================================================================
        static inline bool isDecimalDigit( const treecore_wchar c ) noexcept
        {
            return c >= '0' && c <= '9';
        }

        bool readChar( const treecore_wchar required ) noexcept
        {
            if (*text == required)
            {
                ++text;
                return true;
            }

            return false;
        }

        bool readOperator( const char* ops, char* const opType = nullptr ) noexcept
        {
            text = text.findEndOfWhitespace();

            while (*ops != 0)
            {
                if ( readChar( (treecore_wchar) (uint8) * ops ) )
                {
                    if (opType != nullptr)
                        *opType = *ops;

                    return true;
                }

                ++ops;
            }

            return false;
        }

        bool readIdentifier( String& identifier ) noexcept
        {
            text = text.findEndOfWhitespace();
            String::CharPointerType t( text );
            int numChars = 0;

            if (t.isLetter() || *t == '_')
            {
                ++t;
                ++numChars;

                while (t.isLetterOrDigit() || *t == '_')
                {
                    ++t;
                    ++numChars;
                }
            }

            if (numChars > 0)
            {
                identifier = String( text, (size_t) numChars );
                text = t;
                return true;
            }

            return false;
        }

        Term* readNumber() noexcept
        {
            text = text.findEndOfWhitespace();
            String::CharPointerType t( text );

            const bool isResolutionTarget = (*t == '@');
            if (isResolutionTarget)
            {
                ++t;
                t    = t.findEndOfWhitespace();
                text = t;
            }

            if (*t == '-')
            {
                ++t;
                t = t.findEndOfWhitespace();
            }

            if ( isDecimalDigit( *t ) || ( *t == '.' && isDecimalDigit( t[1] ) ) )
                return new Constant( CharacterFunctions::readDoubleValue( text ), isResolutionTarget );

            return nullptr;
        }

        TermPtr readExpression()
        {
            TermPtr lhs( readMultiplyOrDivideExpression() );

            char opType;
            while ( lhs != nullptr && readOperator( "+-", &opType ) )
            {
                TermPtr rhs( readMultiplyOrDivideExpression() );

                if (rhs == nullptr)
                    throw ParseError( "Expected expression after \"" + String::charToString( (treecore_wchar) (uint8) opType ) + "\"" );

                if (opType == '+')
                    lhs = new Add( lhs.get(), rhs.get() );
                else
                    lhs = new Subtract( lhs.get(), rhs.get() );
            }

            return lhs;
        }

        TermPtr readMultiplyOrDivideExpression()
        {
            TermPtr lhs( readUnaryExpression() );

            char opType;
            while ( lhs != nullptr && readOperator( "*/", &opType ) )
            {
                TermPtr rhs( readUnaryExpression() );

                if (rhs == nullptr)
                    throw ParseError( "Expected expression after \"" + String::charToString( (treecore_wchar) (uint8) opType ) + "\"" );

                if (opType == '*')
                    lhs = new Multiply( lhs.get(), rhs.get() );
                else
                    lhs = new Divide( lhs.get(), rhs.get() );
            }

            return lhs;
        }

        TermPtr readUnaryExpression()
        {
            char opType;
            if ( readOperator( "+-", &opType ) )
            {
                TermPtr e( readUnaryExpression() );

                if (e == nullptr)
                    throw ParseError( "Expected expression after \"" + String::charToString( (treecore_wchar) (uint8) opType ) + "\"" );

                if (opType == '-')
                    e = e->negated();

                return e;
            }

            return readPrimaryExpression();
        }

        TermPtr readPrimaryExpression()
        {
            TermPtr e( readParenthesisedExpression() );
            if (e != nullptr)
                return e;

            e = readNumber();
            if (e != nullptr)
                return e;

            return readSymbolOrFunction();
        }

        TermPtr readSymbolOrFunction()
        {
            String identifier;
            if ( readIdentifier( identifier ) )
            {
                if ( readOperator( "(" ) ) // method call...
                {
                    Function* const f = new Function( identifier );
                    ScopedPointer<Term> func( f );  // (can't use ScopedPointer<Function> in MSVC)

                    TermPtr param( readExpression() );

                    if (param == nullptr)
                    {
                        if ( readOperator( ")" ) )
                            return func.release();

                        throw ParseError( "Expected parameters after \"" + identifier + " (\"" );
                    }

                    f->parameters.add( Expression( param.get() ) );

                    while ( readOperator( "," ) )
                    {
                        param = readExpression();

                        if (param == nullptr)
                            throw ParseError( "Expected expression after \",\"" );

                        f->parameters.add( Expression( param.get() ) );
                    }

                    if ( readOperator( ")" ) )
                        return func.release();

                    throw ParseError( "Expected \")\"" );
                }

                if ( readOperator( "." ) )
                {
                    TermPtr rhs( readSymbolOrFunction() );

                    if (rhs == nullptr)
                        throw ParseError( "Expected symbol or function after \".\"" );

                    if (identifier == "this")
                        return rhs;

                    return new DotOperator( new SymbolTerm( identifier ), rhs.get() );
                }

                // just a symbol..
                treecore_assert( identifier.trim() == identifier );
                return new SymbolTerm( identifier );
            }

            return TermPtr();
        }

        TermPtr readParenthesisedExpression()
        {
            if ( !readOperator( "(" ) )
                return TermPtr();

            const TermPtr e( readExpression() );
            if ( e == nullptr || !readOperator( ")" ) )
                return TermPtr();

            return e;
        }

        TREECORE_DECLARE_NON_COPYABLE( Parser )
    };
};

//==============================================================================
Expression::Expression()
    : term( new Expression::Helpers::Constant( 0, false ) )
{}

Expression::~Expression()
{}

Expression::Expression ( Term* const term_ )
    : term( term_ )
{
    treecore_assert( term != nullptr );
}

Expression::Expression ( const double constant )
    : term( new Expression::Helpers::Constant( constant, false ) )
{}

Expression::Expression ( const Expression& other )
    : term( other.term )
{}

Expression& Expression::operator = ( const Expression& other )
{
    term = other.term;
    return *this;
}

Expression::Expression ( Expression&& other ) noexcept
    : term( static_cast<RefCountHolder<Term>&&>(other.term) )
{}

Expression& Expression::operator = ( Expression&& other ) noexcept
{
    term = static_cast<RefCountHolder<Term>&&>(other.term);
    return *this;
}

Expression::Expression ( const String& stringToParse )
{
    String::CharPointerType text( stringToParse.getCharPointer() );
    Helpers::Parser parser( text );
    term = parser.readUpToComma();
}

Expression Expression::parse( String::CharPointerType& stringToParse )
{
    Helpers::Parser parser( stringToParse );
    return Expression( parser.readUpToComma().get() );
}

double Expression::evaluate() const
{
    return evaluate( Expression::Scope() );
}

double Expression::evaluate( const Expression::Scope& scope ) const
{
    try
    {
        return term->resolve( scope, 0 )->toDouble();
    }
    catch(Helpers::EvaluationError&)
    {}

    return 0;
}

double Expression::evaluate( const Scope& scope, String& evaluationError ) const
{
    try
    {
        return term->resolve( scope, 0 )->toDouble();
    }
    catch(Helpers::EvaluationError& e)
    {
        evaluationError = e.description;
    }

    return 0;
}

Expression Expression::operator + ( const Expression& other ) const { return Expression( new Helpers::Add( term, other.term ) ); }
Expression Expression::operator - ( const Expression& other ) const { return Expression( new Helpers::Subtract( term, other.term ) ); }
Expression Expression::operator * ( const Expression& other ) const { return Expression( new Helpers::Multiply( term, other.term ) ); }
Expression Expression::operator / ( const Expression& other ) const { return Expression( new Helpers::Divide( term, other.term ) ); }
Expression Expression::operator - () const { return Expression( term->negated() ); }
Expression Expression::symbol( const String& symbol )              { return Expression( new Helpers::SymbolTerm( symbol ) ); }

Expression Expression::function( const String& functionName, const Array<Expression>& parameters )
{
    return Expression( new Helpers::Function( functionName, parameters ) );
}

Expression Expression::adjustedToGiveNewResult( const double targetValue, const Expression::Scope& scope ) const
{
    ScopedPointer<Term> newTerm( term->clone() );

    Helpers::Constant* termToAdjust = Helpers::findTermToAdjust( newTerm, true );

    if (termToAdjust == nullptr)
        termToAdjust = Helpers::findTermToAdjust( newTerm, false );

    if (termToAdjust == nullptr)
    {
        newTerm = new Helpers::Add( newTerm.release(), new Helpers::Constant( 0, false ) );
        termToAdjust = Helpers::findTermToAdjust( newTerm, false );
    }

    treecore_assert( termToAdjust != nullptr );

    const Term* const parent = Helpers::findDestinationFor( newTerm, termToAdjust );

    if (parent == nullptr)
    {
        termToAdjust->value = targetValue;
    }
    else
    {
        const Helpers::TermPtr reverseTerm( parent->createTermToEvaluateInput( scope, termToAdjust, targetValue, newTerm ) );

        if (reverseTerm == nullptr)
            return Expression( targetValue );

        termToAdjust->value = reverseTerm->resolve( scope, 0 )->toDouble();
    }

    return Expression( newTerm.release() );
}

Expression Expression::withRenamedSymbol( const Expression::Symbol& oldSymbol, const String& newName, const Scope& scope ) const
{
    treecore_assert( newName.toLowerCase().containsOnly( "abcdefghijklmnopqrstuvwxyz0123456789_" ) );

    if (oldSymbol.symbolName == newName)
        return *this;

    Expression e( term->clone() );
    e.term->renameSymbol( oldSymbol, newName, scope, 0 );
    return e;
}

bool Expression::referencesSymbol( const Expression::Symbol& symbolToCheck, const Scope& scope ) const
{
    Helpers::SymbolCheckVisitor visitor( symbolToCheck );

    try
    {
        term->visitAllSymbols( visitor, scope, 0 );
    }
    catch(Helpers::EvaluationError&)
    {}

    return visitor.wasFound;
}

void Expression::findReferencedSymbols( Array<Symbol>& results, const Scope& scope ) const
{
    try
    {
        Helpers::SymbolListVisitor visitor( results );
        term->visitAllSymbols( visitor, scope, 0 );
    }
    catch(Helpers::EvaluationError&)
    {}
}

String Expression::toString() const { return term->toString(); }
bool Expression::usesAnySymbols() const { return Helpers::containsAnySymbols( term ); }
Expression::Type Expression::getType() const noexcept   { return term->getType(); }
String Expression::getSymbolOrFunction() const { return term->getName(); }
int Expression::getNumInputs() const { return term->getNumInputs(); }
Expression Expression::getInput( int index ) const { return Expression( term->getInput( index ) ); }

//==============================================================================
Expression::Term* Expression::Term::negated()
{
    return new Helpers::Negate( this );
}

//==============================================================================
Expression::ParseError::ParseError ( const String& message )
    : description( message )
{
    TREECORE_DBG( "Expression::ParseError: " + message );
}

//==============================================================================
Expression::Symbol::Symbol ( const String& scopeUID_, const String& symbolName_ )
    : scopeUID( scopeUID_ ), symbolName( symbolName_ )
{}

bool Expression::Symbol::operator == ( const Symbol& other ) const noexcept
{
    return symbolName == other.symbolName && scopeUID == other.scopeUID;
}

bool Expression::Symbol::operator != ( const Symbol& other ) const noexcept
{
    return !operator == ( other );
}

//==============================================================================
Expression::Scope::Scope()  {}
Expression::Scope::~Scope() {}

Expression Expression::Scope::getSymbolValue( const String& symbol ) const
{
    if ( symbol.isNotEmpty() )
        throw Helpers::EvaluationError( "Unknown symbol: " + symbol );

    return Expression();
}

double Expression::Scope::evaluateFunction( const String& functionName, const double* parameters, int numParams ) const
{
    if (numParams > 0)
    {
        if (functionName == "min")
        {
            double v = parameters[0];
            for (int i = 1; i < numParams; ++i)
                v = jmin( v, parameters[i] );

            return v;
        }

        if (functionName == "max")
        {
            double v = parameters[0];
            for (int i = 1; i < numParams; ++i)
                v = jmax( v, parameters[i] );

            return v;
        }

        if (numParams == 1)
        {
            if (functionName == "sin")  return sin( parameters[0] );
            if (functionName == "cos")  return cos( parameters[0] );
            if (functionName == "tan")  return tan( parameters[0] );
            if (functionName == "abs")  return std::abs( parameters[0] );
        }
    }

    throw Helpers::EvaluationError( "Unknown function: \"" + functionName + "\"" );
}

void Expression::Scope::visitRelativeScope( const String& scopeName, Visitor& ) const
{
    throw Helpers::EvaluationError( "Unknown symbol: " + scopeName );
}

String Expression::Scope::getScopeUID() const
{
    return String();
}

}
