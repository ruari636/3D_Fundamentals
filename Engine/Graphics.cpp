﻿/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include <functional>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
	:
	sysBuffer( ScreenWidth,ScreenHeight )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL	featureLevelsRequested = D3D_FEATURE_LEVEL_9_1;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	featureLevelsSupported;
	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef _DEBUG
#ifdef USE_DIRECT3D_DEBUG_RUNTIME
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		&featureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		&featureLevelsSupported,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}
}

Graphics::~Graphics()
{
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// perform the copy line-by-line
	sysBuffer.Present( mappedSysBufferTexture.RowPitch,
		reinterpret_cast<BYTE*>(mappedSysBufferTexture.pData) );
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
	}
}

void Graphics::BeginFrame()
{
	sysBuffer.Clear( Colors::Red );
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

void Graphics::DrawLine( float x1,float y1,float x2,float y2,Color c )
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;

	if( dy == 0.0f && dx == 0.0f )
	{
		PutPixel( int( x1 ),int( y1 ),c );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dx / dy;
		float y = y1;
		int lastIntY;
		for( float x = x1; y < y2; y += 1.0f,x += m )
		{
			lastIntY = int( y );
			PutPixel( int( x ),lastIntY,c );
		}
		if( int( y2 ) > lastIntY )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
	else
	{
		if( dx < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dy / dx;
		float x = x1;
		int lastIntX;
		for( float y = y1; x < x2; x += 1.0f,y += m )
		{
			lastIntX = int( x );
			PutPixel( lastIntX,int( y ),c );
		}
		if( int( x2 ) > lastIntX )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
}

void Graphics::DrawFlatTopTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, Color c)
{
	const float dy = v2.y - v0.y;
	const float g0 = (v2.x - v0.x) / dy;
	const float g1 = (v2.x - v1.x) / dy;
	const int startY = (int)ceil(v0.y - 0.5f);
	const int endY = (int)ceil(v2.y - 0.5f);

	for (int y = startY, n = 0; y != endY; n++, y++)
	{
		const int startX = (int)ceil(v0.x + (float)n * g0 - 0.5f);
		const int endX = (int)ceil(v1.x + (float)n * g1 - 0.5f);

		for (int x = startX; x != endX; x++)
		{
			PutPixel(x, y, c);
		}
	}
}

void Graphics::DrawFlatBottomTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, Color c)
{
	const float dy = v1.y - v0.y;
	const float g1 = (v2.x - v0.x) / dy;
	const float g0 = (v1.x - v0.x) / dy;
	const int startY = (int)ceil(v0.y - 0.5f);
	const int endY = (int)ceil(v1.y - 0.5f);

	for (int y = startY, n = 0; y != endY; n++, y++)
	{
		const int startX = (int)ceil(v0.x + (float)n * g0 - 0.5f);
		const int endX = (int)ceil(v0.x + (float)n * g1 - 0.5f);

		for (int x = startX; x != endX; x++)
		{
			PutPixel(x, y, c);
		}
	}
}

void Graphics::DrawFlatTopTriangleTex(const TexVec& v0, const TexVec& v1, const TexVec& v2, const Surface& Tex)
{
	const float dy = v2.y - v0.y;
	const float g0 = (v2.x - v0.x) / dy;
	const float g1 = (v2.x - v1.x) / dy;
	const int startY = (int)ceil(v0.y - 0.5f);
	const int endY = (int)ceil(v2.y - 0.5f);
	const float TexWidth = float(Tex.GetWidth());
	const float TexHeight = float(Tex.GetHeight());
	const float uClamp = TexWidth - 1.0f;
	const float vClamp = TexHeight - 1.0f;

	Vec2 v0v2uvStep = (v2.uv - v0.uv) / dy;
	Vec2 v1v2uvStep = (v2.uv - v1.uv) / dy;

	Vec2 v0v2CurUV = v0v2uvStep * (startY - v0.y + 0.5f) + v0.uv; //+ 0.5f ia there to account for us looking up in the texture at a
	Vec2 v1v2CurUV = v1v2uvStep * (startY - v0.y + 0.5f) + v1.uv;//pixels center point

	Vec2 v0v1uvStep = (v1.uv - v0.uv) / (v1.x - v0.x);

	for (int y = startY, n = 0; y != endY; n++, y++, v0v2CurUV += v0v2uvStep, v1v2CurUV += v1v2uvStep)
	{
		const int startX = (int)ceil(v0.x + (float)n * g0 - 0.5f);
		const int endX = (int)ceil(v1.x + (float)n * g1 - 0.5f);
		Vec2 uvStart = v0v1uvStep * (float(startX) - v0.x + 0.5f) + v0v2CurUV;

		for (int x = startX; x != endX; x++, uvStart += v0v1uvStep)
		{								//making sure we don't look outside Tex
			PutPixel(x, y, Tex.GetPixel(std::max(0.0f, std::min(uvStart.x * TexWidth, uClamp)), std::max(0.0f, std::min(uvStart.y * TexHeight, vClamp))));
		}
	}
}

void Graphics::DrawFlatBottomTriangleTex(const TexVec& v0, const TexVec& v1, const TexVec& v2, const Surface& Tex)
{
	const float dy = v1.y - v0.y;
	const float g1 = (v2.x - v0.x) / dy;
	const float g0 = (v1.x - v0.x) / dy;
	const int startY = (int)ceil(v0.y - 0.5f);
	const int endY = (int)ceil(v1.y - 0.5f);
	const float TexWidth = float(Tex.GetWidth());
	const float TexHeight = float(Tex.GetHeight());
	const float uClamp = TexWidth - 1.0f;
	const float vClamp = TexHeight - 1.0f;

	Vec2 v0v1uvStep = (v1.uv - v0.uv) / dy;
	Vec2 v0v2uvStep = (v2.uv - v0.uv) / dy;\

	Vec2 v0v1CurUV = v0v1uvStep * (startY - v0.y + 0.5f) + v0.uv;//+ 0.5f ia there to account for us looking up in the texture at a
	Vec2 v0v2CurUV = v0v2uvStep * (startY - v0.y + 0.5f) + v1.uv;//pixels center point

	Vec2 v1v2uvStep = (v2.uv - v1.uv) / (v2.x - v1.x);

	for (int y = startY, n = 0; y != endY; n++, y++)
	{
		const int startX = (int)ceil(v0.x + (float)n * g0 - 0.5f);
		const int endX = (int)ceil(v0.x + (float)n * g1 - 0.5f);
		Vec2 uvStart = v0v1uvStep * (float(startX) - v0.x + 0.5f) + v0v1CurUV;

		for (int x = startX; x != endX; x++, uvStart += v1v2uvStep)
		{								//making sure we don't look outside Tex
			PutPixel(x, y, Tex.GetPixel(std::max(0.0f, std::min(uvStart.x * TexWidth, uClamp)), std::max(0.0f, std::min(uvStart.y * TexHeight, vClamp))));
		}
	}
}

void Graphics::DrawTriangle(const Vec2& p0, const Vec2& p1, const Vec2& p2, Color c)
{
	const Vec2* v0 = &p0;
	const Vec2* v1 = &p1;
	const Vec2* v2 = &p2;

	if (v0->y > v1->y) std::swap(v0, v1);
	if (v1->y > v2->y) std::swap(v1, v2);
	if (v0->y > v1->y) std::swap(v0, v1);

	if (v0->y == v1->y)
	{
		if (v0->x > v1->x) std::swap(v0, v1);
		DrawFlatTopTriangle(*v0, *v1, *v2, c);
	}
	else if (v1->y == v2->y)
	{
		if (v1->x > v2->x) std::swap(v1, v2);
		DrawFlatBottomTriangle(*v0, *v1, *v2, c);
	}
	else
	{
		float ratio = (v1->y - v0->y) / (v2->y - v0->y);
		const Vec2 p3 = *v0 + (*v2 - *v0) * ratio;
		const Vec2* v3 = &p3;
		if (v3->x < v1->x) std::swap(v1, v3);
		DrawFlatBottomTriangle(*v0, *v1, *v3, c);
		DrawFlatTopTriangle(*v1, *v3, *v2, c);
	}
}

void Graphics::DrawTriangleTex(const TexVec& p0, const TexVec& p1, const TexVec& p2, const Surface& Tex)
{
	const TexVec* v0 = &p0;
	const TexVec* v1 = &p1;
	const TexVec* v2 = &p2;

	if (v0->y > v1->y) std::swap(v0, v1);
	if (v1->y > v2->y) std::swap(v1, v2);
	if (v0->y > v1->y) std::swap(v0, v1);

	if (v0->y == v1->y)
	{
		if (v0->x > v1->x) std::swap(v0, v1);
		DrawFlatTopTriangleTex(*v0, *v1, *v2, Tex);
	}
	else if (v1->y == v2->y)
	{
		if (v1->x > v2->x) std::swap(v1, v2);
		DrawFlatBottomTriangleTex(*v0, *v1, *v2, Tex);
	}
	else
	{
		float ratio = (v1->y - v0->y) / (v2->y - v0->y);
		const TexVec p3 = *v0 + (*v2 - *v0) * ratio;
		const TexVec* v3 = &p3;
		if (v3->x < v1->x) std::swap(v1, v3);
		DrawFlatBottomTriangleTex(*v0, *v1, *v3, Tex);
		DrawFlatTopTriangleTex(*v1, *v3, *v2, Tex);
	}
}
