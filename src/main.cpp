/*
 * Butano Engine - Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bn_core.h"
#include <bn_fixed.h>
#include <bn_vector.h>
#include "bn_display.h"
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
#include "bn_sprite_items_numbers.h"
#include "bn_sprite_items_lives.h"
#include "bn_regular_bg_items_starsbackground.h"
#include "bn_regular_bg_items_startscreen.h"
#include "bn_regular_bg_items_gameoverscreen.h"
#include "bn_regular_bg_items_gameoverscreen_text.h"

using namespace bn;
using namespace core;
using namespace keypad;
using namespace sprite_items;
using namespace regular_bg_items;
using namespace sound_items;

const fixed_t<12> TIME_CONST = 1.88;

enum ShipState
{
    state_playing,
    state_dead,
    state_wait,
    state_loading
};

bool close(fixed_t<12> x1, fixed_t<12> x2, fixed_t<12> y1, fixed_t<12> y2, int threshold)
{
    return abs(x1 - x2) <= threshold && abs(y1 - y2) <= threshold;
}

int update_vector_score(vector<sprite_ptr, 5> &vect, int value)
{
    int val[5];
    int divisor = 1;
    int max_ = 0;

    for (int i = 0; i < 5; i++)
    {
        val[i] = (value / divisor) % 10;
        divisor *= 10;
        if (value >= divisor)
        {
            max_ = i + 1;
        }
    }
    max_++;

    for (int t = 0; t < 5; t++)
    {
        vect.at(t) = numbers.create_sprite(vect.at(t).x(), vect.at(t).y(), (t < max_) ? val[t] + 1 : 0);
        vect.at(t).set_z_order(-9);
    }

    return 0;
}

void play_bgm(int &music_ticker)
{
    if (music_ticker >= 0 && music_ticker % 60 == 0)
    {
        switch ((music_ticker % 732) / 60)
        {
        case 0:
            sound_items::bgm0.play();
            break;
        case 1:
            sound_items::bgm1.play();
            break;
        case 2:
            sound_items::bgm2.play();
            break;
        case 3:
            sound_items::bgm3.play();
            break;
        case 4:
            sound_items::bgm4.play();
            break;
        case 5:
            sound_items::bgm5.play();
            break;
        case 6:
            sound_items::bgm6.play();
            break;
        case 7:
            sound_items::bgm7.play();
            break;
        case 8:
            sound_items::bgm8.play();
            break;
        case 9:
            sound_items::bgm9.play();
            break;
        case 10:
            sound_items::bgm10.play();
            break;
        case 11:
            sound_items::bgm11.play();
            break;
        case 12:
            sound_items::bgm12.play();
            music_ticker = -7;
            break;
        default:
            break;
        }
    }
    music_ticker++;
}

void move_bg(regular_bg_ptr &bg, int delta)
{
    bg.set_x(bg.x() - delta);
}

int main()
{
    init();

    int music_ticker = -48;

    auto bg = starsbackground.create_bg(0, 48);
    bg.set_blending_enabled(true);
    blending::set_transparency_alpha(0);

    for (int t = 0; t < 26 * TIME_CONST; t++)
    {
        update();
    }

    fixed_t<12> alpha_level = 0;
    for (int t = 0; t < 15; t++)
    {
        blending::set_transparency_alpha(alpha_level);
        if (alpha_level < 1)
        {
            alpha_level += 0.067;
        }
        update();
    }
    bg.set_blending_enabled(false);

    for (int t = 0; t < 46; t++)
    {
        update();
    }

    auto spr_ship = ship.create_sprite(-76 + 6, 28 - 16, 0);
    spr_ship.set_scale(2, 2);

    int ticker = 0;

    auto spr_nick = nick.create_sprite(-73 + 8, 30 - 16, 0);
    spr_nick.set_z_order(-9);

    {
        auto title = startscreen.create_bg(8, 64 - 16);

        while (ticker < 177)
        {
            ticker++;

            if (ticker == 9)
            {
                sound_items::start_jingle.play();
            }

            if (ticker > 27 * TIME_CONST)
            {
                title.set_visible(true);
            }

            update();
        }
    }

    spr_nick.set_visible(false);

    int score_high = 0;

    bool isPlayingContinue = true;
    while (isPlayingContinue)
    {

        ticker = 0;
        while (spr_ship.horizontal_scale() > 1)
        {
            if (ticker == 7)
            {
                sound_items::spawn.play();
            }
            ticker++;

            auto new_scale = spr_ship.horizontal_scale() - 0.0208;
            spr_ship.set_scale(new_scale, new_scale);
            update();
        }
        ticker = 0;

        auto rnd = random();

        auto spr_score_label = score.create_sprite(-82, -67);
        auto spr_score_high_label = hiscore.create_sprite(84, -67);
        spr_score_label.set_z_order(-9);
        spr_score_high_label.set_z_order(-9);

        auto lives_label = lives.create_sprite(-101 - 2, 68 - 4);
        auto lives_label_text = lives.create_sprite(-101 + 16 - 2, 68 - 4, 1);
        lives_label.set_z_order(-9);
        lives_label_text.set_z_order(-9);

        vector<sprite_ptr, 5> spr_score;
        vector<sprite_ptr, 5> spr_score_high;
        for (int t = 0; t < 5; t++)
        {
            auto new_local = numbers.create_sprite(-82 + 21 - (t * 8), -67 + 8, 1);
            auto new_high = numbers.create_sprite(84 + 29 - (t * 8), -67 + 8, 1);

            new_local.set_z_order(-9);
            new_high.set_z_order(-9);

            spr_score.push_back(new_local);
            spr_score_high.push_back(new_high);
        }

        // Define minimum horizontal spacing
        const int MIN_ASTEROID_SPACING = 85; // Adjust as needed
        const int MIN_ITEM_SPACING = 75;     // Adjust as needed
        int asteroid_rotation_speed[4];
        int initial_x_position = 300; // Starting x position off-screen

        vector<sprite_ptr, 6> spr_items;
        vector<sprite_ptr, 4> asteroids;

        for (int t = 0; t < 4; t++)
        {
            // Calculate x position ensuring minimum spacing
            int x_pos = initial_x_position + t * MIN_ASTEROID_SPACING;

            // Alternate y-position
            int y_multiplier = (t % 2 == 0) ? -1 : 1;
            int y_offset = rnd.get_int(72);
            int y_pos = y_offset * y_multiplier;

            auto asteroid = asteroid1.create_sprite(x_pos, y_pos, rnd.get_int(2));
            asteroid.set_scale(2, 2);
            asteroids.push_back(asteroid);

            asteroid_rotation_speed[t] = rnd.get_int(10) - 5;
            if (asteroid_rotation_speed[t] == 0)
                asteroid_rotation_speed[t] = 1;
        }

        for (int t = 0; t < 6; t++)
        {
            // Calculate x position ensuring minimum spacing
            int x_pos = initial_x_position + t * MIN_ITEM_SPACING;

            // Alternate y-position
            int y_offset = -24 + rnd.get_int(64); // Random between 20 and 51
            int y_pos = y_offset;

            auto item = items.create_sprite(x_pos, y_pos, rnd.get_int(5));
            spr_items.push_back(item);
        }

        int score = 0;
        int score_lives = 2;
        update_vector_score(spr_score, score);
        update_vector_score(spr_score_high, score_high);

        ShipState state = state_playing;
        int state_battery = 0;
        bool isPlaying = true;

        while (isPlaying)
        {

            if (ticker == 13)
            {
                sound_items::bgmintro.play();
            }

            play_bgm(music_ticker);

            if (state == state_loading)
            {
                move_bg(bg, 2);
            }
            else
            {
                move_bg(bg, ticker % 2);
            }

            bool moving = false;

            switch (state)
            {
            case state_playing:
            {
                state_battery = 126;

                if (a_pressed())
                {
                    sound_items::thrusters.play();
                }

                if (a_held())
                {
                    moving = true;
                    spr_ship.set_x(spr_ship.x() + 1);
                }
                else if (spr_ship.x() > -104)
                {
                    spr_ship.set_x(spr_ship.x() - 2);
                }

                if (a_released())
                {
                    moving = false;
                }

                if (up_held() && spr_ship.y() > -72)
                {
                    spr_ship.set_y(spr_ship.y() - 1);
                }
                else if (down_held() && spr_ship.y() < 72)
                {
                    spr_ship.set_y(spr_ship.y() + 1);
                };

                if (moving)
                {
                    spr_ship.set_tiles(ship.tiles_item(), (ticker / 4) % 2);
                }
                else
                {
                    spr_ship.set_tiles(ship.tiles_item(), 0);
                }

                break;
            }
            case state_dead:
            {
                spr_ship.set_tiles(ship.tiles_item(), 2 + ((ticker / 16) % 2));

                if (state_battery > 0)
                {
                    state_battery--;
                }
                else
                {
                    state = state_wait;
                    state_battery = 53;
                }

                if (state_battery < 32)
                {
                    spr_items.clear();
                    asteroids.clear();
                }

                break;
            }
            case state_wait:
            {
                spr_ship.set_visible(false);

                if (state_battery > 0)
                {
                    state_battery--;
                }
                else
                {
                    state = state_loading;
                    spr_ship = ship.create_sprite(-160, 0, 0);

                    score_lives--;
                    if (score_lives > -1)
                    {
                        lives_label_text = lives.create_sprite(-101 + 16 - 2, 68 - 4, 3 - score_lives);
                        sound_items::respawn.play();
                    }

                    for (int t = 0; t < 4; t++)
                    {
                        // Calculate x position ensuring minimum spacing
                        int min_spacing_used = MIN_ASTEROID_SPACING;

                        // Alternate y-position
                        int y_multiplier = (t % 2 == 0) ? -1 : 1;
                        int y_offset = rnd.get_int(72);
                        int y_pos = y_offset * y_multiplier;

                        if (rnd.get_int(4) == 1)
                        {
                            min_spacing_used = rnd.get_int(MIN_ASTEROID_SPACING - 32) + 32;

                            y_offset = rnd.get_int(22) + 50;
                            y_pos = y_offset * y_multiplier;
                        }

                        int x_pos = initial_x_position + t * min_spacing_used;

                        auto asteroid = asteroid1.create_sprite(x_pos, y_pos, rnd.get_int(2));
                        asteroid.set_scale(2, 2);
                        asteroids.push_back(asteroid);

                        asteroid_rotation_speed[t] = rnd.get_int(10) - 5;
                        if (asteroid_rotation_speed[t] == 0)
                            asteroid_rotation_speed[t] = 1;
                    }

                    for (int t = 0; t < 6; t++)
                    {
                        // Calculate x position ensuring minimum spacing
                        int x_pos = initial_x_position + t * MIN_ITEM_SPACING;

                        // Alternate y-position
                        int y_offset = -24 + rnd.get_int(64); // Random between 20 and 51
                        int y_pos = y_offset;

                        int tries = 0;
                        if (state == state_playing)
                        {
                            bool overlap = false;
                            do
                            {
                                tries++;
                                overlap = false;
                                y_offset = -24 + rnd.get_int(64);
                                for (int i = 0; i < asteroids.size(); i++)
                                {
                                    if (close(asteroids.at(i).x(), x_pos, asteroids.at(i).y(), y_pos, 24))
                                    {
                                        overlap = true;
                                    }
                                }
                            } while (overlap && tries < 32);
                        }

                        auto item = items.create_sprite(x_pos, y_pos, rnd.get_int(5));
                        spr_items.push_back(item);
                    }
                }

                break;
            }
            case state_loading:
            {
                if (score_lives < 0)
                {
                    isPlaying = false;
                }
                else
                {
                    spr_ship.set_visible(true);
                    update_vector_score(spr_score, score);

                    if (spr_ship.x() < 57)
                    {
                        spr_ship.set_tiles(ship.tiles_item(), (ticker / 4) % 2);
                        spr_ship.set_x(spr_ship.x() + 4);
                    }
                    else
                    {
                        state = state_playing;
                    }
                }

                break;
            }
            default:
            {
                break;
            }
            }

            for (int t = 0; t < spr_items.size(); t++)
            {
                spr_items.at(t).set_x(spr_items.at(t).x() - 1);

                if (close(spr_items.at(t).x(), spr_ship.x(), spr_items.at(t).y(), spr_ship.y(), 16) && spr_items.at(t).visible() && state == state_playing)
                {
                    spr_items.at(t).set_visible(false);
                    sound_items::collect.play();

                    score += 200;
                    if (score_high < score)
                        score_high = score;

                    update_vector_score(spr_score, score);
                    update_vector_score(spr_score_high, score_high);
                }

                // repair the silly overlap issue
                for (int i = 0; i < asteroids.size(); i++)
                {
                    if (close(asteroids.at(i).x(), spr_items.at(t).x(), asteroids.at(i).y(), spr_items.at(t).y(), 24))
                    {
                        spr_items.at(t).set_y(-24 + rnd.get_int(64));
                    }
                }

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
                    int new_x = max_x + MIN_ITEM_SPACING + rnd.get_int(64) - 32;
                    int new_y = 0;

                    bool overlap = false;

                    // Prevent overlap
                    int tries = 3;
                    do
                    {
                        new_y = -24 + rnd.get_int(64);
                        overlap = false;
                        for (int i = 0; i < asteroids.size(); i++)
                        {
                            if (close(asteroids.at(i).x(), new_x, asteroids.at(i).y(), new_y, 36))
                            {
                                overlap = true;
                            }
                        }
                    } while (overlap && tries < 32);

                    // Update asteroid position
                    spr_items.at(t).set_position(new_x, new_y);
                    spr_items.at(t).set_visible(true);
                }
            }

            for (int t = 0; t < asteroids.size(); t++)
            {
                fixed_t<12> new_rotation = asteroids.at(t).rotation_angle();
                new_rotation += asteroid_rotation_speed[t];

                if (new_rotation < 0)
                    new_rotation = 360;
                if (new_rotation > 360)
                    new_rotation = 0;

                asteroids.at(t).set_rotation_angle(new_rotation);
                // Move left
                asteroids.at(t).set_x(asteroids.at(t).x() - 1);

                if (close(asteroids.at(t).x(), spr_ship.x(), asteroids.at(t).y(), spr_ship.y(), 24) && state == state_playing)
                {
                    state = state_dead;
                    sound_items::die.play();
                }

                // Check if off-screen
                if (asteroids.at(t).x() < -132)
                {
                    // Find maximum x among other asteroids
                    int max_x = -152; // initial_x_position;
                    int new_y;

                    // Alternate y-position
                    int y_multiplier = (t % 2 == 0) ? -1 : 1;
                    int y_offset = rnd.get_int(64);

                    for (int i = 0; i < asteroids.size(); i++)
                    {
                        if (i != t && asteroids.at(i).x() > max_x)
                        {
                            max_x = asteroids.at(i).x().integer();
                        }
                    }
                    // Set new x position ensuring minimum spacing
                    int new_x = max_x + MIN_ASTEROID_SPACING;
                    int rnd_value = rnd.get_int(12);
                    if (rnd_value == 1)
                    {
                        new_x = max_x + rnd.get_int(MIN_ASTEROID_SPACING);
                        y_offset = 32 + rnd.get_int(32);
                    }

                    if (new_x < 152)
                        new_x = 152;

                    new_y = y_offset * y_multiplier;

                    // Update asteroid position
                    asteroids.at(t).set_position(new_x, new_y);
                    // Assign new rotation speed
                    asteroid_rotation_speed[t] = rnd.get_int(10) - 5;
                    if (asteroid_rotation_speed[t] == 0)
                        asteroid_rotation_speed[t] = 1;
                }
            }

            ticker++;
            update();
        }

        for (int t = 0; t < 38; t++)
        {
            if (sound_master_volume_manager::get() > 0.2)
            {
                sound_master_volume_manager::set(sound_master_volume_manager::get() - 0.021);
            }

            move_bg(bg, ticker % 2);
            play_bgm(music_ticker);
            ticker++;
            update();
        }

        sound_master_volume_manager::set(1);

        auto bg2 = gameoverscreen.create_bg(8, 64 - 16);

        spr_ship = ship.create_sprite(-76 + 6, 28 - 16, 0);
        spr_ship.set_scale(2, 2);
        ticker = 0;
        spr_nick.set_visible(true);

        int play_mode = 0;
        bool extro_played = false;

        auto bg3 = gameoverscreen_text.create_bg(8, 64 - 16);
        bg3.set_visible(false);

        while (play_mode != 1)
        {
            move_bg(bg, ticker % 2);

            if (ticker % 15 == 0)
            {
                for (int t = 0; t < spr_score_high.size(); t++)
                {
                    spr_score_high.at(t).set_visible(!spr_score_high.at(t).visible());
                }
            }

            if (ticker > 91)
            {
                bg3.set_visible(true);
            }

            if (music_ticker % 60 == 0 && extro_played == false)
            {
                extro_played = true;

                sound_items::ending_jingle.play();
            }
            music_ticker++;

            if (a_held() || b_held())
            {
                play_mode = 1;
            }

            ticker++;
            update();
        }

        spr_nick.set_visible(false);
        music_ticker = 0;
    }

    return 0;
}