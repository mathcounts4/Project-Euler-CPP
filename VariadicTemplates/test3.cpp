#include <iostream>

class Info
{
  private:
    unsigned int m_numFields;
    const char* m_name;
    const char** m_fieldNames;

    Info(unsigned int numFields) :
            m_numFields(numFields),
            m_fieldNames(const_cast<const char**>(new char*[numFields]())) {}

    static Info makeInfo(unsigned int numFields)
    {
        return Info(numFields);
    }

    template<typename... Args>
    static Info makeInfo(unsigned int numFields,
                         const char* field1,
                         Args... moreFields)
    {
        Info info = makeInfo(numFields + 1U, moreFields...);
        info.m_fieldNames[numFields] = field1;
        return info;
    }

  public:
    static Info makeInfo(); // unimplemented

    template<typename... Args>
    static Info makeInfo(const char* name,
                         Args... fieldNames)
    {
        Info info = makeInfo(0U,fieldNames...);
        info.m_name = name;
        return info;
    }

//    template<typename... Args>
//    static Info makeInfo(Args... badArgs); // unimplemented

    ~Info()
    {
        delete[] m_fieldNames;
    }

    int getNumFields() const { return m_numFields; }
    const char* getName() const { return m_name; }
    const char** getFieldNames() const { return m_fieldNames; }
};

void printFieldNames(Info& info)
{
    int numFields = info.getNumFields();
    const char* name = info.getName();
    const char** fieldNames = info.getFieldNames();

    printf("%s:\n",name);
    for (int i = 0; i < numFields; i++)
    {
        printf("%d: %s\n",i,fieldNames[i]);
    }
    printf("\n");
}

int main()
{
    Info x = Info::makeInfo("Eminem Info","Hello","my","name","is","Slim","Shady");
    Info y = Info::makeInfo("Duuuude Info","Templates dude","Yeah dude");

    printFieldNames(x);
    printFieldNames(y);

    return 0;
}
