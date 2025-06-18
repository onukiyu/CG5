#pragma once

#include <d3d12.h>

class IndexBuffer {
public:
	// IndexBuffer生成
	void Create(const UINT size, const UINT stride);

	// ゲッター
	ID3D12Resource* Get();               // インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW* GetView(); // インデックスバッファービュー

	// コンストラクタ
	IndexBuffer();
	// デストラクタ
	~IndexBuffer();

private:
	ID3D12Resource* indexBuffer_ = nullptr;        // 頂点バッファ
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{}; // 頂点バッファビュー

};
