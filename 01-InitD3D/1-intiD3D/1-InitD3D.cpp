
#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include <wRL/client.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

// Global Variables
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ComPtr<ID3D11Device>g_pd3dDevice ;                  //ID3D11Device*   g_pd3dDevice = nullptr;
ComPtr<ID3D11DeviceContext>  g_pImmediateContext ;  //ID3D11DeviceContext*    g_pImmediateContext = nullptr;
ComPtr<IDXGISwapChain>g_pSwapChain ;                //IDXGISwapChain*         g_pSwapChain = nullptr; 
ComPtr<ID3D11RenderTargetView>g_pRenderTargetView; //ID3D11RenderTargetView* g_pRenderTargetView ; // //


// Forward declarations

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}



// Register class and create window

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION); //LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION); //LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 1: Direct3D 11 Basics",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}



// Called every time the application receives a message

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}



// Create Direct3D device and swap chain

HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;//left,top(0.0)
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	/*
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);*/

	
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	/*for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];*/
	hr = D3D11CreateDeviceAndSwapChain(
		    nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&sd,
			&g_pSwapChain,
			&g_pd3dDevice,
			&g_featureLevel,
			&g_pImmediateContext);


		/*if (SUCCEEDED(hr))
			break;
	}*/
	if (FAILED(hr))
		return hr;


	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;


	/*深度模板缓存
	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  //这里表示24位用于深度缓存，8位用于模板缓存
	dsDesc.Width = 800;                             //深度模板缓存的宽度
	dsDesc.Height = 600;                            //深度模板缓存的高度
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;    //绑定标识符
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;                      //CPU访问标识符，0为默认值
	dsDesc.SampleDesc.Count = 1;                    //多重采样的属性，本例中不采用多重采样即，
	dsDesc.SampleDesc.Quality = 0;                  //所以Count=1，Quality=0
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;

	//创建深度模板缓存
	hr = g_pd3dDevice->CreateTexture2D(&dsDesc, 0, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil buffer failed!", L"ERROR", MB_OK);
		return false;
	}
	//创建深度模板缓存视图
	hr = g_pd3dDevice->CreateDepthStencilView(depthStencilBuffer, 0, depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil view failed!", L"ERROR", MB_OK);
		return false;
	}*/

	g_pImmediateContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), nullptr);
	/*ID3D11RenderTargetView* RTs[] = { g_pRenderTargetView.Get() };
	g_pImmediateContext->OMSetRenderTargets(_countof(RTs), RTs, nullptr);*/

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}



// Render the frame

void Render()
{
	// Just clear the backbuffer
	float clearColor[4] = { 0.5f,0.1f,0.2f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), clearColor);
	g_pSwapChain->Present(0, 0);
}



// Clean up the objects we've created

void CleanupDevice()
{
	//if (g_pImmediateContext) g_pImmediateContext->ClearState();

	g_pRenderTargetView.Reset(); //if (g_pRenderTargetView) g_pRenderTargetView->Release(); // ////

	g_pSwapChain.Reset(); //

	g_pImmediateContext.Reset(); //if (g_pImmediateContext) g_pImmediateContext->Release();////

	g_pd3dDevice.Reset(); //if (g_pd3dDevice) g_pd3dDevice->Release();////
}
