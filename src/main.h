#define MAX_SPRITES 512

fixed lerp(fixed a, fixed b, fixed t)
{
    return a * (1 - t) + b * t;
}

struct position_3d
{
    fixed x = 0;
    fixed y = 0;
    fixed z = 0;
};