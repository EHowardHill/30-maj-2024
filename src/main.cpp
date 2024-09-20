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

    auto bg = starsbackground.create_bg(0, 0);

    auto spr_score = score.create_sprite(-88, -48);
    auto spr_hiscr = score.create_sprite(88, -48);

    while (true)
    {
        bg.set_x(bg.x() - 1);
        update();
    }
}