#pragma once

#include <string>
//#include <d3d12.h>

#include <d3dcompiler.h>
#include <dxcapi.h>

class Shader {
public:
	// シェーダーファイルを読み込み、コンパイル済みデータを生成する
	void Load(const std::wstring& filePath, const std::wstring& shaderModel);
	void LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel);

	// 生成したコンパイル済みデータを取得する
	ID3DBlob* GetBlob();
	IDxcBlob* GetDxcBlob();

	// コンストラクタ
	Shader();
	// デストラクタ
	~Shader();

private:
	ID3DBlob* blob_ = nullptr;  // コンストラクタで初期化しなくていい　※C++以降

	IDxcBlob* dxcBlob_ = nullptr;   // コンパイル済みのシェーダーデータ　※外部コンパイル版
};
