enum class WallAxis
{
    Vertical,
    Horizontal
};

struct HitResult
{
    double prepDistance;
    WallAxis axis;
};
