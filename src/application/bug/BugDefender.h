/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __Invaders_include__
#define __Invaders_include__

#include "lib/async/ThreadPool.h"
#include "lib/game/HHUEngine.h"
#include "lib/game/GameObject.h"
#include "application/bug/Ship.h"
#include "application/bug/Enemy.h"
#include "application/bug/Fleet.h"
#include "lib/game/Game.h"

class BugDefender : public Game {

  private:

    BugDefender(const BugDefender &copy);

    const static int enemiesPerLine = 11;

    const static int enemyLines = 6;

    static int enemyCount;

    static int score;

    static int lifes;

    bool isMenuVisible;

    static bool isGameOver;

    static bool isGameWon;

    static Image *heartSprite;

    static Image *background;

    static ThreadPool *beepThreadPool;

  public:

    BugDefender();

    ~BugDefender();

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void checkCollisions(GameObject* o);

    void drawInfo(LinearFrameBuffer* g2d);

    void showMenu(LinearFrameBuffer* g2d);

    void showGameOver(LinearFrameBuffer* g2d);

    void showGameWon(LinearFrameBuffer* g2d);

    int getPoints();

    static void addPoints(int points);

    static void takeDamage(int amount);

    static void gameOver();

    static void enemyShot();
};

#endif
