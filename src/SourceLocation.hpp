#ifndef SOURCE_LOCATION
#define SOURCE_LOCATION

class SourceLocation{
    public:
       SourceLocation(){}
       SourceLocation(int line, int column): line(line), column(column){}
       inline int GetLine() const {return line;}
       inline int GetColumn() const {return column;}
    private:
       int line;
       int column;
};

#endif
