#include "TextDrawer.h"
#include "RDirectX.h"
#include "Util.h"
#include "PathUtil.h"

using namespace std;

FontTexture TextDrawer::GetFontTexture(std::string glyph, std::string fontTypeFace, uint32_t fontSize, bool useAlign)
{
	wstring wGlyph = Util::ConvertStringToWString(glyph);
	wstring wFontTypeFace = Util::ConvertStringToWString(fontTypeFace);
	return GetFontTexture(wGlyph, wFontTypeFace, fontSize, useAlign);
}

FontTexture TextDrawer::GetFontTexture(std::wstring glyph, std::wstring fontTypeFace, uint32_t fontSize, bool useAlign)
{
	TextDrawer* drawer = GetInstance();
	lock_guard<recursive_mutex> lock(drawer->mMutex);

	if (glyph.length() != 1) {
		return FontTexture();
	}

	Glyph glyphData = { glyph, fontTypeFace, fontSize };

	auto itr = drawer->mGlyphMap.find(glyphData);
	if (itr != drawer->mGlyphMap.end()) {
		return itr->second;
	}

	TEXTMETRIC tm;
	GLYPHMETRICS gm;
	CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };

	HFONT _font = CreateFont(fontSize, 0, 0, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, fontTypeFace.c_str());

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, _font);
	uint32_t code = (uint32_t)*glyph.c_str();

#ifdef _DEBUG
	OutputDebugString((glyph + L"\n").c_str());
#endif

	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &Mat);
	vector<BYTE> buffer;
	buffer.resize(size == 0 ? 1 : size);
	GetTextMetrics(hdc, &tm);
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, size, &buffer[0], &Mat);

	// オブジェクトの開放
	SelectObject(hdc, oldFont);
	DeleteObject(_font);
	ReleaseDC(NULL, hdc);

	int32_t fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
	int32_t fontHeight = size / fontWidth;

	int32_t baseLineY = tm.tmAscent;
	int32_t glyphOriginY = 0;

	if (useAlign) {
		glyphOriginY = baseLineY - gm.gmptGlyphOrigin.y;
		fontHeight = glyphOriginY + fontHeight;
	}

	int32_t fontDataCount = fontWidth * fontHeight;

#ifdef _DEBUG
	OutputDebugString((wstring(L"fontWidth: ") + to_wstring(fontWidth) + L"(" + to_wstring(gm.gmBlackBoxX) + L")\n").c_str());
	OutputDebugString((wstring(L"fontHeight: ") + to_wstring(fontHeight) + L"(" + to_wstring(size / fontWidth) + L")\n").c_str());
#endif

	vector<Color> imageData;
	imageData.resize(fontDataCount);
	for (size_t i = 0; i < fontDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	for (size_t i = 0; i < size; i++) {
		size_t posX = i % fontWidth;
		size_t posY = glyphOriginY + (i / fontWidth);

		size_t access = (posY * fontWidth) + posX;
		assert(access < fontDataCount);

		float grayScale = (float)buffer[i];
		imageData[access].r = 1.0f;
		imageData[access].g = 1.0f;
		imageData[access].b = 1.0f;
		imageData[access].a = (grayScale * 255.0f / 16) / 255.0f;
	}

	FontTexture ftex;
	ftex.tm = tm;
	ftex.gm = gm;

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = fontWidth;
	textureResourceDesc.Height = fontHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	HRESULT result;
	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ftex.texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = ftex.texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(Color) * fontWidth,
		sizeof(Color) * fontDataCount
	);
	assert(SUCCEEDED(result));

	drawer->mGlyphMap[glyphData] = ftex;

	return ftex;
}

TextureHandle TextDrawer::CreateStringTexture(std::string text, std::string fontTypeFace, uint32_t fontSize, std::string handle)
{
	wstring wText = Util::ConvertStringToWString(text);
	wstring _wTypeFace = Util::ConvertStringToWString(fontTypeFace);

	list<FontTexture> glyphlist;

	for (size_t i = 0; i < wText.size(); i++) {
		glyphlist.push_back(GetFontTexture(wText.substr(i, 1), _wTypeFace, fontSize, true));
	}

	size_t originPos = 0;
	size_t textureWidth = 0;
	uint32_t textureHeight = 0;

	for (FontTexture fTex : glyphlist) {
		originPos += fTex.gm.gmptGlyphOrigin.x;
		textureWidth += fTex.texture.mResource->GetDesc().Width;
		originPos += fTex.gm.gmCellIncX;
		if (textureWidth < originPos) {
			textureWidth += originPos - textureWidth;
		}

		uint32_t height = fTex.texture.mResource->GetDesc().Height;
		if (textureHeight < height) {
			textureHeight = height;
		}
	}

	size_t imageDataCount = textureWidth * textureHeight;

	vector<Color> imageData;
	imageData.resize(imageDataCount == 0 ? 1 : imageDataCount);
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	size_t currentPos = 0; //現在の原点位置

	for (FontTexture tex : glyphlist) {
		size_t _width = tex.texture.mResource->GetDesc().Width;
		size_t _height = tex.texture.mResource->GetDesc().Height;
		size_t _dataCount = _width * _height;

		vector<Color> _image;
		_image.resize(_dataCount);

		HRESULT result;
		result = tex.texture.mResource->ReadFromSubresource(&_image[0], (UINT)(sizeof(Color) * _width), (UINT)(sizeof(Color) * _height), 0, nullptr);
		assert(SUCCEEDED(result));

		for (size_t i = 0; i < _dataCount; i++) {
			size_t posX = currentPos + tex.gm.gmptGlyphOrigin.x + (i % _width);
			size_t posY = i / _width;

			size_t access = (posY * textureWidth) + posX;
			assert(access < imageDataCount);

			imageData[access].r = _image[i].r;
			imageData[access].g = _image[i].g;
			imageData[access].b = _image[i].b;
			imageData[access].a = _image[i].a;
		}

		currentPos += tex.gm.gmCellIncX;
	}

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);

	if (imageDataCount == 0) {
		textureWidth = textureHeight = 1;
	}

	// テクスチャバッファ
	// ヒープ設定s
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	HRESULT result;
	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(Color) * (UINT)textureWidth,
		sizeof(Color) * (UINT)imageDataCount
	);
	assert(SUCCEEDED(result));

	string _handle = handle;
	if (_handle.empty()) {
		_handle = "NoNameStringTextureHandle_" + fontTypeFace + "_" + to_string(fontSize) + text;
	}
	return TextureManager::Register(texture, _handle);
}

TextureHandle TextDrawer::CreateStringTexture(std::wstring text, std::string fontTypeFace, uint32_t fontSize, std::string handle)
{
	wstring _wTypeFace = Util::ConvertStringToWString(fontTypeFace);

	list<FontTexture> glyphlist;

	for (size_t i = 0; i < text.size(); i++) {
		glyphlist.push_back(GetFontTexture(text.substr(i, 1), _wTypeFace, fontSize, true));
	}

	size_t originPos = 0;
	size_t textureWidth = 0;
	uint32_t textureHeight = 0;

	for (FontTexture fTex : glyphlist) {
		originPos += fTex.gm.gmptGlyphOrigin.x;
		textureWidth += fTex.texture.mResource->GetDesc().Width;
		originPos += fTex.gm.gmCellIncX;
		if (textureWidth < originPos) {
			textureWidth += originPos - textureWidth;
		}

		uint32_t height = fTex.texture.mResource->GetDesc().Height;
		if (textureHeight < height) {
			textureHeight = height;
		}
	}

	size_t imageDataCount = textureWidth * textureHeight;

	vector<Color> imageData;
	imageData.resize(imageDataCount);
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = Color(0, 0, 0, 0);
	}

	size_t currentPos = 0; //現在の原点位置

	for (FontTexture tex : glyphlist) {
		size_t _width = tex.texture.mResource->GetDesc().Width;
		size_t _height = tex.texture.mResource->GetDesc().Height;
		size_t _dataCount = _width * _height;

		vector<Color> _image;
		_image.resize(_dataCount);

		HRESULT result;
		result = tex.texture.mResource->ReadFromSubresource(&_image[0], (UINT)(sizeof(Color) * _width), (UINT)(sizeof(Color) * _height), 0, nullptr);
		assert(SUCCEEDED(result));

		for (size_t i = 0; i < _dataCount; i++) {
			size_t posX = currentPos + tex.gm.gmptGlyphOrigin.x + (i % _width);
			size_t posY = i / _width;

			size_t access = (posY * textureWidth) + posX;
			assert(access < imageDataCount);

			imageData[access].r = _image[i].r;
			imageData[access].g = _image[i].g;
			imageData[access].b = _image[i].b;
			imageData[access].a = _image[i].a;
		}

		currentPos += tex.gm.gmCellIncX;
	}

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);

	// テクスチャバッファ
	// ヒープ設定s
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	HRESULT result;
	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(Color) * (UINT)textureWidth,
		sizeof(Color) * (UINT)imageDataCount
	);
	assert(SUCCEEDED(result));

	string _handle = handle;
	if (_handle.empty()) {
		_handle = "NoNameStringTextureHandle_" + fontTypeFace + "_" + to_string(fontSize) + Util::ConvertWStringToString(text);
	}
	return TextureManager::Register(texture, _handle);
}

bool TextDrawer::LoadFontFromFile(std::string path)
{
	return AddFontResourceEx(PathUtil::ConvertAbsolute(path).c_str(), FR_PRIVATE, NULL) != 0;
}

void TextDrawer::Init()
{
	mPipeline = RDirectX::GetDefPipeline();
	mPipeline.mDesc.DepthStencilState.DepthEnable = true;
	mPipeline.mDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	mPipeline.Create();
}
