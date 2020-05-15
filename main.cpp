#include <iostream>
#include <SDL.h>
//#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <unistd.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>



//#include "SDL_utils.h"
const int W_WIDTH = 868;
const int W_HEIGHT = 696;
const int SCREEN_WIDTH = 522;
const int SCREEN_HEIGHT = 696;
const int NUM_BLOCK = 2;
const std::string WINDOW_TITLE = "a";
const int M = 24;
const int N = 18;
//size of block
const int b_w = (SCREEN_WIDTH/N);
const int b_h = (SCREEN_HEIGHT/M);
// text font and color
//TTF_Font *FONT = TTF_OpenFont("fonts/LibreFranklin-Medium.ttf", 30);

SDL_Color fontColor = {255, 255, 255, 255};


int CalPoint = 0;
int sum_lines = 0;
// ON OFF MUSIC
bool MUSIC = false;
bool GAME_OVER = false;

using namespace std;

int maps[M][N] = {0};
SDL_Rect block[4];
SDL_Rect nextblock[4];
int shapes[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};
//**************************************************************
void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer) ;

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);


void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);


void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
//*******************************************

struct Point{
    int x, y;
    };

class Queue{
private:
    int queue_size;
    int first = 0;
    int last  = 0;
    int* buffer;
public:
    Queue(int n){
        queue_size = n;
        buffer = new int[n];
    }
    void enqueue(int a){
        if(last < queue_size) buffer[last++] = a;
        else if(manageQ()) buffer[last++] = a;
        else{
            cout << "queue is full" << endl;
        }
    }
    int dequeue(){
        if(first < last) return buffer[first++];
        else{
            cout << "Queue is empty" << endl;
            return -1;
        }

    }
    int top(){
        return buffer[last-1];
    }
    int bottom(){
        return buffer[0];
    }
    bool manageQ(){
        if(first == 0){
            return false;
        }else{
            for(int i =0; i < last - first; i++){
                buffer[i] = buffer[i+ first];
            }
            last = last - first;
            first = 0;
            return true;
        }
        }
    };

//void createblock(Queue &shape, Queue &colors){
//    int n, color;
//    n=rand()%7;
//    color = rand()% 7 + 1;
//    shape.enqueue(n);
//    colors.enqueue(color);
//}


bool valid(){

    for(int i = 0; i < 4; i++){

        if(block[i].x  > (SCREEN_WIDTH - b_w) || block[i].y+b_h >= SCREEN_HEIGHT || block[i].x < 0 || block[i].y < 0){
            return false;
        }else if(maps[(block[i].y+b_h)/b_h][(block[i].x)/b_w]){
            return false;
            }
    }
    return true;

}

void initblock(SDL_Renderer* renderer,SDL_Texture *image, SDL_Rect &crop, const int& iw, const int& ih, const int& n, const int& color){

    for(int i =0; i < 4; i++){
        block[i].x = (shapes[n][i]/2)*b_w;
        block[i].y = (shapes[n][i]%2)*b_h;
        block[i].w = b_w;
        block[i].h = b_h;
        crop.x = (color-1)*18;
        crop.y = 0;
        crop.w = iw/8+2;
        crop.h = ih;
        SDL_RenderCopy(renderer, image, &crop, &block[i]);
    }
    if(!valid()){
        SDL_RenderClear(renderer);
        GAME_OVER = true;
    }

}

//next block
void next_block(SDL_Renderer* renderer,SDL_Texture *image, SDL_Rect &crop, const int& iw, const int& ih,const int& n, const int& color){
    for(int i =0; i < 4; i++){
        nextblock[i].x = (shapes[n][i]/2)*(b_w+10) + 620;
        nextblock[i].y = (shapes[n][i]%2)*(b_h+10) + 560;
        nextblock[i].w = b_w+10;
        nextblock[i].h = b_h+10;
        crop.x = (color-1)*18;
        crop.y = 0;
        // + them 2 cho do~ bi lech block :D
        crop.w = iw/8+2;
        crop.h = ih;
        SDL_RenderCopy(renderer, image, &crop, &nextblock[i]);
    }


}
void checkgame_over(){
        for(int j = 0; j < N; j++){
            if(maps[0][j]) GAME_OVER = true;
        }
}

void show_gameover(SDL_Renderer* renderer, SDL_Texture *gameover, const int& gov_w, const int& gov_h){
    renderTexture(gameover, renderer, 40, 48, gov_w, gov_h);


}


void draw_shadow(SDL_Renderer* renderer){
    int shadow[4];
    for(int b = 0; b < 4; b++){
        shadow[b] = block[b].y;
    }

    while(valid()){
        for(int i = 0; i < 4; i++){
            block[i].y += b_h;
        }
    }
    for(int i = 0; i < 4; i++){
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0); // white
        SDL_RenderDrawRect(renderer, &block[i]);
        block[i].y = shadow[i];
    }


}

//game menu
int game_menu(SDL_Texture *menu, SDL_Renderer* renderer,SDL_Texture *direct, SDL_Texture *music_off, SDL_Rect cropDR[], SDL_Rect blockDR[], const int& margin_top, const int& margin_l, const int& b_w, const int& b_h){
    // distance between button
    float dist = 24.5;
    int choice;
    int x, y;
    SDL_Event e;
    while(true){
        while( SDL_PollEvent(&e) ){
                SDL_RenderClear(renderer);
                // render on or off music  !!
                if(MUSIC){
                    renderTexture(menu, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                }else{
                renderTexture(music_off, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                }
                switch( e.type ){
                    case SDL_MOUSEMOTION:
                        x = e.motion.x;
                        y = e.motion.y;
                        if(y >= margin_top && y <= (margin_top+b_h) && x >= margin_l && x <= margin_l + b_w){
                            for(int i = 0; i < 2; i++){
                                cropDR[i].y = margin_top ;
                                SDL_RenderCopy(renderer, direct, &blockDR[i], &cropDR[i]);
                            }
                        }else if(y >= (margin_top + b_h + dist) && y <= (margin_top+ 2 * b_h + dist) && x >= margin_l && x <= margin_l + b_w){
                            for(int i = 0; i < 2; i++){
                                cropDR[i].y = margin_top + b_h + dist;
                                SDL_RenderCopy(renderer, direct, &blockDR[i], &cropDR[i]);

                            }
                        }else if(y >= (margin_top + 2 * b_h + 2 * dist) && y <= (margin_top+ 3*b_h + 2*dist) && x >= margin_l && x <= margin_l + b_w){
                            for(int i = 0; i < 2; i++){
                                cropDR[i].y = margin_top + 2*b_h + 2*dist;
                                SDL_RenderCopy(renderer, direct, &blockDR[i], &cropDR[i]);
                            }
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        x = e.motion.x;
                        y = e.motion.y;
                        if(y >= margin_top && y <= (margin_top+b_h) && x >= margin_l && x <= margin_l + b_w){
                            SDL_RenderClear(renderer);
                            return 1;
                        }else if(y >= (margin_top + b_h + dist) && y <= (margin_top+ 2 * b_h + dist) && x >= margin_l && x <= margin_l + b_w){
                            SDL_RenderClear(renderer);
                            return 0;
                        }else if(y >= (margin_top + 2 * b_h + 2 * dist) && y <= (margin_top+ 3*b_h + 2*dist) && x >= margin_l && x <= margin_l + b_w){
                            return 0;
                        }else if(x >= 400 && x <= 430 && y >= 580 && y <= 625){
                            SDL_RenderClear(renderer);
                            renderTexture(menu, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                            MUSIC = true;
                        }else if(x >= 438 && x <= 475 && y >= 580 && y <= 625){
                            SDL_RenderClear(renderer);
                            renderTexture(music_off, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                            MUSIC = false;

                        }
                        break;

                }
                SDL_RenderPresent(renderer);

        }
    }
}
//compute point
int compute_points(int level, int line_count)
{
    switch (line_count)
    {
    case 1:
        return 40 * (level + 1);
    case 2:
        return 100 * (level + 1);
    case 3:
        return 300 * (level + 1);
    case 4:
        return 1200 * (level + 1);
    }
    return 0;
}
void ttf_handle(SDL_Renderer* renderer, SDL_Texture* font_Texture, SDL_Surface* ttf_Surface, int i, SDL_Rect Text_c[]){
    font_Texture = SDL_CreateTextureFromSurface(renderer, ttf_Surface);
    SDL_RenderCopy(renderer, font_Texture, NULL, &Text_c[i]);
    return;
}
void render_score(SDL_Renderer* renderer, SDL_Texture* font_Texture, int &level , const int& line_count,SDL_Rect Text_c[], TTF_Font *FONT){
    if(CalPoint <= 500) level = 1;
    else if(CalPoint <= 1500 && CalPoint > 500) level = 2;
    else if(CalPoint > 1500) level = 3;
    string score = to_string(CalPoint)+ "        ";
    string level_str = to_string(level)+ "        ";
    string lines = to_string(sum_lines)+ "        ";
    // ERROR!!!! THIS LINE FUCK !! Undermaintained!!!!!!!!!!!
    SDL_Surface* score_Surface = TTF_RenderText_Solid(FONT, score.c_str(), fontColor);
    SDL_Surface* lines_Surface = TTF_RenderText_Solid(FONT, lines.c_str(), fontColor);
    SDL_Surface* level_Surface = TTF_RenderText_Solid(FONT, level_str.c_str(), fontColor);


    cout << TTF_GetError()  << endl;

    ttf_handle(renderer, font_Texture, score_Surface, 2, Text_c);
    ttf_handle(renderer, font_Texture, lines_Surface, 0, Text_c);
    ttf_handle(renderer, font_Texture, level_Surface, 1, Text_c);


    return;



}

int main(int argc, char *argv[]){
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Renderer* new_render;
    initSDL(window, renderer);
    // load image
    SDL_Texture *image = loadTexture("images/tiles.png", renderer);
    SDL_Texture *back_ground = loadTexture("images/new_113.png", renderer);
    SDL_Texture *menu = loadTexture("images/menu.png", renderer);
    SDL_Texture *gameover = loadTexture("images/gameover_new.png", renderer);
    SDL_Texture *music_off = loadTexture("images/musicoff.png", renderer);


    SDL_Texture* font_Texture;
    int gov_w, gov_h;
    SDL_QueryTexture(gameover, NULL, NULL, &gov_w, &gov_h);
    //check TTF
    if (TTF_Init() < 0) {
    // Error handling code
            return 0;
    }

    //setting ttf
    TTF_Font *FONT = TTF_OpenFont("fonts/LibreFranklin-Medium.ttf", 30);
    //int params to calculate scores
    int lines = 0, level = 1;
    // Rect contain text
    SDL_Rect Text_c[3];
    for(int i = 0; i < 3; i++){
        Text_c[i].w = 100;
        Text_c[i].h = 30;
        Text_c[i].x = 700;
        Text_c[i].y = 60*(i+1);
    }


    //START MUSIC OOOOOO!!!!!!!!!!
     if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        cout << "ERROR!" << endl;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        cout << "EERRR!" << Mix_GetError() << endl;
    }
    Mix_Music *bgm = Mix_LoadMUS("music/Tetris_theme.ogg.mp3");


    // settle some texttures
    SDL_Texture *direct  = loadTexture("images/direct.png", renderer);
    SDL_Rect cropDR[2], blockDR[2];
    int drw, drh;
    SDL_QueryTexture(direct, NULL, NULL, &drw, &drh);
    //define button size
    int bdr_w = 270;
    int bdr_h = 93;
    int margin_l = 255;
    int margin_top = 195;
    // init params
    for(int i = 0; i < 2; i++){
        cropDR[i].w = drw/2;
        cropDR[i].h = drh;
        blockDR[i].w = 70;
        blockDR[i].h = 60;
        if(i == 0){
            cropDR[i].x = margin_l - 80;
            blockDR[i].x = 0;
        }else{
            cropDR[i].x = margin_l + bdr_w + 10;
            blockDR[i].x = 75;
        }
        blockDR[i].y = 0;
    }




    Point prev[4];
    SDL_Rect crop, crop_next;
    // init variable
    Queue shape(2), colors(2);
    int iw, ih;
    SDL_QueryTexture(image, NULL, NULL, &iw, &ih);
    srand(time(NULL));


    //init variable to handle nextblock
    int color, n;
    for(int i = 0; i < 2; i++){
        n=rand()%7;
        color = rand()% 7 + 1;
        shape.enqueue(n);
        colors.enqueue(color);
    }
    //menu selection :D
    int choice;
    choice = game_menu(menu, renderer, direct,music_off, cropDR, blockDR, margin_top, margin_l, bdr_w, bdr_h);

    if(choice == 1){
        // start game

        renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
        initblock(renderer, image, crop, iw, ih, shape.dequeue(), colors.dequeue());
        next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
        render_score(renderer, font_Texture, level, lines, Text_c, FONT);
        SDL_RenderPresent(renderer);
        //define block width and height
        SDL_Rect prev_block, crop_a;
        prev_block.w = b_w;
        prev_block.h = b_h;
        crop_a.w = iw/8;
        crop_a.h = ih;
        crop_a.y = 0;

        //init gameplay handle
        bool rotates = false;
        bool running = true;
        SDL_Event e;
        float delay = 500;
        unsigned int current_time , last_time=0;
        int dx=0, dy=1;



        while(running){


            while( SDL_PollEvent(&e) ){
                if(MUSIC){
                    if( Mix_PlayingMusic() == 0 )
                    {
                            Mix_PlayMusic( bgm, -1 );
                            MUSIC = true;
                    }
                    else
                    {
                        if( Mix_PausedMusic() == 1 )
                        {
                         Mix_ResumeMusic();
                         }
                    }
                }else{
                    Mix_HaltMusic();

                }
                switch( e.type ){
                    // Look for a keypress
                    case SDL_KEYDOWN:
                        switch( e.key.keysym.sym ){
                            case SDLK_LEFT:
                                dx -= 1;
                                break;
                            case SDLK_RIGHT:
                                dx += 1;
                                break;
                            case SDLK_UP:
                                rotates = true;
                                break;
                            case SDLK_DOWN:
                                delay = 1;
                                break;
                            default:
                                break;
                        }
                    }
            }


            //previous position
            for(int p = 0; p < 4; p++){
                prev[p].x = block[p].x;
                prev[p].y = block[p].y;

            }

            // move horizontal direction <-->
            if(dx != 0){
                SDL_RenderClear(renderer);
                renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                render_score(renderer, font_Texture, level, lines, Text_c, FONT);

                for(int i = 0; i < 4; i++){
                    block[i].x += dx*b_w;
                    SDL_RenderCopy(renderer, image, &crop, &block[i]);

                }
                if(!valid()){

                    SDL_RenderClear(renderer);
                    renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                    next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                    render_score(renderer, font_Texture, level, lines, Text_c, FONT);
                    for(int i =0; i < 4; i++){
                        block[i].x = prev[i].x;
                        block[i].y = prev[i].y;
                        SDL_RenderCopy(renderer, image, &crop, &block[i]);
                    }
                }
                dx = 0;
            }


            //init center to rotate
            // default center block[1]
            if(rotates){
                rotates = false;
                SDL_RenderClear(renderer);
                renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                render_score(renderer, font_Texture, level, lines, Text_c, FONT);

                Point center;
                center.x = block[1].x;
                center.y = block[1].y;
                for(int i = 0; i < 4; i++){
                    int y = block[i].x - center.x;
                    int x = block[i].y - center.y;
                    //rotate 90 degree matrix
                    //[[0,-1],
                    // [1,0]]
                    block[i].x = -x + center.x;
                    block[i].y = y + center.y;
                    SDL_RenderCopy(renderer, image, &crop, &block[i]);

                }
                if(!valid()){
                    SDL_RenderClear(renderer);
                    renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                    next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                    render_score(renderer, font_Texture, level, lines, Text_c, FONT);

                    for(int i =0; i < 4; i++){
                        block[i].x = prev[i].x;
                        block[i].y = prev[i].y;
                        SDL_RenderCopy(renderer, image, &crop, &block[i]);
                    }
                }
            }
            current_time = SDL_GetTicks();
            //store what we already played
            if(current_time > delay + last_time){

                SDL_RenderClear(renderer);
                renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                render_score(renderer, font_Texture, level, lines, Text_c, FONT);

                for(int i = 0; i < 4; i++){
                    block[i].y += dy*b_h;
                    SDL_RenderCopy(renderer, image, &crop, &block[i]);

                }

                if(!valid()){
                    cout << "color is: " << colors.bottom();
                    for(int i = 0; i < 4; i++){
                        maps[block[i].y/b_h][block[i].x/b_w] = colors.bottom();
                    }

                    SDL_RenderClear(renderer);
                    renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);
                    next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());
                    render_score(renderer, font_Texture, level, lines, Text_c, FONT);



                    n=rand()%7;
                    color = rand()% 7 + 1;
                    shape.enqueue(n);
                    colors.enqueue(color);

                    initblock(renderer, image, crop, iw, ih, shape.dequeue(), colors.dequeue());
                    next_block(renderer, image, crop_next, iw, ih, shape.top(), colors.top());


                }
                delay = 500, last_time = current_time;

            }
            //check line
            int cut = M-1;
            for(int i = M-1; i > 0; i--){
                int same = 0;
                for(int j = 0; j < N; j++){
                    if(maps[i][j]) same+= 1;
                    maps[cut][j] = maps[i][j];
                }
                if(same < N) cut -= 1;
                else lines += 1;
            }
            //calculate point per lines
            sum_lines += lines;
            CalPoint += compute_points(level, lines);
            lines = 0;


            for(int i = M-1; i >= 0; i--){
                for(int j = 0; j < N; j++){
                    if(maps[i][j] != 0){
                        // + 2 cho block dep hon :D
                        crop_a.x = (maps[i][j]-1)*18 + 2;
                        prev_block.x = j*b_w;
                        prev_block.y = i*b_h;
                        SDL_RenderCopy(renderer, image, &crop_a, &prev_block);
                    }
                }


            }

            //draw the shadowbox
            draw_shadow(renderer);


    //        checkgame_over();
            if(GAME_OVER){
                cout << "Game over!!!";
//                renderTexture(back_ground, renderer, 0, 0, W_WIDTH, W_HEIGHT);

//                show_gameover(renderer, gameover, gov_w, gov_h);
//                SDL_RenderPresent(renderer);
                sleep(3);


            }

            SDL_RenderPresent(renderer);


        }
    }
    waitUntilKeyPressed();
    SDL_DestroyTexture(menu);
    SDL_DestroyTexture(gameover);
    SDL_DestroyTexture(music_off);
    SDL_DestroyTexture(image);
    SDL_DestroyTexture(back_ground);
    SDL_DestroyRenderer(renderer);
    quitSDL(window, renderer);
    Mix_Quit();
    IMG_Quit();
    return 0;


}



void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
       SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN);
    //window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
    //   SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);


    //Khi thông thường chạy với môi trường bình thường ở nhà
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
    //Khi chạy ở máy thực hành WinXP ở trường (máy ảo)
    //renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));
    if (renderer == nullptr) logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, W_WIDTH, W_HEIGHT);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
	//Khởi tạo là nullptr để tránh lỗi 'dangling pointer'
	SDL_Texture *texture = nullptr;
	//Nạp ảnh từ tên file (với đường dẫn)
	SDL_Surface *loadedImage = IMG_Load(file.c_str());
	//Nếu không có lỗi, chuyển đổi về dạng texture and và trả về
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Đảm bảo việc chuyển đổi không có lỗi
		if (texture == nullptr){
			logSDLError(std::cout, "CreateTextureFromSurface", false);
		}
	}
	else {
		logSDLError(std::cout, "LoadPNG", false);
	}
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
	//Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Truy vẫn texture để lấy chiều rộng và cao (vào chiều rộng và cao tương ứng của hình chữ nhật đích)
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
* Vẽ một SDL_Texture lên một SDL_Renderer tại toạ độ (x, y), với
* chiều rộng và cao mới
* @param tex: texture nguồn chúng ta muốn vẽ ra
* @param ren: thiết bị renderer chúng ta muốn vẽ vào
* @param x: hoành độ
* @param y: tung độ
* @param w: chiều rộng (mới)
* @param h: độ cao (mới)
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h)
{

	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    dst.w = w;
    dst.h = h;

	SDL_RenderCopy(ren, tex, NULL, &dst);
}
















