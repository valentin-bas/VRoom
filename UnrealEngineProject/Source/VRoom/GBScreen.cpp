

#include "VRoom.h"
#include "GBScreen.h"

#include "libvrgb/VRGBEmulator.h"
#include "libvrgb/Cartridge.h"
#include "libvrgb/Pad.h"

static const uint8 g_Palettes[][4][3] =
{
	{
		{ 16, 57, 16 },
		{ 49, 99, 49 },
		{ 140, 173, 16 },
		{ 156, 189, 16 }
	},
	{
		{ 0, 0, 0 },
		{ 85, 85, 85 },
		{ 170, 170, 170 },
		{ 255, 255, 255 }
	}
};

AGBScreen::AGBScreen(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), m_Emulator(nullptr)
{
	screen = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("screen"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/MyAssets/Models/GBScreen_SM.GBScreen_SM'"));
	
	screen->SetStaticMesh(StaticMesh.Object);
	RootComponent = screen;

	PrimaryActorTick.bCanEverTick = true;
}

void	AGBScreen::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//create dynamic material
	DynamicMaterial = UMaterialInstanceDynamic::Create(MasterMaterialRef, RootComponent);

	// create dynamic texture
	DynamicTexture = UTexture2D::CreateTransient(CGBFramebuffer::kFrameBufferWidth, CGBFramebuffer::kFrameBufferHeight);
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	screen->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetTextureParameterValue(FName("T2DTexture"), DynamicTexture);

	m_Emulator = new vrgb::VRGBEmulator();				//CHECK //LEAK
	if (m_Emulator->Init())
	{
		//CHECK
	}

	m_Cartridge = new vrgb::Cartridge("tetris.gb");	//CHECK //LEAK
	if (m_Cartridge->IsLoaded())
	{
		m_Emulator->SetScreen(&m_Framebuffer);
		m_Emulator->LoadCartridge(m_Cartridge);
	}
}

void	AGBScreen::BeginPlay()
{
	Super::BeginPlay();

	FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	if (Iterator)
	{
		EnableInput(*Iterator);
	}
}

void	AGBScreen::Tick(float dt)
{
	Super::Tick(dt);

	m_Emulator->Update(); //FIXME // DO IT IN A THREAD ?

	static FUpdateTextureRegion2D Region = FUpdateTextureRegion2D(0, 0, 0, 0, CGBFramebuffer::kFrameBufferWidth, CGBFramebuffer::kFrameBufferHeight);
	if (m_Framebuffer.UniqueNeedRefresh())
	{
		DynamicTexture->UpdateResource();
		UpdateTextureRegions(DynamicTexture, 0, 1, &Region, CGBFramebuffer::kFrameBufferWidth * 4, 4, m_Framebuffer.Data(), false);
	}
}

void	AGBScreen::UpButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbUP, pressed);
}

void	AGBScreen::DownButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbDOWN, pressed);
}

void	AGBScreen::LeftButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbLEFT, pressed);
}

void	AGBScreen::RightButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbRIGHT, pressed);
}

void	AGBScreen::AButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbA, pressed);
}

void	AGBScreen::BButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbB, pressed);
}

void	AGBScreen::SelectButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbSELECT, pressed);
}

void	AGBScreen::StartButtonChangeState(bool pressed)
{
	vrgb::Pad::SetState(vrgb::Pad::gbSTART, pressed);
}


void	AGBScreen::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
				if (RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->Texture2DResource->GetTexture2DRHI(),
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
				}
			}
			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
			});
	}
}

AGBScreen::CGBFramebuffer::CGBFramebuffer()
	: m_RefreshScreen(false)
{
}

void	AGBScreen::CGBFramebuffer::OnPreDraw()
{
}

void	AGBScreen::CGBFramebuffer::OnPostDraw()
{
}

void	AGBScreen::CGBFramebuffer::OnDrawPixel(int idColor, int x, int y)
{
	uint8	r = g_Palettes[0][idColor][0];
	uint8	g = g_Palettes[0][idColor][1];
	uint8	b = g_Palettes[0][idColor][2];

	m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 0] = r;
	m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 1] = g;
	m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 2] = b;
	m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 3] = 255;
}

void	AGBScreen::CGBFramebuffer::OnRefreshScreen()
{
	m_RefreshScreen = true;
}
void	AGBScreen::CGBFramebuffer::OnClear()
{
	for (int r = 0; r < kFrameBufferWidth; ++r)
		for (int c = 0; c < kFrameBufferHeight; ++c)
			for (int k = 0; k < 4; ++k)
				m_Framebuffer[r * kFrameBufferWidth * 4 + c * 4 + k] = 255;
}

bool	AGBScreen::CGBFramebuffer::UniqueNeedRefresh()
{
	if (m_RefreshScreen)
	{
		m_RefreshScreen = false;
		return true;
	}
	return false;
}