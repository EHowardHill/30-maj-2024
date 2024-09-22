/*
 * Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bn_core.h"
#include "bn_log.h"
#include <bn_fixed.h>
#include <bn_vector.h>
#include <bn_random.h>
#include "bn_display.h"
#include <bn_math.h>
#include <bn_keypad.h>
#include "bn_blending.h"
#include <bn_sprite_ptr.h>
#include <bn_camera_ptr.h>
#include <bn_sprite_animate_actions.h>
#include <bn_sprite_text_generator.h>
#include <bn_sprite_font.h>
// #include "bn_sound_items.h"
#include <bn_string.h>
#include <bn_random.h>
#include <bn_math.h>
#include <bn_sound_actions.h>
#include <bn_cameras.h>
#include "bn_camera_actions.h"
#include <bn_optional.h>
#include <bn_blending.h>

#include "bn_music.h"
// #include "bn_music_items.h"
// #include "bn_music_items_info.h"

#include "bn_regular_bg_ptr.h"

#include "bn_sprite_items_score.h"
#include "bn_sprite_items_hiscore.h"
#include "bn_sprite_items_ship.h"
#include "bn_sprite_items_asteroid1.h"
#include "bn_regular_bg_items_starsbackground.h"

using namespace bn;
using namespace core;
using namespace keypad;
using namespace sprite_items;
using namespace regular_bg_items;

#include "main.h"

int main()
{
    init();

    auto rnd = random();
    auto bg = starsbackground.create_bg(0, 64);

    auto spr_score = score.create_sprite(-82, -67);
    auto spr_hiscr = hiscore.create_sprite(84, -67);
    auto spr_ship = ship.create_sprite(-82, 0, 0);

    vector<sprite_ptr, 4> asteroids;
    int asteroid_speed[4];

    for (int t = 0; t < 4; t++)
    {
        auto a = asteroid1.create_sprite(t * 64, (20 + rnd.get_int(20)) * (((t % 2) << 1) - 1), rnd.get_int(2));
        a.set_scale(2, 2);
        asteroids.push_back(a);
        asteroid_speed[t] = rnd.get_int(7) + 1;
    }

    int ticker = 0;
    while (true)
    {
        bg.set_x(bg.x() - 1);
        bool moving = false;

        if (spr_ship.x() > -82)
        {
            spr_ship.set_x(spr_ship.x() - 1);
        }

        if (right_held())
        {
            moving = true;
            spr_ship.set_x(spr_ship.x() + 2);
        }
        else if (left_held() && spr_ship.x() > -82)
        {
            moving = true;
            spr_ship.set_x(spr_ship.x() - 1);
        }

        if (up_held() && spr_ship.y() > -35)
        {
            spr_ship.set_y(spr_ship.y() - 1);
        }
        else if (down_held() && spr_ship.y() < 35)
        {
            spr_ship.set_y(spr_ship.y() + 1);
        };

        if (moving)
        {
            spr_ship = ship.create_sprite(spr_ship.x(), spr_ship.y(), (ticker / 4) % 2);
        }
        else
        {
            spr_ship = ship.create_sprite(spr_ship.x(), spr_ship.y(), 0);
        }

        for (int t = 0; t < asteroids.size(); t++)
        {
            asteroids.at(t).set_rotation_angle(((ticker * 16) / asteroid_speed[t]) % 360);
            asteroids.at(t).set_x(asteroids.at(t).x() - 1);

            if (asteroids.at(t).x() < -152)
            {
                asteroids.at(t).set_position(152, (20 + rnd.get_int(20)) * (((t % 2) << 1) - 1));
                asteroid_speed[t] = rnd.get_int(7) + 1;
            }
        }

        ticker++;
        update();
    }
}