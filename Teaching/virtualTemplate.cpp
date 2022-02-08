#include <unordered_map>
#include <string_view>
#include <iostream>
#include <optional>

// This wonky construction models an inheritance hierarchy where a "language" can incorporate (multiple) other languages.
// We want to be able to make the following queries at runtime:
// Does X incorporate Y?
// Set a setting to X or Y, but not Z, though Z is valid in the first query.
// Map from setting to name
// Map from name to setting

enum class TargetLanguageEnum {C, CPP, OpenCL_C, OpenCL_CPP};

class TargetLanguage {
  public:
    static bool isa(TargetLanguageEnum actual, std::string_view expected) {
        auto expectedTarget = fromName(expected);
        if (!expectedTarget) {
            return false;
        }
        return expectedTarget->castsToMe(fromEnum(actual));
    }

    static std::string_view enumToName(TargetLanguageEnum enumVal) {
        return fromEnum(enumVal)->name();
    }

  private:
    virtual std::string_view name() const = 0;
    virtual bool castsToMe(TargetLanguage const* someTargetLang) const = 0;

  protected:
    virtual ~TargetLanguage() = default;

  private:
    static std::unordered_map<TargetLanguageEnum, TargetLanguage const&> enumToObj;
    static std::unordered_map<std::string_view, TargetLanguage const&> nameToObj;

  protected:
    template<class SpecificTargetLanguage>
    static void init(SpecificTargetLanguage const& specificTargetLang) {
        // Some members of the TargetLanguage hierarchy are not valid on their own,
        //   but act as a common base to others. Those don't have an enum, but do have a name.
        if (std::optional<TargetLanguageEnum> enumValue = SpecificTargetLanguage::m_enumValue) {
            enumToObj.emplace(*enumValue, specificTargetLang);
        }
        nameToObj.emplace(specificTargetLang.name(), specificTargetLang);
    }

  private:
    static TargetLanguage const* fromEnum(TargetLanguageEnum enumVal) {
        // return findOrNull(enumToObj, enumVal);
        auto actualTarget = enumToObj.find(enumVal);
        if (actualTarget == enumToObj.end()) {
            return nullptr;
        } else {
            return &actualTarget->second;
        }
    }

    static TargetLanguage const* fromName(std::string_view name) {
        // return findOrNull(nameToObj, name);
        auto actualTarget = nameToObj.find(name);
        if (actualTarget == nameToObj.end()) {
            return nullptr;
        } else {
            return &actualTarget->second;
        }
    }
};
std::unordered_map<TargetLanguageEnum, TargetLanguage const&> TargetLanguage::enumToObj;
std::unordered_map<std::string_view, TargetLanguage const&> TargetLanguage::nameToObj;

template<class Self, class... Bases>
class TargetLanguageTemplate : public virtual TargetLanguage, public virtual Bases... {
public:
    static void instantiate() {
        static Self self;
        static int x = (init(self), 0);
	(void)x;
    }
    std::string_view name() const override {
        return Self::m_name;
    }
    bool castsToMe(TargetLanguage const* someTargetLang) const override {
        return dynamic_cast<Self const*>(someTargetLang);
    }
};

class TargetLanguageC : public TargetLanguageTemplate<TargetLanguageC> {
  public:
    static constexpr std::string_view m_name = "C";
    static constexpr TargetLanguageEnum m_enumValue = TargetLanguageEnum::C;
};
static int globalInitC = ((void)TargetLanguageC::instantiate(), 0);

class TargetLanguageCPP : public TargetLanguageTemplate<TargetLanguageCPP> {
  public:
    static constexpr std::string_view m_name = "C++";
    static constexpr TargetLanguageEnum m_enumValue = TargetLanguageEnum::CPP;
};
static int globalInitCPP = ((void)TargetLanguageCPP::instantiate(), 0);

class TargetLanguageOpenCL : public TargetLanguageTemplate<TargetLanguageOpenCL> {
  public:
    static constexpr std::string_view m_name = "OpenCL";
    // OpenCL itself is used for name matching, but requires OpenCL_C or OpenCL_CPP, thus has no enum itself.
    static constexpr std::optional<TargetLanguageEnum> m_enumValue = std::nullopt;
};
static int globalInitOpenCL = ((void)TargetLanguageOpenCL::instantiate(), 0);

// OpenCL_C is both C and OpenCL
class TargetLanguageOpenCL_C : public TargetLanguageTemplate<TargetLanguageOpenCL_C, TargetLanguageC, TargetLanguageOpenCL> {
  public:
    static constexpr std::string_view m_name = "OpenCL_C";
    static constexpr TargetLanguageEnum m_enumValue = TargetLanguageEnum::OpenCL_C;
};
static int globalInitOpenCL_C = ((void)TargetLanguageOpenCL_C::instantiate(), 0);

// OpenCL_CPP is both CPP and OpenCL
class TargetLanguageOpenCL_CPP : public TargetLanguageTemplate<TargetLanguageOpenCL_CPP, TargetLanguageCPP, TargetLanguageOpenCL> {
  public:
    static constexpr std::string_view m_name = "OpenCL_CPP";
    static constexpr TargetLanguageEnum m_enumValue = TargetLanguageEnum::OpenCL_CPP;
};
static int globalInitOpenCL_CPP = ((void)TargetLanguageOpenCL_CPP::instantiate(), 0);




static void test(TargetLanguageEnum actual, std::string_view expected) {
    bool isa = TargetLanguage::isa(actual, expected);
    std::cout << TargetLanguage::enumToName(actual) << (isa ? " is a " : " is not a ") << expected << std::endl;
}

int main() {
    test(TargetLanguageEnum::C, "C");
    test(TargetLanguageEnum::CPP, "C++");
    test(TargetLanguageEnum::OpenCL_C, "C");
    test(TargetLanguageEnum::OpenCL_CPP, "C");
    test(TargetLanguageEnum::OpenCL_CPP, "C++");
    test(TargetLanguageEnum::CPP, "OpenCL_CPP");
    test(TargetLanguageEnum::OpenCL_CPP, "OpenCL_CPP");
    test(TargetLanguageEnum::OpenCL_CPP, "blah");
}

