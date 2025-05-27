#include "Shader.h"
#include "MiscUtility.h"
#include <d3dcompiler.h>    // D3DCompileFromFile
#include <cassert>          // assert

#include <dxcapi.h>
#pragma comment(lib, "dxcpiler.lib")


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
void Shader::Load(const std::wstring& filePath, const std::wstring& shaderModel) {
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// wstring => string 文字列変換
	std::string mbShaderModel = ConvertString(shaderModel);

	// Shaderのコンパイル
	HRESULT hr = D3DCompileFromFile(
	    filePath.c_str(), // シェーダーファイル名
	    nullptr,
	    D3D_COMPILE_STANDARD_FILE_INCLUDE,               // インクルード可能にする
	    "main", mbShaderModel.c_str(),                   // エントリーポイント名、シェーダモデル指定
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

// シェーダーファイルを読み込み、コンパイルする
// ※外部コンパイル版   シェーダーモデル　6.0以上で利用する
void Shader::LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel) {
	// DXC(DirectX Shader Compiler)を初期化
	static IDxcUtils* dxcUtils;
	static IDxcCompiler3* dxcCompiler = nullptr;
	static IDxcIncludeHandler* includeHandler = nullptr;

	HRESULT hr;

	// DXCの生成
	if (dxcUtils == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
	}

	if (dxcCompiler == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
	}

	if (includeHandler == nullptr) {
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
	}

	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	// 読み込んだファイルの内容をDxcBufferに設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	// 2. Compileする
	//    Compileに必要なコンパイルオプションの準備
	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E",
		L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",
		shaderModel.c_str(), // ShaderProfileの設定
		L"-Zi",
		L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od",           // 最適化を外しておく
		L"-Zpr",          // メモリレイアウトは行優先
	};  
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,        // 読み込んだファイル
		arguments,                  // コンパイルオプション
		_countof(arguments),        // コンパイルオプションの数
		includeHandler,             // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	//     コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 3. 警告・エラーがでていないか確認する
	P23
}

