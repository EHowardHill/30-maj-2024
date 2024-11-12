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
#include "bn_sound_items.h"
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
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_nick.h"
#include "bn_regular_bg_items_starsbackground.h"
#include "bn_regular_bg_items_startscreen.h"

using namespace bn;
using namespace core;
using namespace keypad;
using namespace sprite_items;
using namespace regular_bg_items;
using namespace sound_items;

#include "main.h"

int close(fixed_t<12> x1, fixed_t<12> x2, fixed_t<12> y1, fixed_t<12> y2, int threshold)
{
    return abs(x1 - x2) <= threshold && abs(y1 - y2) <= threshold;
}

int stage_title()
{

    return 0;
}

int main()
{
    init();

    auto bg = starsbackground.create_bg(0, 64);

    auto spr_ship = ship.create_sprite(-76 + 6, 28 - 16, 0);
    spr_ship.set_scale(2, 2);

    int ticker = 0;

    {
        auto title = startscreen.create_bg(8, 64 - 16);
        auto spr_nick = nick.create_sprite(-73 + 8, 30 - 16, 0);

        while (ticker < 27 + 90)
        {
            ticker++;

            if (ticker > 27)
            {
                title.set_visible(true);
            }

            update();
        }
    }

    while (spr_ship.horizontal_scale() > 1)
    {
        auto new_scale = spr_ship.horizontal_scale() - 0.0625;
        spr_ship.set_scale(new_scale, new_scale);
        update();
    }

    auto rnd = random();

    auto spr_score = score.create_sprite(-82, -67);
    auto spr_hiscr = hiscore.create_sprite(84, -67);

    vector<sprite_ptr, 6> spr_items;
    int asteroid_speed[4];

    // Define minimum horizontal spacing
    const int min_spacing = 100;     // Adjust as needed
    const int min_spacing_item = 75; // Adjust as needed

    // Initialize asteroids
    vector<sprite_ptr, 4> asteroids;
    int asteroid_rotation_speed[4];
    int initial_x_position = 300; // Starting x position off-screen

    for (int t = 0; t < 4; t++)
    {
        // Calculate x position ensuring minimum spacing
        int x_pos = initial_x_position + t * min_spacing;

        // Alternate y-position
        int y_multiplier = (t % 2 == 0) ? -1 : 1;
        int y_offset = 24 + rnd.get_int(32); // Random between 20 and 51
        int y_pos = y_offset * y_multiplier;

        auto asteroid = asteroid1.create_sprite(x_pos, y_pos, rnd.get_int(2));
        asteroid.set_scale(2, 2);
        asteroids.push_back(asteroid);

        // Assign random rotation speed between 1 and 7
        asteroid_rotation_speed[t] = rnd.get_int(7) + 1;
    }

    for (int t = 0; t < 6; t++)
    {
        // Calculate x position ensuring minimum spacing
        int x_pos = initial_x_position + t * min_spacing_item;

        // Alternate y-position
        int y_offset = -24 + rnd.get_int(64); // Random between 20 and 51
        int y_pos = y_offset;

        auto item = items.create_sprite(x_pos, y_pos, rnd.get_int(5));
        spr_items.push_back(item);
    }

    int whoosh = 0;
    while (true)
    {
        bg.set_x(bg.x() - ticker % 2);
        bool moving = false;

        if (spr_ship.x() > -82)
        {
            spr_ship.set_x(spr_ship.x() - 1);
        }

        if (a_held())
        {
            whoosh = 8;
        }

        if (whoosh > 0)
        {
            moving = true;
            spr_ship.set_x(spr_ship.x() + 2);
            whoosh--;
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

        for (int t = 0; t < spr_items.size(); t++)
        {
            spr_items.at(t).set_x(spr_items.at(t).x() - 1);

            // Check if off-screen
            if (spr_items.at(t).x() < -152)
            {
                // Find maximum x among other asteroids
                int max_x = -152; // initial_x_position;
                for (int i = 0; i < spr_items.size(); i++)
                {
                    if (i != t && spr_items.at(i).x() > max_x)
                    {
                        max_x = spr_items.at(i).x().integer();
                    }
                }
                // Set new x position ensuring minimum spacing
                int new_x = max_x + min_spacing_item;

                // Alternate y-position
                int new_y = -24 + rnd.get_int(64);

                // Update asteroid position
                spr_items.at(t).set_position(new_x, new_y);
            }
        }

        for (int t = 0; t < asteroids.size(); t++)
        {
            // Update rotation
            asteroids.at(t).set_rotation_angle(((ticker) / asteroid_rotation_speed[t]) % 360);
            // Move left
            asteroids.at(t).set_x(asteroids.at(t).x() - 1);

            // Check if off-screen
            if (asteroids.at(t).x() < -152)
            {
                // Find maximum x among other asteroids
                int max_x = -152; // initial_x_position;
                for (int i = 0; i < asteroids.size(); i++)
                {
                    if (i != t && asteroids.at(i).x() > max_x)
                    {
                        max_x = asteroids.at(i).x().integer();
                    }
                }
                // Set new x position ensuring minimum spacing
                int new_x = max_x + min_spacing;

                // Alternate y-position
                int y_multiplier = (t % 2 == 0) ? -1 : 1;
                int y_offset = 24 + rnd.get_int(32);
                int new_y = y_offset * y_multiplier;

                // Update asteroid position
                asteroids.at(t).set_position(new_x, new_y);
                // Assign new rotation speed
                asteroid_rotation_speed[t] = rnd.get_int(24) + 1;
            }
        }

        ticker++;
        update();
    }

    return 0;
}