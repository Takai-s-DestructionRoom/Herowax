#pragma once
class Singleton
{
protected:
	//コンストラクタ
	Singleton() = default;
	//コピー禁止
	Singleton(const Singleton&) = delete;
	//デストラクタ
	~Singleton() = default;
	//コピー禁止
	Singleton& operator=(const Singleton&) = delete;
};

