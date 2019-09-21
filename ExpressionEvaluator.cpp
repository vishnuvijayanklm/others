#include<iostream>
#include <string>
#include <stdio.h>
#include "Stl.h"
#include <unistd.h>

using namespace std;

enum class OPERATOR_TYPE
{
    UNKNOWN,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR
};

enum class BRACKET_TYPE
{
    UNKNOWN,
    OPEN_BRACKET,
    CLOSE_BRACKET
};

enum class TOKEN_TYPE
{
    UNKNOWN,
    EXPRESSION,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPERATOR
};
class IToken
{
        public:
                IToken()
                {
                }

                ~IToken()
                {
                }

                virtual bool evaluate(bool lhs,bool rhs)
                {
                    return false;
                }

                virtual bool value()
                {
                    return false;
                }

                virtual void print()
                {

                }

                virtual TOKEN_TYPE getTokeType() = 0;

                virtual unsigned int getPresidence()
                {
                    return 0;
                }
};

class Expression : public IToken
{
        const string &mLhs;
        StlMap<string,string> mRhs;
        OPERATOR_TYPE mOpType;
        string mRhsStr; 
    public:

        template<typename T>
        Expression(const string &lhs,string operationType,T&& rhs) : mLhs(lhs)
        {
            this->mOpType = getOperationType(operationType);
            this->parseRhs(rhs);
        }
        
        Expression(const string &lhs,string operationType,initializer_list<string> &rhs) : mLhs(lhs)
        {
            this->mOpType = getOperationType(operationType);
            StlVector<string> rhstemp(rhs);
            this->parseRhs(rhstemp);
        }

        void parseRhs(const string& rhs)
        {
            this->mRhsStr.append("{ ");
            this->mRhs.insert(rhs,rhs);
            this->mRhsStr.append(rhs);
            this->mRhsStr.append(" }");
        }
        
        void parseRhs(StlVector<string>& rhs)
        {
            string value;
            this->mRhsStr.append("{ ");
            rhs.startGet();
            while(rhs.getNextElement(value))
            {
                this->mRhs.insert(value,value);
                this->mRhsStr.append(value + ",");
            }
            rhs.stopGet();
            this->mRhsStr.pop_back();
            this->mRhsStr.append(" }");
        }

        ~Expression()
        {

        }

        OPERATOR_TYPE getOperationType(const string &operationType)
        {
            switch(operationType[0])
            {
                case '='    :   return OPERATOR_TYPE::EQUAL;
                case '!'    :   if(operationType[1] == '=')
                                {
                                    return OPERATOR_TYPE::NOT_EQUAL;
                                }
                                return OPERATOR_TYPE::UNKNOWN;
                case '&'    :   if(operationType[1] == '&')
                                {
                                    return OPERATOR_TYPE::AND;
                                }
                                return OPERATOR_TYPE::UNKNOWN;
                case '|'    :   if(operationType[1] == '|')
                                {
                                    return OPERATOR_TYPE::OR;
                                }
                                return OPERATOR_TYPE::UNKNOWN;
                default     :   return OPERATOR_TYPE::UNKNOWN;
            }
        }

        void print() override
        {
            cout<<this->mLhs<<this->type()<<this->mRhsStr;
        }

        string type()
        {
            switch(this->mOpType)
            {
                case OPERATOR_TYPE::EQUAL       :   return " = ";
                case OPERATOR_TYPE::NOT_EQUAL   :   return " != ";
                default                         :   return " ?  ";
            }
        }
        TOKEN_TYPE getTokeType()
        {
            return TOKEN_TYPE::EXPRESSION;
        }

        bool value() override
        {
            switch(this->mOpType)
            {
                case OPERATOR_TYPE::EQUAL       :   return this->mRhs.find(this->mLhs);
                case OPERATOR_TYPE::NOT_EQUAL   :   return !this->mRhs.find(this->mLhs); 
                default                         :   return false;
            }
        }
};

class Bracket : public IToken
{
                BRACKET_TYPE mBracketType;       
        public:
                Bracket(string bracket)
                {
                    this->mBracketType = this->getBracketType(bracket);
                }

                ~Bracket()
                {

                }

                BRACKET_TYPE getBracketType(const string &bracket)
                {
                    switch(bracket[0])
                    {
                        case '('    :   return BRACKET_TYPE::OPEN_BRACKET;
                        case ')'    :   return BRACKET_TYPE::CLOSE_BRACKET;
                        default     :   return BRACKET_TYPE::UNKNOWN;
                    }
                }

                void print() override
                {
                    if(this->mBracketType == BRACKET_TYPE::OPEN_BRACKET)
                    {
                        cout<<"(";
                    }
                    else
                    {
                        cout<<")";
                    }
                }

                TOKEN_TYPE getTokeType() 
                {
                    return this->mBracketType == BRACKET_TYPE::OPEN_BRACKET ? TOKEN_TYPE::OPEN_BRACKET: TOKEN_TYPE::CLOSE_BRACKET;
                }
};

class Operator : public IToken
{
            OPERATOR_TYPE mOperatorType;
        public:

            Operator(string operatorType)
            {
                this->mOperatorType = this->getOperationType(operatorType);
            }

            ~Operator()
            {

            }

            OPERATOR_TYPE getOperationType(const string &operationType)
            {
                switch(operationType[0])
                {
                    case '='    :   return OPERATOR_TYPE::EQUAL;
                    case '!'    :   if(operationType[1] == '=')
                                    {
                                        return OPERATOR_TYPE::NOT_EQUAL;
                                    }
                                    return OPERATOR_TYPE::UNKNOWN;
                    case '&'    :   if(operationType[1] == '&')
                                    {
                                        return OPERATOR_TYPE::AND;
                                    }
                                    return OPERATOR_TYPE::UNKNOWN;
                    case '|'    :   if(operationType[1] == '|')
                                    {
                                        return OPERATOR_TYPE::OR;
                                    }
                                    return OPERATOR_TYPE::UNKNOWN;
                    default     :   return OPERATOR_TYPE::UNKNOWN;
                }
            }

            void print() override
            {
                cout<<this->type()<<" ";
            }

            string type()
            {
                switch(this->mOperatorType)
                {
                    case OPERATOR_TYPE::EQUAL       :   return " = ";
                    case OPERATOR_TYPE::NOT_EQUAL   :   return " != ";
                    case OPERATOR_TYPE::AND         :   return " && ";
                    case OPERATOR_TYPE::OR          :   return " || ";
                    default                         :   return " ?  ";
                }
            }

            TOKEN_TYPE getTokeType() override
            {
                    return TOKEN_TYPE::OPERATOR;
            }

            bool evaluate(bool lhs,bool rhs) override
            {
                //cout<<"In evaluate ["<<lhs<<this->type()<<rhs<<"]"<<endl;
                switch(this->mOperatorType)
                {
                    case OPERATOR_TYPE::EQUAL       :   return lhs == rhs;
                    case OPERATOR_TYPE::NOT_EQUAL   :   return lhs != rhs;
                    case OPERATOR_TYPE::AND         :   return lhs && rhs;
                    case OPERATOR_TYPE::OR          :   return lhs || rhs;
                    default                         :   return false;
                }
            }

            unsigned int getPresidence() override
            {
                switch(this->mOperatorType)
                {
                    case OPERATOR_TYPE::EQUAL       :  
                    case OPERATOR_TYPE::NOT_EQUAL   :   return 2;
                    case OPERATOR_TYPE::AND         :
                    case OPERATOR_TYPE::OR          :   return 1;
                    default                         :   return 0;
                }
            }
};
class ExpressionEvaluator
{
        StlList<shared_ptr<IToken>> mExpressionList;
    public:
        ExpressionEvaluator()
        {
        }

        ~ExpressionEvaluator()
        {

        }

        template<typename T>
        void addExpression(const string &lhs,string operation,T&& rhs) 
        {
            this->mExpressionList.insert(make_shared<Expression>(lhs,operation,rhs));
        }

        void addExpression(const string &lhs,string operation,initializer_list<string> rhs)         
        {
            this->mExpressionList.insert(make_shared<Expression>(lhs,operation,rhs));
        }

        void addBracket(string bracket)
        {
            this->mExpressionList.insert(make_shared<Bracket>(bracket));
        }

        void addOperator(string operatorType)
        {
            this->mExpressionList.insert(make_shared<Operator>(operatorType));
        }

        void printList()
        {
            this->mExpressionList.startGet();
            shared_ptr<IToken> token;
            while(this->mExpressionList.getNextElement(token))
            {
                if(token)
                {
                    token->print();
                }
            }
            cout<<" ";//endl;
            this->mExpressionList.stopGet();
        }

        void reset()
        {
            this->mExpressionList.clear();
        }
        bool evaluate()
        {
            StlStack<bool> values;
            StlStack<shared_ptr<IToken>> operators;

            shared_ptr<IToken> token;

            this->mExpressionList.startGet();
           
            while(this->mExpressionList.getNextElement(token))
            {
                if(token)
                {
                        switch(token->getTokeType())
                        {
                            case TOKEN_TYPE::OPEN_BRACKET   :   operators.push(token);
                                                                break;
                            case TOKEN_TYPE::CLOSE_BRACKET  :   {
                                                                    shared_ptr<IToken> pToken;
                                                                    while((operators.pop(pToken)) && (pToken != nullptr) && pToken->getTokeType() != TOKEN_TYPE::OPEN_BRACKET)
                                                                    {
                                                                        bool lhs,rhs;
                                                                        if(values.pop(rhs))
                                                                        {
                                                                            if(values.pop(lhs))
                                                                            {
                                                                                values.push(pToken->evaluate(lhs,rhs));
                                                                            }
                                                                            else
                                                                            {
                                                                                values.push(rhs);
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                                break;
                            case TOKEN_TYPE::EXPRESSION     :   //token->print();
                                                                //cout<<" -> "<<token->value()<<endl;
                                                                values.push(token->value());
                                                                break;
                            case TOKEN_TYPE::OPERATOR       :   {
                                                                    shared_ptr<IToken> pToken;
                                                                    while((operators.front(pToken)) && (pToken != nullptr) && (pToken->getPresidence() >= token->getPresidence()))
                                                                    {
                                                                        bool lhs,rhs;
                                                                        if(values.pop(rhs))
                                                                        {
                                                                            if(values.pop(lhs))
                                                                            {
                                                                                values.push(pToken->evaluate(lhs,rhs));
                                                                            }
                                                                            else
                                                                            {
                                                                                values.push(rhs);
                                                                            }
                                                                        }
                                                                        operators.pop();
                                                                    }
                                                                    operators.push(token);
                                                                }
                                                                break;
                            default                         :   cout<<"UNKNOWN "<<(int)token->getTokeType();
                        }
                }
            }

            this->mExpressionList.stopGet();

            token = nullptr;
            while(operators.pop(token) && token)
            {
                bool lhs,rhs;
                if(values.pop(rhs))
                {
                    if(values.pop(lhs))
                    {
                        values.push(token->evaluate(lhs,rhs));
                    }
                    else
                    {
                        values.push(rhs);
                    }
                }
            }
            
            string resultStr = values.size() == 1 ? "SUCCESS" : "FAILED";
            cout<<"Result "<<resultStr<<" ";
            bool result = false;
            values.front(result);
            return result;
        }
};
int main()
{
    ExpressionEvaluator Exp;

    unsigned int i = 0;
    while(1)
    {
        string opcode = "SOP";
        Exp.addExpression(opcode,"=","BOP");
        Exp.addOperator("||");
        Exp.addExpression(opcode,"=","CRIC");
        Exp.addOperator("||");
        Exp.addExpression(opcode,"=","DRLOUT");
        Exp.addOperator("||");
        Exp.addExpression(opcode,"=","PPP");
        Exp.addOperator("||");
        Exp.addExpression(opcode,"=","SOP1");

        if((++i % 3) == 0)
        {
            Exp.addOperator("||");
            Exp.addExpression(opcode,"=","SOP");
        }
        else
        {
            Exp.addOperator("||");
            Exp.addExpression(opcode,"!=","SSP");
        }


        Exp.printList();
        cout<<"Result "<<Exp.evaluate()<<endl;
        Exp.reset();
        #if 0
        cout<<"==================================================================="<<endl;
        StlVector<string> vec = {"BOP","SOP","SSP"};
        string opcode = "SOP";
        string subcode = "N";

        Exp.addBracket("(");
        Exp.addExpression(opcode,"=",vec);
        Exp.addOperator("&&");
        Exp.addExpression(subcode,"=","N");
        Exp.addBracket(")");
        Exp.addOperator("||");
        Exp.addExpression(opcode,"!=","SOP");
        Exp.printList();


        cout<<"Result "<<Exp.evaluate()<<endl;
        cout<<"==================================================================="<<endl;

        Exp.reset();
        cout<<"==================================================================="<<endl;
        
        string phase = "16";
        Exp.addBracket("(");
        Exp.addExpression(opcode,"=","BOP");
        Exp.addOperator("&&");
        Exp.addExpression(subcode,"=","BOPJET");
        Exp.addBracket(")");
        Exp.addOperator("&&");
        Exp.addExpression(phase,"=","16");
        Exp.printList();

        cout<<"Result "<<Exp.evaluate()<<endl;
        cout<<"==================================================================="<<endl;
        cout<<"==================================================================="<<endl;
        opcode = "BOP";
        subcode = "BOPJET";
        Exp.printList();
        cout<<"Result "<<Exp.evaluate()<<endl;
        Exp.reset();
        cout<<"==================================================================="<<endl;
        usleep(1000);
#endif

        usleep(100000);
    }
    return 0;
}
