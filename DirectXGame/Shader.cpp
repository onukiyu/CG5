#include "Shader.h"
#include <d3dcompiler.h>    // D3DCompileFromFile
#include <cassert>          // assert


// コンパイル済みのシェーダーデータを返す　　※未コンパイルの場合は nullptr となる
ID3DBlob* Shader::GetBlob() { 
	return blob_; 
}

// コンストラクタ
Shader::Shader() {
}

// デストラクタ
Shader::~Shader() { 
	if (blob_ != nullptr) {
		blob_->Release();
		blob_ = nullptr;
	}
}

// シェーダーファイルを読み込み、コンパイルする
void Shader::Load(const std::wstring& filePath, const std::string& shaderModel) {
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
	    filePath.c_str(), // シェーダーファイル名
	    nullptr,
	    D3D_COMPILE_STANDARD_FILE_INCLUDE,               // インクルード可能にする
	    "main", shaderModel.c_str(),                     // エントリーポイント名、シェーダモデル指定
	    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
	    0, &shaderBlob, &errorBlob);
	// エラーが発生した場合止める
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			errorBlob->Release();
		}
		assert(false);
	}
	// 生成したshaderBlobを返す
	blob_ = shaderBlob;
}

