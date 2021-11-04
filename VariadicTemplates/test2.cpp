#include <iostream>

class Info
{
  private:
    int numFields;
    const char** fieldNames;

    explicit Info(int fields) : numFields(fields),
                       fieldNames(const_cast<const char**>(
                               new char*[fields]())) {}

    template<typename... Args>
    explicit Info(int fields, const char* field1, Args... args) :
            Info(fields+1,args...)
    {
        fieldNames[fields] = field1;
    }

  public:
    explicit Info() : Info(0) {}

    template<typename... Args>
    explicit Info(const char* field1, Args... args) : Info(0,field1,args...) {}

    ~Info()
    {
        delete[] fieldNames;
    }

    int getNumFields() const { return numFields; }
    const char** getFieldNames() const { return fieldNames; }
};

void printFieldNames(Info& info)
{
    int numFields = info.getNumFields();
    const char** fieldNames = info.getFieldNames();

    for (int i = 0; i < numFields; i++)
    {
        printf("%d: %s\n",i,fieldNames[i]);
    }
    printf("\n");
}

int main()
{
    Info x("Hello","my","name","is","Slim","Shady");
    Info y("Templates dude","Yeah dude");

    printFieldNames(x);
    printFieldNames(y);

    return 0;
}
