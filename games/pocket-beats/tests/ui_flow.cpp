#define main pocket_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode key){SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=key;SDL_PushEvent(&e);SDL_Delay(45);e.type=SDL_KEYUP;SDL_PushEvent(&e);SDL_Delay(80);}
int main(){char dir[]="/tmp/pocket-beats-flow-XXXXXX";if(!mkdtemp(dir))return 1;setenv("FMG_SAVE_DIR",dir,1);
 std::thread user([]{SDL_Delay(1400);press(SDLK_z);press(SDLK_z);press(SDLK_v);press(SDLK_RIGHT);press(SDLK_z);press(SDLK_x);for(int i=0;i<8;++i)press(SDLK_RIGHT);press(SDLK_v);press(SDLK_RIGHT);press(SDLK_z);press(SDLK_RETURN);SDL_Delay(250);press(SDLK_ESCAPE);});
 char name[]="pocket-ui";char*args[]={name};int result=pocket_application_main(1,args);user.join();pocketbeats::Session loaded;std::string path=std::string(dir)+"/pocket-beats.sav";
 bool ok=result==0&&loaded.load(path)&&loaded.current().notes[0][0]==4&&loaded.current().notes[0][8]==4&&loaded.cursor==8;
 std::remove(path.c_str());rmdir(dir);std::cout<<(ok?"PASS":"FAIL")<<" SDL window flow: add drum, copy first page, paste next page, play, save and exit\n";return ok?0:1;
}
