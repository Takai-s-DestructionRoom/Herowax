#include "ColPrimitive2D.h"

using namespace std;
using namespace ColPrimitive2D;

ColPrimitive2D::Rect::Rect(Vector2 s, Vector2 e) :
    p(Vector2((s.x + e.x) / 2.0f, (s.y + e.y) / 2.0f)),
    w(max(s.x, e.x) - min(s.x, e.x)),
    h(max(s.y, e.y) - min(s.y, e.y)) {}

array<Vector2, 4> ColPrimitive2D::Rect::ToPointArray()
{
    array<Vector2, 4> arr = {};

    float pLeft = p.x - w / 2;
    float pRight = p.x + w / 2;
    float pTop = p.y + h / 2;
    float pBottom = p.y - h / 2;

    arr[0] = { pLeft, pTop }; //左上
    arr[1] = { pRight, pTop }; //右上
    arr[2] = { pLeft, pBottom }; //左下
    arr[3] = { pRight, pBottom } ; //右下

    return arr;
}

std::array<Segment, 4> ColPrimitive2D::Rect::ToSideArray()
{
    std::array<Segment, 4> arr = {};

    float pLeft = p.x - w / 2;
    float pRight = p.x + w / 2;
    float pTop = p.y + h / 2;
    float pBottom = p.y - h / 2;

    arr[0] = { {pLeft, pTop}, {pLeft, pBottom} }; //左
    arr[1] = { {pRight, pTop}, {pRight, pBottom} }; //右
    arr[2] = { {pLeft, pTop}, {pRight, pTop} }; //上
    arr[3] = { {pLeft, pBottom}, {pRight, pBottom} }; //下

    return arr;
}

bool ColPrimitive2D::IsHit(Point p, Rect r)
{
    return p.p.x >= r.p.x - r.w / 2.0f && p.p.x <= r.p.x + r.w / 2.0f
        && p.p.y >= r.p.y - r.h / 2.0f && p.p.y <= r.p.y + r.h / 2.0f;
}

bool ColPrimitive2D::IsHit(Rect a, Rect b)
{
    float xDiff = a.p.x - b.p.x;
    float yDiff = a.p.y - b.p.y;
    float wSum = a.w / 2 + b.w / 2;
    float hSum = a.h / 2 + b.h / 2;

    return abs(xDiff) <= wSum && abs(yDiff) <= hSum;
}

bool ColPrimitive2D::IsHit(Point p, Capsule capsule)
{
    Vector2 ap = p.p - capsule.p;
    Vector2 bp = p.p - (capsule.p + capsule.v);
    Vector2 ab = (capsule.p + capsule.v) - capsule.p;
    Vector2 ba = capsule.p - (capsule.p + capsule.v);

    if (ap.Dot(ab) < 0) {
        float distance = ap.Length();
        if (distance <= capsule.r) {
            return true;
        }
    }
    else if (bp.Dot(ba) < 0) {
        float distance = bp.Length();
        if (distance <= capsule.r) {
            return true;
        }
    }
    else {
        Vector2 ai = capsule.p + ab.GetNormalize() * ap.Dot(ab.GetNormalize());
        float distance = (p.p - ai).Length();
        if (distance <= capsule.r) {
            return true;
        }
    }
    return false;
}

bool ColPrimitive2D::IsHit(Segment seg, Rect rect)
{
    for (Segment rectseg : rect.ToSideArray()) {
        Vector2 v = rectseg.p - seg.p;
        float t = v.Cross(seg.v.GetNormalize()) / seg.v.GetNormalize().Cross(rectseg.v);
        if (t >= 0 && t <= 1) {
            Vector2 p = rectseg.p + t * rectseg.v;
            if ((p - seg.p).LengthSq() == 0 || ((p - seg.p).GetNormalize().Dot(seg.v.GetNormalize()) > 0 && (p - seg.p).LengthSq() <= seg.v.LengthSq())) {
                return true;
            }
        }
    }

    return false;
}

ColResult ColPrimitive2D::CheckHit(Point p, Capsule capsule)
{
    ColResult result;
    Vector2 ap = p.p - capsule.p;
    Vector2 bp = p.p - (capsule.p + capsule.v);
    Vector2 ab = (capsule.p + capsule.v) - capsule.p;
    Vector2 ba = capsule.p - (capsule.p + capsule.v);

    if (ap.Dot(ab) < 0) {
        float distance = ap.Length();
        if (distance <= capsule.r) {
            result.hit = true;
            result.hitPos = p.p;
            result.onLinePos = capsule.p;
        }
    }
    else if (bp.Dot(ba) < 0) {
        float distance = bp.Length();
        if (distance <= capsule.r) {
            result.hit = true;
            result.hitPos = p.p;
            result.onLinePos = capsule.p;
        }
    }
    else {
        Vector2 ai = capsule.p + ab.GetNormalize() * ap.Dot(ab.GetNormalize());
        float distance = (p.p - ai).Length();
        if (distance <= capsule.r) {
            result.hit = true;
            result.hitPos = p.p;
            result.onLinePos = ai;
        }
    }
    return result;
}

ColResult ColPrimitive2D::CheckHit(Segment seg, Rect rect)
{
    ColResult result;

    array<Segment, 4> arr = rect.ToSideArray();
    float nearestHitDistance = 0;

    for (int32_t i = 0; i < 4; i++) {
        Segment rectseg = arr[i];
        Vector2 v = rectseg.p - seg.p;
        Vector2 v2 = seg.p - rectseg.p;
        float t = v.Cross(seg.v.GetNormalize()) / seg.v.GetNormalize().Cross(rectseg.v);
        float t2 = v2.Cross(rectseg.v.GetNormalize()) / rectseg.v.GetNormalize().Cross(seg.v);
        if ((t >= 0 && t <= 1) && (t2 >= 0 && t2 <= 1)) {
            Vector2 p = rectseg.p + t * rectseg.v;
            Vector2 p2 = seg.p + t2 * seg.v;
            if (true) {
                if (!result.hit) {
                    result.hit = true;
                    result.hitPos = p;
                    result.hasHitPos = true;
                    result.hitSide = i;
                    result.hitSideBit.set(i);
                    nearestHitDistance = (p - seg.p).LengthSq();
                }
                else {
                    float hitDistance = (p - seg.p).LengthSq();
                    if (hitDistance < nearestHitDistance) {
                        result.hitPos = p;
                        result.hitSide = i;
                        result.hitSideBit = 0;
                        result.hitSideBit.set(i);
                        nearestHitDistance = hitDistance;
                    }
                    else if (hitDistance == nearestHitDistance) {
                        result.hitPos = p;
                        result.hitSide = i;
                        result.hitSideBit.set(i);
                    }
                }
            }
        }
    }

    return result;
}
