#include "Material.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"

using namespace std;

bool Material::operator==(const Material& o) const
{
    return mName == o.mName
        && mTexture == o.mTexture
        && mColor == o.mColor
        && mAmbient == o.mAmbient
        && mDiffuse == o.mDiffuse
        && mSpecular == o.mSpecular
        && mTiling == o.mTiling;
}

bool Material::operator!=(const Material& o) const
{
    return !(*this == o);
}

void Material::Transfer(MaterialBuffer* target)
{
	target->color = mColor;
	target->ambient = mAmbient;
	target->diffuse = mDiffuse;
	target->specular = mSpecular;
	target->tiling = mTiling;
}
