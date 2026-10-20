#include <vector>
int fmg_application_main(int,char**);
int main(int argc,char**argv){
 std::vector<char*> args(argv,argv+argc);
 char flag[]="--game", game[]="color-mix";
 args.push_back(flag);args.push_back(game);args.push_back(nullptr);
 return fmg_application_main(int(args.size())-1,args.data());
}
