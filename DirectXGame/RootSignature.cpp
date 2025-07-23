#include "RootSignature.h"
#include "KamataEngine.h"
#include "PipelineState.h"

#include <cassert>

using namespace KamataEngine;

// RootSignatureを生成する
void RootSignature::Create() {
	// 既にインスタンスがあるなら解放する        // Createメンバ変数が二度実行された時の対処
	if (rootSignature_) {
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}

	// クラス内で取得するために追加
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// RootSignature作成 --------------------------------
	// 構造的にデータを用意する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE srvDescRange[1]{};
	// t0 レジスタを利用可能にする
	srvDescRange[0].BaseShaderRegister = 0;                         // 0から始まる
	srvDescRange[0].NumDescriptors = 1;                             // 数は 1つ
	srvDescRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;    // SRV
	srvDescRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// RootParametorの用意 ※PixelShaderに読ませるために必要　★00_09 追加
	// 複数設定できるので配列の構造をしている。今回は 1つだけなので、長さ1の配列として用意する
	D3D12_ROOT_PARAMETER rootParameters[1]{};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTable
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           //PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = srvDescRange;           // 拡張しやすくなる
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(srvDescRange); // RangeTable数

	descriptionRootSignature.pParameters = rootParameters;     // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

	// Sampleの設定　★00_09 追加
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;     // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;   // 0.0~1.0の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;   // 0.0~1.0の範囲外をリピート
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;   // 0.0~1.0の範囲外をリピート
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;                   // ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;                           // レジスタ番号 0を使う(s0)
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlog = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlog);

	if (FAILED(hr)) {
		DebugText::GetInstance()->ConsolePrintf(reinterpret_cast<char*>(errorBlog->GetBufferPointer()));
		assert(false);
	}

	// バイナリをもとに作成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// signatureblob は RootSignatureの生成後解放してもいい
	signatureBlob->Release();

	// 生成した RootSignature をとっておく
	rootSignature_ = rootSignature;
}


// 生成した RootSignatureを返す
ID3D12RootSignature* RootSignature::Get() { 
	return rootSignature_; 
}

// コンストラクタ
RootSignature::RootSignature() {
}

// デストラクタ
RootSignature::~RootSignature() {
	if (rootSignature_) {
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}
}


