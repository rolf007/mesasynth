#ifndef _MACRO_H_
#define _MACRO_H_
#include <iostream>

class Ittr {
    struct Macro { const char* name; const char* data; };
public:
    Ittr(const char* str) : stackDepth_(0), numMacros_(0), mostRecentMacro_(-1), anonymousMacroData_(nullptr)
    {
        str_[0] = str;
        macros_[numMacros_++] = Macro{str_[stackDepth_], nullptr};
        while (parseMacro());
    }
    const char& operator*() const { return *str_[stackDepth_]; }
	const char* raw() const { return str_[stackDepth_]; }
    Ittr& operator++() { ++str_[stackDepth_]; while (parseMacro()); return *this; }
private:
    int findMacro()
    {
        for (unsigned m = 0; m < numMacros_; ++m) {
            for (unsigned i = 0; ; ++i) {
                if (macros_[m].name[i] == '{' && !isMacroFollowChar(str_[stackDepth_][i])) {
                    return m;
                } else if (macros_[m].name[i] == 0 || str_[stackDepth_][i] == 0) {
                    break;
                } else if (str_[stackDepth_][i] != macros_[m].name[i]) {
                    break;
                }
            }
        }
        return -1;
    }
	bool isMacroFollowChar(const char c)
	{
		if ((c >= 'a' && c <='z') || (c >= '0' && c <= '9') || c == '_')
			return true;
		return false;
	}
    bool storeMacro(const char* tmp)
    {
        for (const char* tmp2 = tmp; *tmp2; ++tmp2) {
            if (*tmp2 == '}') {
                //cout << "storeMacro: '" << string(str_[stackDepth_], tmp) << "'" << endl;
                //cout << "storeMacrx: '" << string(tmp+1, tmp2) << "'" << endl;
                int m = findMacro();
				//std::cout << "found new macro '" << str_[stackDepth_] << "', " << "'" << tmp << "'" << std::endl;
                if (m == -1) {
                    mostRecentMacro_ = numMacros_;
                    macros_[numMacros_++] = Macro{str_[stackDepth_], tmp};
                } else {
                    mostRecentMacro_ = m;
                    macros_[m] = Macro{str_[stackDepth_], tmp};
                }
                str_[stackDepth_] = tmp2 + 1;
                return true;
            }
        }
        return false;
    }
    bool recallMacro(int m, const char* p)
    {
		//std::cout << "recall macro '" << p << "'" << std::endl;
        if (macros_[m].data == nullptr)
            return false;
        mostRecentMacro_ = m;
        str_[stackDepth_++] = p;
        str_[stackDepth_] = macros_[m].data;
        return true;
    }
    bool parseMacro()
    {
        const char* tmp = str_[stackDepth_];
        if (*tmp >= 'A' && *tmp <= 'Z') {
			//std:: cout << "'" << *tmp << std::endl;
            ++tmp;
            for (;; ++tmp) {
                if (*tmp == '{') {
                    return storeMacro(tmp+1);
                } else if (!isMacroFollowChar(*tmp)) {
					if (*tmp == '@')
						++tmp;
                    int m = findMacro();
                    if (m != -1)
                        return recallMacro(m, tmp);
                    return false;
                }
            }
            return false;
        } else if (*tmp == '}') {
            if (stackDepth_) {
                stackDepth_--;
                return true;
            } else
                return false;
        } else if (*tmp == '@') {
            if (mostRecentMacro_ == -1)
                return false;
            return recallMacro(mostRecentMacro_, tmp+1);
        } else if (*tmp == '{') {
            return storeMacro(tmp+1);
        } else
            return false;
    }
    const char* str_[8]; // max stack depth
    unsigned stackDepth_;
    Macro macros_[8]; // max num macros
    unsigned numMacros_;
    int mostRecentMacro_;
    const char* anonymousMacroData_;
};

#endif
