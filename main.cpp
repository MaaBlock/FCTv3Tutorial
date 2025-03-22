#include "FCT/headers.h"
using namespace FCT;
using namespace std;
int main(){
    Runtime rt;
    fout << "Hello, FCT!" << endl;
    /*
    auto wnd = rt.createWindow(0,0,800,600,"test");
    auto ctx = rt.createContext();
    ctx->create(wnd);
    while (wnd->isRunning()){
        ctx->flush();
    }*/
    fout << "good bye, fct!" << endl;
    return 0;
}