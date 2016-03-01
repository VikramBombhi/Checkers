//
//  Button.cpp
//  SDL_Checkers
//
//  Created by Jacky Chiu on 2016-02-13.
//  Copyright © 2016 Jacky Chiu. 
//

#include "../include/Button.h"
#include "GameState.h"
#include "Texture.h"

Button::Button(){
    buttonPoint.x = 0;
    buttonPoint.y = 0;
}

Button::~Button(){
    buttonPoint.x = NULL;
    buttonPoint.y = NULL;
}

void Button::setPoint(int x, int y){
    buttonPoint.x = x;
    buttonPoint.y = y;
}

void Button::render(){
    //Render button with spritesheet
    spriteSheetTexture.render(buttonPoint.x, buttonPoint.y, &spriteClips[currentSprite-1]);
}

bool Button::insideButton(){

    int x,y;
    bool insideButton = true;
    SDL_GetMouseState(&x,&y);
    
    // Checks if button has been clicked
    // Above button //
    if(y < buttonPoint.y){
        insideButton = false;
    }
    // Under button //
    else if (y > buttonPoint.y + BUTTON_HEIGHT){
        insideButton = false;
    }
    // Left of button //
    else if (x < buttonPoint.x) {
        insideButton = false;
    }
    // Right of button //
    else if (x > buttonPoint.x + BUTTON_WIDTH){
        insideButton = false;
    }

    return insideButton;
}

int Button::getButtonPointX(){
    return buttonPoint.x;
}

int Button::getButtonPointY(){
    return buttonPoint.y;
}