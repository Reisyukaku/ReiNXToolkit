#include "MenuOption.hpp"

using namespace std;

MenuOption::MenuOption(string Name, string Desc, function<void()> Call)
{
    name = Name;
    desc = Desc;
    callback = Call;
}

string MenuOption::getDesc() {
    return desc;
}

void MenuOption::callFunc() {
    if(callback != nullptr) callback();
}