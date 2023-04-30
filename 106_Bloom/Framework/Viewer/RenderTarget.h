﻿#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	ID3D11RenderTargetView* RTV() { return rtv; }
	ID3D11ShaderResourceView* SRV() { return srv; }

	void SaveTexture(wstring file);

	// OM 에서 RTV 와 DSV 를 합쳐야 하기 때문에, 같은 크기로 만들어야 합니다.
	void PreRender(class DepthStencil* depthStencil);
	static void PreRender(RenderTarget** targets, UINT count, class DepthStencil* depthStencil);

private:
	UINT width, height;
	DXGI_FORMAT format;

	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
};
