#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>

void doRender(SDL_Renderer *renderer);
int processInput(SDL_Window *window);

double xCenter, iCenter, zoom;
bool mustRender = true;
const int width =  1000;
const int height = 1000;
int iterations = 100;

SDL_Color pallete[1000];

int main()
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Game Window",
            SDL_WINDOWPOS_UNDEFINED, //Initial x
            SDL_WINDOWPOS_UNDEFINED, //Initial y
            width,                     //width
            height,                     //height
            0
            );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    xCenter = 0; iCenter = 0; zoom = 2;

    time_t t;
    srand((unsigned) time(&t));
    for(int i = 0; i < iterations; i++)
    {
        SDL_Color color;
        color.r = rand() * 255;
        color.g = rand() * 255;
        color.b = rand() * 255;
        color.a = 255;
        pallete[i] = color;
    }
    int done = 0;
    
    // Event Loop
    while(!done)
    {
        // Check for updates
        done = processInput(window);

        if(mustRender)
        {
            // Render display
            doRender(renderer);
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}

SDL_Color getColorForPoint(int numIterations)
{
    //return pallete[numIterations];
    double H = (double)numIterations / (double)iterations * 360;
    double s = 1;
    double v = 1;
    double C = s * v;
    double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    //float m = v - C;
    float r, g, b;
    if(H >= 0 && H < 60)
    {
        r = C; g = X; b = 0;
    }
    else if(H >= 60 && H < 120)
    {
        r = X; g = C; b = 0;
    }
    else if(H >= 120 && H < 180)
    {
        r = 0; g = C; b = X;
    }
    else if(H >= 180 && H < 240)
    {
        r = 0; g = X; b = C;
    }
    else if(H >= 240 && H < 300)
    {
        r = X; g = 0; b = C;
    }
    else
    {
        r = C; g = 0; b = X;
    }

    SDL_Color color;
    color.r = r * 255;
    color.g = g * 255;
    color.b = b * 255;
    color.a = 255;
    return color;
}

// Handles rendering of the game
void doRender(SDL_Renderer *renderer)
{
    //Set the background color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for(int x = 0; x < width; x++)
    {
        for(int i = 0; i < height; i++)
        {
            double xCoord = ((double)(width - x - (width / 2)) / (double)(width / 2)) * zoom + xCenter;
            double iCoord = ((double)(height - i - (height / 2)) / (double)(height / 2)) * zoom + iCenter;
            double complex coordinate = xCoord + iCoord * I;
            double complex z = 0;
            int lastIteration;
            bool headsOut = false;
            for(int a = 0; a < iterations; a++)
            {
                z = z * z + coordinate;
                if(sqrt(creal(z) * creal(z) + cimag(z) * cimag(z)) >= 2)
                {
                    headsOut = true;
                    lastIteration = a;
                    break;
                }
            }
            if(headsOut)
            {
                SDL_Color color = getColorForPoint(lastIteration);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                const int ESCAPE_RADIUS = 2;
                //double eta = lastIteration + (log(2 * log(ESCAPE_RADIUS)) - log(log(abs(creal(z) + cimag(z))))) / log(2);
                //double eta = lastIteration + 1 - log(log(abs(creal(z) + cimag(z)))) / log(2);
                //printf("%f\n", eta);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }
            SDL_RenderDrawPoint(renderer, width - x, i);
        }
    }
    printf("x: %f, i: %f, zoom: %f\n", xCenter, iCenter, zoom);


    SDL_RenderPresent(renderer);
    mustRender = false;
}

void moveView(double xDelta, double iDelta, double zoomDelta)
{
    xCenter += xDelta; iCenter += iDelta; zoom += zoomDelta;
    mustRender = true;
}

int processInput(SDL_Window *window)
{
    SDL_Event event;
    short done = 0;
    double xDelta = 0, yDelta = 0, zoomDelta = 0;
    double moveFactor = .1;
    double zoomFactor = .1;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT_CLOSE:
            {
                if(window)
                {
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done = 1;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:
                        yDelta = moveFactor * zoom;
                        break;
                    case SDLK_DOWN:
                        yDelta = -moveFactor * zoom;
                        break;
                    case SDLK_LEFT:
                        xDelta = -moveFactor * zoom;
                        break;
                    case SDLK_RIGHT:
                        xDelta = moveFactor * zoom;
                        break;
                    case SDLK_x:
                        zoomDelta = zoomFactor * zoom;
                        break;
                    case SDLK_z:
                        zoomDelta = -zoomFactor * zoom;
                        break;
                    case SDLK_ESCAPE:
                        // Exit the program
                        done = 1; 
                        break;
                }
            }
            break;
            case SDL_QUIT:
            {
                // Exit the program
                done = 1;
            }
            break;
        }
    }

    if(xDelta != 0 || yDelta != 0 || zoomDelta != 0)
    {
        moveView(xDelta, yDelta, zoomDelta);
    }

    return done;
}
